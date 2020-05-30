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
 *
 *
 *
 * codigo_operacion = codigo sobre el cual se decidira que accion tomar
 * ... = lista variable, depende del codigo de operacion que hacer
 *
 *
 * TENER EN CUENTA QUE SE MALLOCKEAN T_MENSAJES!! HAY QUE VER DONDE HACERLES FREE.
 *
 *
 * Deberia ir en mensajes.c???
 */

t_mensaje* crear_mensaje(int argc, ...){
	va_list args;
	va_start(args, argc);

	t_pokemon* pokemon;
	t_mensaje* mensaje = malloc(sizeof(t_mensaje));

	mensaje->codigo_operacion = va_arg(args, uint32_t);

	switch(mensaje->codigo_operacion){
		case SUBSCRIPCION:// se le pasa el tipo y la cola a subscribirse
			mensaje->contenido.subscripcion = va_arg(args, uint32_t);
			va_end(args);
			return mensaje;
			break;
		case NEW_POKEMON:;
			pokemon = malloc(sizeof(t_pokemon));
			pokemon->nombre = va_arg(args, char*);
			pokemon->pos_x = va_arg(args, uint32_t);
			pokemon->pos_y = va_arg(args, uint32_t);
			mensaje->contenido.new_pokemon->pokemon=pokemon;
			mensaje->contenido.new_pokemon->cantidad = va_arg(args,uint32_t);
			va_end(args);
			return mensaje;
			break;
		case APPEARED_POKEMON:;
			pokemon = malloc(sizeof(t_pokemon));
			pokemon->nombre = va_arg(args, char*);
			pokemon->pos_x = va_arg(args, uint32_t);
			pokemon->pos_y = va_arg(args, uint32_t);
			mensaje->contenido.appeared_pokemon->pokemon = pokemon;
			mensaje->contenido.appeared_pokemon = va_arg(args,uint32_t);
			va_end(args);
			return mensaje;
			break;
		case CATCH_POKEMON:;
			t_pokemon* pokemon = malloc(sizeof(t_pokemon));
			pokemon->nombre = va_arg(args, char*);
			pokemon->pos_x = va_arg(args, uint32_t);
			pokemon->pos_y = va_arg(args, uint32_t);
			mensaje->contenido.new_pokemon->pokemon;
			va_end(args);
			return mensaje;
			break;
		case CAUGHT_POKEMON:
			mensaje->contenido.caught_pokemon->id_correlativo= va_arg(args,uint32_t);
			mensaje->contenido.caught_pokemon->caught_confirmation = va_arg(args, bool);
			va_end(args);
			return mensaje;
			break;
		case GET_POKEMON:
			mensaje->contenido.get_pokemon->pokemon= va_arg(args, char*);
			mensaje->contenido.get_pokemon->size_pokemon = strlen(mensaje->contenido.get_pokemon->pokemon) + 1;
			va_end(args);
			break;
	}
}

void printear_pokemon(t_pokemon* pokemon){
	printf("nombre:%s\npos x:%d\npos y:%d\n", pokemon->nombre,pokemon->pos_x,pokemon->pos_y);
}

void printear_mensaje(t_mensaje* mensaje){
	printf("MENSAJE\n");
	printf("id:%d\nop_code:%d\n", mensaje->id,mensaje->codigo_operacion);
	switch(mensaje->codigo_operacion){
		case SUBSCRIPCION:// se le pasa el tipo y la cola a subscribirse
			printf("suscripcion:%d\n",mensaje->contenido.subscripcion);
			break;
		case NEW_POKEMON:;
			break;
		case APPEARED_POKEMON:;
			printear_pokemon(mensaje->contenido.appeared_pokemon->pokemon);
			printf("id correlativo:%d\n", mensaje->contenido.appeared_pokemon->id_correlativo);
			printf(":%d\n",mensaje->contenido.subscripcion);
			break;
		case CATCH_POKEMON:;
			break;
		case CAUGHT_POKEMON:
			break;
		case GET_POKEMON:
			break;
	}



}

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
			tamanio += sizeof(uint32_t) + mensaje->contenido.get_pokemon->size_pokemon;
			break;



	}
	return tamanio;
}

/* serializar_get_pokemon
 * get_pokemon* = puntero de estructura get_pokemon
 * devuelve un void* serializado con el get_pokemon
 */

void* serializar_get_pokemon(t_get_pokemon* get_pokemon){
	void* magic = malloc(sizeof(uint32_t) + strlen(get_pokemon->pokemon) + 1);
	int offset = 0;

	memcpy(magic + offset, &(get_pokemon->size_pokemon), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(magic + offset, &(get_pokemon->pokemon), get_pokemon->size_pokemon);
	offset += get_pokemon->size_pokemon;

	return magic;

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
		uint32_t cola= mensaje->contenido.subscripcion;
		memcpy(magic + offset, &cola, size_contenido_mensaje);
		offset += size_contenido_mensaje;
		break;
	case GET_POKEMON:;
		stream = serializar_get_pokemon(mensaje->contenido.get_pokemon);
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
	//printear_mensaje(mensaje);
	sendall(socket_a_enviar,stream, size);
}


/* deserializar_mensaje
 * codigo_operacion = codigo sobre el cual se decidira que accion tomar
 * stream = donde esta contenida la informacion
 *
 */

t_mensaje* deserializar_mensaje(int codigo_operacion, void* stream){

	switch(codigo_operacion){
		/*case SUBSCRIPCION:;
			int cola_recibida = deserializar_subscripcion(buffer->stream);
			printf("size:%d\n cola: %d\n", buffer->size, cola_recibida);
			t_mensaje* un_mensaje = crear_mensaje(3, codigo_operacion, socket_cliente, cola_recibida);
			return un_mensaje;
			break;*/
		case APPEARED_POKEMON:;
			break;

		default:
			puts("default");
			break;
	}
}

/*
int main(void) {
	int socket_servidor;

	return EXIT_SUCCESS;
}
*/
