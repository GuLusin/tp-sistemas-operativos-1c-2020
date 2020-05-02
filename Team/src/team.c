/*                                                  HITO 2
 * Objetivos:
Proceso Team: Permitir solamente planificar de forma FIFO un conjunto de entrenadores.

Proceso Broker: Implementación completa de la administración de las colas de mensajes.
Aceptar suscripciones a una cola de mensajes específica.

Proceso GameBoy: Permitir el envío de varios mensajes al proceso Broker y el mensaje Appeared Pokemon
al proceso Team.
*/

/*
 * team.c: In function ‘iniciar_logger’:
team.c:21:10: warning: assignment makes pointer from integer without a cast [-Wint-conversion]
  if((log = log_create("broker.log","log",1,LOG_LEVEL_DEBUG)==NULL))
          ^
team.c: In function ‘subscribirse_a_colas’:
team.c:46:17: warning: implicit declaration of function ‘serializar_subscripcion’ [-Wimplicit-function-declaration]
  void* stream = serializar_subscripcion(2);
                 ^~~~~~~~~~~~~~~~~~~~~~~
team.c:46:17: warning: initialization makes pointer from integer without a cast [-Wint-conversion]
/tmp/ccdsswnv.o: In function `subscribirse_a_colas':
team.c:(.text+0x705): undefined reference to `serializar_subscripcion'
collect2: error: ld returned 1 exit status
 *
 */



#include "team.h"

//#include<readline/readline.h>

//gcc team.c -lpthread -lcommons -o team
//./team

int subscribirse_a_cola(cola_code cola){
	int socket_broker = connect_to(ip_broker,puerto_broker,wait_time);
	void* stream = serializar_subscripcion(cola);
	sendall(socket_broker, stream, sizeof(uint32_t)*3);
	free(stream);
	if(wait_ack(socket_broker)){
		puts("recibio acknowlegment del mensaje");
	}
	//close(socket_broker);
	return socket_broker;
}



t_entrenador* crear_entrenador(char* posicion, char* pokemones, char* objetivos,int i){
	t_entrenador* entrenador = malloc(sizeof(t_entrenador));

	entrenador->id = i;

	char **auxiliar = string_split(posicion,"|");

	entrenador->posicion_x = atoi(auxiliar[0]);
	entrenador->posicion_y = atoi(auxiliar[1]);

	auxiliar = string_split(pokemones,"|"); //ultima posicion tiene null

	entrenador->pokemones = list_create();

	while(*auxiliar){
		list_add(entrenador->pokemones,*auxiliar);
		auxiliar++;
	}

	auxiliar = string_split(objetivos,"|"); //ultima posicion tiene null

	entrenador->objetivos = list_create();
	while(*auxiliar){
		list_add(entrenador->objetivos,*auxiliar);
		auxiliar++;
	}

	return entrenador;
}


t_list* obtener_entrenadores(){
	new_entrenadores = list_create();
	char **lista_de_objetivos, **lista_de_pokemones, **lista_de_posiciones;
	t_entrenador* entrenador;

	lista_de_posiciones = config_get_array_value(config,"POSICIONES_ENTRENADORES");

	lista_de_pokemones = config_get_array_value(config,"POKEMON_ENTRENADORES");

	lista_de_objetivos = config_get_array_value(config,"OBJETIVOS_ENTRENADORES");

    int i =0;
	while(*lista_de_posiciones){
		entrenador = crear_entrenador(*lista_de_posiciones, *lista_de_pokemones, *lista_de_objetivos,i);
		list_add(new_entrenadores, entrenador);
		lista_de_objetivos++;
		lista_de_pokemones++;
		lista_de_posiciones++;
		i++;
	}

	return new_entrenadores;
}


//funcion para debug, eliminar
void mostrar_lista(t_list* lista){
	for (int i=0;i<list_size(lista);i++)
		printf("Elemento n%d: %s\n",i,list_get(lista,i));
}

t_list* obtener_pokemones_objetivo(t_list* lista_de_entrenadores){

	t_list* pokemones_ya_obtenidos = list_create();
	t_list* pokemones_objetivo = list_create();
	int i,j;

	t_entrenador* aux;

	for (i=0;i<list_size(lista_de_entrenadores);i++){
		aux=list_get(lista_de_entrenadores,i);
		list_add_all(pokemones_ya_obtenidos,aux->pokemones);
		list_add_all(pokemones_objetivo,aux->objetivos);
	}

	for (i=0;i<list_size(pokemones_objetivo);i++){
		for(j=0;j<list_size(pokemones_ya_obtenidos);j++){
			if(!strcmp(list_get(pokemones_objetivo,i),list_get(pokemones_ya_obtenidos,j))){
				list_remove(pokemones_objetivo,i);
				list_remove(pokemones_ya_obtenidos,j);
//				printf("Se remueve un pokemon que ya tenia: %s\n",list_get(pokemones_objetivo,i));
				break;
			}
		}
	}

	list_destroy(pokemones_ya_obtenidos);
//	mostrar_lista(pokemones_objetivo);

	return pokemones_objetivo;

}


void enviar_un_mensaje_get(int socket_a_enviar,char* un_pokemon){

}


void enviar_mensajes_get(int socket_a_enviar,t_list* todos_los_pokemones_que_faltan){
	t_list* ya_enviado = list_create();
	int i;

	bool es_el_mismo(void* un_pokemon){
		return !strcmp((char*)un_pokemon,(char*)list_get(todos_los_pokemones_que_faltan,i));
	}

	for (i=0;i<list_size(todos_los_pokemones_que_faltan);i++){
		if(list_any_satisfy(ya_enviado,(void*)es_el_mismo))
			continue; //si ya fue enviado que no haga nada
		printf("Tengo que enviar: %s\n",list_get(todos_los_pokemones_que_faltan,i));
		list_add(ya_enviado,list_get(todos_los_pokemones_que_faltan,i));
		enviar_un_mensaje_get(socket_a_enviar,list_get(todos_los_pokemones_que_faltan,i));
	}
}







void inicializar_team(){

	int socket_broker, socket_cola_localized, socket_cola_caught, socket_cola_appeared;
	t_list* pokemones_objetivo;

	logger = log_create("team.log","log",1,LOG_LEVEL_DEBUG);
	config = config_create("config");


	new_entrenadores = obtener_entrenadores();
	pokemones_objetivo = obtener_pokemones_objetivo(new_entrenadores);

	//Obtiene los datos IP,PUERTO WAIT_TIME desde la config



	ip_broker = config_get_string_value(config,"IP_BROKER");
	log_debug(logger,config_get_string_value(config,"IP_BROKER")); //pido y logueo ip
	puerto_broker = config_get_string_value(config,"PUERTO_BROKER");
	log_debug(logger,config_get_string_value(config,"PUERTO_BROKER")); //pido y logueo puerto
	wait_time = config_get_int_value(config,"TIEMPO_RECONEXION");

	socket_cola_appeared = subscribirse_a_cola(COLA_APPEARED_POKEMON);
	socket_cola_localized = subscribirse_a_cola(COLA_LOCALIZED_POKEMON);
	socket_cola_caught = subscribirse_a_cola(COLA_CAUGHT_POKEMON);


	enviar_mensajes_get(socket_broker,pokemones_objetivo);



//	enviar_mensajes_get();

}


int main(void) {


	inicializar_team();


	return EXIT_SUCCESS;
}
