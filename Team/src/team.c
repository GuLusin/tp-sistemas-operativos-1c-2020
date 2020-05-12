*                                                  HITO 2
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

void mostrar_entrenador(t_entrenador *t){
	printf("x :%d\n",t->posicion_x);
	printf("y :%d\n",t->posicion_y);
	printf("id :%d\n",t->id);
}

//---------------------------------------PLANIFICACION---------------------------------------------------------------------

void inicializar_semaforo_entrenadores(){
	for (int i=0;i<list_size(entrenadores);i++){
		pthread_mutex_init(ejecutar_entrenador+i, NULL);
	}
}

void crear_listas_globales(){
	entrenadores = list_create();
	lista_corto_plazo=list_create();
	pokemons_recibidos=list_create();
	cantidad_pokemons_globales=0;
	cantidad_objetivos_globales=0;
}

int distancia_menor(int posicion_pokemon_x,int posicion_pokemon_y){
	int distancia(t_entrenador* entrenador){
		return (abs(entrenador->posicion_x - posicion_pokemon_x) + abs(entrenador->posicion_y - posicion_pokemon_y));}

	bool es_mayor(int numero1, int numero2){
		return (numero2) >= (numero1);}

	t_list* distancia_de_entrenadores =  list_map(entrenadores, distancia);
    list_sort(distancia_de_entrenadores, es_mayor);
	return list_get(distancia_de_entrenadores, 0);
}

t_entrenador *entrenador_mas_cerca(int posicion_pokemon_x,int posicion_pokemon_y){ //verifica que no tenga un objetivo
	int distancia(t_entrenador* entrenador){
		return (abs(entrenador->posicion_x - posicion_pokemon_x) + abs(entrenador->posicion_y - posicion_pokemon_y));}

	int dist_menor = distancia_menor(posicion_pokemon_x,posicion_pokemon_y);

	bool es_igual_distancia(t_entrenador* entrenador){
		//Pregunta si no tiene tiene un objetivo ya asignado
		int distancia1 = distancia(entrenador);
		return ((dist_menor == distancia1) && (entrenador->objetivo_temporal == NULL));}

	t_list* entrenadores_mas_cerca = list_filter(entrenadores, es_igual_distancia);
	return list_get(entrenadores_mas_cerca, 0);	//saco al primero por defecto si hay 2 a la misma distancia
}

void entrenador_mas_cerca_a_lista_corto_plazo(t_pokemon* pokemon){
	t_entrenador *entrenador_elegido;
	entrenador_elegido = entrenador_mas_cerca(pokemon->pos_x, pokemon->pos_y);
	entrenador_elegido->objetivo_temporal=malloc(sizeof(t_pokemon));
	entrenador_elegido->objetivo_temporal=pokemon;
    list_add(lista_corto_plazo, entrenador_elegido);
    //semaforo
}//ojo semaforos


void deadlock(){
	int index = 0;
	/*while(entrenadores){
		while(tiene_pokemons_faltantes(list_get(entrenador,index)){
			entrenador_actual = list_get(entrenador,index)
			entrenador_a_buscar = entrenador_que_le_sobra_este_pokemon(pokemon_faltante(entrenador_actual));
		    ir_a_buscar_al_entrenador(entrenador_actual->id,entrenador_a_buscar->id);
		    intercambiar(entrenador_actual->id,entrenador_a_buscar->id);
		}
		//habilitar semaforo
		entrenadores++;
		index++;
	}*/
}

void atrapar_pokemon(){
	//QUE INCREMENTE LA VARIABLE POKEMONS Y PONGA EN NULL EL TEMPORAL
	//envia mensaje catch_pokemon(pokemon);
	//guarda el id para ver si el id correlativo corresponde
	//si recibe confirmacion lo agrego a la lista de pokemons y se queda en esta posicion
	//sino nada, queda en esta posicion pero no se agrega el pokemon
}

void avanzar(int id){
	t_entrenador *entrenador = list_get(entrenadores, id);
	int distancia_x = abs(entrenador->objetivo_temporal->pos_x - entrenador->posicion_x);
	int distancia_y = abs(entrenador->objetivo_temporal->pos_y - entrenador->posicion_y);

	if(distancia_x - distancia_y > 0)
		entrenador->posicion_x++;
	else
		entrenador->posicion_y++;
}

bool cumplio_objetivo_entrenador(int id){ //verifica si las listas de objetivos y pokemones son iguales
	t_entrenador *entrenador = list_get(entrenadores, id);
	int exito;
	t_list* pokemones_ya_obtenidos = list_create();
	t_list* pokemones_objetivo = list_create();
	int i,j;

	list_add_all(pokemones_ya_obtenidos,entrenador->pokemones);
	list_add_all(pokemones_objetivo,entrenador->objetivos);

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

	if(list_is_empty(pokemones_objetivo))
		exito = true;
	else
		exito = false;
	list_destroy(pokemones_ya_obtenidos);
	list_destroy(pokemones_objetivo);
	return exito;
}

bool tiene_cantidad(int id){
	t_entrenador *entrenador = list_get(entrenadores, id);
	return (list_size(entrenador->pokemones) == list_size(entrenador->objetivos));
}

bool llego_al_objetivo(t_entrenador *entrenador){
	return ((entrenador->posicion_x == entrenador->objetivo_temporal->pos_x) && (entrenador->posicion_y == entrenador->objetivo_temporal->pos_y));
}

void entrenador(int id){
	/*
	while(!tiene_cantidad(id)){
		//esperar semaforo
		//avanzar(id);
		//avisar al algoritmo planificador
		if(llego_al_objetivo(list_get(entrenadores,id)))
		   atrapar(int id);
	}

	while(!cumplio_objetivo_entrenador(id)){
    //wait(deadlock); //segun su id
	}*/
}

bool cumplio_cantidad_global(){
     return cantidad_pokemons_globales == cantidad_objetivos_globales;
}

void planificacionFIFO(){
	/*int retardo = config_get_int_value(config,"RETARDO_CICLO_CPU");
	while(!cumplio_cantidad_global()){
	//pregunta si puede sacar uno con un semaforo
	t_entrenador *entrenador = list_get(lista_corto_plazo,0);
	while(!llego_al_objetivo(entrenador)){
	activar_semaforo_entreandor(entrenador->id);

    //esperar_confirmacion del entrandor que se movio
	desactivar_semaforo_entrenador(entrenador->id);
	sleep(retardo);
	}
	list_remove(lista_corto_plazo,0);
	}*/
}
void planificacionRR(){
	/*int retardo = config_get_int_value(config,"RETARDO_CICLO_CPU");
	int quantum = config_get_int_value(config,"QUANTUM");
	int cantidad = 0;
	while(!cumplio_cantidad_global()){
	//pregunta si puede sacar uno con un semaforo
	t_entrenador *entrenador = list_get(lista_corto_plazo,0);
	while(!llego_al_objetivo(entrenador) && cantidad < quantum)){
	activar_semaforo_entreandor(entrenador->id);

    //esperar_confirmacion del entrandor que se movio
	desactivar_semaforo_entrenador(entrenador->id);
	sleep(retardo);
	cantidad++;
    }
	cantidad = 0;
	if(llego_al_objetivo(entrenador))
	list_remove(lista_corto_plazo,0);
	else
	list_add(lista_corto_plazo,entrenador);
	}*/
}
void planificacionSJF_CD(){
}
void planificacionSJF_SD(){
}

void planificar(){
	t_pokemon *pokemon = list_get(pokemons_recibidos,0);
	list_remove(pokemons_recibidos,0);
	entrenador_mas_cerca_a_lista_corto_plazo(pokemon);
}

int convertir(char* algoritmo_nombre){
	if (!strcmp(algoritmo_nombre,"FIFO")) return 0;
	if (!strcmp(algoritmo_nombre,"RR")) return 1;
	if (!strcmp(algoritmo_nombre,"SJF_SD")) return 2;
	if (!strcmp(algoritmo_nombre,"SJF_CD")) return 3;
	return -1;
}

void leer_algoritmo(){
	char* algoritmo_nombre = config_get_string_value(config,"ALGORITMO_PLANIFICACION");
	int algoritmo = convertir(algoritmo_nombre);
    switch(algoritmo){
    	case FIFO:
    		{pthread_t hilo_planificacion;
    		pthread_create(&hilo_planificacion, NULL, (void*)planificacionFIFO, NULL);
    		pthread_detach(hilo_planificacion);
    		break;}
    	case RR:
    		{puts("RR");
    		pthread_t hilo_planificacion;
    		pthread_create(&hilo_planificacion, NULL, (void*)planificacionRR, NULL);
    		pthread_detach(hilo_planificacion);
    		break;}
    	case SJF_CD:
    		{pthread_t hilo_planificacion;
    		pthread_create(&hilo_planificacion, NULL, (void*)planificacionSJF_CD, NULL);
    		pthread_detach(hilo_planificacion);
    		break;}
    	case SJF_SD:
    		{pthread_t hilo_planificacion;
    		pthread_create(&hilo_planificacion, NULL, (void*)planificacionSJF_SD, NULL);
    		pthread_detach(hilo_planificacion);
    		break;}
    	default:
    	    {puts("--------------------------------ERROR EN ALGORTIMO DE PLANIFICACION-----------------------------\n");
    	    break;}

    }
}

void planificador(){
	leer_algoritmo();
	while(!cumplio_cantidad_global()){ //cada entrenador tiene sus *cantidades* deseadas (no hace falta q coincidan)!
		//sem_wait(&hay_pokemones);
		//planificar();
		break;
	}
}

//------------------------------------- FIN DE PLANIFICACION ---------------------------------------------------

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
	entrenador->objetivo_temporal=NULL;

	char **auxiliar = string_split(posicion,"|");

	entrenador->posicion_x = atoi(auxiliar[0]);
	entrenador->posicion_y = atoi(auxiliar[1]);

	auxiliar = string_split(pokemones,"|"); //ultima posicion tiene null

	entrenador->pokemones = list_create();

	while(*auxiliar){
		list_add(entrenador->pokemones,*auxiliar);
		cantidad_pokemons_globales++;
		auxiliar++;
	}

	auxiliar = string_split(objetivos,"|"); //ultima posicion tiene null

	entrenador->objetivos = list_create();
	while(*auxiliar){
		list_add(entrenador->objetivos,*auxiliar);
		cantidad_objetivos_globales++;
		auxiliar++;
	}

	return entrenador;
}


void obtener_entrenadores(){
	char **lista_de_objetivos, **lista_de_pokemones, **lista_de_posiciones;
	t_entrenador* entrenador;

	lista_de_posiciones = config_get_array_value(config,"POSICIONES_ENTRENADORES");

	lista_de_pokemones = config_get_array_value(config,"POKEMON_ENTRENADORES");

	lista_de_objetivos = config_get_array_value(config,"OBJETIVOS_ENTRENADORES");

    int i =0;
	while(*lista_de_posiciones){
		entrenador = crear_entrenador(*lista_de_posiciones, *lista_de_pokemones, *lista_de_objetivos,i);
		list_add(entrenadores, entrenador);
		lista_de_objetivos++;
		lista_de_pokemones++;
		lista_de_posiciones++;
		i++;
	}
}


//funcion para debug, eliminar
void mostrar_lista(t_list* lista){
	for (int i=0;i<list_size(lista);i++)
		printf("Elemento n%d: %s\n",i,list_get(lista,i));
}

t_list* obtener_pokemones_objetivo(){

	t_list* pokemones_ya_obtenidos = list_create();
	t_list* pokemones_objetivo = list_create();
	int i,j;

	t_entrenador* aux;

	for (i=0;i<list_size(entrenadores);i++){
		aux=list_get(entrenadores,i);
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
    crear_listas_globales();

	logger = log_create("team.log","log",1,LOG_LEVEL_DEBUG);
	config = config_create("config");


	obtener_entrenadores();
	inicializar_semaforo_entrenadores();

	pokemones_objetivo = obtener_pokemones_objetivo();
    //mostrar_entrenador(list_get(entrenadores,0));
	//Obtiene los datos IP,PUERTO WAIT_TIME desde la config
    planificador(); //TENDRIA QUE ESTAR ABAJO, LUEGO DE SUSCRIBIRSE A LAS COLAS TENDRIAN Q HABER HILOS ACTIVOS A LA
    //ESPERA DE UN MENSAJE Y ACTUALIZAR LAS COLAS, ASI EL PLANIFICADOR PUEDE ACTUAR BIEN EN INDEPENDENCIA

    /*
    t_pokemon* pokemon = malloc(sizeof(t_pokemon));
    pokemon->nombre=NULL;
    pokemon->pos_x=1;
    pokemon->pos_y=3;
    entrenador_mas_cerca_a_lista_corto_plazo(pokemon);
    mostrar_entrenador(list_get(lista_corto_plazo,0));*/

	ip_broker = config_get_string_value(config,"IP_BROKER");
	log_debug(logger,config_get_string_value(config,"IP_BROKER")); //pido y logueo ip
	puerto_broker = config_get_string_value(config,"PUERTO_BROKER");
	log_debug(logger,config_get_string_value(config,"PUERTO_BROKER")); //pido y logueo puerto
	wait_time = config_get_int_value(config,"TIEMPO_RECONEXION");

	//estas subscripciones se harian en los threads de abajo
	socket_cola_appeared = subscribirse_a_cola(COLA_APPEARED_POKEMON);
	socket_cola_localized = subscribirse_a_cola(COLA_LOCALIZED_POKEMON);
	socket_cola_caught = subscribirse_a_cola(COLA_CAUGHT_POKEMON);

	//1 thread por socket intentando recibir mensajes aca?
	//1 thread por socket intentando recibir mensajes aca?
	//1 thread por socket intentando recibir mensajes aca?
}

int main(void) {


	inicializar_team();

	//planificador();
    //deadlock();
    //liberar_recursos();
	return EXIT_SUCCESS;
}
