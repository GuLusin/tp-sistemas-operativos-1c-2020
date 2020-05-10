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
		case SUBSCRIPCION:
			mensaje->contenido.subscripcion.socket = va_arg(args, int);
			mensaje->contenido.subscripcion.cola = va_arg(args, int);
			va_end(args);
			return mensaje;
			break;

	}
}


/* enviar_mensaje
 * socket_a_enviar = socket para enviar el mensaje
 * mensaje = mensaje normal, sin serializacion ni empaquetado
 */

void enviar_mensaje(int socket_a_enviar, char* mensaje){
	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->size = strlen(mensaje) + 1;
	void* stream = malloc(buffer->size);
	memcpy(stream, mensaje, buffer->size);
	buffer->stream = stream;
	t_paquete* paquete = malloc(sizeof(t_paquete)); //ARMADO DEL PAQUETE

	paquete->codigo_operacion = STRING;
	paquete->buffer = buffer;

	int tam_paquete = paquete->buffer->size + 2*sizeof(uint32_t);
	void* data_a_enviar = serializar_paquete(paquete,tam_paquete);


	send(socket_a_enviar, data_a_enviar, tam_paquete, 0);
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

t_mensaje* deserializar_buffer(int codigo_operacion, t_buffer* buffer, int socket_cliente){
	void* mensaje = malloc(buffer->size);
	switch(codigo_operacion){
		case STRING:
			puts("Entra a STRING");
			memcpy(mensaje,buffer->stream, buffer->size);
			puts((char *)mensaje);
			break;
		case SUBSCRIPCION: ;
			//log_debug(logger,"Entra a SUBSCRIPCION") lo usariamos para informar que llega una subscripcion, no se puede usar log ya que esta en otro .h
			int cola_recibida = deserializar_subscripcion(buffer->stream);
			printf("size:%d\n cola: %d\n", buffer->size, cola_recibida);
			t_mensaje* un_mensaje = crear_mensaje(3, codigo_operacion, socket_cliente, cola_recibida);
			return un_mensaje;
			break;
		case APPEARED_POKEMON:


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
