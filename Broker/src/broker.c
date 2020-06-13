/*                                                  HITO 2
 * Objetivos:
Proceso Team: Permitir solamente planificar de forma FIFO un conjunto de entrenadores.

Proceso Broker: Implementación completa de la administración de las colas de mensajes.
Aceptar suscripciones a una cola de mensajes específica.

Proceso GameBoy: Permitir el envío de varios mensajes al proceso Broker y el mensaje Appeared Pokemon
al proceso Team.

    delimitador[] = ",";
    string_split(cadena, delimitador);

*/

#include "broker.h"

bool id_validation(int socket_cliente) {
	send_ack(socket_cliente, ACK); //Broker envia confirmacion del mensaje recibido
	if(check_ack(socket_cliente, ACK))// Broker espera al team para que confirme la confirmacion
		send_ack(socket_cliente, id_mensajes_globales++); // Broker le manda el id del mensaje
		return check_ack(socket_cliente, id_mensajes_globales - 1); //Broker espera la confirmacion del id y sale si es correcta.}

}
//#include<readline/readline.h>

//gcc broker.c -lpthread -lcommons -o broker
//./broker

void notificar_mensaje_cacheados(int cola, int socket_cliente){
	int i=-1;
	void enviar_mensaje_condicional(void* stream){
		i++;
		t_partition* particion = (t_partition*)stream;
		if(!es_suscriptor_confirmado(particion->suscriptores_confirmados,socket_cliente)){
			t_mensaje* mensaje_aux;
			mensaje_aux = get_mensaje_cacheado(cola,i);
			pthread_mutex_lock(&mutex_enviar);
			enviar_mensaje(socket_cliente,mensaje_aux);
			pthread_mutex_unlock(&mutex_enviar);
			pthread_mutex_lock(&mutex_recibir);
			check_ack(socket_cliente,ACK);
			pthread_mutex_unlock(&mutex_recibir);
		}
	}
	list_iterate(administracion_colas[cola].particiones,enviar_mensaje_condicional);
}

void manejar_subscripcion(int cola,t_suscriptor* suscriptor){

	pthread_mutex_lock(&mutex_cola_suscriptores[cola]);
	list_add(suscriptores[cola],suscriptor);
	pthread_mutex_unlock(&mutex_cola_suscriptores[cola]);

	puts("Mensaje recibido con exito!");
	pthread_mutex_lock(&mutex_enviar);
	send_ack(suscriptor->socket_cliente,ACK);
	pthread_mutex_unlock(&mutex_enviar);
	puts("Aviso de retorno con exito!");
	notificar_mensaje_cacheados(cola,suscriptor->socket_cliente);
	return;
}

void manejar_catch(int socket_cliente,t_mensaje* mensaje){//MANDA ACK, LUEGO EL ID Y ESPERA CONFIRMACION CHEQUEANDO CON EL RECIBIDO
	puts("envia ack");
	send_ack(socket_cliente,ACK);
	puts("envia id");
	send_ack(socket_cliente,id_mensajes_globales++);
	puts("recibe confirmacion del id");
	check_ack(socket_cliente,id_mensajes_globales);
}

void manejar_mensaje(int socket_cliente,t_mensaje* mensaje){
    switch(mensaje->codigo_operacion){
    	case GET_POKEMON:;
    		break;
    	case CATCH_POKEMON:
    		manejar_catch(socket_cliente,mensaje);
    		break;
    }

    close(socket_cliente);

}


/* recibir_mensaje
 * socket_cliente = socket del cual hace recv para obtener el mensaje que llega.
 *
 * Primero delega a serializar buffer el cual devuelve un t_mensaje* el cual luego es pasado a
 * manejar mensaje que se encarga de procesarlo.
 */



void recibir_mensaje(int *socket_cliente){
	//printf("a recibir mensaje le llega el socket %d\n",*socket_cliente);
	uint32_t codigo_operacion;

	//pthread_mutex_lock(&mutex_recibir);

	if(recv(*socket_cliente, &(codigo_operacion),sizeof(uint32_t), MSG_WAITALL)==-1){
		perror("Falla recv() op_code");
	}

	//printf("op_code: %d\n", codigo_operacion);

	uint32_t id;

	if(recv(*socket_cliente, &(id), sizeof(uint32_t), MSG_WAITALL) == -1){
		perror("Falla recv() id");
	}

	//printf("id:%d\n", id);

	uint32_t size_contenido_mensaje;

	if(recv(*socket_cliente, &(size_contenido_mensaje), sizeof(uint32_t), MSG_WAITALL) == -1){
		perror("Falla recv() size_contenido_mensaje");
	}


	//printf("size contenido:%d\n", size_contenido_mensaje);

	if(codigo_operacion==SUBSCRIPCION){
		int cola;
		recv(*socket_cliente, &(cola),size_contenido_mensaje, MSG_WAITALL);
		printf("cola:%d\n", cola);
		int socket_suscriptor = *socket_cliente;
		manejar_subscripcion(cola, crear_suscriptor(id,socket_suscriptor));
		return;
	}

	void* stream = malloc(size_contenido_mensaje);

	if(recv(*socket_cliente, stream, size_contenido_mensaje, MSG_WAITALL) == -1){
		perror("Falla recv() contenido");
	}

	//pthread_mutex_unlock(&mutex_recibir);
	t_mensaje* mensaje = deserializar_mensaje(codigo_operacion, stream);
	mensaje->id=id_mensajes_globales++;
	manejar_mensaje(*socket_cliente,mensaje);
}


/* recibir_cliente
 * socket_servidor = socket del cual se esperara la solicitud de conexion
 */

void recibir_cliente(int *socket_servidor){
	while(1){
		esperar_cliente(*socket_servidor,recibir_mensaje);
	}
}

void notificar_mensaje(t_mensaje* mensaje){
	void notificar_suscriptores(void* stream){
		t_suscriptor* suscriptor = stream;
		enviar_mensaje(suscriptor->socket_cliente,mensaje);
		if(check_ack(suscriptor->socket_cliente, ACK))
			confirmar_suscriptor(suscriptor,mensaje);
		else
			remover_suscriptor(suscriptor,mensaje);
	}
	printf("cachear_mensaje\n");
	cachear_mensaje(mensaje);
	printf("itera notificacion\n");
	list_iterate(suscriptores[mensaje->codigo_operacion],notificar_suscriptores);
}


void enviar_appeared_pokemon(t_pokemon* pokemon){
	puts("envia appeared");
	int id_correlativo = 455;
	t_mensaje* mensaje = crear_mensaje(5, APPEARED_POKEMON,pokemon->nombre,pokemon->pos_x,pokemon->pos_y,id_correlativo);
	int socket_aux = (int)list_get(suscriptores[APPEARED_POKEMON],0);
	printear_mensaje(mensaje);
	enviar_mensaje(socket_aux,mensaje);
	printf("Se envio el mensaje al socket:%d\n", socket_aux);
	check_ack(socket_aux,ACK);
	puts("ACK con exito\n");
}

void printear_lista_suscriptores(t_list* lista){
	void printear_suscriptor(void* stream){
		t_suscriptor* suscriptor = stream;
		printf("|ID:%d,SOCKET:%d|\n",suscriptor->id_team,suscriptor->socket_cliente);
	}
	list_iterate(lista,printear_suscriptor);
}

void printear_estado(){
	for(int i=0;i<CANTIDAD_COLAS;i++){
		printf("%s\n",cola_string(i));
		printear_lista_suscriptores(suscriptores[i]);
	}
}

void descachear_particiones(void* stream){
	t_partition* particion = stream;
	int posicion;
	encontrar_particion(particion->msg_id,particion->cola_code,&posicion);
	sacar_particion(particion->cola_code,posicion);
}

void envio_mensaje(){
	t_pokemon* pokemon;
	t_pokemon* pokemon1;
	t_pokemon* pokemon2;
	t_pokemon* pokemon3;
	t_pokemon* pokemon4;
	t_mensaje* mensaje_aux;
	t_pokemon_especie* especie_pikachu;
	t_suscriptor* suscriptor;
	int i = 0;
	while(true){
		int msg = getchar();
		switch(msg){
			case 'p':
				pokemon = crear_pokemon("Pikachu",rand()%9,rand()%9);
				mensaje_aux = crear_mensaje(5, APPEARED_POKEMON,pokemon->nombre,pokemon->pos_x,pokemon->pos_y,455);
				puts("envia appeared pokemon");
				notificar_mensaje(mensaje_aux);
				break;
			case 'P':
				puts("Localized");
				//------SE CREA UN LOCALIZED----------

				pokemon1 = crear_pokemon("Pikachu",-1,2);
				pokemon2 = crear_pokemon("Pikachu",9,5);
				pokemon3 = crear_pokemon("Pikachu",-1,2);
				pokemon4 = crear_pokemon("Pikachu",-1,4);
				especie_pikachu = crear_pokemon_especie("Pikachu");
				agregar_pokemon_a_especie(especie_pikachu,pokemon1);
				agregar_pokemon_a_especie(especie_pikachu,pokemon2);
				agregar_pokemon_a_especie(especie_pikachu,pokemon3);
				agregar_pokemon_a_especie(especie_pikachu,pokemon4);

				mensaje_aux = crear_mensaje(3, LOCALIZED_POKEMON, 666, especie_pikachu);
				mensaje_aux->id=ID_DEFAULT;

				notificar_mensaje(mensaje_aux);
				break;

			case 'S':
				puts("Localized");
				//------SE CREA UN LOCALIZED----------

				pokemon1 = crear_pokemon("Squirtle",-12,12);
				pokemon2 = crear_pokemon("Squirtle",-12,12);
				pokemon3 = crear_pokemon("Squirtle",12,12);
				pokemon4 = crear_pokemon("Squirtle",-12,12);
				especie_pikachu = crear_pokemon_especie("Squirtle");
				agregar_pokemon_a_especie(especie_pikachu,pokemon1);
				agregar_pokemon_a_especie(especie_pikachu,pokemon2);
				agregar_pokemon_a_especie(especie_pikachu,pokemon3);
				agregar_pokemon_a_especie(especie_pikachu,pokemon4);

				mensaje_aux = crear_mensaje(3, LOCALIZED_POKEMON, 455, especie_pikachu);
				mensaje_aux->id=ID_DEFAULT;

				notificar_mensaje(mensaje_aux);
				break;
			case 'B':
				puts("Localized");
				//------SE CREA UN LOCALIZED----------

				pokemon1 = crear_pokemon("Bulbasaur",-2,7);
				pokemon2 = crear_pokemon("Bulbasaur",6,3);
				pokemon3 = crear_pokemon("Bulbasaur",-2,7);
				pokemon4 = crear_pokemon("Bulbasaur",5,4);
				especie_pikachu = crear_pokemon_especie("Bulbasaur");
				agregar_pokemon_a_especie(especie_pikachu,pokemon1);
				agregar_pokemon_a_especie(especie_pikachu,pokemon2);
				agregar_pokemon_a_especie(especie_pikachu,pokemon3);
				agregar_pokemon_a_especie(especie_pikachu,pokemon4);

				mensaje_aux = crear_mensaje(3, LOCALIZED_POKEMON, 4555, especie_pikachu);
				mensaje_aux->id=ID_DEFAULT;

				notificar_mensaje(mensaje_aux);
				break;
			case 'b':
				pokemon = crear_pokemon("Bulbasaur",rand()%9,rand()%9);
				mensaje_aux = crear_mensaje(5, APPEARED_POKEMON,pokemon->nombre,pokemon->pos_x,pokemon->pos_y,455);
				puts("envia appeared pokemon");
				notificar_mensaje(mensaje_aux);
				break;
			case 's':
				pokemon = crear_pokemon("Squirtle",rand()%9,rand()%9);
				mensaje_aux = crear_mensaje(5, APPEARED_POKEMON,pokemon->nombre,pokemon->pos_x,pokemon->pos_y,455);
				puts("envia appeared pokemon");
				notificar_mensaje(mensaje_aux);
				break;
			case 'c':
				pokemon = crear_pokemon("Charmander",rand()%9,rand()%9);
				mensaje_aux = crear_mensaje(5, APPEARED_POKEMON,pokemon->nombre,pokemon->pos_x,pokemon->pos_y,455);
				puts("envia appeared pokemon");
				notificar_mensaje(mensaje_aux);
				break;
			case 'r':
				printear_estado();
				printear_estado_memoria();
				break;
			case 'k':;
				pokemon = crear_pokemon("Pikachu",-1,2);
				mensaje_aux = crear_mensaje(5, APPEARED_POKEMON,pokemon->nombre,pokemon->pos_x,pokemon->pos_y,455);
				cachear_mensaje(mensaje_aux);

				pokemon = crear_pokemon("Bulbasaur",5,-3);
				mensaje_aux = crear_mensaje(5, NEW_POKEMON,pokemon->nombre,pokemon->pos_x,pokemon->pos_y,7855);
				cachear_mensaje(mensaje_aux);

				mensaje_aux = crear_mensaje(3, CAUGHT_POKEMON,ID_DEFAULT,1);
				cachear_mensaje(mensaje_aux);

				pokemon = crear_pokemon("Squirtle",12,6);
				especie_pikachu = crear_pokemon_especie("Squirtle");
				agregar_pokemon_a_especie(especie_pikachu,pokemon);
				mensaje_aux = crear_mensaje(3, LOCALIZED_POKEMON,ID_DEFAULT,especie_pikachu);
				cachear_mensaje(mensaje_aux);

				pokemon = crear_pokemon("Charmander",-45,-6);
				mensaje_aux = crear_mensaje(2, GET_POKEMON,pokemon->nombre);
				cachear_mensaje(mensaje_aux);

				pokemon = crear_pokemon("Pikachu",13,0);
				mensaje_aux = crear_mensaje(4,CATCH_POKEMON,pokemon->nombre,pokemon->pos_x,pokemon->pos_y);
				cachear_mensaje(mensaje_aux);

				break;
			case 'd':
				for(int i=0;i<CANTIDAD_COLAS;i++){
					mensaje_aux = get_mensaje_cacheado(i,0);
					printear_mensaje(mensaje_aux);
				}
				break;
			case 'h':
				for(int i=0;i<CANTIDAD_COLAS;i++)
					list_iterate(administracion_colas[i].particiones,descachear_particiones);
				break;
			case '0':
				sacar_particion(NEW_POKEMON,0);
				break;
			case '1':
				sacar_particion(GET_POKEMON,0);
				break;
			case '2':
				sacar_particion(CATCH_POKEMON,0);
				break;
			case '3':
				sacar_particion(CAUGHT_POKEMON,0);
				break;
			case '4':
				sacar_particion(LOCALIZED_POKEMON,0);
				break;
			case '5':
				sacar_particion(APPEARED_POKEMON,0);
				break;
			case '7':
				sacar_particion(APPEARED_POKEMON,1);
				break;
			case '8':
				sacar_particion(APPEARED_POKEMON,2);
				break;
			case 'o':
				mensaje_aux = crear_mensaje(3, CAUGHT_POKEMON,i,true);
				i++;
				suscriptor = list_get(suscriptores[COLA_CAUGHT_POKEMON],0);
				enviar_mensaje(suscriptor->socket_cliente,mensaje_aux);
				break;
			default:
				break;
		}
	}
}

//=============================SUSCRIPTORES ============================

t_suscriptor* crear_suscriptor(int id, int socket_cliente){
	t_suscriptor* suscriptor = malloc(sizeof(t_suscriptor));
	suscriptor->id_team=id;
	suscriptor->socket_cliente=socket_cliente;
	return suscriptor;
}

void liberar_suscriptor(t_suscriptor* suscriptor){//todo liberar suscriptor chequear que se confirmen bien
	free(suscriptor);
}

bool es_suscriptor_confirmado(t_list* suscriptores_confirmados,int id_team){
	puts("entra a suscr confir");
	bool pertenece(void* stream){
		int id_team_lista=(int)stream;
		return id_team_lista == id_team;
	}

	return list_any_satisfy(suscriptores_confirmados,pertenece);
	//todo arreglar a que funcione como deberia

}

void confirmar_suscriptor(t_suscriptor* suscriptor,t_mensaje* mensaje){
	printf("confirmar\n");
	int posicion;
	t_partition* particion = encontrar_particion(mensaje->id,mensaje->codigo_operacion,&posicion);
	list_add(particion->suscriptores_confirmados,suscriptor->id_team);
	printf("sale confirmar\n");
	//agregar el id_team a esa particion todo?
}

void remover_suscriptor(t_suscriptor* suscriptor,t_mensaje* mensaje){ // Remueve suscriptor de la cola correspondiente a ese tipo de mensaje
	printf("remover\n");
	int i=-1;
	bool es_suscriptor(void* stream){
		i++;
		t_suscriptor* suscriptor_lista = stream;
		return suscriptor->id_team==suscriptor_lista->id_team;
	}
	list_find(suscriptores[mensaje->codigo_operacion], es_suscriptor);
	liberar_suscriptor(list_remove(suscriptores[mensaje->codigo_operacion],i));
	close(suscriptor->socket_cliente);
}


//=========================PARTICIONES DINAMICAS==================================

//------------------------------------MEMORY SERIALIZATION MENSAJE-----------------------


/* memser_mensaje
 * ret_size* = puntero al que devuelve el tamaño del mensaje
 * mensaje = mensaje t_mensaje al que va a serializar
 * devuelve un void* serializado segun lo aplicado a memoria
 */

/* tamanio_contenido_mensaje
 * mensaje = mensaje t_mensaje al que va obtiene el tamaño del contenido
 * devuelve un int con el tamaño del contenido
 */

int tamanio_mensaje_memoria(t_mensaje* mensaje){
	int tamanio=0;
	int i=0;
	switch(mensaje->codigo_operacion){
		case GET_POKEMON:
			tamanio += sizeof(uint32_t) + strlen(mensaje->contenido.get_pokemon.nombre_pokemon);
			break;
		case APPEARED_POKEMON:
			tamanio += sizeof(uint32_t)*3 + strlen(mensaje->contenido.appeared_pokemon.pokemon->nombre);
			break;
		case NEW_POKEMON:
			tamanio += sizeof(uint32_t)*4 + strlen(mensaje->contenido.new_pokemon.pokemon->nombre);
			break;
		case CATCH_POKEMON:
			tamanio += sizeof(uint32_t)*3 + strlen(mensaje->contenido.catch_pokemon.pokemon->nombre);
			break;
		case CAUGHT_POKEMON:
			tamanio += sizeof(uint32_t);
			break;
		case LOCALIZED_POKEMON:
			i = cant_coordenadas_especie_pokemon(mensaje->contenido.localized_pokemon.pokemon_especie);
			printf("i:%d\n",i);
			tamanio += sizeof(uint32_t) + strlen(mensaje->contenido.localized_pokemon.pokemon_especie->nombre_especie) +
			sizeof(uint32_t) + i*2*sizeof(uint32_t);
			break;
	}
	return tamanio;
}

void* memser_get_pokemon(t_get_pokemon get_pokemon, int size){
	void* magic = malloc(size);
	int offset = 0;
	int str_len = strlen(get_pokemon.nombre_pokemon);
	memcpy(magic, &str_len, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(magic+offset, get_pokemon.nombre_pokemon,str_len);
	return magic;
}

void* memser_appeared_pokemon(t_appeared_pokemon appeared_pokemon, int size){
	void* magic = malloc(size);
	int offset = 0;
	int str_len = strlen(appeared_pokemon.pokemon->nombre);
	printf("str_len memser:%d\n",str_len);
	memcpy(magic, &str_len, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(magic+offset, appeared_pokemon.pokemon->nombre,str_len);
	offset += str_len;
	memcpy(magic+offset, &appeared_pokemon.pokemon->pos_x, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(magic+offset, &appeared_pokemon.pokemon->pos_y, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	printf("sale de memser appeared");
	return magic;
}

void* memser_new_pokemon(t_new_pokemon new_pokemon, int size){
	void* magic = malloc(size);
	int offset = 0;
	int str_len = strlen(new_pokemon.pokemon->nombre);
	memcpy(magic, &str_len, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(magic+offset, new_pokemon.pokemon->nombre,str_len);
	offset += str_len;
	memcpy(magic+offset, &new_pokemon.pokemon->pos_x, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(magic+offset, &new_pokemon.pokemon->pos_y, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(magic+offset, &new_pokemon.cantidad, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	return magic;
}

void* memser_catch_pokemon(t_catch_pokemon catch_pokemon, int size){
	void* magic = malloc(size);
	int offset = 0;
	int str_len = strlen(catch_pokemon.pokemon->nombre);
	memcpy(magic, &str_len, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(magic+offset, catch_pokemon.pokemon->nombre,str_len);
	offset += str_len;
	memcpy(magic+offset, &catch_pokemon.pokemon->pos_x, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(magic+offset, &catch_pokemon.pokemon->pos_y, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	return magic;
}

void* memser_caught_pokemon(t_caught_pokemon caught_pokemon, int size){
	void* magic = malloc(size);
	memcpy(magic, &caught_pokemon.caught_confirmation, sizeof(uint32_t));

	return magic;
}

void* memser_localized_pokemon(t_localized_pokemon localized_pokemon, int size){

	void* magic = malloc(size);
	int offset = 0, cant_coords=0;
	int str_len = strlen(localized_pokemon.pokemon_especie->nombre_especie);

	memcpy(magic, &str_len, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(magic+offset,localized_pokemon.pokemon_especie->nombre_especie,str_len);
	offset += str_len;

	cant_coords = cant_coordenadas_especie_pokemon(localized_pokemon.pokemon_especie);

	memcpy(magic+offset, &cant_coords, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	void memser_posiciones(char* key, void* stream){
		int posx,posy,cant,i;
		cant = (int)stream;
		char** str_aux = string_split(key,"|");
		posx = atoi(str_aux[0]);
		posy = atoi(str_aux[1]);

		for(i=0;i<cant;i++){
			memcpy(magic+offset, &posx, sizeof(uint32_t));
			offset += sizeof(uint32_t);
			memcpy(magic+offset, &posy, sizeof(uint32_t));
			offset += sizeof(uint32_t);
		}
	}
	dictionary_iterator(localized_pokemon.pokemon_especie->posiciones_especie, memser_posiciones);
	return magic;
}

void* memser_mensaje(t_mensaje* mensaje, int *ret_size){
	uint32_t size = tamanio_mensaje_memoria(mensaje);
	void* stream;
	switch(mensaje->codigo_operacion){
	case GET_POKEMON:;
		stream = memser_get_pokemon(mensaje->contenido.get_pokemon, size);
		break;
	case APPEARED_POKEMON:;
		stream = memser_appeared_pokemon(mensaje->contenido.appeared_pokemon,size);
		break;
	case LOCALIZED_POKEMON:;
		stream = memser_localized_pokemon(mensaje->contenido.localized_pokemon,size);
		break;
	case CATCH_POKEMON:;
		stream = memser_catch_pokemon(mensaje->contenido.catch_pokemon,size);
		break;
	case CAUGHT_POKEMON:;
		stream = memser_caught_pokemon(mensaje->contenido.caught_pokemon,size);
		break;
	case NEW_POKEMON:;
		stream = memser_new_pokemon(mensaje->contenido.new_pokemon,size);
		break;
	}
	*ret_size = (int)size;
	return stream;
}

//------------------------------------MEMORY DESERIALIZATION MENSAJE-----------------------

t_get_pokemon memdes_get_pokemon(void* stream){
	t_get_pokemon get_pokemon;
	int size_str,offset=0;
	char* str;

	memcpy(&size_str,stream,sizeof(uint32_t));
	offset += sizeof(uint32_t);
	str=malloc(size_str+1);
	memcpy(str,stream+offset,size_str);
	offset += size_str;
	str[size_str]='\0';

	get_pokemon.nombre_pokemon = str;
	return get_pokemon;
}

t_appeared_pokemon memdes_appeared_pokemon(void* stream){
	t_appeared_pokemon appeared_pokemon;
	int size_str,offset=0,pos_x,pos_y;
	char* str;

	memcpy(&size_str,stream,sizeof(uint32_t));
	offset += sizeof(uint32_t);
	str=malloc(size_str+1);
	memcpy(str,stream+offset,size_str);
	offset += size_str;
	str[size_str]='\0';

	memcpy(&pos_x,stream+offset,sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&pos_y,stream+offset,sizeof(uint32_t));
	offset += sizeof(uint32_t);

	appeared_pokemon.pokemon = crear_pokemon(str,pos_x,pos_y);
	free(str);
	return appeared_pokemon;
}

t_localized_pokemon memdes_localized_pokemon(void* stream){
	t_localized_pokemon localized_pokemon;
	int size_str,offset=0,cant_coords,pos_x,pos_y;
	t_pokemon* pokemon_aux;
	char* str;

	memcpy(&size_str,stream,sizeof(uint32_t));
	offset += sizeof(uint32_t);
	str=malloc(size_str+1);
	memcpy(str,stream+offset,size_str);
	offset += size_str;
	str[size_str]='\0';

	t_pokemon_especie* pokemon_especie = crear_pokemon_especie(str);

	memcpy(&cant_coords,stream+offset,sizeof(uint32_t));
	offset += sizeof(uint32_t);

	for(int i=0;i<cant_coords;i++){
		memcpy(&pos_x,stream+offset,sizeof(uint32_t));
		offset += sizeof(uint32_t);
		memcpy(&pos_y,stream+offset,sizeof(uint32_t));
		offset += sizeof(uint32_t);

		pokemon_aux = crear_pokemon(str,pos_x,pos_y);
		agregar_pokemon_a_especie(pokemon_especie,pokemon_aux);
		liberar_pokemon(pokemon_aux);
	}

	localized_pokemon.pokemon_especie = pokemon_especie;
	free(str);
	return localized_pokemon;
}

t_catch_pokemon memdes_catch_pokemon(void* stream){
	t_catch_pokemon catch_pokemon;
	int size_str,offset=0,pos_x,pos_y;
	t_pokemon* pokemon_aux;
	char* str;

	memcpy(&size_str,stream,sizeof(uint32_t));
	offset += sizeof(uint32_t);
	str=malloc(size_str+1);
	memcpy(str,stream+offset,size_str);
	offset += size_str;
	str[size_str]='\0';


	memcpy(&pos_x,stream+offset,sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&pos_y,stream+offset,sizeof(uint32_t));
	offset += sizeof(uint32_t);

	pokemon_aux = crear_pokemon(str,pos_x,pos_y);

	catch_pokemon.pokemon=pokemon_aux;
	free(str);
	return catch_pokemon;
}

t_caught_pokemon memdes_caught_pokemon(void* stream){
	t_caught_pokemon caught_pokemon;
	memcpy(&caught_pokemon.caught_confirmation,stream,sizeof(uint32_t));
	return caught_pokemon;
}

t_new_pokemon memdes_new_pokemon(void* stream){
	t_new_pokemon new_pokemon;
	int size_str,offset=0,pos_x,pos_y;
	t_pokemon* pokemon_aux;
	char* str;

	memcpy(&size_str,stream,sizeof(uint32_t));
	offset += sizeof(uint32_t);
	str=malloc(size_str+1);
	memcpy(str,stream+offset,size_str);
	offset += size_str;
	str[size_str]='\0';


	memcpy(&pos_x,stream+offset,sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&pos_y,stream+offset,sizeof(uint32_t));
	offset += sizeof(uint32_t);

	pokemon_aux = crear_pokemon(str,pos_x,pos_y);
	free(str);
	new_pokemon.pokemon=pokemon_aux;

	memcpy(&new_pokemon.cantidad,stream+offset,sizeof(uint32_t));
	offset += sizeof(uint32_t);

	return new_pokemon;
}

t_mensaje* memdes_mensaje(t_partition* particion){
	t_mensaje* mensaje;
	void* magic = particion->inicio;
	int op_code = particion->cola_code;
	int id_correlativo = particion->id_correlativo;

	switch(op_code){
	case GET_POKEMON:;
		t_get_pokemon get_pokemon = memdes_get_pokemon(magic);
		mensaje = crear_mensaje(2,op_code,get_pokemon.nombre_pokemon);
		break;
	case APPEARED_POKEMON:;
		t_appeared_pokemon appeared_pokemon = memdes_appeared_pokemon(magic);
		mensaje = crear_mensaje(5,op_code,appeared_pokemon.pokemon->nombre,appeared_pokemon.pokemon->pos_x,appeared_pokemon.pokemon->pos_y,id_correlativo);
		//liberar_pokemon(appeared_pokemon.pokemon);
		break;
	case LOCALIZED_POKEMON:;
		t_localized_pokemon localized_pokemon = memdes_localized_pokemon(magic);
		mensaje = crear_mensaje(3,op_code,id_correlativo,localized_pokemon.pokemon_especie);
		break;
	case CATCH_POKEMON:;
		t_catch_pokemon catch_pokemon = memdes_catch_pokemon(magic);
		mensaje = crear_mensaje(4,op_code,catch_pokemon.pokemon->nombre,catch_pokemon.pokemon->pos_x,catch_pokemon.pokemon->pos_y);
		break;
	case CAUGHT_POKEMON:;
		t_caught_pokemon caught_pokemon = memdes_caught_pokemon(magic);
		mensaje = crear_mensaje(3,op_code,id_correlativo,caught_pokemon.caught_confirmation);
		break;
	case NEW_POKEMON:;
		t_new_pokemon new_pokemon = memdes_new_pokemon(magic);
		mensaje = crear_mensaje(5,op_code,new_pokemon.pokemon->nombre,new_pokemon.pokemon->pos_x,new_pokemon.pokemon->pos_y,new_pokemon.cantidad);
		break;
	}

	return mensaje;
}


//--------------------------------------ALGORITMOS DE PARTICION LIBRE-------------------------

void* asignar_particion_libre(t_free_partition* particion_libre, int size){
	if(particion_libre==NULL)
		return NULL;
	void* aux = particion_libre->inicio;
	particion_libre->inicio += size;
	particion_libre->size -= size;
	printf("particion _libre_nueva\n inicio:%d | size:%d\n", (int)particion_libre->inicio,particion_libre->size);
	if(particion_libre->size)
		list_add(particiones_libres,particion_libre);
	else
		free(particion_libre);
	unificar_particiones_libres(); //No se si es taaan necesario aca pero me aseguro que la lista global siga ordenada
	return aux;
}

void* first_fit(int size){
	int i = -1;
	t_free_partition* particion_libre;
	bool tiene_espacio(void* stream){
		i++;
		t_free_partition* particion_libre = stream;
		printf("r:%d\n",particion_libre->size>=size);
		return particion_libre->size >= size;
	}
	void* aux = list_remove_by_condition(particiones_libres,tiene_espacio);
	if(!aux)
		return NULL;
	else
		particion_libre = (t_free_partition*)aux;
	printf("PL inicio:%d | size:%d\n", (int)particion_libre->inicio,particion_libre->size);

	printf("PL:%d\n",(int)particion_libre);
	return asignar_particion_libre(particion_libre,size);
}

void* best_fit(int size){
	int i = -1;
	int pos=-1;
	int tam_min=1000000000; // Numero muy grande para que tenga valized el algoritmo. Deberia ser el total de la memoria;

	void espacio_optimo(void* stream){
		i++;
		t_free_partition* particion_libre = stream;
		printf("particion->size:%d | size:%d | tamaño minimo:%d\n", particion_libre->size, size,tam_min);
		printf("primer condicion:%d && segunda condicion:%d\n",(particion_libre->size <= size),(particion_libre->size < tam_min));
		if((size <= particion_libre->size) && (particion_libre->size < tam_min)){ // ( puede alojar && es la mejor que encontro)
			pos=i;
			tam_min=particion_libre->size;
		}
	}
	printf("size particiones libres:%d\n",list_size(particiones_libres));
	list_iterate(particiones_libres,espacio_optimo);
	printf("pos:%d | size:%d | tamaño minimo:%d\n",pos, size,tam_min);
	if(pos==-1)
		return NULL;
	t_free_partition* particion_libre = list_remove(particiones_libres, pos);

	//obtener un puntero de tamaño size y descontarle size a la particion libre. luego agregar la particion modificada a la lista
	// de particiones globales de nuevo y ordenarla y unificar, si hace falta.
	return asignar_particion_libre(particion_libre, size);
}

/*
 * unificar_particiones_libres
 *
 * hay una lista de particiones libres las cuales estan ordenadas.
 *
 * 1. chequea una por una la particion (i) con la siguiente (i+1) si estas son contiguas
 * 2. si lo son, las unifica, removiendo la particion siguiente (i+1) y la libera
 * 3. unificada pasa a false para reiniciar el alogritmo y breakea para reiniciar la ejecucion
 * 4. si unificada es false reinicia el algoritmo, paso 1.  else, paso 5.
 * 5. sale ya que no se puede unificar ninguna particion
 *
 * tener en cuenta que el tamaño de las particiones libres va a variar cada vez que se unifique
 * por esta razon es un valor calculado en el for.
 *
 */


/*
 * unificar
 *
 * Se la va a utilizar para unir particiones que sean LIBRES y CONTIGUAS, no para las de uso comun.
 * Ya que se perderian las limitaciones del contenido que guarda.
 *
 * El funcionamiento se basa en unir los datos de la particion2 a la particion1 y destruir la particion2,
 * devolviendo la union en particion1.
 *
 */


void unificar_particiones_libres(){
	void unificar(t_free_partition* particion1, t_free_partition* particion2){
		particion1->size += particion2->size;
		free(particion2);
	}
	bool unificada = false;
	while(!unificada){
		unificada = true;
		for(int i=0;i<list_size(particiones_libres)-1;i++){
			t_free_partition* una_particion = list_get(particiones_libres,i);
			t_free_partition* otra_particion = list_get(particiones_libres,i+1);
			if(particiones_libres_contiguas(una_particion,otra_particion)){
				unificar(una_particion, list_remove(particiones_libres,i+1));
				unificada=false;
				break;
			}
		}
	}
}

void normalizar_particiones_libres(){
	bool en_orden(void* un_stream, void* otro_stream){
		t_free_partition* una_particion = un_stream;
		t_free_partition* otra_particion = otro_stream;
		if(puntero_cmp(una_particion->inicio,otra_particion->inicio)<0)
			return true;
		else
			return false;
	}
	list_sort(particiones_libres, en_orden);
	unificar_particiones_libres();
}

//========================== COMPACTACION =====================================

void compactar_memoria(){//todo guardar los id_team confirmados de cada mensaje

	void* magic;
	t_list* mensajes_aux = list_create();
	void transpaso_a_aux(void* stream){
		int pos;
		t_partition* particion = stream;
		t_mensaje* mensaje = leer_particion_cache(particion);
		printear_mensaje(mensaje);
		encontrar_particion(particion->msg_id,particion->cola_code,&pos);
		sacar_particion(particion->cola_code,pos);
		list_add(mensajes_aux,mensaje);
	}

	for(int i=0;i<CANTIDAD_COLAS;i++)
		while(list_size(administracion_colas[i].particiones))
			transpaso_a_aux(list_get(administracion_colas[i].particiones,0));

	for(int i=0;i<CANTIDAD_COLAS;i++)
		printf("list_size:%d de cola:%d\n", list_size(administracion_colas[i].particiones),i);

	printf("list_size aux:%d\n",list_size(mensajes_aux));

	printear_estado_memoria();
	sleep(5);
	unificar_particiones_libres();
	magic = mem_alloc;

	while(list_size(mensajes_aux)){
		t_mensaje* mensaje = list_remove(mensajes_aux,0);
		cachear_mensaje(mensaje);
	}

	printear_estado_memoria();
	sleep(5);
	puts("MEMORIA COMPACTADA");
	list_destroy(mensajes_aux);
}

/*
 * pmalloc
 *
 * "partition_malloc"
 */

void* pmalloc(int size){
	int i=0;
	void* aux=NULL;
	puts("pmalloc\n");
	while(!aux){
		puts("ENTRA AL WHILE");
		switch(APL){
			case FF:
				pthread_mutex_lock(&mutex_particiones_libres);
				aux = first_fit(size);
				pthread_mutex_unlock(&mutex_particiones_libres);
				break;
			case BF:
				pthread_mutex_lock(&mutex_particiones_libres);
				aux = best_fit(size);
				pthread_mutex_unlock(&mutex_particiones_libres);
				break;
		}
		normalizar_particiones_libres();

		if(!aux){
			puts("ES NULL\n");
			compactar_memoria();//todo se deberia ver si ya se compacto una vez, en ese caso activa protocolo eleccion de victima
			}else{
				puts("es puntero valido");
			}
		puts("sale pmalloc\n");
	}


	return aux;
}


/*
 * puntero_cmp
 *
 * compara dos punteros por posiciones
 *
 * retorna 0 si apuntan al mismo espacio de memoria
 * >0 si el primer puntero esta por delante del segundo puntero
 * <0 si el primer puntero esta por detras del segundo puntero
 *
 */

int puntero_cmp(void* un_puntero, void* otro_puntero){
	 return un_puntero-otro_puntero;
}

/*
 * particiones_contiguas
 *
 * chequea si la primer particion es contigua a la segunda.
 * Estando por detras.
 *
 * retorna 1 si son contiguas, 0 si no lo son;
 *
 * Ej particion 1 --> particion 2
 */

bool particiones_contiguas(t_partition* particion1,t_partition* particion2){
	void* aux = particion1->inicio + particion1->size;
	return !puntero_cmp(aux,particion2->inicio);
}

bool particiones_libres_contiguas(t_free_partition* particion1,t_free_partition* particion2){
	void* aux = particion1->inicio + particion1->size;
	bool r = !puntero_cmp(aux,particion2->inicio);
	printf("son contiguas:%d\n",r);
	return r;
}

//----------------------------------PARTICION LIBRE-------------------------------------

adm_cola crear_adm_cola(){
	t_list* particiones = list_create();
	adm_cola cola;
	cola.particiones = particiones;
	return cola;
}

//----------------------------------PARTICION LIBRE-------------------------------------

t_free_partition* crear_particion_libre(void* stream, int size){
	t_free_partition* particion_libre = malloc(sizeof(t_free_partition));
	particion_libre->inicio = stream;
	particion_libre->size = size;
	return particion_libre;
}

void agregar_particion_libre(t_free_partition* particion_libre){
	list_add(particiones_libres,particion_libre);
	normalizar_particiones_libres();
}

//----------------------------------PARTICION-------------------------------------

//FALTARIA AGREGAR ID?

t_partition* crear_particion(t_mensaje* mensaje){//todo aca vendria validacion del algoritmo de compactacion y
	int size;
	void* magic = memser_mensaje(mensaje, &size);
	puts("crear_particion\n");
	t_partition* particion = malloc(sizeof(t_partition));
	particion->inicio=pmalloc(size);
	particion->msg_id=mensaje->id;
	particion->cola_code=mensaje->codigo_operacion;
	asignar_id_correlativo_a_particion(particion,mensaje);
	printf("inicio:%d\nstream:%d\nsize:%d\n", (int)particion->inicio,(int)magic,size);
	memcpy(particion->inicio,magic,size);
	particion->size = size;
	particion->suscriptores_confirmados = list_create();
	//free(stream);
	free(magic);
	return particion;
}

void liberar_particion(t_partition* particion){
	t_free_partition* particion_libre = crear_particion_libre(particion->inicio,particion->size);
	agregar_particion_libre(particion_libre);
	list_destroy(particion->suscriptores_confirmados);
	free(particion);
}

void agregar_particion(adm_cola adm_cola, t_partition* particion){
	list_add(adm_cola.particiones,particion);
}

void sacar_particion(int cola, int index){//SE USA ASI??
	printf("libero particion en indice:%d\n",index);
	liberar_particion(list_remove(administracion_colas[cola].particiones,index));
}

void asignar_id_correlativo_a_particion(t_partition* particion,t_mensaje* mensaje){
	switch(mensaje->codigo_operacion){
	case GET_POKEMON:;
		particion->id_correlativo = ID_DEFAULT;
		break;
	case APPEARED_POKEMON:;
		particion->id_correlativo=mensaje->contenido.appeared_pokemon.id_correlativo;
		break;
	case LOCALIZED_POKEMON:;
		particion->id_correlativo=mensaje->contenido.appeared_pokemon.id_correlativo;
		break;
	case CATCH_POKEMON:;
		particion->id_correlativo = ID_DEFAULT;
		break;
	case CAUGHT_POKEMON:;
		particion->id_correlativo=mensaje->contenido.appeared_pokemon.id_correlativo;
		break;
	case NEW_POKEMON:;
		particion->id_correlativo = ID_DEFAULT;
		break;
	}
}

void asignar_id_correlativo_a_mensaje(t_mensaje* mensaje, t_partition* particion){
	switch(particion->cola_code){
	case GET_POKEMON:;
		break;
	case APPEARED_POKEMON:;
		mensaje->contenido.appeared_pokemon.id_correlativo=particion->id_correlativo;
		break;
	case LOCALIZED_POKEMON:;
		mensaje->contenido.appeared_pokemon.id_correlativo=particion->id_correlativo;
		break;
	case CATCH_POKEMON:;
		break;
	case CAUGHT_POKEMON:;
		mensaje->contenido.appeared_pokemon.id_correlativo=particion->id_correlativo;
		break;
	case NEW_POKEMON:;
		break;
	}
}

void cachear_mensaje(t_mensaje* mensaje){
	t_partition* particion = crear_particion(mensaje);
	puts("crea particion");
	agregar_particion(administracion_colas[mensaje->codigo_operacion],particion);
}

void descachear_mensaje(int msg_id,int cola){
	//SE PODRIA IMPLEMENTAR QUE SAQUE SEGUN ID MENSAJE
	int posicion;
	encontrar_particion(msg_id,cola,&posicion);
	printf("posicion:%d\n", posicion);
	sacar_particion(cola,posicion);
}

t_mensaje* leer_particion_cache(t_partition* particion){
	t_mensaje* mensaje = memdes_mensaje(particion);
	asignar_id_correlativo_a_mensaje(mensaje, particion);
	return mensaje;
}

t_mensaje* get_mensaje_cacheado(int cola_code, int index){
	t_partition* particion = list_get(administracion_colas[cola_code].particiones, index);
	t_mensaje* mensaje = leer_particion_cache(particion);
	return mensaje;
}

/*
 * encontrar_particion le paso un id de mensaje a buscar, la cola a la cual pertenece,
 * devuelve la particion y la posicion en la cual esta ubicada
 *
 */

t_partition* encontrar_particion(int msg_id,int cola,int* posicion){
	int i=-1;
	bool corresponde_a_id(void* stream){
		i++;
		t_partition* particion_lista = stream;
		return particion_lista->msg_id == msg_id;
	}
	t_partition* particion = list_find(administracion_colas[cola].particiones,corresponde_a_id);
	if(posicion!=NULL)
		*posicion=i;
	return particion;
}

//============================ INTERPRETADORES ===================================

algoritmo_particion_libre interpretar_APL(char* word){
	if(!strcmp(word,"FF")) return FF;
	if(!strcmp(word,"BF")) return BF;
}

algoritmo_reemplazo interpretar_AR(char* word){
	if(!strcmp(word,"LRU")) return LRU;
	if(!strcmp(word,"FIFO")) return FIFO;
}

char* cola_string(int cola){
	if(cola==COLA_APPEARED_POKEMON) return "COLA_APPEARED_POKEMON";
	if(cola==COLA_GET_POKEMON) return "COLA_GET_POKEMON";
	if(cola==COLA_CAUGHT_POKEMON) return "COLA_CAUGHT_POKEMON";
	if(cola==COLA_CATCH_POKEMON) return "COLA_CATCH_POKEMON";
	if(cola==COLA_NEW_POKEMON) return "COLA_NEW_POKEMON";
	if(cola==COLA_LOCALIZED_POKEMON) return "COLA_LOCALIZED_POKEMON";
	return NULL;
}

void printear_estado_memoria(){
	int i=-1;

	void printear_particion_libre(void* stream){
		i++;
		t_free_partition* particion = stream;
		printf("Particion %d | Inicio:%d | Tamaño:%d \n", i,(int)particion->inicio, particion->size);
	}

	void printear_suscriptor(void* id_suscriptor){
		printf("%d|",(int)id_suscriptor);
	}

	void printear_particion(void* stream){
		i++;
		t_partition* particion = stream;
		printf("Particion %d | Inicio:%d | Tamaño:%d | sockets confirmados [", i,(int)particion->inicio, particion->size);
		list_iterate(particion->suscriptores_confirmados,printear_suscriptor);
		printf("]\n");
	}


	printf("mem_alloc:%d\nParticiones Ocupadas\n-------------------------\n",(int)mem_alloc);

	for(int j=0; j<CANTIDAD_COLAS;j++){
		printf("Particiones Cola %s\n", cola_string(j));
		list_iterate(administracion_colas[j].particiones,printear_particion);
		printf("\n----------------------------\n");
		i=-1;
	}

	printf("Particiones Libres\n");

	list_iterate(particiones_libres,printear_particion_libre);
	printf("-------------------------------\n");

}

//============================== INICIALIZACION ==============================

void inicializar_memoria(){
	mem_alloc = malloc(TAMANO_MEMORIA);
	void* aux = mem_alloc;
	int var = 0;

	for(int i=0;i<TAMANO_MEMORIA;i++)
		memcpy(aux+i,&var,1);


	pthread_mutex_init(&mutex_particiones_libres, NULL);
	administracion_colas=malloc(sizeof(adm_cola)*6);
	for(int i=0;i<CANTIDAD_COLAS;i++){
		administracion_colas[i] = crear_adm_cola();
	}
	particiones_libres = list_create();
	list_add(particiones_libres, crear_particion_libre(mem_alloc,TAMANO_MEMORIA));
}

void inicializar_broker(){


	char *ip,*puerto,*algoritmo_particion_libre;
	int tamanio_memoria;

	id_mensajes_globales=0;

	pthread_t pthread_atender_cliente;
	pthread_mutex_init(&mutex_recibir, NULL);
	pthread_mutex_init(&mutex_enviar, NULL);

	mutex_cola_suscriptores = malloc(sizeof(pthread_mutex_t)*CANTIDAD_COLAS);
	for(int i=0;i<CANTIDAD_COLAS;i++)
		pthread_mutex_init(&mutex_cola_suscriptores[i], NULL);

	logger = log_create("../broker.log", "log", true, LOG_LEVEL_DEBUG);
	config = config_create("../config");

	suscriptores = malloc(sizeof(t_list)*CANTIDAD_COLAS);
	for(int i=0;i<CANTIDAD_COLAS;i++)
		suscriptores[i] = list_create();

	ip = config_get_string_value(config,"IP_BROKER");
	log_debug(logger,ip); //pido y logueo ip

    puerto = config_get_string_value(config,"PUERTO_BROKER");
	log_debug(logger,puerto); //pido y logueo puerto

	//revisar esto:
	if((socket_broker = listen_to(ip,puerto)) == -1)
		return;

	log_debug(logger,"Socket: %d, escuchando",socket_broker);	//Socket queda escuchado

	//.............................

	puts("Espera de 5s antes de comenzar a recibir sockets:\n");
	sleep(2);

	pthread_create(&pthread_atender_cliente, NULL,(void*)recibir_cliente, &socket_broker);
	pthread_detach(pthread_atender_cliente);
	log_debug(logger,"Recibi al cliente");	//Recibi al cliente

	//getchar();


    //-------------------------------------------


	tamanio_memoria= config_get_int_value(config,"TAMANO_MEMORIA");
	char* string_aux = string_itoa(tamanio_memoria);
	log_debug(logger, string_aux);
	algoritmo_particion_libre = config_get_string_value(config,"ALGORITMO_PARTICION_LIBRE");
	log_debug(logger,algoritmo_particion_libre);

	TAMANO_MEMORIA = tamanio_memoria;
	APL = interpretar_APL(algoritmo_particion_libre);

	free(string_aux);



	inicializar_memoria();
	printear_estado_memoria();
	/*
	 * cachear_mensaje para guardarlo en cache
	 * get_mensaje_cacheado para leer mensaje en cache
	 * descachear
	 */


}

int main(void) {



	inicializar_broker();
	sleep(2);

	pthread_t envio_mensaje_t;
	pthread_create(&envio_mensaje_t, NULL, (void*)envio_mensaje, NULL);
	sem_t esperar;
	sem_init(&esperar, 0,0);
	sem_wait(&esperar);
	//printf("cola appeared: %d\ncola caught: %d\ncola localized: %d\n", (int)list_get(sockets_cola_appeared,0),(int)list_get(sockets_cola_caught,0),(int)list_get(sockets_cola_localized,0));
	close(socket_broker);


	return EXIT_SUCCESS;
}
