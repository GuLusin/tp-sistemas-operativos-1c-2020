#include "gamecard.h"

int subscribirse_a_cola(cola_code cola){
	int socket_aux = connect_to(ip_broker,puerto_broker, tiempo_reintento_conexion, logger);
	t_mensaje* mensaje = crear_mensaje(2, SUBSCRIPCION, cola);
	mensaje->id=ID_SUSCRIPCION;
	enviar_mensaje(socket_aux, mensaje);
	check_ack(socket_aux, ACK);
	return socket_aux;
}

void leer_config() {
	config = config_create("../config");
	tiempo_reintento_conexion = config_get_int_value(config,"TIEMPO_DE_REINTENTO_CONEXION");
	tiempo_reintento_operacion = config_get_int_value(config,"TIEMPO_DE_REINTENTO_OPERACION");
	tiempo_retardo_operacion = config_get_int_value(config,"TIEMPO_RETARDO_OPERACION");
	ip_broker = config_get_string_value(config,"IP_BROKER");
	log_debug(logger,config_get_string_value(config,"IP_BROKER")); //pido y logueo ip
	puerto_broker = config_get_string_value(config,"PUERTO_BROKER");
	log_debug(logger,config_get_string_value(config,"PUERTO_BROKER")); //pido y logueo puerto
	punto_montaje = config_get_string_value(config,"PUNTO_MONTAJE_TALLGRASS");
	//log_debug(logger,config_get_string_value(config,"PUNTO_MONTAJE_TALLGRASS")); // Ver si es necesario loggear esto
}

void recibir_new(t_mensaje *mensaje){
	char* ruta = generar_ruta_metadata(mensaje->contenido.new_pokemon.pokemon->nombre); //puede fallar
	if(!verificar_si_existe(ruta)){
		crear_dir_pokemon(mensaje->contenido.new_pokemon.pokemon->nombre, ruta);
	}
	t_metadata* metadata;
	metadata = leer_archivo_metadata(ruta);
	agregar_pokemones(metadata);

	/*
	 Verificar si las posiciones ya existen dentro del archivo.
	  En caso de existir, se deben agregar la cantidad pasada por parámetro
	   a la actual. En caso de no existir se debe agregar al final del archivo
	    una nueva línea indicando la cantidad de pokémon pasadas.
	 */
}

void recibir_get(t_mensaje *mensaje){


}

void recibir_catch(t_mensaje *mensaje){


}

int verificar_si_existe(char* ruta){ //podria fijarse en lista global si esta el pokemon
	FILE *archivo_pokemon = txt_open_for_append(ruta);
	int existe=1;
	if(!archivo_pokemon){
		existe = 0;
	}
	txt_close(archivo_pokemon);
	return existe;
}

t_metadata* leer_archivo_metadata(char* ruta){//TODO falla :(
	t_config* aux_metadata = config_create(ruta);
	t_metadata* metadata;
	metadata->directory = config_get_int_value(aux_metadata, "DIRECTORY");
	if(metadata->directory == 'N'){
		metadata->size = config_get_int_value(aux_metadata, "SIZE");
		metadata->blocks = config_get_array_value(aux_metadata, "BLOCKS");
		metadata->open = config_get_int_value(aux_metadata, "OPEN");
	}
	config_destroy(aux_metadata);
	return metadata;
}

char* generar_ruta_metadata(char* nombre_pokemon){//TODO falla :(
	char* ruta = new_string();
	string_append(&ruta,punto_montaje);
	string_append(&ruta,"/Files/");//ver si hace falta capitalizar con: string_capitalized(char *text)
	string_append(&ruta,nombre_pokemon);
	string_append(&ruta,"/Metadata.bin");
	return ruta;
}

void crear_dir_pokemon(char* nombre_pokemon, char* ruta){

}

void manejar_mensaje(t_mensaje* mensaje){
	puts("maneja mensaje");
	switch(mensaje->codigo_operacion){
		case NEW_POKEMON:
			recibir_new(mensaje);
			break;
		case CATCH_POKEMON:;
		//TODO implementar logica

			break;
		case GET_POKEMON:;
		//TODO implementar logica

			break;
		default:

			break;
	}
	puts("sale de manejar mensaje");
}

bool recibir_mensaje(int un_socket){
	uint32_t codigo_operacion,id,size_contenido_mensaje;
	if(recv(un_socket,&codigo_operacion,sizeof(uint32_t),0)<=0)
		return false;

	if(recv(un_socket,&id,sizeof(uint32_t),0)<=0)
		return false;

	if(recv(un_socket,&size_contenido_mensaje,sizeof(uint32_t),0)<=0)
		return false;

	void* stream = malloc(size_contenido_mensaje);

	if(recv(un_socket, stream, size_contenido_mensaje, 0)<=0)
		return false;

	send_ack(un_socket,ACK);

	t_mensaje* mensaje = deserializar_mensaje(codigo_operacion, stream);
	mensaje->id=id;
	printear_mensaje(mensaje);
	manejar_mensaje(mensaje);
	return true;
}


void protocolo_recibir_mensaje(cola_code cola){

	while(true){
		pthread_mutex_lock(&mutex_recibir);

		int socket_cola = subscribirse_a_cola(cola);

		pthread_mutex_unlock(&mutex_recibir);

		printf("socket_suscripcion:%d\n",socket_cola);
		while(recibir_mensaje(socket_cola));
		close(socket_cola);
	}
}

void inicializar_gamecard() { // Suscribirse a NEW_POKEMON, CATCH_POKEMON, GET_POKEMON
	logger = log_create("../gamecard.log","log",1,LOG_LEVEL_DEBUG);
	leer_config();

	pthread_t pthread_cola_new;
	pthread_t pthread_cola_catch;
	pthread_t pthread_cola_get;

    pthread_mutex_init(&mutex_recibir, NULL);

	pthread_create(&pthread_cola_new, NULL,(void*)protocolo_recibir_mensaje, COLA_NEW_POKEMON);
	pthread_detach(pthread_cola_new);

	pthread_create(&pthread_cola_catch, NULL,(void*)protocolo_recibir_mensaje, COLA_CATCH_POKEMON);
	pthread_detach(pthread_cola_catch);

	pthread_create(&pthread_cola_get, NULL,(void*)protocolo_recibir_mensaje, COLA_GET_POKEMON);
	pthread_detach(pthread_cola_get);
}

void cerrar_gamecard(){
	log_destroy(logger);
	config_destroy(config);
	close(socket_cola_new);
}

int main() {
	inicializar_gamecard();
	getchar();
	cerrar_gamecard();
	return EXIT_SUCCESS;
}
