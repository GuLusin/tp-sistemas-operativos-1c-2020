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
		//free(value_in_dictionary);
		return NULL;
	}
	return config_get_array_value(config,key);
}

void liberar_argpointer(char** array){
	char **a;

	for (a = array ; *a ; a++)
		free (*a);

	free(array);

}


int subscribirse_a_cola(cola_code cola){
	int socket_aux = connect_to(ip_broker,puerto_broker, tiempo_reintento_conexion, logger);
	t_mensaje* mensaje = crear_mensaje(2, SUBSCRIPCION, cola);
	mensaje->id=id_gamecard;
	enviar_mensaje(socket_aux, mensaje);
	check_ack(socket_aux, ACK);
	liberar_mensaje(mensaje);
	return socket_aux;
}

t_metadata* leer_archivo_metadata_y_notificar_apertura(char* ruta){

	char* aux = string_from_format("%s/Metadata.bin",ruta);

	t_config* aux_metadata = config_create(aux);
	t_metadata* metadata = malloc(sizeof(t_metadata));

	metadata->directory = own_config_get_char_value(aux_metadata, "DIRECTORY");

	if(metadata->directory == 'N'){
		metadata->size = config_get_int_value(aux_metadata, "SIZE");
		metadata->blocks = list_create();
		char** blocks = own_config_get_array_value(aux_metadata, "BLOCKS");
		if(blocks){
			for(char** aux = blocks ; *aux ; aux++)
				list_add(metadata->blocks,(void*)atoi(*aux));
			liberar_argpointer(blocks);
		}

		metadata->opened = own_config_get_char_value(aux_metadata, "OPEN");

		if(metadata->opened == 'Y'){
			printf("No se puede abrir el file ya estaba abierto, reintentando\n");
			config_destroy(aux_metadata);
			free(aux);
			free(metadata);
			return NULL;
		}
		config_set_value(aux_metadata,"OPEN","Y");
		config_save_in_file(aux_metadata,aux);
	}

	config_destroy(aux_metadata);
	free(aux);
	return metadata;
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

t_metadata* leer_metadata(char* ruta){
	pthread_mutex_lock(&mutex_metadata);
	t_metadata* metadata = leer_archivo_metadata_y_notificar_apertura(ruta);
	pthread_mutex_unlock(&mutex_metadata);
	while(!metadata){
		log_debug(logger,"El archivo de ruta: %s, estaba abierto, se espera %d segundos para volver a intentar abrir...",ruta,tiempo_reintento_operacion);
		sleep(tiempo_reintento_operacion);
		pthread_mutex_lock(&mutex_metadata);
		metadata = leer_archivo_metadata_y_notificar_apertura(ruta);
		pthread_mutex_unlock(&mutex_metadata);
	}
	return metadata;
}

void escribir_archivo_metadata_y_cerrar(t_metadata* metadata, char* ruta){

	char* block_list_to_array(t_list* block_list){
		char* string = string_from_format("[");
		for(int i=0;i<list_size(block_list);i++){
			if(i)
				string_append(&string,",");
			char* _bloque = string_itoa((int)list_get(block_list,i));
			string_append(&string,_bloque);
			free(_bloque);
		}
		string_append(&string,"]");
		return string;
	}

	char* aux = string_from_format(ruta);
	string_append(&aux,"/Metadata.bin");

	//debug_print_metadata(metadata);

	log_debug(logger,"Sleep de %d segundos para simular tiempo de operacion...",tiempo_retardo_operacion);
	sleep(tiempo_retardo_operacion);

	t_config* aux_metadata = config_create(aux);

	char* block_list_string = block_list_to_array(metadata->blocks);
	char* size = string_itoa(metadata->size);

	config_set_value(aux_metadata,"SIZE",size);
	config_set_value(aux_metadata,"BLOCKS",block_list_string);
	config_set_value(aux_metadata,"OPEN","N");

	free(size);
	free(block_list_string);

	config_save_in_file(aux_metadata,aux);

	printf("Se cerro el archivo de %s\n",aux);
	config_destroy(aux_metadata);
	free(aux);

}

char* generar_ruta(t_mensaje* mensaje){

	switch(mensaje->codigo_operacion){
	case NEW_POKEMON:
		return string_from_format("%s/Files/%s",punto_montaje,mensaje->contenido.new_pokemon.pokemon->nombre);
	case CATCH_POKEMON:
		return string_from_format("%s/Files/%s",punto_montaje,mensaje->contenido.catch_pokemon.pokemon->nombre);
	case GET_POKEMON:
		return string_from_format("%s/Files/%s",punto_montaje,mensaje->contenido.get_pokemon.nombre_pokemon);
	default:
		printf("ERROR cod_op generar_ruta\n");
		return NULL;
	}
}

bool directorioExiste(char* ruta){
	struct stat st = {0}; // stat(2) precisa de un struct stat a modo de buffer para llenar info del archivo que nosotros no necesitamos.
	char* aux = string_duplicate(ruta);
	bool existe = (stat(aux,&st) != -1);
	free(aux);
	return existe;

}

void verificar_y_crear_pokemon_files(t_mensaje* mensaje){

	char* aux = generar_ruta(mensaje);

	if(!directorioExiste(aux)){
		mkdir(aux,0700);
		string_append(&aux,"/Metadata.bin");
		FILE* new_archivo_metadata = fopen(aux,"w");
		fprintf(new_archivo_metadata, "DIRECTORY=N\nSIZE=0\nBLOCKS=[]\nOPEN=N\n");
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
			msync(bitmap, global_metadata->blocks,MS_SYNC); //puedo cambiar el flag por MS_ASYNC si quiero que le escritura sea asincrona y no bloqueante
			pthread_mutex_unlock(&mutex_bitmap);
			return i;
		}
	}
	pthread_mutex_unlock(&mutex_bitmap);
	log_debug(logger,"No hay bloques libres!!!");
	return -1;
}

void bitarray_devolver_bloque(int bloque){
	pthread_mutex_lock(&mutex_bitmap);
	bitarray_clean_bit(bitmap,bloque);
	msync(bitmap, global_metadata->blocks,MS_SYNC);
	pthread_mutex_unlock(&mutex_bitmap);
}

char* mensaje_to_pokedata(t_mensaje* mensaje){
	char* pokedata = string_new();
	int cod = mensaje->codigo_operacion;
	int pos_x;
	int pos_y;
	switch(cod){
		case NEW_POKEMON:
			pos_x = mensaje->contenido.new_pokemon.pokemon->pos_x;
			pos_y = mensaje->contenido.new_pokemon.pokemon->pos_y;
			int cantidad = mensaje->contenido.new_pokemon.cantidad;
			string_append_with_format(&pokedata,"%d-%d=%d",pos_x,pos_y,cantidad);
			break;
		case CATCH_POKEMON:
			pos_x = mensaje->contenido.catch_pokemon.pokemon->pos_x;
			pos_y = mensaje->contenido.catch_pokemon.pokemon->pos_y;
			string_append_with_format(&pokedata,"%d-%d=%d",pos_x,pos_y,0);
			break;
		default:
			printf("INVALID CODE_OP mensaje_to_pokedata\n");
			free(pokedata);
			return NULL;
	}

	return pokedata;
}

bool modificar_poke_string_list(t_list* poke_strings_list, t_mensaje* mensaje){

	bool misma_posicion(char* str1, char* str2){
		int i=0;
		while(str1[i] != '=' && str2[i] != '='){
			if(str1[i] == str2[i]){
				i++;
				continue;
			}
			else
				return false;
		}
		if(str1[i] == str2[i])
			return true;
		else
			return false;
	}

	char* incrementar_cantidad(char* pok_1, char* pok_2){
		char **datos_pok1 = string_n_split(pok_1,2,"=");
		char **datos_pok2 = string_n_split(pok_2,2,"=");

		int cantidad_1 = atoi(datos_pok1[1]);
		int cantidad_2 = atoi(datos_pok2[1]);

		char* pok_aux = string_from_format("%s=%d",datos_pok1[0],cantidad_1+cantidad_2);

		liberar_argpointer(datos_pok1);
		liberar_argpointer(datos_pok2);

		return pok_aux;

	}

	char* disminuir_cantidad(char* pok){
		char **datos_pok = string_n_split(pok,2,"=");
		int cantidad = atoi(datos_pok[1]);
		cantidad--;

		if(!cantidad){
			free(datos_pok[0]);
			free(datos_pok[1]);
			free(datos_pok);
			return NULL;
		}

		char* pok_aux = string_from_format("%s=%d",datos_pok[0],cantidad);

		liberar_argpointer(datos_pok);
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

				free(old_pok);
				list_add(poke_strings_list,new_pok);
				free(data);
				return true;
			}
		}
		list_add(poke_strings_list,data);
		return true;
	case CATCH_POKEMON:
		for (int i = 0 ; i < list_size(poke_strings_list) ; i++){
			if(misma_posicion(list_get(poke_strings_list,i),data)){
				char* old_pok = list_remove(poke_strings_list,i);
				char* new_pok = disminuir_cantidad(old_pok); //devuelve NULL si la nueva cantidad es 0

				free(old_pok);
				if(new_pok)
					list_add(poke_strings_list,new_pok);
				free(data);
				return true;
			}
		}
		free(data);
		return false;
	default:
		printf("INVALID CODE_OP\n");
		free(data);
		return false;
	}
}

char* poke_list_a_poke_string(t_list* list){
	char* aux = string_new();

	for(int i=0 ; i<list_size(list) ; i++)
		string_append_with_format(&aux,"%s\n",list_get(list,i));

	return aux;
}

int cantidad_bloques_necesarios(char* str){
	int tam_bloque = global_metadata->block_size;
	int tam_str = strlen(str);

	return (tam_str + (tam_bloque-1)) / tam_bloque; // la division es asi para que haga el redondeo para arriba
}

FILE* abrir_bloque(int num_bloque,char* open_mode){
	char* ruta = string_from_format("%s/Blocks/%d.bin",punto_montaje,num_bloque);
	printf("Apertura en modo %s path: %s\n",open_mode,ruta);

	FILE* file = fopen(ruta,open_mode);
	free(ruta);
	return file;
}

//recibe una t_list de int bloques y devuelve un t_list con su contenido:["1-1=4",...,"5-7=1"]
t_list* pokemon_data_de_bloques(t_list* bloques){

	t_list* poke_data = list_create();
	char* poke_data_string = string_new();

	for(int i=0 ; i < list_size(bloques) ; i++){

		FILE* block_file = abrir_bloque((int)list_get(bloques,i),"r");

		fseek(block_file,0,SEEK_END);
		int block_size = ftell(block_file);
		fseek(block_file,0,SEEK_SET);

		char* auxiliar = malloc(block_size+1);
		fread(auxiliar,1,block_size,block_file);
		auxiliar[block_size] = '\0';

		string_append(&poke_data_string,auxiliar);
		free(auxiliar);

		fclose(block_file);

	}

	if(!list_is_empty(bloques)){
		char** poke_strings = string_split(poke_data_string,"\n");

		for(char** aux = poke_strings ; *aux ; aux++)
			list_add(poke_data,*aux);

		free(poke_strings);

	}

	free(poke_data_string);

	return poke_data;

}



void escribir_bloques(t_list* blocklist,char* data){

	char* data_aux = string_duplicate(data);

	int remaining_data = strlen(data_aux);
	int sent_data = 0;

	for(int i=0 ; i<list_size(blocklist) ; i++){

		int num_bloque = (int) list_get(blocklist,i);
		//pthread_mutex_lock(&mutex_testeo);
		FILE *file_stream = abrir_bloque(num_bloque,"w");

		if(remaining_data >= global_metadata->block_size){
			char* data_que_falta_enviar = string_substring_from(data_aux,sent_data);
			fwrite(data_que_falta_enviar,global_metadata->block_size,1,file_stream);
			remaining_data -= global_metadata->block_size;
			sent_data += global_metadata->block_size;
			free(data_que_falta_enviar);
		}else{
			char* data_que_falta_enviar = string_substring_from(data_aux,sent_data);
			fwrite(data_que_falta_enviar,1,remaining_data,file_stream);
			free(data_que_falta_enviar);
		}


		fclose(file_stream);
		//pthread_mutex_unlock(&mutex_testeo);
	}

	free(data_aux);

}

/*
 Verificar si las posiciones ya existen dentro del archivo.
  En caso de existir, se deben agregar la cantidad pasada por mensaje
  a la actual. En caso de no existir se debe agregar al final del archivo
  una nueva línea indicando la cantidad de pokémon pasadas.
 */

bool agregar_pokemones(t_mensaje* mensaje){
	bool operacion_exitosa;

	char* ruta = generar_ruta(mensaje);

	t_metadata* metadata;
	metadata = leer_metadata(ruta);
	log_debug(logger,"Se leyo correctamente los datos de: %s, de size: %d",ruta,metadata->size);

	//["22-4=1",""]
	t_list* poke_strings_list = pokemon_data_de_bloques(metadata->blocks);

	operacion_exitosa = modificar_poke_string_list(poke_strings_list,mensaje);

	/* En este punto se comianza a revisar segun la cantidad de bloques
	 * que tenga asignado el metadata, se evalua si esa cantidad se debe aumentar
	 * o se puede disminuir, y se procede a hacer eso interactuando con el bitarray global
	 */

	char* poke_string = poke_list_a_poke_string(poke_strings_list);
	int cant_bloq_necesarios = cantidad_bloques_necesarios(poke_string);

	if(cant_bloq_necesarios > list_size(metadata->blocks))
		list_add(metadata->blocks,(void*)bitarray_pedir_bloque());

	else if(cant_bloq_necesarios < list_size(metadata->blocks)){
		bitarray_devolver_bloque( (int) list_remove(metadata->blocks,list_size(metadata->blocks)-1));
	}
	escribir_bloques(metadata->blocks,poke_string);
	metadata->size = strlen(poke_string);

	escribir_archivo_metadata_y_cerrar(metadata,ruta);
	log_debug(logger,"Se escribio correctamente los datos de: %s, de tamaño: %d",ruta,metadata->size);

	free(poke_string);
	list_destroy_and_destroy_elements(poke_strings_list,free);
	list_destroy(metadata->blocks);
	free(metadata);
	free(ruta);

	return operacion_exitosa;
}




void recibir_new(t_mensaje *mensaje){

	verificar_y_crear_pokemon_files(mensaje);

	agregar_pokemones(mensaje);

	t_mensaje* mensajeAEnviar = crear_mensaje(5,APPEARED_POKEMON,mensaje->contenido.new_pokemon.pokemon->nombre,mensaje->contenido.new_pokemon.pokemon->pos_x,mensaje->contenido.new_pokemon.pokemon->pos_y,mensaje->id);
	printf("Se intenta enviar al broker el siguiente mensaje:\n");
	//printear_mensaje(mensajeAEnviar);

	pthread_mutex_lock(&mutex_envio_mensaje);

	int socket_broker = try_connect_to(ip_broker,puerto_broker);

	if(socket_broker != -1){
		enviar_mensaje(socket_broker,mensajeAEnviar);
		check_ack(socket_broker,ACK);
		close(socket_broker);

	}
	else{
		log_debug(logger,"No se pudo establecer la conexion con el broker para enviarle el mensaje appeared");
	}

	pthread_mutex_unlock(&mutex_envio_mensaje);

	liberar_mensaje(mensajeAEnviar);
	liberar_mensaje(mensaje);

}

void recibir_catch(t_mensaje *mensaje){

	char* aux = generar_ruta(mensaje);
	bool operacion_exitosa;

	if(!directorioExiste(aux)){
		printf("El archivo pokemon no se encuentra en el FILE SYSTEM!! :c\n");
		operacion_exitosa = false;
	}
	else{
		operacion_exitosa = agregar_pokemones(mensaje);
	}
	free(aux);

	t_mensaje* mensajeAEnviar = crear_mensaje(3, CAUGHT_POKEMON,mensaje->id,operacion_exitosa);
	printf("Se intenta enviar al broker el siguiente mensaje:\n");
	printear_mensaje(mensajeAEnviar);

	pthread_mutex_lock(&mutex_envio_mensaje);

	int socket_broker = try_connect_to(ip_broker,puerto_broker);

	if(socket_broker != -1){
		enviar_mensaje(socket_broker,mensajeAEnviar);
		puts("------------------------------------------------------------");
		check_ack(socket_broker,ACK);
		puts("------------------------------------------------------------");
		close(socket_broker);
	}
	else{
		log_debug(logger,"No se pudo establecer la conexion con el broker para enviarle el mensaje caught");
	}

	pthread_mutex_unlock(&mutex_envio_mensaje);

	liberar_mensaje(mensajeAEnviar);
	liberar_mensaje(mensaje);

}

void recibir_get(t_mensaje *mensaje){

	//printear_mensaje(mensaje);

	char* ruta = generar_ruta(mensaje);

	if(!directorioExiste(ruta)){
		log_debug(logger,"El archivo pokemon no se encuentra en el FILE SYSTEM!! :c");
		//liberar_mensaje(mensaje);

		free(ruta);
		return;
	}

	t_metadata* metadata;
	metadata = leer_metadata(ruta);

	if(!metadata->blocks){ //Cash archivo vacio
		log_debug(logger,"El archivo en cuestion no tenia bloques asignados");
		escribir_archivo_metadata_y_cerrar(metadata,ruta);

		free(mensaje);
		free(ruta);
		return;

	}
	t_list* poke_strings_list = pokemon_data_de_bloques(metadata->blocks); // "x-y=cantidad
	debug_print_string_list(poke_strings_list);

	t_pokemon_especie* poke_especie = crear_pokemon_especie(mensaje->contenido.get_pokemon.nombre_pokemon);

	for (int i=0 ; i<list_size(poke_strings_list) ; i++)
		agregar_ubicacion_a_especie(poke_especie,list_get(poke_strings_list,i));

	escribir_archivo_metadata_y_cerrar(metadata,ruta);
	free(ruta);

	t_mensaje* mensajeAEnviar = crear_mensaje(3, LOCALIZED_POKEMON, mensaje->id, poke_especie);
    //printf("IDm :%d",mensaje->id);
    //printf("IDc :%d",mensajeAEnviar->contenido.localized_pokemon.id_correlativo);
	printf("Se intenta enviar al broker el siguiente mensaje:\n");
	printear_mensaje(mensajeAEnviar);

	pthread_mutex_lock(&mutex_envio_mensaje);

	int socket_broker = try_connect_to(ip_broker,puerto_broker);

	if(socket_broker != -1){
		enviar_mensaje(socket_broker,mensajeAEnviar);
		puts("------------------------------------------------------------");
		check_ack(socket_broker,ACK);
		puts("------------------------------------------------------------");
		close(socket_broker);
	}
	else{
		log_debug(logger,"No se pudo establecer la conexion con el broker para enviarle el mensaje LOCALIZED_POKEMON");
	}

	pthread_mutex_unlock(&mutex_envio_mensaje);
	//list_destroy_and_destroy_elements(poke_strings_list,free);
	list_destroy(metadata->blocks);
	free(metadata);
	liberar_mensaje(mensajeAEnviar);
	//liberar_mensaje(mensaje);
}

void manejar_mensaje(t_mensaje* mensaje){
	puts("maneja mensaje");
	switch(mensaje->codigo_operacion){
		case NEW_POKEMON:
			recibir_new(mensaje);
			break;
		case CATCH_POKEMON:;
			recibir_catch(mensaje);
			break;
		case GET_POKEMON:;
			recibir_get(mensaje);
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
	////printear_mensaje(mensaje);
	manejar_mensaje(mensaje);
	free(stream);
	return true;
}


void protocolo_recibir_mensaje(cola_code cola){
	while(true){
		pthread_mutex_lock(&mutex_subscripcion);
		int socket_cola = subscribirse_a_cola(cola);
		pthread_mutex_unlock(&mutex_subscripcion);
		//printf("socket_suscripcion:%d\n",socket_cola);
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
		printf("Se crea el directorio%s\n",path);
		mkdir(path,0700);
	}
	for(int i=0; i < global_metadata->blocks ;i++){
		char* path_relative = string_duplicate(path);
		string_append_with_format(&path_relative, "/%d.bin",i);
		FILE* f = fopen(path_relative, "a");
		free(path_relative);
		fclose(f);
	}
	free(path);
	log_debug(logger,"Se crearon los bloques!");
}



t_bitarray* crear_bitarray_y_mapear(){

	char* ruta = string_from_format("%s/Metadata/Bitmap.bin",punto_montaje);
	int fd = open(ruta, O_CREAT | O_RDWR, 0664);

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

	free(ruta);
	return bitmap;
}

void esperar_mensaje(int *socket){

	uint32_t codigo_operacion;

	if(recv(*socket, &(codigo_operacion),sizeof(uint32_t), MSG_WAITALL)==-1){
		perror("Falla recv() op_code");
	}

	uint32_t id;

	if(recv(*socket, &(id), sizeof(uint32_t), MSG_WAITALL) == -1){
		perror("Falla recv() id");
	}

	uint32_t size_contenido_mensaje;

	if(recv(*socket, &(size_contenido_mensaje), sizeof(uint32_t), MSG_WAITALL) == -1){
		perror("Falla recv() size_contenido_mensaje");
	}

	void* stream = malloc(size_contenido_mensaje);

	if(recv(*socket, stream, size_contenido_mensaje, MSG_WAITALL) == -1){
		perror("Falla recv() contenido");
	}

	t_mensaje* mensaje = deserializar_mensaje(codigo_operacion, stream);
	mensaje->id=id;

	send_ack(*socket,ACK);

	manejar_mensaje(mensaje);

}

void protocolo_escuchar_gameboy(){
	char* ip_gameboy_escucha = "127.0.0.1";
	char* puerto_gameboy_escucha = "5003";
	int socket_escucha = listen_to(ip_gameboy_escucha,puerto_gameboy_escucha);

	while(1)
		esperar_cliente(socket_escucha,esperar_mensaje);

}


void inicializar_gamecard() {
	logger = log_create("../gamecard.log","log",1,LOG_LEVEL_DEBUG);
	leer_config();
	leer_metadata_global();
	crear_bloques();
	bitmap = crear_bitarray_y_mapear();

	pthread_t pthread_cola_new;
	pthread_t pthread_cola_catch;
	pthread_t pthread_cola_get;
	pthread_t escuchar_gameboy;

	pthread_mutex_init(&mutex_bitmap, NULL);
    pthread_mutex_init(&mutex_envio_mensaje, NULL);
    pthread_mutex_init(&mutex_metadata, NULL);
    pthread_mutex_init(&mutex_subscripcion, NULL);

	pthread_create(&pthread_cola_get, NULL,(void*)protocolo_recibir_mensaje,(void*) COLA_GET_POKEMON);
	pthread_detach(pthread_cola_get);

	pthread_create(&pthread_cola_new, NULL,(void*)protocolo_recibir_mensaje,(void*) COLA_NEW_POKEMON);
	pthread_detach(pthread_cola_new);

	pthread_create(&pthread_cola_catch, NULL,(void*)protocolo_recibir_mensaje,(void*) COLA_CATCH_POKEMON);
	pthread_detach(pthread_cola_catch);

	pthread_create(&escuchar_gameboy, NULL, (void*)protocolo_escuchar_gameboy,NULL);
	pthread_detach(escuchar_gameboy);

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
	puts("B -> PRINT ESTADO BITARRAY");
	puts("G -> SIMULAR GET SQUIRTLE");
	puts("M -> MOSTRAR MENU");
	puts("N -> SIMULAR NEW SQUIRTLE 4-7=455");
	puts("R -> SIMULAR NEW SQUIRTLE DATOS RANDOM");
	puts("X -> BITARRAY CLEAN");
	puts("Z -> SALIR");
	puts("---------------------------");
}

void debug(){
	mostrar_menu();

	t_mensaje* mensaje;

	while(true){
		char msg;
		scanf("%c",&msg);
		switch(msg){
			case 'B':
				debug_print_bitarray(bitmap);
				break;
			case 'G':
				mensaje = crear_mensaje(5, GET_POKEMON,"Squirtle");
				mensaje->id =123;
				recibir_get(mensaje);
				break;
			case 'M':
				mostrar_menu();
				break;
			case 'N':
				mensaje = crear_mensaje(5, NEW_POKEMON,"Squirtle",4,7,455);
				recibir_new(mensaje);
				break;
			case 'R':
				srand(time(NULL));
				mensaje = crear_mensaje(5, NEW_POKEMON,"Squirtle",rand()%10,rand()%10,rand()%5);
				printf("Se recibe un new random del siguiente mensaje: \n");
				//printear_mensaje_(mensaje);
				recibir_new(mensaje);
				break;
			case 'X':;
				size_t tope = bitarray_get_max_bit(bitmap);
				for(int i = 0; i < tope; i++)
					 bitarray_clean_bit(bitmap, i);
				printf("Se limpio el bitarray\n");
				break;
			case 'Z':
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
