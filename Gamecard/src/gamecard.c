#include "gamecard.h"

int subscribirse_a_cola(cola_code cola){
	int socket_aux = connect_to(ip_broker,puerto_broker, tiempo_reintento_conexion, logger);
	t_mensaje* mensaje = crear_mensaje(2, SUBSCRIPCION, cola);
	mensaje->id=id_gamecard;
	enviar_mensaje(socket_aux, mensaje);
	check_ack(socket_aux, ACK);
	return socket_aux;
}

void leer_config() {
	config = config_create("../config");
	tiempo_reintento_conexion = config_get_int_value(config,"TIEMPO_DE_REINTENTO_CONEXION");
	log_debug(logger,config_get_string_value(config,"TIEMPO_DE_REINTENTO_CONEXION"));

	tiempo_reintento_operacion = config_get_int_value(config,"TIEMPO_DE_REINTENTO_OPERACION");
	log_debug(logger,config_get_string_value(config,"TIEMPO_DE_REINTENTO_OPERACION"));

	tiempo_retardo_operacion = config_get_int_value(config,"TIEMPO_RETARDO_OPERACION");
	log_debug(logger,config_get_string_value(config,"TIEMPO_RETARDO_OPERACION"));

	ip_broker = config_get_string_value(config,"IP_BROKER");
	log_debug(logger,config_get_string_value(config,"IP_BROKER"));

	puerto_broker = config_get_string_value(config,"PUERTO_BROKER");
	log_debug(logger,config_get_string_value(config,"PUERTO_BROKER"));

	punto_montaje = config_get_string_value(config,"PUNTO_MONTAJE_TALLGRASS");
	log_debug(logger,config_get_string_value(config,"PUNTO_MONTAJE_TALLGRASS"));

	id_gamecard = config_get_int_value(config, "ID_GAMECARD");
	log_debug(logger,config_get_string_value(config,"ID_GAMECARD"));
}

t_metadata* leer_archivo_metadata(char* ruta){
	printf("%s\n", ruta);
	t_config* aux_metadata = config_create(ruta);
	printf("1\n");
	t_metadata* metadata = malloc(sizeof(t_metadata));
	printf("1.1\n");
	metadata->directory = config_get_int_value(aux_metadata, "DIRECTORY");
	printf("2\n");

	if(metadata->directory == 'N'){
		printf("3\n");

		metadata->size = config_get_int_value(aux_metadata, "SIZE");
		metadata->blocks = config_get_array_value(aux_metadata, "BLOCKS");
		metadata->open = config_get_int_value(aux_metadata, "OPEN");
		printf("3\n");

	}
	printf("4\n");

	config_destroy(aux_metadata);
	printf("5\n");

	return metadata;
}


char* generar_ruta(char* nombre_pokemon){
	char* ruta = string_new();
	string_append(&ruta,punto_montaje);
	string_append(&ruta,"/Files/");//ver si hace falta capitalizar con: string_capitalized(char *text)
	string_append(&ruta,nombre_pokemon);
	return ruta;
}

void verificar_y_crear_pokemon_files(char*ruta, t_mensaje* mensaje){
	struct stat st = {0};
	if((stat(ruta,&st) != -1)){
		mkdir(ruta,0700);
		string_append(&ruta,"/Metadata.bin");
		FILE* new_archivo_metadata=fopen(ruta,"w");
		fprintf(new_archivo_metadata, "DIRECTORY=N\nSIZE=0\nBLOCKS=[]\nOPEN=N");
		fclose(new_archivo_metadata);

	}else{
		string_append(&ruta,"/Metadata.bin");
	}

}
void recibir_new(t_mensaje *mensaje){//TODO
	char* ruta = generar_ruta(mensaje->contenido.new_pokemon.pokemon->nombre); //puede fallar
	printf("Sale de generar ruta con: %s\n", ruta);
	verificar_y_crear_pokemon_files(ruta,mensaje);
	printf("linea 65 %s\n", ruta);

	t_metadata* metadata;
	fflush(stdout);
	printf("%s\n",ruta);

	metadata = leer_archivo_metadata(ruta);

	//agregar_pokemones(metadata);

	/*
	 Verificar si las posiciones ya existen dentro del archivo.
	  En caso de existir, se deben agregar la cantidad pasada por parámetro
	   a la actual. En caso de no existir se debe agregar al final del archivo
	    una nueva línea indicando la cantidad de pokémon pasadas.
	 */
	free(ruta);
	free(metadata);
}

void recibir_get(t_mensaje *mensaje){


}


void recibir_catch(t_mensaje *mensaje){

}



int verificar_si_existe(char* ruta){ //podria fijarse en lista global si esta el pokemon


	/*
	FILE *archivo_pokemon = fopen(ruta, "r");
	int existe=1;
	if(!archivo_pokemon){
		existe = 0;
	}
	fclose(archivo_pokemon);
	return existe;*/
}


void crear_dir_pokemon(char* nombre_pokemon, char* ruta){
	//crear carpeta y archivo del pokemon

}/*
DIRECTORY=N
SIZE=0
BLOCKS=[]
OPEN=N
*/
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
	free(mensaje);
	free(stream);
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


void leer_metadata_global(){
	char* ruta = string_new();
	global_metadata = malloc(sizeof(t_global_metadata));
	string_append_with_format(&ruta, "%s/Metadata/Metadata.bin", punto_montaje);
	t_config* aux_metadata = config_create(ruta);
	global_metadata->block_size = config_get_int_value(aux_metadata, "BLOCK_SIZE");
	global_metadata->blocks = config_get_int_value(aux_metadata, "BLOCKS");
	global_metadata->magic_number = config_get_string_value(aux_metadata, "MAGIC_NUMBER");
	config_destroy(aux_metadata);
}

void crear_bloques(){
	blocks = malloc((global_metadata->blocks) * sizeof(FILE*));//tamaño de un file: 148 bytes y de un punt file: 4 bytes
	char* path = string_new();
	string_append_with_format(&path, "%s/Blocks/", punto_montaje);
	for(int i=0; i < global_metadata->blocks ;i++){
		char* path_relative = string_duplicate(path);
		string_append_with_format(&path_relative, "%d.bin",i);
		blocks[i]=fopen(path_relative, "a");
		free(path_relative);
	}
}

void inicializar_gamecard() {
	logger = log_create("../gamecard.log","log",1,LOG_LEVEL_DEBUG);
	leer_config();
	leer_metadata_global();
	crear_bloques();
	bitmap = bitarray_create(malloc(global_metadata->blocks), global_metadata->blocks);//para saber que blocks estan abiertos/cerrados
//TODO
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
	for(int i=0; i < global_metadata->blocks; i++){
		fclose(blocks[i]);
	}
	free(global_metadata);
	free(blocks);
	bitarray_destroy(bitmap);
	//close(socket_cola_new);
}

int main() {
	inicializar_gamecard();
	getchar();
	cerrar_gamecard();
	return EXIT_SUCCESS;
}
