/*	TODO: preguntas
 *
 * Que pasa con el bitmap si cambia la cantidad cantidad de bloques del sistema? se sobrescribe?
 * Puede que al inicio el bitmap no este todo en 0? que hacemos si pasa eso?
 *
 *
 *
 *
 *
 *
 */







#include "gamecard.h"

void debug_print_metadata(t_metadata* metadata){
	printf("debug_print_metadata:\n");
	if(metadata->directory == 'N'){
		printf("DIRECTORY=N\n");
		printf("SIZE=%d\n",metadata->size);
		printf("BLOCKS=[");
		for(int i=0;i<list_size(metadata->blocks);i++){
			if(i)
				printf(",");
			printf("%d",(int)list_get(metadata->blocks,i));
		}
		printf("]\n");
		if(metadata->open == 'Y')
			printf("OPEN=Y\n");
		else
			printf("OPEN=N\n");
		return;
	}
	if(metadata->directory == 'Y'){
		printf("DIRECTORY=Y\n");
		return;
	}
	printf("metadata invalido\n");

}

char own_config_get_char_value(t_config* config, char *key){
	char* string = config_get_string_value(config, key);
	char caracter = string[0];
	free(string);
	return caracter;
}

char** own_config_get_array_value(t_config* config, char *key){
	char* value_in_dictionary = config_get_string_value(config, key);
	if(!strcmp(value_in_dictionary,"[]")){
		return NULL;
	}
	return string_get_string_as_array(value_in_dictionary);
}


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
	char *aux = malloc(strlen(ruta)); // +1 ?
	strcpy(aux,ruta);
	string_append(&aux,"/Metadata.bin");

	t_config* aux_metadata = config_create(aux);
	t_metadata* metadata = malloc(sizeof(t_metadata));

	printf("adsadas\n");
	metadata->directory = own_config_get_char_value(aux_metadata, "DIRECTORY");
	printf("metadata->directory == %d\n",metadata->directory);

	if(metadata->directory == 'N'){
		metadata->size = config_get_int_value(aux_metadata, "SIZE");

		printf("1\n");
		metadata->blocks = list_create();
		printf("2\n");
		char** blocks = own_config_get_array_value(aux_metadata, "BLOCKS");
		printf("3\n");
		while(blocks){ //blocks
			list_add(metadata->blocks,atoi(*blocks));
			free(*blocks);
			blocks++;
		}
		printf("4\n");

		metadata->open = config_get_int_value(aux_metadata, "OPEN");

	}

	config_destroy(aux_metadata);

	free(aux);
	return metadata;
}


char* generar_ruta(char* nombre_pokemon){
	char* ruta = string_new();
	string_append(&ruta,punto_montaje);
	string_append(&ruta,"/Files/");//ver si hace falta capitalizar con: string_capitalized(char *text)
	string_append(&ruta,nombre_pokemon);
	return ruta;
}

void verificar_y_crear_pokemon_files(char* ruta, t_mensaje* mensaje){
	struct stat st = {0}; // stat(2) precisa de un struct stat a modo de buffer para llenar info del archivo que nosotros no necesitamos.
	char* aux = malloc(strlen(ruta));
	strcpy(aux,ruta);

	if(stat(aux,&st) == -1){
		printf("Se crea un directorio pokemon para %s con su respectivo metadata.bin en %s\n",mensaje->contenido.new_pokemon.pokemon->nombre,aux);
		mkdir(ruta,0700);
		string_append(&aux,"/Metadata.bin");
		FILE* new_archivo_metadata=fopen(aux,"w");
		fprintf(new_archivo_metadata, "DIRECTORY=N\nSIZE=0\nBLOCKS=[]\nOPEN=N");
		fclose(new_archivo_metadata);
	}else
		printf("Se encuentra un directorio pokemon para %s\n",mensaje->contenido.new_pokemon.pokemon->nombre);

	free(aux);

}
void recibir_new(t_mensaje *mensaje){//TODO
	char* ruta = generar_ruta(mensaje->contenido.new_pokemon.pokemon->nombre); //puede fallar

	verificar_y_crear_pokemon_files(ruta,mensaje);

	t_metadata* metadata;
	metadata = leer_archivo_metadata(ruta);
	debug_print_metadata(metadata);
	sleep(10);

	//agregar_pokemones(metadata);

	/*
	 Verificar si las posiciones ya existen dentro del archivo.
	  En caso de existir, se deben agregar la cantidad pasada por parámetro
	   a la actual. En caso de no existir se debe agregar al final del archivo
	    una nueva línea indicando la cantidad de pokémon pasadas.
	 */
	liberar_mensaje(mensaje);
	free(ruta);
	//free(metadata);
}

void recibir_get(t_mensaje *mensaje){


}


void recibir_catch(t_mensaje *mensaje){

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
	free(ruta);
}

void crear_bloques(){
	blocks = malloc((global_metadata->blocks) * sizeof(FILE*)); //tamaño de un file: 148 bytes y de un punt file: 4 bytes
	char* path = string_new();
	string_append_with_format(&path, "%s/Blocks/", punto_montaje);
	for(int i=0; i < global_metadata->blocks ;i++){
		char* path_relative = string_duplicate(path);
		string_append_with_format(&path_relative, "%d.bin",i);
		blocks[i]=fopen(path_relative, "a");
		free(path_relative);
	}
	free(path);
	printf("Se crearon los bloques!\n");
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

void mostrar_menu(){
	puts("-----------MENU -----------");
	puts("INGRESE UN VALOR");
	puts("M -> MOSTRAR MENU");
	puts("N -> SIMULAR NEW SQUIRTLE");
	puts("Z -> SALIR");
	puts("---------------------------");
}

void debug(){
	mostrar_menu();

	t_pokemon* pokemon;
	t_mensaje* mensaje;

	while(true){
		char msg;
		scanf("%c",&msg);
		switch(msg){
			case 'M':
				mostrar_menu();
				break;
			case 'N':
				pokemon = crear_pokemon("Squirtle",4,7);
				mensaje = crear_mensaje(5, NEW_POKEMON,pokemon->nombre,pokemon->pos_x,pokemon->pos_y,455);;
				recibir_new(mensaje);
				break;
			case 'F':
				puts("----- SE FINALIZA EL PROCESO... -----");
				return;
		}
	}
}



int main() {
	inicializar_gamecard();

	debug();

	cerrar_gamecard();
	return EXIT_SUCCESS;
}
