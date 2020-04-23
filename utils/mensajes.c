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
