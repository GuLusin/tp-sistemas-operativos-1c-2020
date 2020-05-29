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

t_pokemon* crear_pokemon(char* nombre,uint32_t px, uint32_t py){
	t_pokemon* pokemon = malloc(sizeof(t_pokemon));
	pokemon->nombre=strdup(nombre);
	pokemon->pos_x=px;
	pokemon->pos_y=py;
	return pokemon;
}

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
			return mensaje;
			break;
		case NEW_POKEMON:;
			str_aux = va_arg(args, char*);
			auxa = va_arg(args, uint32_t);
			auxb = va_arg(args, uint32_t);
			pokemon = crear_pokemon(str_aux, auxa,auxb);
			mensaje->contenido.new_pokemon.pokemon=pokemon;
			mensaje->contenido.new_pokemon.cantidad = va_arg(args,uint32_t);
			va_end(args);
			return mensaje;
			break;
		case APPEARED_POKEMON:;
			str_aux = va_arg(args, char*);
			auxa = va_arg(args, uint32_t);
			auxb = va_arg(args, uint32_t);
			pokemon = crear_pokemon(str_aux, auxa,auxb);
			mensaje->contenido.appeared_pokemon.pokemon=pokemon;
			mensaje->contenido.appeared_pokemon.id_correlativo= va_arg(args,uint32_t);
			va_end(args);
			return mensaje;
			break;
		case CATCH_POKEMON:;
			str_aux = va_arg(args, char*);
			auxa = va_arg(args, uint32_t);
			auxb = va_arg(args, uint32_t);
			pokemon = crear_pokemon(str_aux, auxa,auxb);
			mensaje->contenido.new_pokemon.pokemon = pokemon;
			va_end(args);
			return mensaje;
			break;
		case CAUGHT_POKEMON:
			mensaje->contenido.caught_pokemon.id_correlativo= va_arg(args,uint32_t);
			mensaje->contenido.caught_pokemon.caught_confirmation = va_arg(args, uint32_t);
			va_end(args);
			return mensaje;
			break;
		case GET_POKEMON:
			mensaje->contenido.get_pokemon.pokemon= va_arg(args, char*);
			va_end(args);
			return mensaje;
			break;
		case LOCALIZED_POKEMON:;

			va_end(args);
			break;
	}
}




void printear_pokemon(t_pokemon* pokemon){
	printf("nombre:%s\npos x:%d\npos y:%d\n", pokemon->nombre,pokemon->pos_x,pokemon->pos_y);
}

void printear_mensaje(t_mensaje* mensaje){
	printf("----------------------------------------\nMENSAJE\n");
	printf("id:%d\nop_code:%d\n", mensaje->id,mensaje->codigo_operacion);
	switch(mensaje->codigo_operacion){
		case SUBSCRIPCION:// se le pasa el tipo y la cola a subscribirse
			printf("suscripcion:%d\n",mensaje->contenido.subscripcion);
			break;
		case NEW_POKEMON:;
			printear_pokemon(mensaje->contenido.new_pokemon.pokemon);
			printf("cantidad:%d\n", mensaje->contenido.new_pokemon.cantidad);
			break;
		case APPEARED_POKEMON:;
			printear_pokemon(mensaje->contenido.appeared_pokemon.pokemon);
			printf("id correlativo:%d\n", mensaje->contenido.appeared_pokemon.id_correlativo);
			break;
		case CATCH_POKEMON:;
			printear_pokemon(mensaje->contenido.catch_pokemon.pokemon);
			break;
		case CAUGHT_POKEMON:
			printf("id_correlativo:%d\ncaught_confirmation:%d\n", mensaje->contenido.caught_pokemon.id_correlativo,mensaje->contenido.caught_pokemon.caught_confirmation);
			break;
		case GET_POKEMON:
			printf("Pokemon:%s\n", mensaje->contenido.get_pokemon.pokemon);
			break;
		case LOCALIZED_POKEMON:;
			break;
	}

	printf("----------------------------------------\n");

}

int tamanio_pokemon(t_pokemon* pokemon){
	return strlen(pokemon->nombre) + 1 + sizeof(uint32_t)*2;
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
	void* magic = malloc(sizeof(uint32_t) + strlen(get_pokemon.pokemon) + 1);
	memcpy(magic, &(get_pokemon.pokemon), strlen(get_pokemon.pokemon)+1);
	return magic;
}

/* deserializar_get_pokemon
 * void* stream = puntero a deserializar
 * devuelve un get_pokemon deserializado
 */

t_get_pokemon deserializar_get_pokemon(void* stream){
	t_get_pokemon get_pokemon;
	get_pokemon.pokemon = strdup(stream);
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
//todo

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
			tamanio += strlen(mensaje->contenido.get_pokemon.pokemon) + 1;
			break;
		case APPEARED_POKEMON:
			tamanio += sizeof(uint32_t) + tamanio_pokemon(mensaje->contenido.appeared_pokemon.pokemon);
			break;
		case NEW_POKEMON:
			tamanio += sizeof(uint32_t) + tamanio_pokemon(mensaje->contenido.get_pokemon.pokemon);
			break;
		case CATCH_POKEMON:
			tamanio += tamanio_pokemon(mensaje->contenido.get_pokemon.pokemon);
			break;
		case CAUGHT_POKEMON:
			tamanio += sizeof(uint32_t)*2;
			break;
		case LOCALIZED_POKEMON:
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
		//stream = serializar_get_pokemon(mensaje->contenido.get_pokemon);
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
		//stream = serializar_localized_pokemon(mensaje->contenido.localized_pokemon);
		//memcpy(magic + offset, stream, size_contenido_mensaje);
		//offset += size_contenido_mensaje;
		//free(stream);
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
 */


void enviar_mensaje(int socket_a_enviar, t_mensaje* mensaje){
	int size;
	void* stream = serializar_mensaje(mensaje,&size);
	sendall(socket_a_enviar,stream, size);
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
			break;

		default:
			puts("default");
			break;
	}
	return mensaje;
}

/*
int main(void) {
	int socket_servidor;

	return EXIT_SUCCESS;
}
*/
