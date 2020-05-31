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

void debug_dic(t_dictionary* pokemones_deseados){

	void leeeer(char* key, void* count){
	//	enviar_get(key);
		printf("Nombre: %s    Cantidad:%d\n",key,(int)count);
	}

	printf("Lectura diccionario:\n");
	dictionary_iterator(pokemones_deseados,(void*)leeeer);
	printf("-------------------------------------------\n");
}

void debug_leer_lista(t_list* lista){
	printf("Comienza lectura de lista:\n");
	t_pokemon* aux;
	for (int i=0;i<list_size(lista);i++){
		aux = list_get(lista,i);
		printf("Pokemon %s, posx %d, posy %d\n",aux->nombre,aux->pos_x,aux->pos_y);
	}
	printf("----------------------------------------");

}



void mostrar_entrenador(t_entrenador *t){
	printf("EN x :%d ",t->posicion_x);
	printf("y En y :%d ",t->posicion_y);
	printf("| Entrenador id :%d\n",t->id);
}

void mostrar_pokemon(t_pokemon *pokemon){
	printf("nombre:%s\nposx:%d\npos y:%d\n",pokemon->nombre,pokemon->pos_x,pokemon->pos_y);
}

//---------------------------------------PLANIFICACION---------------------------------------------------------------------

void inicializar_semaforo_entrenadores(){
	ejecutar_entrenador = malloc(sizeof(sem_t)*list_size(entrenadores));
	for (int i=0;i<list_size(entrenadores);i++){
		sem_init(&(ejecutar_entrenador[i]), 0,0);
	}
	sem_init(&activar_algoritmo, 0,0);
	sem_init(&cumplio_objetivo_global, 0,0);
	sem_init(&hay_pokemones, 0, 0);
	sem_init(&hay_entrenador_corto_plazo,0,0);
}

void crear_listas_globales(){
	entrenadores = list_create();
	lista_corto_plazo=list_create();
	pokemons_recibidos=list_create();
	cantidad_pokemons_globales=0;
	cantidad_objetivos_globales=0;
}

void agregar_pokemon_recibido(t_pokemon *pokemon){
	pthread_mutex_lock(&mutex_pokemones_recibidos);
    list_add(pokemons_recibidos,pokemon);
    pthread_mutex_unlock(&mutex_pokemones_recibidos);
    sem_post(&hay_pokemones);
}

/*
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
}*/

void deadlock(){
	/*int index = 0;
	while(entrenadores){
		while(tiene_pokemons_faltantes(list_get(entrenador,index)){
			entrenador_actual = list_get(entrenador,index);
			entrenador_a_buscar = entrenador_que_le_sobra_este_pokemon(pokemon_faltante(entrenador_actual));
		    ir_a_buscar_al_entrenador(entrenador_actual->id,entrenador_a_buscar->id);
		    intercambiar(entrenador_actual->id,entrenador_a_buscar->id);
		}
		//habilitar semaforo
		entrenadores++;
		index++;
	}*/

	//PREGUNTAR
	//leee de la config
	//tarda 4 ciclos
	//intercambia
	//vuelve a preguntar
	//sleep(25);
	//sem_post(&cumplio_objetivo_global);
}

/*
void inicializar_deadlock_entrenadores(){
	deadlock_entrenador = malloc(sizeof(sem_t)*list_size(entrenadores));
	for(int i=0;i<list_size(entrenadores);i++){
		sem_init(&(dead_lock[i]), 0,0);
	}
	sem_init(&activar_algoritmo, 0,0);
}
*/

//---------------------------------------------- ENTRENADOR ----------------------------------------------------

bool tiene_cantidad(int id){
	t_entrenador *entrenador = list_get(entrenadores, id);
	return (list_size(entrenador->pokemones) == list_size(entrenador->objetivos));
}

bool llego_al_objetivo(t_entrenador *entrenador){
	return ((entrenador->posicion_x == entrenador->objetivo_temporal->pos_x) && (entrenador->posicion_y == entrenador->objetivo_temporal->pos_y));
}

void atrapar_pokemon(int id){
	//QUE INCREMENTE LA VARIABLE POKEMONS Y PONGA EN NULL EL TEMPORAL
	//envia mensaje catch_pokemon(pokemon);
	//guarda el id para ver si el id correlativo corresponde
	//si recibe confirmacion lo agrego a la lista de pokemons y se queda en esta posicion
	//sino nada, queda en esta posicion pero no se agrega el pokemon

	t_entrenador *entrenador = list_get(entrenadores,id);
	char* nombre = malloc(sizeof(char) * 15);
	strcpy(nombre,entrenador->objetivo_temporal->nombre);
	list_add(entrenador->pokemones, nombre);
	printf("Cantidades Globales: cantidad_pokemons_globales %d ", cantidad_pokemons_globales);
	printf("cantidad_pokemons_globales %d \n", cantidad_objetivos_globales);
	cantidad_pokemons_globales++;
	printf("Incremento la cantidad global de pokemones: ");
	printf("cantidad_pokemons_globales %d ", cantidad_pokemons_globales);
	printf("cantidad_pokemons_globales %d\n", cantidad_objetivos_globales);
	puts("ATRAPE POKEMON");
}

void avanzar(int id){
	t_entrenador *entrenador = list_get(entrenadores, id);
	int distancia_x = entrenador->objetivo_temporal->pos_x - entrenador->posicion_x;
	int distancia_y = entrenador->objetivo_temporal->pos_y - entrenador->posicion_y;

	if(distancia_x>0)
		entrenador->posicion_x++;
	else if(distancia_x<0)
		entrenador->posicion_x--;
	else if(distancia_y>0)
		entrenador->posicion_y++;
	else if(distancia_y<0)
		entrenador->posicion_y--;
}

void entrenador(int id){
	//deadlock?
	while(!tiene_cantidad(id)){
		sem_wait(&(ejecutar_entrenador[id]));
		sleep(retardo);
		avanzar(id);
		printf("Avance -> ");
		mostrar_entrenador(list_get(entrenadores,id));
		sem_post(&activar_algoritmo);
		if(llego_al_objetivo(list_get(entrenadores,id)))
		  {atrapar_pokemon(id);
		  //deadlock si entras en deadlock, solo se ejecuta todo el deadlock hasta terminar el intercambio?
		  //o tambien se puede hacer actuar el planificador? es monoprocesador no?
		  }
	}

	/*
	while(!cumplio_objetivo_entrenador(id)){ //hilo de deadlock que verifique para hacer el intercambio
    wait(deadlock); //segun su id
	}*/

	printf("CUMPLI MI OBJETIVO Y SOY FELIZ, MIS POKEMONS: ");
	t_entrenador *entrenador = list_get(entrenadores,id);
	for(int i=0; i < list_size(entrenador->pokemones); i++){
		printf("Pokemon numero: %d %s | ",i+1,list_get(entrenador->pokemones,i));
	}
}

void crear_hilos_entrenadores(){ //creamos todos los entrenadores, estos se quedan esperando la habilitacion del algoritmo para avanzar
	pthread_t *hilo_entrenador = malloc(sizeof(pthread_t)*list_size(entrenadores));
	for(int i=0; i < list_size(entrenadores);i++){
    	pthread_create(&(hilo_entrenador[i]), NULL, (void*)entrenador, i); //OJOOOOO
    	pthread_detach(hilo_entrenador[i]);
     }
}

//------------------------------FUNCION LARGA PARA ENCONTRAR EL MAS CERCANO------------------------------

int distancia_menor(int posicion_pokemon_x,int posicion_pokemon_y){
	int distancia(t_entrenador* entrenador){
		return (abs(entrenador->posicion_x - posicion_pokemon_x) + abs(entrenador->posicion_y - posicion_pokemon_y));}

	bool es_mayor(int numero1, int numero2){
		return (numero2) >= (numero1);}

	bool disponible(t_entrenador* entrenador){
		return (entrenador->objetivo_temporal == NULL);
	}

	t_list* distancia_disponibles = list_filter(entrenadores, disponible);
	t_list* distancia_de_entrenadores =  list_map(distancia_disponibles, distancia);
    list_sort(distancia_de_entrenadores, es_mayor);
	return list_get(distancia_de_entrenadores, 0);
}

t_entrenador *entrenador_mas_cerca(int posicion_pokemon_x,int posicion_pokemon_y){ //verifica que no tenga un objetivo
	int distancia(t_entrenador* entrenador){
		return (abs(entrenador->posicion_x - posicion_pokemon_x) + abs(entrenador->posicion_y - posicion_pokemon_y));}

	int dist_menor = distancia_menor(posicion_pokemon_x,posicion_pokemon_y);

	bool es_igual_distancia(t_entrenador* entrenador){
		int distancia1 = distancia(entrenador);
		return ((dist_menor == distancia1) && (entrenador->objetivo_temporal == NULL));}

	t_list* entrenadores_mas_cerca = list_filter(entrenadores, es_igual_distancia);
	return list_get(entrenadores_mas_cerca, 0);	//saco al primero por defecto si hay 2 a la misma distancia
}

void entrenador_mas_cerca_a_lista_corto_plazo(t_pokemon* pokemon){
	t_entrenador *entrenador_elegido;
	entrenador_elegido = entrenador_mas_cerca(pokemon->pos_x, pokemon->pos_y);
	//entrenador_elegido->objetivo_temporal=malloc(sizeof(t_pokemon));
	entrenador_elegido->objetivo_temporal=pokemon;
    list_add(lista_corto_plazo, entrenador_elegido);
    sem_post(&hay_entrenador_corto_plazo);
}

//--------------------------------------FUNCIONES GENERALES ----------------------------------------------

bool cumplio_cantidad_global(){
     return cantidad_pokemons_globales == cantidad_objetivos_globales;
}

int cantidad_necesitada(){
	return cantidad_objetivos_globales - cantidad_pokemons_globales;
}

//-------------------------------------- ---PLANIFICAR----------------------------------------------------

void planificar(){
	t_pokemon *pokemon = list_remove(pokemons_recibidos,0);
	entrenador_mas_cerca_a_lista_corto_plazo(pokemon);
}

//------------------------------------------ALGORITMOS-------------------------------------------------------

void retirar_entrenador(t_entrenador *entrenador){
	    free(entrenador->objetivo_temporal->nombre);
		free(entrenador->objetivo_temporal);
		entrenador->objetivo_temporal = NULL;
		puts("");
		puts("------------------------------------------------------------");
		puts("Llego a su objetivo y su nueva posicion es: ");
		mostrar_entrenador(entrenador);
		puts("------------------------------------------------------------");
		list_remove(lista_corto_plazo,0);
		//getchar();
}

void planificacionFIFO(){
	while(!cumplio_cantidad_global()){
	sem_wait(&hay_entrenador_corto_plazo);
	puts("                               SACO A UN ENTRENADOR");
	t_entrenador *entrenador = list_get(lista_corto_plazo,0);
	puts("------------------------------------------------------------------------------------------");
	mostrar_entrenador(entrenador);
	puts("------------------------------------------------------------------------------------------");
	while(!llego_al_objetivo(entrenador)){
	sem_post(&(ejecutar_entrenador[entrenador->id]));
    sem_wait(&activar_algoritmo);
	}
	retirar_entrenador(entrenador);}
	puts("CUMPLIO EL TEAM SU OBJETIVO, IUPIIIII");}

void planificacionRR(){
	int quantum = config_get_int_value(config,"QUANTUM");
	int cantidad = 0;
	while(!cumplio_cantidad_global()){
	sem_wait(&hay_entrenador_corto_plazo);
	puts("                               SACO A UN ENTRENADOR");
	t_entrenador *entrenador = list_get(lista_corto_plazo,0);
	puts("------------------------------------------------------------------------------------------");
	mostrar_entrenador(entrenador);
	puts("------------------------------------------------------------------------------------------");
	while(!llego_al_objetivo(entrenador) && (cantidad < quantum)){
    sem_post(&(ejecutar_entrenador[entrenador->id]));
	sem_wait(&activar_algoritmo);
	cantidad++;
    }
	cantidad = 0;
	if(llego_al_objetivo(entrenador))
	    retirar_entrenador(entrenador);
	else{
		list_remove(lista_corto_plazo,0);
	    list_add(lista_corto_plazo,entrenador);
	    sem_post(&hay_entrenador_corto_plazo);}}
	puts("CUMPLIO EL TEAM SU OBJETIVO, IUPIIIII");}

//---
bool llego_uno_nuevo(){
	return list_size(lista_corto_plazo) != largo_lista_conocida;
}

int distancia(t_entrenador* entrenador){
int x = (abs(entrenador->posicion_x - entrenador->objetivo_temporal->pos_x) + abs(entrenador->posicion_y - entrenador->objetivo_temporal->pos_y));
return x;}

void ordenar_lista(){
	bool es_mayor(void* entrenador1,void* entrenador2){
		return (distancia((t_entrenador *)entrenador1) <= distancia((t_entrenador *)entrenador2));}
    list_sort(lista_corto_plazo, es_mayor);
}
//---

void planificacionSJF_CD(){
	t_entrenador *entrenador2;
	t_entrenador *entrenador;
	bool sin_cambio = true;
	while(!cumplio_cantidad_global()){
		sem_wait(&hay_entrenador_corto_plazo);
		ordenar_lista();
		largo_lista_conocida = list_size(lista_corto_plazo);
		sin_cambio = true;
		puts("                               PLANIFICO A UN ENTRENADOR");
		entrenador = list_get(lista_corto_plazo,0);
		puts("------------------------------------------------------------------------------------------");
		mostrar_entrenador(entrenador);
		puts("------------------------------------------------------------------------------------------");
		while(!llego_al_objetivo(entrenador)){
			sem_post(&(ejecutar_entrenador[entrenador->id]));
			sem_wait(&activar_algoritmo);
			if(llego_uno_nuevo()){ //variable global tamaño_list para ver si cambio la lista
				if(llego_al_objetivo(entrenador)){
					retirar_entrenador(entrenador);
					break;
				}
				puts("                               CAMBIO A UN ENTRENADOR");
				puts("------------------------------------------------------------------------------------------");
				mostrar_entrenador(entrenador);
				puts("------------------------------------------------------------------------------------------");
				ordenar_lista();
				entrenador2 = list_get(lista_corto_plazo,0);
				puts("                               POR ESTE ENTRENADOR");
				puts("------------------------------------------------------------------------------------------");
				mostrar_entrenador(entrenador2);
				puts("------------------------------------------------------------------------------------------");
				sin_cambio = false;
				sem_post(&hay_entrenador_corto_plazo);
				break;
			}
        }
		if(llego_al_objetivo(entrenador) && sin_cambio)
			retirar_entrenador(entrenador);}
		puts("CUMPLIO EL TEAM SU OBJETIVO, IUPIIIII");}

void planificacionSJF_SD(){
	while(!cumplio_cantidad_global()){
	sem_wait(&hay_entrenador_corto_plazo);
	ordenar_lista();
	puts("                               SACO A UN ENTRENADOR");
	t_entrenador *entrenador = list_get(lista_corto_plazo,0);
	puts("------------------------------------------------------------------------------------------");
	mostrar_entrenador(entrenador);
	puts("------------------------------------------------------------------------------------------");
	while(!llego_al_objetivo(entrenador)){
		sem_post(&(ejecutar_entrenador[entrenador->id]));
		sem_wait(&activar_algoritmo);
	}
	retirar_entrenador(entrenador);}
	puts("CUMPLIO EL TEAM SU OBJETIVO, IUPIIIII");}

int convertir(char* algoritmo_nombre){
	if (!strcmp(algoritmo_nombre,"FIFO")) return FIFO;
	if (!strcmp(algoritmo_nombre,"RR")) return RR;
	if (!strcmp(algoritmo_nombre,"SJF_SD")) return SJF_SD;
	if (!strcmp(algoritmo_nombre,"SJF_CD")) return SJF_CD;
	return -1;
}

void leer_algoritmo(){
	char* algoritmo_nombre = config_get_string_value(config,"ALGORITMO_PLANIFICACION");
	int algoritmo = convertir(algoritmo_nombre);
	pthread_t hilo_planificacion;
    switch(algoritmo){
    	case FIFO:
    		{pthread_create(&hilo_planificacion, NULL, (void*)planificacionFIFO, NULL); break;}
    	case RR:
    		{pthread_create(&hilo_planificacion, NULL, (void*)planificacionRR, NULL); break;}
    	case SJF_CD:
    	    {pthread_create(&hilo_planificacion, NULL, (void*)planificacionSJF_CD, NULL); break;}
    	case SJF_SD:
    		{pthread_create(&hilo_planificacion, NULL, (void*)planificacionSJF_SD, NULL); break;}
    	default:
    	    {puts("--------------------------------ERROR EN ALGORTIMO DE PLANIFICACION-----------------------------\n");
    	    break;}
    }
    pthread_detach(hilo_planificacion);
}

//------------------------------------------------------------

void agregar_a_diccionario(t_dictionary* pokemones_deseados,char* un_pokemon){
	int count;
	if(dictionary_has_key(pokemones_deseados,un_pokemon)){
		count = (int) dictionary_remove(pokemones_deseados,un_pokemon);
		count++;
	}
	else
		count = 1;
	dictionary_put(pokemones_deseados,un_pokemon,(void*)count);
}

void remover_de_diccionario(t_dictionary* pokemones_deseados,char* un_pokemon){
	int count;
	if(dictionary_has_key(pokemones_deseados,un_pokemon)){
		count = (int) dictionary_remove(pokemones_deseados,un_pokemon);
		if(count > 1){
			count--;
			dictionary_put(pokemones_deseados,un_pokemon,(void*)count);
		}
		return;
	}
	printf("ERROR, se intenta sacar del diccionario a un pokemon que no estaba\n");
}


//-------------------------------------------POKEMONES PRUEBA------------------------------------------------

void probar_pokemones(){
    t_pokemon* pokemon = malloc(sizeof(t_pokemon));
    pokemon->nombre=malloc(sizeof(char) * 15);
    strcpy(pokemon->nombre,"Pikachu");
    pokemon->pos_x=-1;
    pokemon->pos_y=2;
    list_add(pokemons_recibidos,pokemon);
    sem_post(&hay_pokemones);

    t_pokemon* pokemon2 = malloc(sizeof(t_pokemon));
    pokemon2->nombre=malloc(sizeof(char) * 15);
    strcpy(pokemon2->nombre,"Bulbasaur");
    pokemon2->pos_x=9;
    pokemon2->pos_y=5;
    list_add(pokemons_recibidos,pokemon2);
    sem_post(&hay_pokemones);
}

void probar_pokemon_SJF_CD(){
    sleep(5);
    t_pokemon* pokemon1 = malloc(sizeof(t_pokemon));
    pokemon1->nombre=malloc(sizeof(char) * 15);
    strcpy(pokemon1->nombre,"Squirtle");
    pokemon1->pos_x=4;
    pokemon1->pos_y=7;
    list_add(pokemons_recibidos,pokemon1);
    sem_post(&hay_pokemones);
}


int cantidad_entrenadores_disponibles(){
	int cantidad=0;
	t_entrenador *entrenador;

	for(int i=0;i<list_size(entrenadores);i++){
		entrenador = list_get(entrenadores,i);
		if(entrenador->objetivo_temporal == NULL)
			cantidad++;
	}
	return cantidad;
}


//------------------------------------------------------


int distancia_pokemon_entrenador(t_pokemon *pokemon,t_entrenador *entrenador){
	return (abs(entrenador->posicion_x - pokemon->pos_x) + abs(entrenador->posicion_y - pokemon->pos_y));
}

int encontrar_distancia_minima(t_pokemon *pokemon){
	int distancia = 10000;
	t_entrenador *entrenador_aux;
	for(int i=0;i<list_size(entrenadores);i++){
		entrenador_aux = list_get(entrenadores,i);
		if(!entrenador_aux->objetivo_temporal){
			if(distancia > distancia_pokemon_entrenador(pokemon,entrenador_aux))
				distancia = distancia_pokemon_entrenador(pokemon,entrenador_aux);
		}
	}
	return distancia;
}

t_list* ordenar_lista_new(){  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!11

	//return list_duplicate(list_pok_new);


	t_list *pokemones_aux = list_duplicate(list_pok_new);

	bool distancia_mas_cercana(void* pokemon1,void* pokemon2){
	    return encontrar_distancia_minima((t_pokemon*)pokemon1) < encontrar_distancia_minima((t_pokemon*)pokemon2);//ojo los signos
	}

	list_sort(pokemones_aux, distancia_mas_cercana);
	return pokemones_aux;
}



t_pokemon* remover_pokemon(t_list* una_lista, t_pokemon* un_pokemon){
	t_pokemon* aux;

	bool es_el_mismo_pokemon(t_pokemon* uno, t_pokemon* otro){
		if(!strcmp(uno->nombre,otro->nombre))
			if(uno->pos_x == otro->pos_x)
				if(uno->pos_y == otro->pos_y)
					return true;
		return false;
	}

	for (int i=0 ; i<list_size(una_lista) ; i++){
		if(es_el_mismo_pokemon(list_get(una_lista,i),un_pokemon)){
			aux = list_remove(una_lista,i);
			return aux;
		}
	}
	return NULL;
}


void mover_a_ready(t_pokemon* un_pokemon){
	t_pokemon* aux = remover_pokemon(list_pok_new, un_pokemon);
	list_add(list_pok_ready,aux);
	agregar_a_diccionario(dic_pok_ready_o_exec, un_pokemon->nombre);
}


bool me_sirve(t_pokemon* un_pokemon){
	//Para que un pokemon me sirva, la cantidad objetivo > (cantidad exec + cantidad ready)
	if(dictionary_has_key(dic_pok_obj,un_pokemon->nombre)){
		if(dictionary_has_key(dic_pok_ready_o_exec,un_pokemon->nombre))
			return ((int) dictionary_get(dic_pok_ready_o_exec,un_pokemon->nombre) < (int) dictionary_get(dic_pok_obj,un_pokemon->nombre));
		return true;
	}
	return false;
}

bool ya_no_me_sirve(t_pokemon* un_pokemon){
	//Para que un pokemon ya no me sirva, no debe seguir existiendo en el diccionario de pokemones objetivo
	return !dictionary_has_key(dic_pok_obj,un_pokemon->nombre);
}

bool me_puede_servir(t_pokemon* un_pokemon){
	//Para que un pokemon me pueda servir, la cantidad obj debe ser igual a la cantidad en exec+ready;
	if(dictionary_has_key(dic_pok_obj,un_pokemon->nombre)){
		if(dictionary_has_key(dic_pok_ready_o_exec,un_pokemon->nombre))
			return ((int) dictionary_get(dic_pok_ready_o_exec,un_pokemon->nombre) == (int) dictionary_get(dic_pok_obj,un_pokemon->nombre));
		return true;
	}
	return false;
}


void pasar_a_ready_al_pokemon_adecuado(t_list* pokemons, int interacion){
//	printf("Se ejecuta pasar_a_ready_al_pokemon_adecuado iteracion %d\n",interacion);

	if (interacion == list_size(pokemons) || interacion < 0)
		return;

	t_pokemon* aux = list_get(pokemons,interacion);

	if (me_sirve(aux)){
//		printf("entra a me_sirve con el pokemon %s\n",aux->nombre);
		mover_a_ready(aux); //hace signal de pokemones en ready y lo agrega a la t_list pokemones_ready
		return;
	}

	if (ya_no_me_sirve(aux)){
//		printf("entra a ya_no_me_sirve con %s\n",aux->nombre);
		pasar_a_ready_al_pokemon_adecuado(pokemons,interacion+1);
		free(remover_pokemon(list_pok_new, aux));
		return;
	}

	if(me_puede_servir(aux)){
//		printf("entra a me_puede_servir con %s\n", aux->nombre);
		pasar_a_ready_al_pokemon_adecuado(pokemons,interacion+1);
	}

	return;

}


void exec_algoritmo_largo_plazo(){ //llamar con 0
	t_list* pokemon_aux = ordenar_lista_new(); //pokemones_ordenados() devuelve una lista de los pokemones ordenados por conveniencia en relacion a los entrenadores disponibles

	printf("lista_ordenada: \n");
	debug_leer_lista(pokemon_aux);

	pasar_a_ready_al_pokemon_adecuado(pokemon_aux,0);
	return;

}

/*
void planificador_largo_plazo(){
	while(1){
		wait(llega_un_pokemon_a_cola_new);
		wait(hayentrenadorlibre);
		exec_algoritmo_largo_plazo;
	}
}
*/







//-------------------------------------------PLANIFICADOR------------------------------------------------

void planificador(){
	leer_algoritmo();
	//probar_pokemones();

	while(cantidad_necesitada()){ //cada entrenador tiene sus *cantidades* deseadas (no hace falta q coincidan)!
		sem_wait(&hay_pokemones);
		planificar();
	}
}

//------------------------------------- FIN DE PLANIFICACION ---------------------------------------------------

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


//-------------------------------------------------------------





t_dictionary* obtener_pokemones_objetivo(){

	t_dictionary* pokemones_deseados = dictionary_create();
	t_list* pokemones_ya_obtenidos = list_create();
	t_list* pokemones_objetivo = list_create();

	t_entrenador* aux;

	for (int i=0;i<list_size(entrenadores);i++){
		aux=list_get(entrenadores,i);
		list_add_all(pokemones_ya_obtenidos,aux->pokemones);
		list_add_all(pokemones_objetivo,aux->objetivos);
	}

	for (int i=0;i<list_size(pokemones_objetivo);i++)
		agregar_a_diccionario(pokemones_deseados,list_get(pokemones_objetivo,i));

	for (int i=0;i<list_size(pokemones_ya_obtenidos);i++)
		remover_de_diccionario(pokemones_deseados,list_get(pokemones_ya_obtenidos,i));

	list_destroy(pokemones_objetivo);
	list_destroy(pokemones_ya_obtenidos);

	return pokemones_deseados;

}

//----------------------------------------------






void enviar_mensajes_get(int socket_a_enviar){

	void enviar_un_mensaje_get(char* nombre_pokemon, void* data){
		int socket_broker = connect_to(ip_broker,puerto_broker,wait_time);
		t_mensaje* mensaje = crear_mensaje(2,GET_POKEMON,nombre_pokemon);
		enviar_mensaje(socket_broker,mensaje);
		int id_correlativo = wait_ack(socket_broker);
		dictionary_put(ids_a_esperar,string_itoa(id_correlativo),(void*)GET_POKEMON);

		close(socket_broker);
	}

	dictionary_iterator(dic_pok_obj,enviar_un_mensaje_get);
}


t_mensaje* recibir_mensaje(int* socket_broker){
	uint32_t codigo_operacion=CODIGO_OPERACION_DEFAULT;


	if(recv(*socket_broker, &(codigo_operacion),sizeof(uint32_t), 0)==-1){
		perror("Falla recv() op_code");
		return 0;
	}

	printf("op_code:%d\n", codigo_operacion);

	if(codigo_operacion==CODIGO_OPERACION_DEFAULT){
		return 0;
	}
	uint32_t id;

	if(recv(*socket_broker, &(id), sizeof(uint32_t), 0) == -1){
		perror("Falla recv() id");
		return 0;
	}

	printf("id:%d\n", id);

	uint32_t size_contenido_mensaje;

	if(recv(*socket_broker, &(size_contenido_mensaje), sizeof(uint32_t), 0) == -1){
		perror("Falla recv() size_contenido_mensaje");
		return 0;
	}

	printf("size contenido:%d\n", size_contenido_mensaje);


	void* stream = malloc(size_contenido_mensaje);

	if(recv(*socket_broker, stream, size_contenido_mensaje, 0) == -1){
		perror("Falla recv() contenido");
		return 0;
	}

	t_mensaje* mensaje = deserializar_mensaje(codigo_operacion, stream);
	mensaje->id=id;
	return mensaje;
}

int recibir_appeared(int* socket_broker){


	t_mensaje* mensaje = recibir_mensaje(socket_broker);

	//ya tendriamos el appeared
	// hace ACK del mensaje recibido

	send_ack(*socket_broker,ACK);
	printf("envio ACK\n");

	//printear_mensaje(mensaje);
	printf("Se recibio el mensaje\n");

	agregar_pokemon_recibido(mensaje->contenido.appeared_pokemon.pokemon);
	return 1;

}

int recibir_localized(int* socket_broker){

	t_mensaje* mensaje = recibir_mensaje(socket_broker);

	//ya tendriamos el localized

	send_ack(*socket_broker,ACK);
	printf("envio ACK\n");

	printear_mensaje(mensaje);
	printf("Se recibio el mensaje\n");

	//MANEJAR MENSAJE LOCALIZED


	return 1;

}

int recibir_caught(int* socket_broker){

	t_mensaje* mensaje = recibir_mensaje(socket_broker);

	//ya tendriamos el appeared

	send_ack(*socket_broker,ACK);
	printf("envio ACK\n");

	//printear_mensaje(mensaje);
	printf("Se recibio el mensaje\n");

	//MANEJAR MENSAJE CAUGHT


	return 1;

}



void protocolo_recibir_mensaje(cola_code cola){
	pthread_mutex_lock(&mutex_recibir);
	int socket_cola = subscribirse_a_cola(cola);
	pthread_mutex_unlock(&mutex_recibir);
	printf("socket_suscripcion:%d\n",socket_cola);
	switch(cola){
		case COLA_APPEARED_POKEMON:;
			puts("appeared");
			while(recibir_appeared(&socket_cola));
			protocolo_recibir_mensaje(cola);
			break;
		case COLA_LOCALIZED_POKEMON:;
			puts("localized");
			while(recibir_localized(&socket_cola));
			protocolo_recibir_mensaje(cola);
			break;
		case COLA_CAUGHT_POKEMON:;
			puts("caught");
			while(recibir_caught(&socket_cola));
			protocolo_recibir_mensaje(cola);
			break;
		default:
			break;
	}

}

void crear_hilos_planificar_recursos(){
	pthread_t hilo_planificador;
	pthread_t hilo_deadlock;
	pthread_create(&hilo_planificador, NULL, (void*)planificador, NULL); //OJOOOOO
	pthread_create(&hilo_deadlock, NULL, (void*)deadlock, NULL); //OJOOOOO
}

void inicializar_team(){

	//----------------planificacion

    crear_listas_globales();
    pthread_mutex_init(&mutex_pokemones_recibidos, NULL);
    pthread_mutex_init(&mutex_recibir, NULL);

    //------------------

	logger = log_create("team.log","log",1,LOG_LEVEL_DEBUG);
	config = config_create("../config");
	retardo = config_get_int_value(config,"RETARDO_CICLO_CPU");
	obtener_entrenadores();

	//----------------planificacion

	inicializar_semaforo_entrenadores();
    crear_hilos_entrenadores();
    crear_hilos_planificar_recursos();

    //--------------------------

	//Obtiene los datos IP,PUERTO WAIT_TIME desde la config
    dic_pok_obj = obtener_pokemones_objetivo();



	ip_broker = config_get_string_value(config,"IP_BROKER");
	log_debug(logger,config_get_string_value(config,"IP_BROKER")); //pido y logueo ip
	puerto_broker = config_get_string_value(config,"PUERTO_BROKER");
	log_debug(logger,config_get_string_value(config,"PUERTO_BROKER")); //pido y logueo puerto
	wait_time = config_get_int_value(config,"TIEMPO_RECONEXION");

	//estas subscripciones se harian en los threads de abajo
	//socket_cola_appeared = subscribirse_a_cola(COLA_APPEARED_POKEMON);
	//int socket_cola_localized = subscribirse_a_cola(COLA_LOCALIZED_POKEMON);
	//int socket_cola_caught = subscribirse_a_cola(COLA_CAUGHT_POKEMON);

	//------Se crean 3 threads para escuchar las notificaciones del broker-----

	pthread_t recibir_cola_appeared;
	pthread_t recibir_cola_caught;
	pthread_t recibir_cola_localized;


	pthread_create(&recibir_cola_appeared, NULL, (void*)protocolo_recibir_mensaje,(void*) COLA_APPEARED_POKEMON);
	pthread_create(&recibir_cola_caught, NULL, (void*)protocolo_recibir_mensaje,(void*) COLA_CAUGHT_POKEMON);
	pthread_create(&recibir_cola_localized, NULL, (void*)protocolo_recibir_mensaje, (void*)COLA_LOCALIZED_POKEMON);
	puts("crear hilo");
	pthread_detach(recibir_cola_appeared);
	pthread_detach(recibir_cola_caught);
	pthread_detach(recibir_cola_localized);
}


int main(void) {

	//inicializar_team();

	//sem_wait(&cumplio_objetivo_global);
    //liberar_recursos();


	//-----------------DEBUG DE DICCIONARIOS-----------------
	//-----------BORRAR DESDE ACA------------------


    crear_listas_globales();
    list_pok_new = list_create();
    list_pok_ready = list_create();

    dic_pok_ready_o_exec = dictionary_create();
    ids_a_esperar = dictionary_create();


	config = config_create("../config");
	obtener_entrenadores();


	dic_pok_obj = obtener_pokemones_objetivo();
	debug_dic(dic_pok_obj);

	list_add(list_pok_new,crear_pokemon("Pikachu",1,2));
	list_add(list_pok_new,crear_pokemon("PikaGSSDFchu",30,54));
	list_add(list_pok_new,crear_pokemon("CACA",5,7));
	list_add(list_pok_new,crear_pokemon("CACA",1,5));
	list_add(list_pok_new,crear_pokemon("Charmander",14,9));
	list_add(list_pok_new,crear_pokemon("CACA",1,2));
	list_add(list_pok_new,crear_pokemon("CACA",30,54));
	list_add(list_pok_new,crear_pokemon("CACA",5,7));
	list_add(list_pok_new,crear_pokemon("CACA",1,5));
	list_add(list_pok_new,crear_pokemon("Pikachu",14,9));

	agregar_a_diccionario(dic_pok_ready_o_exec,"Pikachu");

	printf("Lista de new antes del algoritmo:\n");
	debug_leer_lista(list_pok_new);

	for(int i=0;i<2;i++)
		exec_algoritmo_largo_plazo();

	printf("Lista de new:\n");
	debug_leer_lista(list_pok_new);

	printf("Lista de ready:\n");
	debug_leer_lista(list_pok_ready);

	//------------HASTA ACA---------------




	return EXIT_SUCCESS;
}
