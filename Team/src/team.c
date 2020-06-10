/*
TODO
agregar logs:
* cambio de un entrenador de cola de planifacacion (indicando razon)
* resultado del team (especificado anteriormente)
*/

#include "team.h"

//------------------------------------------ FUNCIONES ṔARA MOSTRAR

void debug_dic(t_dictionary* pokemones_deseados){

	void leer(char* key, void* count){
	//	enviar_get(key);
		printf("Nombre: %s    Cantidad:%d\n",key,(int)count);
	}

	printf("Lectura diccionario:\n");
	dictionary_iterator(pokemones_deseados,(void*)leer);
	printf("-------------------------------------------\n");
}

void debug_leer_lista(t_list* lista){
	printf("Comienza lectura de lista:\n");
	t_pokemon* aux;
	for (int i=0;i<list_size(lista);i++){
		aux = list_get(lista,i);
		printf("Pokemon %s, posx %d, posy %d\n",aux->nombre,aux->pos_x,aux->pos_y);
	}
	printf("--------------------------------------\n");
}

void leer_lista_entrenadores(t_list* lista){
	printf("*             Comienza lectura de lista de entrenadores:               *\n");
	t_entrenador* aux;
	for (int i=0;i<list_size(lista);i++){
		aux = list_get(lista,i);
		printf("Entrenador con ID: %d, bloq_exec: %d, posx: %d, posy: %d, exit: %d\n",aux->id,aux->bloq_exec,aux->posicion_x,aux->posicion_y,aux->exit);
	    puts("-Sus objetivos: ");
		for (int i=0;i<list_size(aux->objetivos);i++){
				printf("Pokemon numero: %d %s\n",i+1,(char *) list_get(aux->objetivos,i));
			}
		printf("\n");
		puts("+Sus pokemones: ");
	    for (int i=0;i<list_size(aux->pokemones);i++){
			printf("Pokemon numero: %d %s\n",i+1,(char *) list_get(aux->pokemones,i));
			}
		printf("--------------------------------------\n");
	}
	printf("--------------------------------------\n");
}

void mostrar_entrenador(t_entrenador *t){
	printf("EN x :%d ",t->posicion_x);
	printf("y En y :%d ",t->posicion_y);
	printf("| Entrenador id :%d, bloq_exec :%d, exit:%d\n",t->id,t->bloq_exec,t->exit);
}

void mostrar_pokemon(t_pokemon *pokemon){
	printf("nombre:%s\nposx:%d\npos y:%d\n",pokemon->nombre,pokemon->pos_x,pokemon->pos_y);
}
//--------------------------------------------- FUNCIONES DICCIONARIO

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
	//printf("ERROR, se intenta sacar del diccionario a un pokemon que no estaba\n"); SINO ME LO TIRA EL CUMPLIO OBJETIVO
}



//---------------------------------------PLANIFICACION---------------------------------------------------------------------

void inicializar_semaforos(){
	ejecutar_entrenador = malloc(sizeof(sem_t)*list_size(entrenadores));
	for (int i=0;i<list_size(entrenadores);i++){
		sem_init(&(ejecutar_entrenador[i]), 0,0);
	}
	deadlock_entrenadores = malloc(sizeof(sem_t)*list_size(entrenadores));
	for(int i=0;i<list_size(entrenadores);i++){
		sem_init(&(deadlock_entrenadores[i]), 0,0);
	}
	sem_init(&activar_algoritmo, 0,0);
	sem_init(&cumplio_objetivo_global, 0,0);
	sem_init(&hay_pokemones, 0, 0);
	sem_init(&hay_entrenador_corto_plazo,0,0);
    sem_init(&revisar_pokemones_new,0,0);
    sem_init (&sem_deadlock,0,0);
	sem_init(&hayentrenadorlibre, 0,list_size(entrenadores));
    pthread_mutex_init(&mutex_pokemones_recibidos, NULL);
    pthread_mutex_init(&mutex_recibir, NULL);
    pthread_mutex_init(&list_pok_new_mutex, NULL);
    pthread_mutex_init(&list_pok_ready_mutex, NULL);

    pthread_mutex_init(&mutexPRUEBA, NULL);

	espera_caught = malloc(sizeof(sem_t)*list_size(entrenadores));
	for (int i=0;i<list_size(entrenadores);i++){
		sem_init(&(espera_caught[i]), 0,0);
	}

}

void inicializar_estructuras_globales(){
	entrenadores = list_create();
	lista_corto_plazo=list_create();
    list_pok_new = list_create();
    list_pok_ready = list_create();
    ids_a_esperar = dictionary_create();
    dic_pok_obj = dictionary_create();
    dic_pok_ready_o_exec = dictionary_create();

	respuesta_caught = malloc(sizeof(uint32_t)*list_size(entrenadores));
	for (int i=0;i<list_size(entrenadores);i++){
		respuesta_caught[i] = 0;
	}
}

//--------------------------------------LIBERAR RECURSOS---------------------------------------------

void liberar_pokemon(t_pokemon *pokemon){
	free(pokemon->nombre);
	free(pokemon);
}

void liberar_entrenador(t_entrenador *entrenador){
	list_destroy(entrenador->pokemones);
	list_destroy(entrenador->objetivos);
	free(entrenador);
}

void liberar_recursos_globales(){
    puts("1");
	dictionary_destroy(dic_pok_obj);
	puts("2");
	dictionary_destroy(dic_pok_ready_o_exec);
	puts("3");
	dictionary_destroy(ids_a_esperar);
	puts("4");
	free(ejecutar_entrenador);
	puts("5");
	free(deadlock_entrenadores);
	puts("6");
	free(hilo_entrenador);
	puts("7");
	list_destroy_and_destroy_elements(list_pok_new,(void *)liberar_pokemon);
	puts("8");
	list_destroy_and_destroy_elements(list_pok_ready,(void *)liberar_pokemon);
	puts("9");
	list_destroy_and_destroy_elements(entrenadores,(void *)liberar_entrenador);
	puts("10");
	list_destroy_and_destroy_elements(lista_corto_plazo,(void *)liberar_entrenador);
}

//--------------------------------------------------- DEADLOCK ---------------------------------------

void remover_de_lista_si_esta(t_list* lista_de_pokemones, char* un_pokemon){
	//lista_de_pokemones lista de char* !!!!!!!
	for(int z = 0; z < list_size(lista_de_pokemones) ; z++){;
		if(!strcmp((char*)list_get(lista_de_pokemones,z) , un_pokemon)){
			list_remove(lista_de_pokemones,z);
			break;
		}
	}
}

t_list* pokemones_no_necesarios(t_entrenador* un_entrenador){
	t_list* pok = list_duplicate(un_entrenador->pokemones);
	for(int i = 0; i < list_size(un_entrenador -> objetivos) ; i++)
		remover_de_lista_si_esta(pok , list_get(un_entrenador-> objetivos,i));
	return pok;
}

t_list* pokemones_deseados(t_entrenador* un_entrenador){
	t_list* pok = list_duplicate(un_entrenador->objetivos);
	for(int i = 0; i < list_size(un_entrenador -> pokemones) ; i++)
		remover_de_lista_si_esta(pok , list_get(un_entrenador-> pokemones,i));
	return pok;
}

t_list* intersectar_listas(t_list* una, t_list* otra){
	t_list* una_lista = list_duplicate(una);
	t_list* otra_lista = list_duplicate(otra);

	t_list* lista_auxiliar = list_create();

	dale_dejame_usarlo_porfis:
	for (int i=0;i<list_size(una_lista);i++){
			for(int j=0;j<list_size(otra_lista);j++){
				if(!strcmp(list_get(una_lista,i),list_get(otra_lista,j))){
					list_add(lista_auxiliar,list_remove(una_lista,i));
					list_remove(otra_lista,j);
					goto dale_dejame_usarlo_porfis;
				}
			}
	}

	list_destroy(una_lista);
	list_destroy(otra_lista);
	return lista_auxiliar;

}



void intercambiar_pokemones(t_entrenador* un_entrenador, t_entrenador* otro_entrenador){
	t_list* pok_des = pokemones_deseados(un_entrenador);
	t_list* pok_dis = pokemones_no_necesarios(otro_entrenador);

	t_list* pok_devolucion = pokemones_no_necesarios(un_entrenador);

	t_list* trade = intersectar_listas(pok_dis,pok_des);

	while(list_size(trade)){

		log_debug(logger,"Operacion de intercambio en x:%d y:%d, entre los entrenadores id:%d y id:%d",un_entrenador->posicion_x,un_entrenador->posicion_y,un_entrenador->id,otro_entrenador->id);
		remover_de_lista_si_esta(otro_entrenador->pokemones,list_get(trade,0));
		list_add(un_entrenador->pokemones,list_get(trade,0));

		remover_de_lista_si_esta(un_entrenador->pokemones,list_get(pok_devolucion,0));
		list_add(otro_entrenador->pokemones,list_get(pok_devolucion,0));

		printf("Espera de 5 ciclos para realizar el intercambio...\n");
		sleep(retardo*5);
		printf("Se realizo el intercambio!\n");

		remover_de_lista_si_esta(pok_des,list_get(trade,0));
		remover_de_lista_si_esta(pok_dis,list_get(pok_devolucion,0));

		list_remove(pok_devolucion,0);

		list_destroy(trade);

		trade = intersectar_listas(pok_dis,pok_des);

		sem_post(&deadlock_entrenadores[un_entrenador->id]);
		sem_post(&deadlock_entrenadores[otro_entrenador->id]);

	}

	list_destroy(trade);
	list_destroy(pok_des);
	list_destroy(pok_dis);
	list_destroy(pok_devolucion);
}

void deadlock(){

	log_debug(logger,"Inicio del algoritmo de deteccion de deadlock");

	t_entrenador* aux;
	t_entrenador* aux1;
	t_pokemon *pok_mentira = malloc(sizeof(t_pokemon));


	t_list* pokemones_que_quiere_aux;

	t_list *entrenadores_bloqueados = list_create();
	for (int i=0;i<list_size(entrenadores);i++){
		aux = list_get(entrenadores,i);
		if(aux->bloq_exec && aux->exit==0)
			list_add(entrenadores_bloqueados,aux);
	}

	printf("Lista de entrenadores bloquedos:\n");
	leer_lista_entrenadores(entrenadores_bloqueados);


	bool puedeIntercambiarConAux(void* unEntrenador){
		bool auxLoQuiere(void* un_pokemon){
			//fijarse si un_pokemon esta dentro de la lista pokemones_que_quiere_aux
			for (int i=0 ; i<list_size(pokemones_que_quiere_aux) ; i++)
				if(!strcmp(((char*)un_pokemon) , (char*)list_get(pokemones_que_quiere_aux,i)))
					return true;

			return false;
		}


		t_list* pokemones_otorgables = pokemones_no_necesarios(unEntrenador);

		bool valor_a_retornar = list_any_satisfy(pokemones_otorgables,auxLoQuiere);

		list_destroy(pokemones_otorgables);

		return valor_a_retornar;
	}



	while(!list_is_empty(entrenadores_bloqueados)){
		aux = list_remove(entrenadores_bloqueados,0);
		pokemones_que_quiere_aux = pokemones_deseados(aux);

		aux1 = list_remove_by_condition(entrenadores_bloqueados,puedeIntercambiarConAux);

		pok_mentira->pos_x = (uint32_t) aux1->posicion_x;
		pok_mentira->pos_y = (uint32_t) aux1->posicion_y;

		aux->objetivo_temporal = pok_mentira;

		sem_post(&deadlock_entrenadores[aux->id]);
		sem_wait(&sem_deadlock);

		intercambiar_pokemones(aux,aux1);

		mostrar_entrenador(aux);
		mostrar_entrenador(aux1);

		if(!aux->exit)
			list_add(entrenadores_bloqueados,aux);

		if(!aux1->exit)
			list_add(entrenadores_bloqueados,aux1);

		list_destroy(pokemones_que_quiere_aux);

		leer_lista_entrenadores(entrenadores);
	}
	free(pok_mentira);
    list_destroy(entrenadores_bloqueados);
	log_debug(logger,"Finaliza el algoritmo de deteccion de deadlock con exito");

}

//---------------------------------------------- ENTRENADOR ----------------------------------------------------

bool cumplio_objetivo_entrenador(int id){ //verifica si las listas de objetivos y pokemones son iguales


	pthread_mutex_lock(&mutexPRUEBA);

	int exito;
	t_dictionary* pokemones_deseados = dictionary_create();
	t_list* pokemones_ya_obtenidos = list_create();
	t_list* pokemones_objetivo = list_create();

	t_entrenador* aux;

	aux=list_get(entrenadores,id);
	list_add_all(pokemones_ya_obtenidos,aux->pokemones);
	list_add_all(pokemones_objetivo,aux->objetivos);

	for (int i=0;i<list_size(pokemones_objetivo);i++)
		agregar_a_diccionario(pokemones_deseados,list_get(pokemones_objetivo,i));

	for (int i=0;i<list_size(pokemones_ya_obtenidos);i++)
		remover_de_diccionario(pokemones_deseados,list_get(pokemones_ya_obtenidos,i));

	list_destroy(pokemones_objetivo);
	list_destroy(pokemones_ya_obtenidos);

	if(dictionary_is_empty(pokemones_deseados))
		exito = true;
	else
		exito = false;
	dictionary_destroy(pokemones_deseados);

	pthread_mutex_unlock(&mutexPRUEBA);
	return exito;
}

bool tiene_cantidad(int id){
	t_entrenador *entrenador = list_get(entrenadores, id);
	return (list_size(entrenador->pokemones) == list_size(entrenador->objetivos));
}

bool llego_al_objetivo(t_entrenador *entrenador){
	return ((entrenador->posicion_x == entrenador->objetivo_temporal->pos_x) && (entrenador->posicion_y == entrenador->objetivo_temporal->pos_y));
}

void enviar_catch(t_entrenador* entrenador){
	t_pokemon* pokemon = entrenador->objetivo_temporal;
	t_mensaje* mensaje = crear_mensaje(4,CATCH_POKEMON,pokemon->nombre,pokemon->pos_x,pokemon->pos_y);

	int socket_broker = connect_to(ip_broker,puerto_broker,wait_time,logger);

	enviar_mensaje(socket_broker,mensaje);

	check_ack(socket_broker,ACK);

	int id_correlativo = wait_ack(socket_broker);

	printf("id_correlativo recibido:%d\n",id_correlativo);

	send_ack(socket_broker,id_correlativo);

	liberar_mensaje(mensaje);//todo chequear id a confirmar correlativo

	agregar_id_esperado(entrenador->id,id_correlativo);
}

bool espera_confirmacion(int id){
	sem_wait(&espera_caught[id]);
	printf("confirmacion:%d\n",respuesta_caught[id]);
	return respuesta_caught[id];
}

void atrapar_pokemon(int id){
	t_entrenador *entrenador = list_get(entrenadores,id);
    char* nombre = malloc(strlen(entrenador->objetivo_temporal->nombre));
	strcpy(nombre,entrenador->objetivo_temporal->nombre);

	enviar_catch(entrenador);
	uint32_t confirmacion = espera_confirmacion(entrenador->id);
	printf("confirmacion:%d\n",confirmacion);

	log_debug(logger,"El entrenador id:%d intenta atapar al pokemon %s en la posicion x:%d y:%d",id,nombre,entrenador->posicion_x,entrenador->posicion_y);
	if(confirmacion){//envia mensaje catch_pokemon(pokemon);
		log_debug(logger,"El entrenador id:%d logro atapar al pokemon %s en la posicion x:%d y:%d",id,nombre,entrenador->posicion_x,entrenador->posicion_y);
		debug_dic(dic_pok_obj);
		remover_de_diccionario(dic_pok_obj,nombre);
		list_add(entrenador->pokemones, nombre);
    }
	else{
		//free
		log_debug(logger,"El entrenador id:%d no logro atapar al pokemon %s en la posicion x:%d y:%d",id,nombre,entrenador->posicion_x,entrenador->posicion_y);
	}

	pthread_mutex_lock(&list_pok_ready_mutex);
	remover_de_diccionario(dic_pok_ready_o_exec,nombre);
	pthread_mutex_unlock(&list_pok_ready_mutex);
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
	t_entrenador *entrenador = list_get(entrenadores, id);
	while(!tiene_cantidad(id)){//con el deadlock tiene q ser q cumpla el objetivo
		sem_wait(&(ejecutar_entrenador[id]));

		sleep(retardo);
		avanzar(id);
		log_debug(logger,"Se mueve el entredor id:%d a las coordenadas x:%d y:%d",entrenador->id,entrenador->posicion_x,entrenador->posicion_y);

		if(llego_al_objetivo(list_get(entrenadores,id))){
			atrapar_pokemon(id);
			//TODO
		  //liberar(); o algo asi para el deadlock si entras en deadlock, solo se ejecuta todo el deadlock hasta terminar el intercambio?
		  //o tambien se puede hacer actuar el planificador? es monoprocesador no?
		  }
		sem_post(&activar_algoritmo);
	}


    entrenador->bloq_exec = 1;

    while(!cumplio_objetivo_entrenador(id)){
    	sem_wait(&deadlock_entrenadores[id]);
    	if(cumplio_objetivo_entrenador(id))
    		break;
    	if(!entrenador->objetivo_temporal)
    		continue;

    	while(!llego_al_objetivo(list_get(entrenadores,id))){
    		sleep(retardo);
    		avanzar(id);
    		log_debug(logger,"DEADLOCK: Se mueve el entredor id:%d a las coordenadas x:%d y:%d",entrenador->id,entrenador->posicion_x,entrenador->posicion_y);
    	}
    	printf("El entrenador llego al objetivo para hacer el intercambio\n");
    	entrenador->objetivo_temporal = NULL;
		sem_post(&sem_deadlock);
    }

    entrenador->exit = 1;


	printf("------ Termino entrenador %d -------\n",id);
	for(int i=0; i < list_size(entrenador->pokemones); i++){
		printf("Pokemon numero: %d %s\n",i+1,(char *) list_get(entrenador->pokemones,i));
	}
	printf("---------------------------------\n");
	free(entrenador); //OJO CON LOS LOGS
}

void crear_hilos_entrenadores(){ //creamos todos los entrenadores, estos se quedan esperando la habilitacion del algoritmo para avanzar
	hilo_entrenador = malloc(sizeof(pthread_t)*list_size(entrenadores));
	for(int i=0; i < list_size(entrenadores);i++){
    	pthread_create(&(hilo_entrenador[i]), NULL, (void*)entrenador,(void *)i); //OJOOOOO
    	pthread_detach(hilo_entrenador[i]);
     }
}

//------------------------------FUNCION LARGA PARA ENCONTRAR EL MAS CERCANO------------------------------

int distancia_menor(int posicion_pokemon_x,int posicion_pokemon_y){

	void* distancia(void *entrenador){
		return (void *)(abs(((t_entrenador*)entrenador)->posicion_x - posicion_pokemon_x) + abs(((t_entrenador*)entrenador)->posicion_y - posicion_pokemon_y));}

	bool es_mayor(void *numero1, void *numero2){
		return ((int)numero2) >= ((int)numero1);}

	bool disponible(void * entrenador){
		//leer_lista_entrenadores(entrenadores);
		return (((t_entrenador *)entrenador)->objetivo_temporal == NULL) && (((t_entrenador*)entrenador)->bloq_exec == 0);
	}

	t_list* distancia_disponibles = list_filter(entrenadores, disponible);
	t_list* distancia_de_entrenadores =  list_map(distancia_disponibles, distancia);
    list_sort(distancia_de_entrenadores, es_mayor);
	int n = (int)list_remove(distancia_de_entrenadores, 0);
	list_destroy(distancia_de_entrenadores);
	list_destroy(distancia_disponibles);
	return n;
}

t_entrenador *entrenador_mas_cerca(int posicion_pokemon_x,int posicion_pokemon_y){ //verifica que no tenga un objetivo
	int distancia(t_entrenador* entrenador){
		return (abs(entrenador->posicion_x - posicion_pokemon_x) + abs(entrenador->posicion_y - posicion_pokemon_y));}

	int dist_menor = distancia_menor(posicion_pokemon_x,posicion_pokemon_y);

	bool es_igual_distancia(void *entrenador){
		//leer_lista_entrenadores(entrenadores);
		int distancia1 = distancia((t_entrenador*)entrenador);
		return ((((t_entrenador*)entrenador)->bloq_exec == 0) && (dist_menor == distancia1) && (((t_entrenador*)entrenador)->objetivo_temporal == NULL));
	}

	t_list* entrenadores_mas_cerca = list_filter(entrenadores, es_igual_distancia);
	t_entrenador *entrenador = list_remove(entrenadores_mas_cerca, 0);	//saco al primero por defecto si hay 2 a la misma distancia
	list_destroy(entrenadores_mas_cerca);
	return entrenador;
}

void entrenador_mas_cerca_a_lista_corto_plazo(t_pokemon* pokemon){
	t_entrenador *entrenador_elegido;
	//leer_lista_entrenadores(entrenadores);
	entrenador_elegido = entrenador_mas_cerca(pokemon->pos_x, pokemon->pos_y);
	//printf("ENTRENADOR POS X: %d POS Y: %d\n", entrenador_elegido->posicion_x,entrenador_elegido->posicion_y);
	entrenador_elegido->objetivo_temporal=pokemon;
    list_add(lista_corto_plazo, entrenador_elegido);
    sem_post(&hay_entrenador_corto_plazo);
}

//------------------------------------------PLANIFICAR----------------------------------------------------

void planificar(){
	pthread_mutex_lock(&list_pok_ready_mutex);
	t_pokemon *pokemon = list_remove(list_pok_ready,0);
	//debug_leer_lista(list_pok_ready);
	pthread_mutex_unlock(&list_pok_ready_mutex);
	//puts("---------------POKEMON ELEGIDO POR EL PLANIFICADOR DE CORTO PLAZO------------------");
	//printf("POKEMON -> POS X: %d POS Y: %d", pokemon->pos_x, pokemon->pos_y);
	//puts("-----------------------------------------------------------------------------------");
	entrenador_mas_cerca_a_lista_corto_plazo(pokemon);
}

//------------------------------------------ALGORITMOS-------------------------------------------------------

void retirar_entrenador(t_entrenador *entrenador){
	    free(entrenador->objetivo_temporal->nombre);
		free(entrenador->objetivo_temporal);
		if(list_size(entrenador->objetivos)>list_size(entrenador->pokemones))
			sem_post(&hayentrenadorlibre);

		entrenador->objetivo_temporal = NULL;
		list_remove(lista_corto_plazo,0);
		puts("LO REMUEVE");
}

void planificacionFIFO(){
	while(dictionary_size(dic_pok_obj) > 0){
		sem_wait(&hay_entrenador_corto_plazo);
		puts("                               SACO A UN ENTRENADOR");
		t_entrenador *entrenador = list_get(lista_corto_plazo,0);
		puts("------------------------------------------------------------------------------------------");
		mostrar_entrenador(entrenador);
		puts("------------------------------------------------------------------------------------------");
		if(llego_al_objetivo(entrenador)){
			sem_post(&(ejecutar_entrenador[entrenador->id]));
			sem_wait(&activar_algoritmo);
		}
		while(!llego_al_objetivo(entrenador)){
			sem_post(&(ejecutar_entrenador[entrenador->id]));
			sem_wait(&activar_algoritmo);
		}
		retirar_entrenador(entrenador);
	}
	sem_post(&cumplio_objetivo_global);
}

void planificacionRR(){
	int quantum = config_get_int_value(config,"QUANTUM");
	int cantidad = 0;
	while(!(dictionary_size(dic_pok_obj) == 0)){
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
}

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
	while(!(dictionary_size(dic_pok_obj) == 0)){
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
}

void planificacionSJF_SD(){
	while(!(dictionary_size(dic_pok_obj) == 0)){
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
}

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
    		pthread_create(&hilo_planificacion, NULL, (void*)planificacionFIFO, NULL);
    		break;
    	case RR:
    		pthread_create(&hilo_planificacion, NULL, (void*)planificacionRR, NULL);
    		break;
    	case SJF_CD:
    	    pthread_create(&hilo_planificacion, NULL, (void*)planificacionSJF_CD, NULL);
    	    break;
    	case SJF_SD:
    		pthread_create(&hilo_planificacion, NULL, (void*)planificacionSJF_SD, NULL);
    		break;
    	default:
    	    puts("--------------------------------ERROR EN ALGORTIMO DE PLANIFICACION-----------------------------\n");
    	    break;
    }
    pthread_detach(hilo_planificacion);
}

//-------------------------------------------CANTIDAD ENTRENADORES DISPONIBLES------------------------------------------------

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

//------------------------------------ PLANIFICADOR LARGO PLAZO

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

t_list* ordenar_lista_new(){
	pthread_mutex_lock(&list_pok_new_mutex);
	t_list *pokemones_aux = list_duplicate(list_pok_new);
	/*puts("POKEMON NEW");
	debug_leer_lista(list_pok_new);
	puts("------------");*/
	pthread_mutex_unlock(&list_pok_new_mutex);

	bool distancia_mas_cercana(void* pokemon1,void* pokemon2){
	    return encontrar_distancia_minima((t_pokemon*)pokemon1) < encontrar_distancia_minima((t_pokemon*)pokemon2);//ojo los signos
	}

	list_sort(pokemones_aux, distancia_mas_cercana);
	return pokemones_aux;
}

bool es_el_mismo_pokemon(t_pokemon* uno, t_pokemon* otro){
	if(!strcmp(uno->nombre,otro->nombre))
		if(uno->pos_x == otro->pos_x)
			if(uno->pos_y == otro->pos_y)
				return true;
	return false;
}

t_pokemon* remover_pokemon(t_list* una_lista, t_pokemon* un_pokemon){
	t_pokemon* aux;

	for (int i=0 ; i<list_size(una_lista) ; i++){
		if(es_el_mismo_pokemon(list_get(una_lista,i),un_pokemon)){
			aux = list_remove(una_lista,i);
			return aux;
		}
	}
	return NULL;
}

void mover_a_ready(t_pokemon* un_pokemon){
	pthread_mutex_lock(&list_pok_new_mutex);
	t_pokemon* aux = remover_pokemon(list_pok_new, un_pokemon);
	pthread_mutex_unlock(&list_pok_new_mutex);
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

	if (interacion == list_size(pokemons) || interacion < 0){
		sem_post(&hayentrenadorlibre);
		return;
	}

	t_pokemon* aux = list_get(pokemons,interacion);

	if (me_sirve(aux)){
		mover_a_ready(aux);
		sem_post(&hay_pokemones);//hace signal de pokemones en ready y lo agrega a la t_list pokemones_ready
		return;
	}

	if (ya_no_me_sirve(aux)){
		pasar_a_ready_al_pokemon_adecuado(pokemons,interacion+1);
		remover_pokemon(list_pok_new, aux);
		liberar_pokemon(aux); //NUEVA
		return;
	}

	if(me_puede_servir(aux)){
		pasar_a_ready_al_pokemon_adecuado(pokemons,interacion+1);
	}
	return;
}


void exec_algoritmo_largo_plazo(){ //llamar con 0
	t_list* pokemon_aux = ordenar_lista_new(); //pokemones_ordenados() devuelve una lista de los pokemones ordenados por conveniencia en relacion a los entrenadores disponibles
	//debug_leer_lista(pokemon_aux);

	pthread_mutex_lock(&list_pok_ready_mutex);
	pasar_a_ready_al_pokemon_adecuado(pokemon_aux,0);
	pthread_mutex_unlock(&list_pok_ready_mutex);
	list_destroy(pokemon_aux);
	return;
}

void planificador_largo_plazo(){
	while(1){
		sem_wait(&revisar_pokemones_new);
		sem_wait(&hayentrenadorlibre);
		exec_algoritmo_largo_plazo();

	}
}

//-------------------------------------------PLANIFICADOR CORTO PLAZO ------------------------------------------------

void planificador(){
	leer_algoritmo();

	while(dictionary_size(dic_pok_obj)){ //cada entrenador tiene sus *cantidades* deseadas (no hace falta q coincidan)!
	    sem_wait(&hay_pokemones);
	    puts("HAY UN NUEVO POKEMON");
	    planificar();
	}
}

//-------------------------------------------HILOS PLANIFICADOR------------------------------------------------

void crear_hilos_planificar_recursos(){
	pthread_t hilo_planificador_corto_plazo;
	pthread_t hilo_planificador_largo_plazo;

	pthread_create(&hilo_planificador_largo_plazo, NULL, (void*)planificador_largo_plazo,NULL);
	pthread_create(&hilo_planificador_corto_plazo, NULL, (void*)planificador, NULL); //OJOOOOO

    pthread_detach(hilo_planificador_largo_plazo);
    pthread_detach(hilo_planificador_corto_plazo);
}

//------------------------------------- ENTRENADORES ---------------------------------------------------

t_entrenador* crear_entrenador(char* posicion, char* pokemones, char* objetivos,int i){
	t_entrenador* entrenador = malloc(sizeof(t_entrenador));

	entrenador->id = i;
	entrenador->objetivo_temporal=NULL;
	entrenador->bloq_exec = 0;
	entrenador->exit = 0;


	char **auxiliar = string_split(posicion,"|");

	entrenador->posicion_x = atoi(auxiliar[0]);
	entrenador->posicion_y = atoi(auxiliar[1]);

	//free(auxiliar);
	free(auxiliar[0]);
	free(auxiliar[1]);
    //free(auxiliar);

	char **auxiliar3 = string_split(pokemones,"|"); //ultima posicion tiene null

	entrenador->pokemones = list_create();

	while(*auxiliar3){
		list_add(entrenador->pokemones,*auxiliar3);
		auxiliar3++;
	}

	//free(auxiliar3);

	char **auxiliar2 = string_split(objetivos,"|"); //ultima posicion tiene null

	entrenador->objetivos = list_create();
	while(*auxiliar2){
		list_add(entrenador->objetivos,*auxiliar2);
		auxiliar2++;
	}

	//OJO CON LOS CHAR * Q SE PASAN POR PARAMETRO, SE LIBERAN???
	//puts("ASDSAD");
	//free(auxiliar2);
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
		free(*lista_de_posiciones);
		free(*lista_de_pokemones);
		free(*lista_de_objetivos);
		lista_de_objetivos++;
		lista_de_pokemones++;
		lista_de_posiciones++;
		i++;
	}
	//free(lista_de_posiciones);
	//free(lista_de_pokemones);
	//free(lista_de_objetivos);
}


//----------------------------------------------- OBJETIVOS

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

//------------------------------------------- MENSAJES DEL TEAM

int subscribirse_a_cola(cola_code cola){
	int socket_aux = connect_to(ip_broker,puerto_broker,wait_time,logger);
	t_mensaje* mensaje = crear_mensaje(2, SUBSCRIPCION, cola);
	mensaje->id=ID_SUSCRIPCION;
	enviar_mensaje(socket_aux, mensaje);
	check_ack(socket_aux, ACK);
	free(mensaje);
	return socket_aux;
}

void enviar_mensajes_get(int socket_a_enviar){

	void enviar_un_mensaje_get(char* nombre_pokemon, void* data){
		int socket_broker = connect_to(ip_broker,puerto_broker,wait_time,logger);
		t_mensaje* mensaje = crear_mensaje(2,GET_POKEMON,nombre_pokemon);
		enviar_mensaje(socket_broker,mensaje);
		int id_correlativo = wait_ack(socket_broker);
		dictionary_put(ids_a_esperar,string_itoa(id_correlativo),(void*)GET_POKEMON);

		close(socket_broker);
	}

	dictionary_iterator(dic_pok_obj,enviar_un_mensaje_get);
}

void agregar_id_esperado(int id_entrenador,int id){
	dictionary_put(ids_a_esperar,string_itoa(id),(void*)id_entrenador);
}

int remover_id(int id){
	return dictionary_remove(ids_a_esperar,string_itoa(id));
}

int comprobar_id_esperado(uint32_t id){
	return dictionary_has_key(ids_a_esperar,string_itoa(id));
}

void manejar_pokemon_especie(t_pokemon_especie* pokemon_especie){
	t_pokemon* pokemon_aux;
	char* str_aux;
	int i=0;
	void agregar_pokemon_a_new(char* key, void* value){
		while(value--){
			str_aux = string_new();
			string_append_with_format(&str_aux,"%s,%s",pokemon_especie->nombre_especie,key);
			pokemon_aux = string_a_pokemon(str_aux);
			pthread_mutex_lock(&list_pok_new_mutex);
			list_add(list_pok_new, pokemon_aux);
			pthread_mutex_unlock(&list_pok_new_mutex);
			i++;
			free(str_aux);
		}
	}
	printear_pokemon_especie(pokemon_especie);
	dictionary_iterator(pokemon_especie->posiciones_especie,agregar_pokemon_a_new);
	while(i--)
		sem_post(&revisar_pokemones_new);
}

void manejar_appeared_pokemon(t_appeared_pokemon appeared_pokemon) {
	pthread_mutex_lock(&list_pok_new_mutex);
	list_add(list_pok_new, appeared_pokemon.pokemon);
	sem_post(&revisar_pokemones_new);
	pthread_mutex_unlock(&list_pok_new_mutex);
}

void avisar_caught(int id, uint32_t confirmacion){
	sem_wait(&espera_caught[id]);
	respuesta_caught[id]=confirmacion;
}

void manejar_mensaje(t_mensaje* mensaje){
	int id_entrenador;
	switch(mensaje->codigo_operacion){
		case APPEARED_POKEMON:
			manejar_appeared_pokemon(mensaje->contenido.appeared_pokemon);
			break;
		case LOCALIZED_POKEMON:;
			if(comprobar_id_esperado(mensaje->contenido.localized_pokemon.id_correlativo)){
				manejar_pokemon_especie(mensaje->contenido.localized_pokemon.pokemon_especie);
				pthread_mutex_lock(&list_pok_new_mutex);
				list_iterate(list_pok_new,(void *)printear_pokemon);
				pthread_mutex_unlock(&list_pok_new_mutex);
			}else
				liberar_mensaje(mensaje);
			break;
		case CAUGHT_POKEMON:
			if(comprobar_id_esperado(mensaje->contenido.caught_pokemon.id_correlativo))
				id_entrenador = remover_id(mensaje->contenido.caught_pokemon.id_correlativo);
				avisar_caught(id_entrenador,mensaje->contenido.caught_pokemon.caught_confirmation);
			break;

		default:
			break;
	}
}

bool recibir_mensaje(int un_socket){
	uint32_t codigo_operacion,id,size_contenido_mensaje;
	if(recv(un_socket,&codigo_operacion,sizeof(uint32_t),MSG_WAITALL)<=0){
		return false;
	}

	if(recv(un_socket,&id,sizeof(uint32_t),MSG_WAITALL)<=0){
		return false;
	}

	if(recv(un_socket,&size_contenido_mensaje,sizeof(uint32_t),MSG_WAITALL)<=0){
		return false;
	}

	void* stream = malloc(size_contenido_mensaje);

	if(recv(un_socket, stream, size_contenido_mensaje,MSG_WAITALL)<=0){
		return false;
	}

	send_ack(un_socket,ACK);

	t_mensaje* mensaje = deserializar_mensaje(codigo_operacion, stream);
	mensaje->id=id;

	logear_llegada_mensaje(logger,mensaje);

	printear_mensaje(mensaje);
	manejar_mensaje(mensaje);
	free(mensaje);
	free(stream);
	return true;
}


void protocolo_recibir_mensaje(cola_code cola){

	while(true){
		pthread_mutex_lock(&mutex_recibir);
		int socket_cola = subscribirse_a_cola(cola); //todo suscribirse y enviar el id del team en el mensaje
		pthread_mutex_unlock(&mutex_recibir);
		printf("socket_suscripcion:%d\n",socket_cola);
		while(recibir_mensaje(socket_cola));
		log_debug(logger,"Error de comunicacion con el broker, se realizara la operacion default");
		log_debug(logger,"Reintentando conexion...");
		close(socket_cola);
	}
}

//---------------------------------------------- INICIALIZAR TEAM

void inicializar_team(){

    inicializar_estructuras_globales();

    //------------------

	logger = log_create("../team.log","log",1,LOG_LEVEL_DEBUG);
	config = config_create("../config");
	retardo = config_get_int_value(config,"RETARDO_CICLO_CPU");

	obtener_entrenadores();

	inicializar_semaforos();
    crear_hilos_entrenadores();

    dic_pok_obj = obtener_pokemones_objetivo();

    crear_hilos_planificar_recursos();

	ip_broker = config_get_string_value(config,"IP_BROKER");
	log_debug(logger,config_get_string_value(config,"IP_BROKER")); //pido y logueo ip
	puerto_broker = config_get_string_value(config,"PUERTO_BROKER");
	log_debug(logger,config_get_string_value(config,"PUERTO_BROKER")); //pido y logueo puerto
	wait_time = config_get_int_value(config,"TIEMPO_RECONEXION");

	//------Se crean 3 threads para escuchar las notificaciones del broker-----

	pthread_t recibir_cola_appeared;
	pthread_t recibir_cola_caught;
	pthread_t recibir_cola_localized;

	pthread_create(&recibir_cola_appeared, NULL, (void*)protocolo_recibir_mensaje,(void*) COLA_APPEARED_POKEMON);
	pthread_detach(recibir_cola_appeared);
	pthread_create(&recibir_cola_caught, NULL, (void*)protocolo_recibir_mensaje,(void*) COLA_CAUGHT_POKEMON);
	pthread_detach(recibir_cola_caught);
	pthread_create(&recibir_cola_localized, NULL, (void*)protocolo_recibir_mensaje, (void*)COLA_LOCALIZED_POKEMON);
	pthread_detach(recibir_cola_localized);
}


void mostrar_menu(){
	puts("-----------MENU -----------");
	puts("INGRESE UN VALOR");
	puts("M -> MOSTRAR MENU");
	puts("N -> LISTA DE NEW");
	puts("R -> LISTA DE READY");
	puts("o -> DICCIONARIO DE OBJETIVOS");
	puts("r -> DICCIONARIO DE READY");
	puts("E -> LISTA DE ENTRENADORES");
	puts("C -> LISTA DE CORTO PLAZO");
	puts("D -> INICIAR CORRECICON DEADLOCK");
	puts("Z -> SALIR");
	puts("---------------------------");
}

void debug(sem_t* sem){
	while(true){
		char msg;
		scanf("%c",&msg);
		switch(msg){
			case 'M':
				mostrar_menu();
				break;
			case 'N':
				debug_leer_lista(list_pok_new);
				puts("----- INGRESE MENSAJE -----");
				break;
			case 'R':
				debug_leer_lista(list_pok_ready);
				puts("----- INGRESE MENSAJE -----");
				break;
			case 'o':
				debug_dic(dic_pok_obj);
				puts("----- INGRESE MENSAJE -----");
				break;
			case 'r':
				debug_dic(dic_pok_ready_o_exec);
				puts("----- INGRESE MENSAJE -----");
				break;
			case 'E':
				leer_lista_entrenadores(entrenadores);
				puts("----- INGRESE MENSAJE -----");
				break;
			case 'C':
				leer_lista_entrenadores(lista_corto_plazo);
		        puts("----- INGRESE MENSAJE -----");
				break;
			case 'D':
				deadlock();
				puts("Se corrio el algoritmo de deteccion de deadlock");
		        puts("----- INGRESE MENSAJE -----");
				break;
			case 'F':
				liberar_recursos_globales();
				puts("----- SE LIBERARON LOS RECURSOS CORRECTAMENTE -----");
				sem_post(sem);
				return;
			case 'c':;
				t_pokemon* pokemon = crear_pokemon("Pikachu",1,2);
				enviar_catch(pokemon);
				liberar_pokemon(pokemon);
				break;

		}
	}
}




int main(void) {

	inicializar_team();
	dictionary_put(ids_a_esperar,"455",(void*)LOCALIZED_POKEMON);
	dictionary_put(ids_a_esperar,"888888",(void*)LOCALIZED_POKEMON);

	//DEBUG
	sem_t sem_debug;
	sem_init(&sem_debug, 0,0);

	pthread_t menu_debug;
	pthread_create(&menu_debug, NULL, (void*)debug, &sem_debug);
	pthread_detach(menu_debug);

	//FIN DEBUG

	sem_wait(&cumplio_objetivo_global);
	puts("CUMPLIO EL TEAM SU OBJETIVO, IUPIIIII");
	sem_wait(&sem_debug); //!!!!!!1111

	log_destroy(logger);

	//liberar_recursos();

	return EXIT_SUCCESS;
}
