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

//---------------------------------------PLANIFICACION---------------------------------------------------------------------

int distancia_menor(int posicion_pokemon_x,int posicion_pokemon_y){
	int distancia(t_entrenador* entrenador){
		return (abs(entrenador->posicion_x - posicion_pokemon_x) + abs(entrenador->posicion_y - posicion_pokemon_y));}

	bool es_mayor(int numero1, int numero2){
		return (numero2) >= (numero1);}

	t_list* distancia_de_entrenadores =  list_map(new_entrenadores, distancia);
    list_sort(distancia_de_entrenadores, es_mayor);
	return list_get(distancia_de_entrenadores, 0);
}

t_entrenador *entrenador_mas_cerca(int posicion_pokemon_x,int posicion_pokemon_y){
	int distancia(t_entrenador* entrenador){
		return (abs(entrenador->posicion_x - posicion_pokemon_x) + abs(entrenador->posicion_y - posicion_pokemon_y));}

	int dist_menor = distancia_menor(posicion_pokemon_x,posicion_pokemon_y);

	bool es_igual_distancia(t_entrenador* entrenador){
		int distancia1 = distancia(entrenador);
		return (dist_menor == distancia1);}

	t_list* entrenadores_mas_cerca = list_filter(new_entrenadores, es_igual_distancia);
	return list_get(entrenadores_mas_cerca, 0);	//saco al primero por defecto si hay 2 a la misma distancia
}

void entrenador_a_ready(int posicion_pokemon_x,int posicion_pokemon_y){
	t_entrenador *entrenador_elegido;
    int index;
    ready_entrenadores = list_create();// OJO
	entrenador_elegido = entrenador_mas_cerca(posicion_pokemon_x, posicion_pokemon_y);
	index = entrenador_elegido->id;
    list_add(ready_entrenadores, entrenador_elegido);//PRIMERO: 7, SEGUNDO: 8, TERCERO: 9, CUARTO:1, QUINTO: 2
	list_remove(new_entrenadores,index);
}


//FIFO => 7,8,9,1,2 => 8,9,1,2 => 9,1,2 => 1,2 => 2 => :)
//SJF (Sin desalojo) => 7,8,9,1,2 => 7,8,9,2 => 7,8,9 => 8,9 => 9 => :)
//RR => 7,8,9,1,2 => 8,9,1,2,5 => 9,1,2,5,6 => 1,2,5,6,7 => 2,5,6,7 => 5,6,7 => 6,7,3 => 7,3,5 => 3,5,5 => 5,5,1 etc...

// FIN planificacion ..........................................

int subscribirse_a_cola(cola_code cola){
	int socket_aux = connect_to(ip_broker,puerto_broker,wait_time);
	t_mensaje* mensaje = crear_mensaje(2, SUBSCRIPCION, cola);
	mensaje->id=ID_SUSCRIPCION;
	//printf("op_code:%d\nid:%d\ncola contenido:%d\n", mensaje->codigo_operacion,mensaje->id,mensaje->contenido.subscripcion);
	enviar_mensaje(socket_aux, mensaje);
	//uint32_t id = id_confirmation(socket_aux); // esta buena para implementar en la confirmacion del id.
	check_ack(socket_aux, ACK);
	return socket_aux;
}



t_entrenador* crear_entrenador(char* posicion, char* pokemones, char* objetivos, int i){
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
	config = config_create("../config");


	new_entrenadores = obtener_entrenadores();
	pokemones_objetivo = obtener_pokemones_objetivo(new_entrenadores);

	//Obtiene los datos IP,PUERTO WAIT_TIME desde la config



	ip_broker = config_get_string_value(config,"IP_BROKER");
	log_debug(logger,config_get_string_value(config,"IP_BROKER")); //pido y logueo ip
	puerto_broker = config_get_string_value(config,"PUERTO_BROKER");
	log_debug(logger,config_get_string_value(config,"PUERTO_BROKER")); //pido y logueo puerto
	wait_time = config_get_int_value(config,"TIEMPO_RECONEXION");

	//estas subscripciones se harian en los threads de abajo


	socket_cola_localized = subscribirse_a_cola(COLA_LOCALIZED_POKEMON);
	socket_cola_appeared = subscribirse_a_cola(COLA_APPEARED_POKEMON);
	socket_cola_caught = subscribirse_a_cola(COLA_CAUGHT_POKEMON);







	//1 thread por socket intentando recibir mensajes aca?
	//1 thread por socket intentando recibir mensajes aca?
	//1 thread por socket intentando recibir mensajes aca?

	/*
	entrenador1 ={pikachu} 1 2
	entrenador2 ={squirtle} 5 7

	sem_entrenadores[2]; //es dinamico, se puede?

	t_list objetivos; // cada elemento es una lista de objetivos de cada entrenador
	t_list adquiridos; // cada elemento es una lista de pokemons adquiridos por cada entrenador
	t_list objetivos_temporales // cada elemento es un unico pokemon
	pthread(entrenador1);
	pthread(entrenador2);

	//te olvidas de los entrenadores




	*/





	//enviar_mensajes_get(socket_broker,pokemones_objetivo);



//	enviar_mensajes_get();

}


/*

atrapar_pokemon(){
	while(!esta_en_posicion_pokemon()){
		acercar(pokemon);
	}
	catch_pokemon(pokemon);


}

entrenador(){
		pokemon a atrapar = lista_pokemones temporales(id) = busca a su pokemon objetivo temporal que le asigna el planificador
							// t_list global, por que?? porque el planificador le tiene que decir a cual atrapar, no se lo
						   // puede pasar por parametro. o si?

		int id;

		while(!cumple_objetivo_entrenador()){
			wait(sem_entrendaores(id);
			atrapar_pokemon(pokemon a atrapar); // esto es pseudo codigo pokemon a atrapar se llama cada vez q entra al while


		}

	}


planificacionFIFO(){



}

planificar(){

	depende del tipo de implementacion //RR, SJF CON O SIN, FIFO
	//PODRIAMOS HACER UN SWITCH PARA CADA TIPO => planificacion RR, planificiacion FIFO, ... SJF, con o sin desalojo
	pokemon = agarra el primer pokemon de los recibidos
	entrenador = elije entrenador optimo, el mas cercano.

	//FIFO
	// lista_planificacion = lista de ids, indican semaforos
	agrega al entrenador a la lista de planificacion

	int lista_corto_plazo[]; // {1} //new ponele

	vacia de a uno => activa al 1

	{1(10),2(3)} => activa al 1(10) y cuando termina => activa al 2(3)

	//SJF
	//sin desalojo

	{1(10)}=> ejecuta al 1(10) => llega el 2(7) => {1(9), 2(7)} => {1(8), 2(7), 3(3)} reordena => {1(7),3(3), 2(7)}

	//SJF
	//con desalojo

	{1(10)}=> ejecuta al 1(10) => llega el 2(7) => {1(9), 2(7)} desaloja y ejecuta => {2(6), 1(9)} y ejecuta => {2(5), 1(9)} y llega 3(3)

	=> reordena y desaloja y ejecuta => {3(2),2(6), 1(9)}

	//RR

	{1(10)} => ejecuta Q cuantos al 1(10) => llega el 2(7) => {1(9-Q), 2(7)} desaloja y ejecuta => {2(7-Q), 1(10-Q)} y ejecuta

			=> {1(10-2Q), 2(7-Q)} y llega 3(3)=> reordena y desaloja y ejecuta => {2(7-2Q),3(3), 1(10-2Q)}




}

planificador(){

	while(!cumplio_objetivo()){
		hacer_get_pokemons = llama a uno de cada especie de pokemones faltantes
		while(pokemones_recibidos_is_empty()); //cambiar por un semaforo mejor, ver mejora de la implementacion
		while(haya_pokemones_recibidos){
			planificar();



		}

	}
}

*/

int main(void) {


	inicializar_team();

	//planificador()


	return EXIT_SUCCESS;
}
