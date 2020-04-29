#include "back_end_broker.h"




/* recibir_cliente
 * socket_cliente = socket de la cual se recibiran datos
 */

void recibir_mensaje(int *socket_cliente){

	int codigo_operacion;

	if(recv(*socket_cliente, &(codigo_operacion),sizeof(uint32_t), MSG_WAITALL)==-1){
		perror("Falla recv() op_code");
	}

	int size;

	if(recv(*socket_cliente, &(size), sizeof(uint32_t), MSG_WAITALL) == -1){
		perror("Falla recv() buffer->size");
	}

	void* stream = malloc(size);

	if(recv(*socket_cliente, stream, size, MSG_WAITALL) == -1){
		perror("Falla recv() buffer->stream");
	}

	t_buffer* buffer= malloc(sizeof(t_buffer));
	buffer->size=size;
	buffer->stream=stream;
    deserializar_buffer(codigo_operacion,buffer);
}


/* recibir_cliente
 * socket_servidor = socket del cual se esperara la solicitud de conexion
 */

void recibir_cliente(int socket_servidor){
	while(1){
		//esperar_cliente(socket_servidor,recibir_mensaje);
	}
}
