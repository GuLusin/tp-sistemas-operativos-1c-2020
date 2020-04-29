/*
 ============================================================================
 Name        : utils.c
 Author      : Madd
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

//CHEQUEAR DONDE SE CIERRA EL SOCKET_CLIENTE

#include "conexiones.h"

/* sendall: funcion sacade de guia beej, asegura que se mande todo el paquete o devuelve error.
 * s = socket al cual enviar
 * buf = stream a enviar
 * len = size del stream
 */

int sendall(int s, void *buf, int *len)
{
    int total = 0;        // cuantos bytes ya enviamos
    int bytesleft = *len; // cuantos bytes nos quedan enviar
    int n;

    while(total < *len) {
        n = send(s, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    *len = total; // devuelve el numero total de

    return n==-1?-1:0; // return -1 si falla, 0 si tiene exito
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

	int bytes_enviados = send(socket_a_enviar, data_a_enviar, tam_paquete, 0);
}

/* connect_to
 * ip = ip a conectarse
 * puerto = puerto del socket a conectarse
 * wait_time = tiempo de espera entre reintentos de conexion (en segundos), en caso de fallo
 */

int connect_to(char* ip, char* puerto,int wait_time){
	struct addrinfo hints;
	struct addrinfo* server_info;

	int socket_cliente;

	memset(&hints,0,sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags=AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	if((socket_cliente=socket(server_info->ai_family,server_info->ai_socktype,server_info->ai_protocol)) == -1){
		perror("No se pudo crear socket");
		return -1;
	}

	// INTENTA CONEXION INDEFINIDAMENTE
	while(connect(socket_cliente,server_info->ai_addr,server_info->ai_addrlen)== -1){
		perror("Fallo Conexion, reintentando...");
		sleep(wait_time);
	}

	freeaddrinfo(server_info);
	return socket_cliente;
}



/* deserializar_buffer
 * codigo_operacion = codigo sobre el cual se decidira que accion tomar
 * buffer = donde esta contenida la informacion
 */

void deserializar_buffer(int codigo_operacion, t_buffer* buffer){
	void* mensaje = malloc(buffer->size);
	switch(codigo_operacion){
		case STRING:
			puts("Entra a STRING");
			memcpy(mensaje,buffer->stream, buffer->size);
			puts((char *)mensaje);
			break;
		case SUBSCRIPCION:
			puts("Entra a SUBSCRIPCION\n");
			//log_debug(logger,"Entra a SUBSCRIPCION") lo usariamos para informar que llega una subscripcion, no se puede usar log ya que esta en otro .h
			cola_code cola_recibida = deserializar_subscripcion(buffer->stream);
			puts("sale de deserializar");

			printf("size:%d\n cola: %d\n", buffer->size, cola_recibida);
			break;

		default:
			puts("default");
			break;
	}
}



/* esperar_cliente
 * socket_servidor = socket en la cual se aceptaran comunicaciones
 * funcion_recibir = funcion especifica que maneja la recepcion del mensaje segun el modulo
 */

void esperar_cliente(int socket_servidor,void* funcion_recibir){
	struct sockaddr_in dir_cliente;
	int tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	pthread_create(&pthread, NULL, funcion_recibir, &socket_cliente);
	pthread_detach(pthread);
}

/* listen_to
 * ip = ip del socket a conectarse
 * puerto = puerto del socket a conectarse
 */

int listen_to(char* ip,char* puerto){

	int socket_servidor;
	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &servinfo);


	if ((socket_servidor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1){
		perror("No se pudo crear socket");
		return -1;
	}

	if (bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
		perror("No se pudo bindear el socket");
		close(socket_servidor);
		return -1;
	}

	puts("Empieza listening");
	if(listen(socket_servidor, 10)==-1){
		perror("No se pudo poner en listen al socket");
		close(socket_servidor);
		return -1;
	}

	freeaddrinfo(servinfo);
	return socket_servidor;
}

/*
int main(void) {
	int socket_servidor;

	socket_servidor = connect_to(IP, PUERTO,2);
	enviar_mensaje(socket_servidor, "Feliz cumple Fabian!");
	puts("Conectado con exito!");
	return EXIT_SUCCESS;
}
*/

