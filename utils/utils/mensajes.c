
/*
 ============================================================================
 Name        : utils.c
 Author      : Madd
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "mensajes.h"

/* crear_mensaje
 *
 * funcion de argumentos variables, se usa para crear todos los tipos de mensaje. Se le pasa el
 * codigo de operacion y los argumentos para completar los campos de cada mensaje.
 *
 * SUBSCRIPCION : se pasa primero el socket que se subscribe y como segundo argumento la cola a la cual
 * suscribirse.
 *
 * APPEARED_POKEMON:
 *
 *
 * codigo_operacion = codigo sobre el cual se decidira que accion tomar
 * ... = lista variable, depende del codigo de operacion que hacer
 *
 *
 * TENER EN CUENTA QUE SE MALLOCKEAN T_MENSAJES!! HAY QUE VER DONDE HACERLES FREE.
 *
 *
 *
 */

//t_mensaje* mensaje = crear_mensaje(5, APPEARED_POKEMON, pokemon->nombre, pokemon->pos_x, pokemon->pos_y, id_correlativo);

t_mensaje* crear_mensaje(int num, ...){
	va_list args;
	va_start(args, num);

	int auxa,auxb;
	char* str_aux;
	t_pokemon* pokemon;
	t_mensaje* mensaje = malloc(sizeof(t_mensaje));

	mensaje->id=ID_DEFAULT;
	mensaje->codigo_operacion = va_arg(args, uint32_t);


	switch(mensaje->codigo_operacion){
		case SUBSCRIPCION:// se le pasa el tipo y la cola a subscribirse
			mensaje->id=ID_SUSCRIPCION;
			mensaje->contenido.subscripcion = va_arg(args, uint32_t);
			va_end(args);
			//return mensaje;
			break;
		case NEW_POKEMON:;
			str_aux = va_arg(args, char*);
			auxa = va_arg(args, uint32_t);
			auxb = va_arg(args, uint32_t);
			pokemon = crear_pokemon(str_aux, auxa,auxb);
			mensaje->contenido.new_pokemon.pokemon = pokemon;
			mensaje->contenido.new_pokemon.cantidad = va_arg(args,uint32_t);
			va_end(args);
			//return mensaje;
			break;
		case APPEARED_POKEMON:;
			str_aux = va_arg(args, char*);
			auxa = va_arg(args, uint32_t);
			auxb = va_arg(args, uint32_t);
			pokemon = crear_pokemon(str_aux, auxa,auxb);
			mensaje->contenido.appeared_pokemon.pokemon = pokemon;
			mensaje->contenido.appeared_pokemon.id_correlativo = va_arg(args,uint32_t);
			va_end(args);
			//return mensaje;
			break;
		case CATCH_POKEMON:;
			str_aux = va_arg(args, char*);
			auxa = va_arg(args, uint32_t);
			auxb = va_arg(args, uint32_t);
			pokemon = crear_pokemon(str_aux, auxa,auxb);
			mensaje->contenido.new_pokemon.pokemon = pokemon;
			va_end(args);
			//return mensaje;
			break;
		case CAUGHT_POKEMON:
			mensaje->contenido.caught_pokemon.id_correlativo = va_arg(args,uint32_t);
			mensaje->contenido.caught_pokemon.caught_confirmation = va_arg(args, uint32_t);
			va_end(args);
			//return mensaje;
			break;
		case GET_POKEMON:
			mensaje->contenido.get_pokemon.nombre_pokemon = va_arg(args, char*);
			va_end(args);
			//return mensaje;
			break;
		case LOCALIZED_POKEMON:;
			mensaje->contenido.localized_pokemon.id_correlativo = va_arg(args, uint32_t);
			mensaje->contenido.localized_pokemon.pokemon_especie = va_arg(args, t_pokemon_especie*);
			va_end(args);
			break;
	}
	return mensaje;
}

void liberar_pokemon(t_pokemon* pokemon){
	free(pokemon->nombre);
}

void liberar_pokemon_especie(t_pokemon_especie* pokemon_especie){
	free(pokemon_especie->nombre_especie);
	//dictionary_destroy_and_destroy_elements(pokemon_especie->posiciones_especie,free);
	dictionary_destroy(pokemon_especie->posiciones_especie);
}

void liberar_mensaje(t_mensaje* mensaje){

	switch(mensaje->codigo_operacion){
		case SUBSCRIPCION:
			break;
		case NEW_POKEMON:
			free(mensaje->contenido.new_pokemon.pokemon->nombre);
			free(mensaje->contenido.new_pokemon.pokemon);
			break;
		case APPEARED_POKEMON:
			free(mensaje->contenido.appeared_pokemon.pokemon->nombre);
			free(mensaje->contenido.appeared_pokemon.pokemon);
			break;
		case CATCH_POKEMON:
			free(mensaje->contenido.catch_pokemon.pokemon->nombre);
			free(mensaje->contenido.catch_pokemon.pokemon);
			break;
		case CAUGHT_POKEMON:
			break;
		case GET_POKEMON:
			free(mensaje->contenido.get_pokemon.nombre_pokemon);
			break;
		case LOCALIZED_POKEMON:
			liberar_pokemon_especie(mensaje->contenido.localized_pokemon.pokemon_especie);
			free(mensaje->contenido.localized_pokemon.pokemon_especie);
			break;
	}
	free(mensaje);
}



void printear_pokemon(t_pokemon* pokemon){
	printf("nombre:%s\npos x:%d\npos y:%d\n", pokemon->nombre,pokemon->pos_x,pokemon->pos_y);
}

void logear_llegada_mensaje(t_log* logger, t_mensaje* mensaje){
	char* tipo;
	char* info;
	char* aux; //STRING_NEW???

	switch(mensaje -> codigo_operacion){
		case APPEARED_POKEMON:
			tipo = malloc(strlen("APPEARED_POKEMON ")+1);
			strcpy(tipo,"APPEARED_POKEMON ");
			info = crear_pokestring(mensaje->contenido.appeared_pokemon.pokemon);
			//free(mensaje->contenido.appeared_pokemon.pokemon->nombre);
		    //free(mensaje->contenido.appeared_pokemon.pokemon);
			break;
		case CAUGHT_POKEMON:
			tipo = malloc(strlen("CAUGHT_POKEMON ")+1);
			strcpy(tipo,"CAUGHT_POKEMON ");
			if(mensaje->contenido.caught_pokemon.caught_confirmation)
				aux = string_from_format("TRUE");
			else
				aux = string_from_format("FALSE");

			info = string_from_format("id correlativo: %d | se logro atrapar: %s",mensaje->contenido.caught_pokemon.id_correlativo,aux);
			free(aux);
			break;
		case LOCALIZED_POKEMON:
			tipo = malloc(strlen("LOCALIZED_POKEMON ")+1);
			strcpy(tipo,"LOCALIZED_POKEMON ");

			info = string_from_format("id correlativo: %d | pokemon_especie: %s",mensaje->contenido.localized_pokemon.id_correlativo,especie_pokemon_a_string(mensaje->contenido.localized_pokemon.pokemon_especie));
			//free(mensaje->contenido.localized_pokemon.pokemon_especie->nombre_especie);
			//free(mensaje->contenido.localized_pokemon.pokemon_especie);
			break;
		default:
			puts("la funcion logear_llegada_mensaje no soporta mensajes de ese tipo todavia :$");
			return;
	}

	log_debug(logger,"Llegada mensaje tipo: %s, %s",tipo,info);
	free(tipo);
	free(info);
	//free(aux);
}



void printear_mensaje(t_mensaje* mensaje){
	printf("----------------------------------------\nMENSAJE\n");
	printf("id:%d\nop_code:%d\n", mensaje->id,mensaje->codigo_operacion);
	switch(mensaje->codigo_operacion){
		case SUBSCRIPCION:// se le pasa el tipo y la cola a subscribirse
			printf("suscripcion:%d\n",mensaje->contenido.subscripcion);
			break;
		case NEW_POKEMON:
			printear_pokemon(mensaje->contenido.new_pokemon.pokemon);
			printf("cantidad:%d\n", mensaje->contenido.new_pokemon.cantidad);
			break;
		case APPEARED_POKEMON:
			printear_pokemon(mensaje->contenido.appeared_pokemon.pokemon);
			printf("id correlativo:%d\n", mensaje->contenido.appeared_pokemon.id_correlativo);
			break;
		case CATCH_POKEMON:
			printear_pokemon(mensaje->contenido.catch_pokemon.pokemon);
			break;
		case CAUGHT_POKEMON:
			printf("id_correlativo:%d\ncaught_confirmation:%d\n", mensaje->contenido.caught_pokemon.id_correlativo,mensaje->contenido.caught_pokemon.caught_confirmation);
			break;
		case GET_POKEMON:
			printf("Pokemon:%s\n", mensaje->contenido.get_pokemon.nombre_pokemon);
			break;
		case LOCALIZED_POKEMON:
			printf("id_correlativo:%d\n",mensaje->contenido.localized_pokemon.id_correlativo);
			printear_pokemon_especie(mensaje->contenido.localized_pokemon.pokemon_especie);
			break;
	}

	printf("----------------------------------------\n");

}


// ------------------- SERIALIZAR APPEARED_POKEMON ---------------------//

void* serializar_appeared_pokemon(t_appeared_pokemon appeared_pokemon){
	void* magic = malloc(sizeof(uint32_t) + tamanio_pokemon(appeared_pokemon.pokemon));
	int size_pokemon=tamanio_pokemon(appeared_pokemon.pokemon);
	void* pokemon_stream = serializar_pokemon(appeared_pokemon.pokemon);
	int offset = 0;
	memcpy(magic + offset, (&appeared_pokemon.id_correlativo), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(magic + offset, pokemon_stream, size_pokemon);
	offset += size_pokemon;

	free(pokemon_stream);
	return magic;
}

t_appeared_pokemon deserializar_appeared_pokemon(void* stream){
	t_appeared_pokemon appeared_pokemon;
	int offset=0;
	memcpy(&(appeared_pokemon.id_correlativo),stream+offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	appeared_pokemon.pokemon = deserializar_pokemon(stream+offset);

	return appeared_pokemon;
}

// ------------------- SERIALIZAR GET_POKEMON ---------------------//

void* serializar_get_pokemon(t_get_pokemon get_pokemon){
	void* magic = malloc(strlen(get_pokemon.nombre_pokemon) + 1);
	memcpy(magic, get_pokemon.nombre_pokemon, strlen(get_pokemon.nombre_pokemon)+1);
	return magic;
}

/* deserializar_get_pokemon
 * void* stream = puntero a deserializar
 * devuelve un get_pokemon deserializado
 */

t_get_pokemon deserializar_get_pokemon(void* stream){
	t_get_pokemon get_pokemon;
	get_pokemon.nombre_pokemon = strdup(stream);
	return get_pokemon;
}

// ------------------- SERIALIZAR CATCH_POKEMON ---------------------//

void* serializar_catch_pokemon(t_catch_pokemon catch_pokemon){
	void* magic = serializar_pokemon(catch_pokemon.pokemon);
	return magic;
}

t_catch_pokemon deserializar_catch_pokemon(void* stream){
	t_catch_pokemon catch_pokemon;
	catch_pokemon.pokemon=deserializar_pokemon(stream);
	return catch_pokemon;
}

// ------------------- SERIALIZAR CAUGHT_POKEMON ---------------------//

void* serializar_caught_pokemon(t_caught_pokemon caught_pokemon){
	void* magic = malloc(sizeof(uint32_t)*2);
	int offset = 0;
	memcpy(magic + offset,&(caught_pokemon.id_correlativo), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(magic + offset, &(caught_pokemon.caught_confirmation),sizeof(uint32_t));
	offset += sizeof(uint32_t);
	return magic;
}

t_caught_pokemon deserializar_caught_pokemon(void* stream){
	t_caught_pokemon caught_pokemon;
	int offset = 0;
	memcpy(&(caught_pokemon.id_correlativo),stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(caught_pokemon.caught_confirmation),stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	return caught_pokemon;
}

// ------------------- SERIALIZAR NEW_POKEMON ---------------------//

void* serializar_new_pokemon(t_new_pokemon new_pokemon){
	void* magic = malloc(sizeof(uint32_t) + tamanio_pokemon(new_pokemon.pokemon));
	int size_pokemon=tamanio_pokemon(new_pokemon.pokemon);
	void* pokemon_stream = serializar_pokemon(new_pokemon.pokemon);
	int offset = 0;
	memcpy(magic + offset, pokemon_stream, size_pokemon);
	offset += size_pokemon;
	memcpy(magic + offset, (&new_pokemon.cantidad), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	free(pokemon_stream);
	return magic;
}

t_new_pokemon deserializar_new_pokemon(void* stream){
	t_new_pokemon new_pokemon;
	int offset=0;
	new_pokemon.pokemon = deserializar_pokemon(stream+offset);
	offset += tamanio_pokemon(new_pokemon.pokemon);
	memcpy(&(new_pokemon.cantidad),stream+offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	return new_pokemon;
}

// ------------------- SERIALIZAR LOCALIZED_POKEMON ---------------------//

void* serializar_localized_pokemon(t_localized_pokemon localized_pokemon){
	void* magic = malloc(sizeof(uint32_t) + tamanio_pokemon_especie(localized_pokemon.pokemon_especie));
	int offset=0;
	memcpy(magic, &localized_pokemon.id_correlativo, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(magic+offset, especie_pokemon_a_string(localized_pokemon.pokemon_especie), tamanio_pokemon_especie(localized_pokemon.pokemon_especie));
	offset += tamanio_pokemon_especie(localized_pokemon.pokemon_especie);
	return magic;
}

t_localized_pokemon deserializar_localized_pokemon(void* stream){
	t_localized_pokemon localized_pokemon;
	int offset = 0;
	memcpy(&localized_pokemon.id_correlativo, stream, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	puts("adasdsaddada");
	localized_pokemon.pokemon_especie = deserializar_pokemon_especie((stream + offset));
	puts("adasdsaddada");
	return localized_pokemon;
}




// ------------------- FIN SERIALIZACIONES ---------------------//

/* tamanio_contenido_mensaje
 * mensaje = mensaje t_mensaje al que va obtiene el tamaño del contenido
 * devuelve un int con el tamaño del contenido
 */

int tamanio_contenido_mensaje(t_mensaje* mensaje){
	int tamanio=0;
	switch(mensaje->codigo_operacion){
		case SUBSCRIPCION:
			tamanio += sizeof(uint32_t);
			break;
		case GET_POKEMON:
			tamanio += strlen(mensaje->contenido.get_pokemon.nombre_pokemon) + 1;
			break;
		case APPEARED_POKEMON:
			tamanio += sizeof(uint32_t) + tamanio_pokemon(mensaje->contenido.appeared_pokemon.pokemon);
			break;
		case NEW_POKEMON:
			tamanio += sizeof(uint32_t) + tamanio_pokemon(mensaje->contenido.new_pokemon.pokemon);
			break;
		case CATCH_POKEMON:
			tamanio += tamanio_pokemon(mensaje->contenido.new_pokemon.pokemon);
			break;
		case CAUGHT_POKEMON:
			tamanio += sizeof(uint32_t)*2;
			break;
		case LOCALIZED_POKEMON:
			tamanio += sizeof(uint32_t) + tamanio_pokemon_especie(mensaje->contenido.localized_pokemon.pokemon_especie);
			break;


	}
	return tamanio;
}




/* serializar_mensaje
 * ret_size* = puntero al que devuelve el tamaño del mensaje
 * mensaje = mensaje t_mensaje al que va a serializar
 * devuelve un void* serializado
 */


void* serializar_mensaje(t_mensaje* mensaje, int *ret_size){
	int size_contenido_mensaje = tamanio_contenido_mensaje(mensaje);
	uint32_t size = size_contenido_mensaje + sizeof(uint32_t)*3; //op code, id, size_contenido_mensaje
	void* magic = malloc(size);
	int offset = 0;
	memcpy(magic + offset, &(mensaje->codigo_operacion), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(magic + offset, &(mensaje->id), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(magic + offset, &(size_contenido_mensaje), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	void* stream;

	switch(mensaje->codigo_operacion){
	case SUBSCRIPCION:;
		memcpy(magic + offset, &mensaje->contenido.subscripcion, size_contenido_mensaje);
		offset += size_contenido_mensaje;
		break;
	case GET_POKEMON:;
		stream = serializar_get_pokemon(mensaje->contenido.get_pokemon);
		memcpy(magic + offset, stream, size_contenido_mensaje);
		offset += size_contenido_mensaje;
		free(stream);
		break;
	case APPEARED_POKEMON:;
		stream = serializar_appeared_pokemon(mensaje->contenido.appeared_pokemon);
		memcpy(magic + offset, stream, size_contenido_mensaje);
		offset += size_contenido_mensaje;
		free(stream);
		break;
	case LOCALIZED_POKEMON:;
		stream = serializar_localized_pokemon(mensaje->contenido.localized_pokemon);
		memcpy(magic + offset, stream, size_contenido_mensaje);
		offset += size_contenido_mensaje;
		free(stream);
		break;
	case CATCH_POKEMON:;
		stream = serializar_catch_pokemon(mensaje->contenido.catch_pokemon);
		memcpy(magic + offset, stream, size_contenido_mensaje);
		offset += size_contenido_mensaje;
		free(stream);
		break;
	case CAUGHT_POKEMON:;
		stream = serializar_caught_pokemon(mensaje->contenido.caught_pokemon);
		memcpy(magic + offset, stream, size_contenido_mensaje);
		offset += size_contenido_mensaje;
		free(stream);
		break;
	case NEW_POKEMON:;
		stream = serializar_new_pokemon(mensaje->contenido.new_pokemon);
		memcpy(magic + offset, stream, size_contenido_mensaje);
		offset += size_contenido_mensaje;
		free(stream);
		break;


	}
	*ret_size=(int)size;
	return magic;

}

/* enviar_mensaje
 * socket_a_enviar = socket para enviar el mensaje
 * mensaje = mensaje t_mensaje
 *
 * devuelve 0 si tiene exito y -1 si falla
 */

int enviar_mensaje(int socket_a_enviar, t_mensaje* mensaje){
	int size;
	void* stream = serializar_mensaje(mensaje,&size);
	return sendall(socket_a_enviar,stream, size);
}

/* deserializar_mensaje
 * codigo_operacion = codigo sobre el cual se decidira que accion tomar
 * stream = donde esta contenida la informacion
 *
 */

t_mensaje* deserializar_mensaje(int codigo_operacion, void* stream){
	t_mensaje* mensaje = malloc(sizeof(t_mensaje));
	mensaje->codigo_operacion=codigo_operacion;
	switch(codigo_operacion){
		case APPEARED_POKEMON:;
			mensaje->contenido.appeared_pokemon=deserializar_appeared_pokemon(stream);
			break;
		case GET_POKEMON:;
			mensaje->contenido.get_pokemon=deserializar_get_pokemon(stream);
			break;
		case CAUGHT_POKEMON:;
			mensaje->contenido.caught_pokemon=deserializar_caught_pokemon(stream);
			break;
		case CATCH_POKEMON:;
			mensaje->contenido.catch_pokemon=deserializar_catch_pokemon(stream);
			break;
		case NEW_POKEMON:;
			mensaje->contenido.new_pokemon=deserializar_new_pokemon(stream);
			break;
		case LOCALIZED_POKEMON:;
			mensaje->contenido.localized_pokemon = deserializar_localized_pokemon(stream);
			break;
		default:
			puts("default");
			break;
	}
	return mensaje;
}

//---------------------POKEMON-------------------------------

t_pokemon* crear_pokemon(char* nombre,uint32_t px, uint32_t py){
	t_pokemon* pokemon = malloc(sizeof(t_pokemon));
	pokemon->nombre=strdup(nombre);
	pokemon->pos_x=px;
	pokemon->pos_y=py;
	return pokemon;
}

int tamanio_pokemon(t_pokemon* pokemon){
	return strlen(pokemon->nombre) + 1 + sizeof(uint32_t)*2;
}

char* crear_pokestring(t_pokemon* pokemon){
	char* pokestring = string_new();
	string_append(&pokestring,pokemon->nombre);
	string_append(&pokestring,",");
	string_append(&pokestring,posicion_string_pokemon(pokemon));

	return pokestring;
}

t_pokemon* string_a_pokemon(char* pokestring){
	char** params = string_split(pokestring,",");
	char** posiciones = string_split(params[1],"|");
	t_pokemon* pokemon = crear_pokemon(params[0],atoi(posiciones[0]),atoi(posiciones[1]));
	//free(params);
	//free(posiciones);
	return pokemon;
}

// ------------------- SERIALIZAR POKEMON ---------------------//


void* serializar_pokemon(t_pokemon* pokemon){
	void* magic = malloc(tamanio_pokemon(pokemon));
	int offset=0;

	memcpy(magic+offset,pokemon->nombre,strlen(pokemon->nombre)+1);
	offset += strlen(pokemon->nombre)+1;
	memcpy(magic+offset,&(pokemon->pos_x),sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(magic+offset,&(pokemon->pos_y),sizeof(uint32_t));
	offset += sizeof(uint32_t);
	return magic;
}

t_pokemon* deserializar_pokemon(void* stream){
	t_pokemon* pokemon = malloc(sizeof(t_pokemon));
	int offset=0;

	pokemon->nombre = strdup(stream);
	offset += strlen(pokemon->nombre)+1;
	memcpy(&(pokemon->pos_x),stream+offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(pokemon->pos_y),stream+offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	return pokemon;
}

char* posicion_string_pokemon(t_pokemon* pokemon){
	char* posicion_string = string_new();
	string_append_with_format(&posicion_string,"%d",pokemon->pos_x);
	string_append(&posicion_string,"|");
	string_append_with_format(&posicion_string,"%d",pokemon->pos_y);
	return posicion_string;
}



//---------------------POKEMON_ESPECIE---------------------------------------------------
/*
 * contiene el nombre de una especie de pokemon
 * y las posiciones con sus respectivas cantidades si es que hay alguna
 *
 */


t_pokemon_especie* crear_pokemon_especie(char* nombre_especie){
	t_pokemon_especie* pokemon_especie = malloc(sizeof(t_pokemon_especie));
	pokemon_especie->nombre_especie = string_duplicate(nombre_especie);
	pokemon_especie->posiciones_especie = dictionary_create();
	return pokemon_especie;
}

/*
 * agregar_pokemon_a_especie
 *
 * HAY QUE TENER EN CUENTA QUE TAMBIEN VALIDA AQUELLOS POKEMONES QUE NO SEAN DE SU MISMA ESPECIE
 * EN ESTE CASO SALE. EN CASO DE QUE SEA LA MISMA ESPECIE AGREGA LA POSICION O LA
 * INCREMENTA EN UNO
 *
 */

void agregar_pokemon_a_especie(t_pokemon_especie* pokemon_especie, t_pokemon* pokemon){
	int cantidad;
	char* posicion_pokemon;
	//VALIDA MISMA ESPECIE
	if(strcmp(pokemon_especie->nombre_especie,pokemon->nombre)==0){
		posicion_pokemon = posicion_string_pokemon(pokemon);
		if(dictionary_has_key(pokemon_especie->posiciones_especie, posicion_pokemon)){
			cantidad = (int)dictionary_remove(pokemon_especie->posiciones_especie, posicion_pokemon);
			dictionary_put(pokemon_especie->posiciones_especie,posicion_pokemon,(void*)++cantidad);
		}else{
			dictionary_put(pokemon_especie->posiciones_especie,posicion_pokemon, (void*)1);
		}
	}
}


/*
 * recibe un char* del estilo "4-8=145" y lo agrega a pokemon_especie en su respectivo formato
 */

void agregar_ubicacion_a_especie(t_pokemon_especie* pokemon_especie, char* posicionycantidad){
	char* posicion_pokemon;

	char** datos = string_split(posicionycantidad,"="); // ["4-8","145"]
	char** posiciones = string_split(datos[0],"-"); // ["4","8"]

	posicion_pokemon = string_from_format("%s|%s",posiciones[0],posiciones[1]);
	dictionary_put(pokemon_especie->posiciones_especie,posicion_pokemon, (void*)atoi(datos[1]));

	free(posicion_pokemon);
	free(posiciones[1]);
	free(posiciones[0]);
	free(posiciones);
	free(datos[1]); // no se hace free porque queda siendo parte de diccionario
	free(datos[0]);
	free(datos);


}

void sacar_pokemon_de_especie(t_pokemon_especie* pokemon_especie, t_pokemon* pokemon){
	int cantidad;
	char* posicion_pokemon;
	//VALIDA MISMA ESPECIE
	if(strcmp(pokemon_especie->nombre_especie,pokemon->nombre)==0){
		posicion_pokemon = posicion_string_pokemon(pokemon);
		if(dictionary_has_key(pokemon_especie->posiciones_especie, posicion_pokemon)){
			cantidad = (int) dictionary_remove(pokemon_especie->posiciones_especie, posicion_pokemon);
			if(--cantidad>0)
				dictionary_put(pokemon_especie->posiciones_especie,posicion_pokemon,(void*)cantidad);
		}
	}
}

/*
 * hay_pokemon_en_posicion
 *
 *	pokemon_especie: pokemon especie creado con la funcion
 *	key : pos_x|pos_y => ejemplo => -1|2 --- 44|76
 *
 */

bool hay_pokemon_en_posicion(t_pokemon_especie* pokemon_especie, char* key){
	return dictionary_has_key(pokemon_especie->posiciones_especie, key);
}

int tamanio_pokemon_especie(t_pokemon_especie* especie_pokemon){
	return strlen(especie_pokemon->nombre_especie) + 1 + strlen(posiciones_a_string(especie_pokemon->posiciones_especie)) + 1;
}

void printear_posicion(char* key, char* value){
	//printf("[%s|%s]",key, value);
}

int cant_coordenadas_especie_pokemon(t_pokemon_especie *pokemon_especie){
	int contador_aux = 0;
	void contador(char *key,void *stream){
		contador_aux += 1;
	}
	dictionary_iterator(pokemon_especie->posiciones_especie, contador);
    printf("Contador devuelve:%d\n",contador_aux);
    return contador_aux;
}

void printear_pokemon_especie(t_pokemon_especie* pokemon_especie){
	char* aux = especie_pokemon_a_string(pokemon_especie);
	puts(aux);
	free(aux);
}

char* posiciones_a_string(t_dictionary* posiciones){
	char* string_pos=string_new();
	void posicion_a_string(char* key, void* value){
		string_append_with_format(&string_pos,"%s|%d,", key, value);
	}
	dictionary_iterator(posiciones,posicion_a_string);
	return string_pos;
}
char* especie_pokemon_a_string(t_pokemon_especie* pokemon_especie){
	char* string = string_new();
	string_append_with_format(&string,"%s,",pokemon_especie->nombre_especie);
	char* posiciones = posiciones_a_string(pokemon_especie->posiciones_especie);
	string_append(&string,posiciones);
	return string;
}

t_pokemon_especie* string_a_pokemon_especie(char* string){
	char** main_str = string_split(string,",");
	t_pokemon_especie* especie_pokemon = crear_pokemon_especie(main_str[0]);
	t_pokemon pokemon_aux;
	int i = 1;
	char** posiciones;
	puts("entra a poke a especie");
	char* str_aux = string_new();
	int int_aux;
	while(main_str[i]!=NULL){
		posiciones = string_split(main_str[i],"|");
		string_append_with_format(&str_aux,"%s|%s",posiciones[0],posiciones[1]);
		int_aux = atoi(posiciones[2]);
		pokemon_aux.nombre = main_str[0];
		pokemon_aux.pos_x = atoi(posiciones[0]);
		pokemon_aux.pos_y = atoi(posiciones[1]);
		for(int j = 0; j<int_aux;j++)
			agregar_pokemon_a_especie(especie_pokemon, &pokemon_aux);
		i++;
	}
	i=0;
	free(str_aux);
	while(posiciones[i]!=NULL)
		free(posiciones[i++]);
	i=0;
	while(main_str[i]!=NULL)
		free(main_str[i++]);
	free(main_str);

	return especie_pokemon;
}

t_pokemon_especie* deserializar_pokemon_especie(void* string){
	t_pokemon_especie* especie_pokemon = string_a_pokemon_especie((char*)string);
	return especie_pokemon;
}
