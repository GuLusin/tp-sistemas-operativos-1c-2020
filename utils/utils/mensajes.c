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

	t_mensaje* mensaje = malloc(sizeof(t_mensaje));

	mensaje->codigo_operacion = va_arg(args, int);

	switch(mensaje->codigo_operacion){
		/*case SUBSCRIPCION:
			mensaje->contenido.subscripcion.socket = va_arg(args, int);
			mensaje->contenido.subscripcion.cola = va_arg(args, int);
			va_end(args);
			return mensaje;
			break;*/

	}
}

/* tamanio_contenido_mensaje
 * mensaje = mensaje t_mensaje al que va obtiene el tamaño del contenido
 * devuelve un int con el tamaño del contenido
 */

int tamanio_contenido_mensaje(t_mensaje* mensaje){
	int tamanio=0;
	switch(mensaje->codigo_operacion){
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
	uint32_t size = size_contenido_mensaje + sizeof(uint32_t)*2;
	void* magic = malloc(size);
	int offset = 0;


	memcpy(magic + offset, &(mensaje->id), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(magic + offset, &(mensaje->codigo_operacion), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(magic + offset, &(size_contenido_mensaje), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	switch(mensaje->codigo_operacion){
	case GET_POKEMON:;
		void* stream = serializar_get_pokemon(mensaje->contenido.get_pokemon);
		memcpy(magic + offset, stream, size);
		offset += size;
		break;


	}
	*ret_size=(int)size;
	return magic;

}

/* enviar_mensaje
 * socket_a_enviar = socket para enviar el mensaje
 * mensaje = mensaje t_mensaje
 */


int enviar_mensaje(int socket_a_enviar, t_mensaje* mensaje){
	int size;
	void* stream = serializar_mensaje(mensaje,&size);
	sendall(socket_a_enviar,stream, size);
	if(wait_ack(socket_a_enviar))
		return 1;
	else
		perror("No se pudo enviar mensaje");
}


/* serializar_paquete
 * paquete = paquete armado sin serializar en un flujo continuo
 * tam_paquete = tamaño del mismo y del futuro flujo
 */


void* serializar_paquete(t_paquete* paquete, int tam_paquete){
	void* stream = malloc(tam_paquete);
	int offset = 0;

	memcpy(stream + offset, &(paquete->codigo_operacion), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, &(paquete->buffer->size), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, (paquete->buffer->stream), paquete->buffer->size);

	return stream;
}

/* deserializar_buffer
 * codigo_operacion = codigo sobre el cual se decidira que accion tomar
 * buffer = donde esta contenida la informacion
 * socket_cliente = socket del cliente que mando el buffer
 */

t_mensaje* deserializar_mensaje(int codigo_operacion, t_mensaje* mensaje){

	switch(codigo_operacion){
		case SUBSCRIPCION: ;
			//log_debug(logger,"Entra a SUBSCRIPCION") lo usariamos para informar que llega una subscripcion, no se puede usar log ya que esta en otro .h
			int cola_recibida = deserializar_subscripcion(buffer->stream);
			printf("size:%d\n cola: %d\n", buffer->size, cola_recibida);
			t_mensaje* un_mensaje = crear_mensaje(3, codigo_operacion, socket_cliente, cola_recibida);
			return un_mensaje;
			break;
		case APPEARED_POKEMON:;
			break;

		default:
			puts("default");
			break;
	}
}




void* serializar_subscripcion(cola_code cola){
	void* magic = malloc(sizeof(uint32_t)*3);
	op_code codigo_operacion = SUBSCRIPCION;
	uint32_t size = sizeof(uint32_t);
	int offset = 0;

	memcpy(magic + offset, &(codigo_operacion), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(magic + offset, &(size), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(magic + offset, &(cola), size);

	printf("%d,%d,%d\n", codigo_operacion, size, cola);
	return magic;

}

cola_code deserializar_subscripcion(void* stream){
	puts("ENTRA A DESERIALIZAR");
	cola_code cola;
	memcpy(&cola, stream, sizeof(uint32_t));
	return cola;
}


/*
int main(void) {
	int socket_servidor;

	return EXIT_SUCCESS;
}
*/
