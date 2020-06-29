/*	TODO: preguntas
 *
 * Que pasa con el bitmap si cambia la cantidad cantidad de bloques del sistema? se sobrescribe?
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
		if(metadata->opened == 'Y')
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

void debug_print_string_list(t_list* list){
	printf("UBICACIONES=[");
	for(int i=0;i<list_size(list);i++){
		if(i)
			printf(",");
		printf("%s",(char*)list_get(list,i));
	}
	printf("]\n");

}

void debug_print_bitarray(t_bitarray* bitarray){
	printf("Bitarray tam:%d\n",bitarray->size);
	for(int j=0 ; j < bitarray->size ; j++)
		printf("|%d|",bitarray_test_bit(bitarray,j));
	printf("\n");
}

char own_config_get_char_value(t_config* config, char *key){
	char* string = config_get_string_value(config, key);
	char caracter = string[0];
	//free(string);
	return caracter;
}

char** own_config_get_array_value(t_config* config, char *key){
	char* value_in_dictionary = config_get_string_value(config, key);
	if(!strcmp(value_in_dictionary,"[]")){
		free(value_in_dictionary);
		return NULL;
	}

	char** string_as_array = string_get_string_as_array(value_in_dictionary);
	free(value_in_dictionary);
	return string_as_array;
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




t_metadata* leer_archivo_metadata_y_notificar_apertura(char* ruta){

	char* aux = string_from_format(ruta);
	string_append(&aux,"/Metadata.bin");

	t_config* aux_metadata = config_create(aux);
	t_metadata* metadata = malloc(sizeof(t_metadata));

	metadata->directory = own_config_get_char_value(aux_metadata, "DIRECTORY");

	if(metadata->directory == 'N'){
		metadata->size = config_get_int_value(aux_metadata, "SIZE");
		metadata->blocks = list_create();
		char** blocks = own_config_get_array_value(aux_metadata, "BLOCKS");
		if(blocks){
			while(*blocks){ //blocks
				list_add(metadata->blocks,atoi(*blocks));
				free(*blocks);
				blocks++;
			}
		}

		//Si el file esta abierto debe ciclar aca hasta que aparezca como cerrado
		metadata->opened = own_config_get_char_value(aux_metadata, "OPEN");
	}

	config_destroy(aux_metadata); //todo por que rompe?
	free(aux);
	return metadata;
}

void escribir_archivo_metadata_y_cerrar(t_metadata* metadata, char* ruta){

	char* block_list_to_array(t_list* block_list){
		char* string = string_from_format("[");
		for(int i=0;i<list_size(block_list);i++){
			if(i)
				string_append(&string,",");
			printf("for iteracion %d\n",i);

			string_append(&string,string_itoa((int)list_get(block_list,i)));
			printf("for iteracion %d\n",i);
		}
		string_append(&string,"]");
		printf("EL STRING DE BLOQUES ES: %s\n",string);

		return string;
	}

	char* aux = string_from_format(ruta);
	string_append(&aux,"/Metadata.bin");

	t_config* aux_metadata = config_create(aux);

	config_set_value(aux_metadata,"SIZE",string_itoa(metadata->size));
	config_set_value(aux_metadata,"BLOCKS",block_list_to_array(metadata->blocks));

	config_save_in_file(aux_metadata,aux);

	config_destroy(aux_metadata);
	free(aux);

}

char* generar_ruta(char* nombre_pokemon){
	char* ruta = string_new();
	string_append(&ruta,punto_montaje);
	string_append(&ruta,"/Files/");
	string_append(&ruta,nombre_pokemon);
	return ruta;
}

bool directorioExiste(char* ruta){
	struct stat st = {0}; // stat(2) precisa de un struct stat a modo de buffer para llenar info del archivo que nosotros no necesitamos.
	char* aux = string_from_format(ruta);
	bool existe = (stat(aux,&st) != -1);
	free(aux);
	return existe;

}

void verificar_y_crear_pokemon_files(t_mensaje* mensaje){

	char* aux = generar_ruta(mensaje->contenido.new_pokemon.pokemon->nombre);

	if(!directorioExiste(aux)){
		mkdir(aux,0700);
		string_append(&aux,"/Metadata.bin");
		FILE* new_archivo_metadata=fopen(aux,"w");
		fprintf(new_archivo_metadata, "DIRECTORY=N\nSIZE=0\nBLOCKS=[]\nOPEN=N");
		fclose(new_archivo_metadata);
	}else
		printf("Se encuentra un directorio pokemon para %s\n",mensaje->contenido.new_pokemon.pokemon->nombre);

	free(aux);

}

int bitarray_pedir_bloque(){
	pthread_mutex_lock(&mutex_bitmap);
	for (int i=0 ; i < bitarray_get_max_bit(bitmap) ; i++ ){
		if(!bitarray_test_bit(bitmap,i)){
			bitarray_set_bit(bitmap,i);
			//mysync()
			pthread_mutex_unlock(&mutex_bitmap);
			return i;
		}
	}
	pthread_mutex_unlock(&mutex_bitmap);
	printf("No hay bloques libres!\n");
	return -1;
}

void bitarray_devolver_bloque(int bloque){
	pthread_mutex_lock(&mutex_bitmap);
	bitarray_set_bit(bitmap,bloque);
	//mysync()
	pthread_mutex_unlock(&mutex_bitmap);
}



char* mensaje_to_pokedata(t_mensaje* mensaje){
	char* pokedata = string_new();
	int cod = mensaje->codigo_operacion;
	switch(cod){
		case NEW_POKEMON:;
			int pos_x = mensaje->contenido.new_pokemon.pokemon->pos_x;
			int pos_y = mensaje->contenido.new_pokemon.pokemon->pos_y;
			int cantidad = mensaje->contenido.new_pokemon.cantidad;

			string_append_with_format(&pokedata,"%d-%d=%d",pos_x,pos_y,cantidad);

			break;
		default:
			printf("INVALID CODE_OP\n");
			free(pokedata);
			return NULL;
	}

	return pokedata;
}

void modificar_poke_string_list(t_list* poke_strings_list, t_mensaje* mensaje){

	bool misma_posicion(char* pok_1, char* pok_2){
		char* posicion = * string_n_split(pok_1,2,"=");
		bool retorno = string_starts_with(pok_2,posicion);
		free(posicion);
		return retorno;
	}

	char* incrementar_cantidad(char* pok_1, char* pok_2){
		char **datos_pok1 = string_n_split(pok_1,2,"=");
		char **datos_pok2 = string_n_split(pok_2,2,"=");

		int cantidad_1 = atoi(datos_pok1[1]);
		int cantidad_2 = atoi(datos_pok2[1]);

		char* pok_aux = string_from_format("%s=%d",datos_pok1[0],cantidad_1+cantidad_2);

		free(datos_pok1[0]);
		free(datos_pok1[1]);
		free(datos_pok1);

		free(datos_pok2[0]);
		free(datos_pok2[1]);
		free(datos_pok2);

		return pok_aux;

	}

	char* data = mensaje_to_pokedata(mensaje);

	int cod = mensaje->codigo_operacion;
	switch(cod){
	case NEW_POKEMON:
		for (int i = 0 ; i < list_size(poke_strings_list) ; i++){
			if(misma_posicion(list_get(poke_strings_list,i),data)){
				char* old_pok = list_remove(poke_strings_list,i);
				char* new_pok = incrementar_cantidad(old_pok,data);

				//free(old_pok); //no se porque rompe esto
				list_add(poke_strings_list,new_pok);
				return;
			}
		}
		list_add(poke_strings_list,data);
		break;
	default:
		printf("INVALID CODE_OP\n");
		return;
	}
}

char* poke_list_a_poke_string(t_list* list){
	char* aux = string_new();

	for(int i=0 ; i<list_size(list) ; i++)
		string_append_with_format(&aux,"%s\n",list_get(list,i));

	return aux;
}

int cantidad_bloques_necesarios(char* str){
	int tam_bloque = 64; //todo tam_bloque hardcodeado, leerlo de la config!
	int tam_str = strlen(str);

	return (tam_str + (tam_bloque-1)) / tam_bloque; // la division es asi para que haga el redondeo para arriba
}


FILE* abrir_bloque(int num_bloque,char* open_mode){
	char* ruta = string_from_format("%s/Blocks/%d.bin",punto_montaje,num_bloque);
	printf("Intento de apertura en modo %s path: %s\n",open_mode,ruta);

	FILE* file = fopen(ruta,open_mode);
	free(ruta);
	return file;
}



//recibe una t_list de int bloques y devuelve un t_list con su contenido:["1-1=4",...,"5-7=1"]
t_list* pokemon_data_de_bloques(t_list* bloques){

	t_list* poke_data = list_create();
	void* poke_data_string = string_new();

	for(int i=0 ; i < list_size(bloques) ; i++){

		FILE* block_file = abrir_bloque((int)list_get(bloques,i),"r"); //todo revisar

		fseek(block_file,0,SEEK_END);
		int block_size = ftell(block_file);
		fseek(block_file,0,SEEK_SET); //rewind(FILE* f);

		fread(poke_data_string,block_size,1,block_file);

		fclose(block_file);

	}

	if(!list_is_empty(bloques)){
		char** poke_strings = string_split(poke_data_string,"\n");
		while (*poke_strings){
			list_add(poke_data,*poke_strings);
			free(*poke_strings); // ojo aca!
			poke_strings++;
		}
	}

	free(poke_data_string); //ojo aca tambien!

	return poke_data;

}

//[1]
//"1234567\n"

void escribir_bloques(t_list* blocklist,char* data){

	char* data_aux = string_from_format(data);
	int remaining_data = strlen(data_aux);
	int sent_data = 0;

	for(int i=0 ; i<list_size(blocklist) ; i++){

		int num_bloque = (int) list_get(blocklist,i);
		FILE *file_stream = abrir_bloque(num_bloque,"w");

		if(remaining_data >= global_metadata->block_size){
			char* data_que_falta_enviar = string_substring_from(data_aux,sent_data); //sent_data+1?
			fwrite(data_que_falta_enviar,global_metadata->block_size,1,file_stream);
			remaining_data -= global_metadata->block_size;
			sent_data += global_metadata->block_size;
			free(data_que_falta_enviar);
		}else{
			char* data_que_falta_enviar = string_substring_from(data_aux,sent_data); //sent_data+1?
			fwrite(data_que_falta_enviar,1,remaining_data,file_stream); // TODO aca sin valgrind no funciona >:C
			free(data_que_falta_enviar);
		}

		fclose(file_stream);
	}

	free(data_aux);

}




/*
 Verificar si las posiciones ya existen dentro del archivo.
  En caso de existir, se deben agregar la cantidad pasada por mensaje
  a la actual. En caso de no existir se debe agregar al final del archivo
  una nueva línea indicando la cantidad de pokémon pasadas.
 */

void agregar_pokemones(t_mensaje* mensaje){
	char* ruta = generar_ruta(mensaje->contenido.new_pokemon.pokemon->nombre);

	t_metadata* metadata;
	metadata = leer_archivo_metadata_y_notificar_apertura(ruta); //solicitar apertura aqui
	debug_print_metadata(metadata);

	t_list* poke_strings_list = pokemon_data_de_bloques(metadata->blocks);

	modificar_poke_string_list(poke_strings_list,mensaje);

	/* En este punto se comianza a revisar segun la cantidad de bloques
	 * que tenga asignado el metadata, se elalua si esa cantidad se debe aumentar
	 * o se puede disminuir, y se procede a hacer eso interactuando con el bitmap
	 */

	// hacer funcion pasar poke_list_a_poke_string

	char* poke_string = poke_list_a_poke_string(poke_strings_list);
	int cant_bloq_necesarios = cantidad_bloques_necesarios(poke_string);

	if(cant_bloq_necesarios > list_size(metadata->blocks)){
		list_add(metadata->blocks,(void*)bitarray_pedir_bloque());
	}
	else if(cant_bloq_necesarios < list_size(metadata->blocks))
		bitarray_devolver_bloque( (int) list_remove(metadata->blocks,list_size(metadata->blocks)-1));

	escribir_bloques(metadata->blocks,poke_string);

	escribir_archivo_metadata_y_cerrar(metadata,ruta);


	//poner en N el open del metadata


	free(ruta);
}




void recibir_new(t_mensaje *mensaje){//TODO


	verificar_y_crear_pokemon_files(mensaje);

	agregar_pokemones(mensaje);

	liberar_mensaje(mensaje);

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
			recibir_new(mensaje); //crear hilo que haga recibir_new(mensaje) y que muera al finalizar
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
	char* path = string_new();
	string_append_with_format(&path, "%s/Blocks", punto_montaje);
	if(!directorioExiste(path)){
		printf("No se crean los bloques porque no existe el directorio %s\n",path);
		return;
	}
	for(int i=0; i < global_metadata->blocks ;i++){
		char* path_relative = string_duplicate(path);
		string_append_with_format(&path_relative, "/%d.bin",i);
		FILE* f = fopen(path_relative, "a");
		free(path_relative);
		fclose(f);
	}
	free(path);
	printf("Se crearon los bloques!\n");
}



t_bitarray* crear_bitarray_y_mapear(){

	int fd = open("../mnt/TALL_GRASS/Metadata/Bitmap.bin", O_CREAT | O_RDWR, 0664); //todo hacer que sea con puntomontaje

	if (fd == -1) {
		perror("open file");
		exit(1);
	}

	ftruncate(fd, global_metadata->blocks);

	void* bmap = mmap(NULL, global_metadata->blocks, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (bmap == MAP_FAILED) {
		perror("mmap");
		close(fd);
		exit(1);
	}

	t_bitarray* bitmap = bitarray_create_with_mode((char*) bmap, global_metadata->blocks, LSB_FIRST);

	close(fd);
//	munmap(bmap, size);

	return bitmap;
}


void inicializar_gamecard() {
	logger = log_create("../gamecard.log","log",1,LOG_LEVEL_DEBUG);
	leer_config();
	leer_metadata_global();
	//crear_bloques();
	bitmap = crear_bitarray_y_mapear();

//TODO
	pthread_t pthread_cola_new;
	pthread_t pthread_cola_catch;
	pthread_t pthread_cola_get;


	pthread_mutex_init(&mutex_bitmap, NULL);
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
	munmap(bitmap, global_metadata->blocks);
	config_destroy(config);
	free(global_metadata);
	bitarray_destroy(bitmap);
	//close(socket_cola_new);
}

void mostrar_menu(){
	puts("-----------MENU -----------");
	puts("INGRESE UN VALOR");
	puts("M -> MOSTRAR MENU");
	puts("N -> SIMULAR NEW SQUIRTLE 4-7=455");
	puts("R -> SIMULAR NEW SQUIRTLE DATOS RANDOM");
	puts("B -> PRINT ESTADO BITARRAY");
	puts("C -> BITARRAY CLEAN, OJO CON ESTO QUE ESTA MAPEADO A MEMORIA NO APRETAR 'C' SI NO ESTAS SEGURO DE LO QUE HACES LCDTM");
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
				mensaje = crear_mensaje(5, NEW_POKEMON,pokemon->nombre,pokemon->pos_x,pokemon->pos_y,455);
				recibir_new(mensaje);
				break;
			case 'R':
				srand(time(NULL));
				pokemon = crear_pokemon("Squirtle",rand()%10,rand()%10);
				mensaje = crear_mensaje(5, NEW_POKEMON,pokemon->nombre,pokemon->pos_x,pokemon->pos_y,rand()%500);
				printf("Se recibe un new random del siguiente mensaje: \n");
				printear_mensaje(mensaje);
				recibir_new(mensaje);
				break;
			case 'Z':
				puts("----- SE FINALIZA EL PROCESO... -----");
				return;
			case 'C':;
				size_t tope = bitarray_get_max_bit(bitmap);
				for(int i = 0; i < tope; i++)
					 bitarray_clean_bit(bitmap, i);
				debug_print_bitarray(bitmap);
				break;
			case 'B':
				debug_print_bitarray(bitmap);
				break;

		}
	}
}

int main() {
	inicializar_gamecard();

	debug();

	cerrar_gamecard();

/* DEBUGGING!

	t_list* poke_debug = list_create();
	list_add(poke_debug,"1-1=7");
	list_add(poke_debug,"4-7=3");
	list_add(poke_debug,"9-1=4");
	list_add(poke_debug,"2-1=5");


	t_pokemon* pokemon;
	t_mensaje* mensaje;

	pokemon = crear_pokemon("Squirtle",4,7);
	mensaje = crear_mensaje(5, NEW_POKEMON,pokemon->nombre,pokemon->pos_x,pokemon->pos_y,455);

	printf("Lista antes de modificacion:\n");
	debug_print_string_list(poke_debug);

	modificar_poke_string_list(poke_debug,mensaje);

	printf("Lista despues de modificacion:\n");
	debug_print_string_list(poke_debug);

	char *stringlist = poke_list_a_poke_string(poke_debug);

	printf("stringlist:\n%s\n",stringlist);
	printf("size: %d\n",strlen(stringlist));

	printf("necesito %d bloques para alocarlo\n", cantidad_bloques_necesarios(stringlist));

*/
	return EXIT_SUCCESS;
}
