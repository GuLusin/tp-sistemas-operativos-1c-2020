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
	void* magic = malloc(sizeof(int)*2);
	uint32_t op_code = SUBSCRIPCION;
	int offset = 0;

	memcpy(magic + offset, &(op_code), sizeof(int));
	offset += sizeof(uint32_t);
	memcpy(magic + offset, &(cola), sizeof(int));

	return magic;

}

cola_code deserializar_subscripcion(void* stream){
	cola_code cola;

	memcpy(&(cola), stream, sizeof(uint32_t));

	return cola;

}



int main(void) {
	int socket_servidor;

	return EXIT_SUCCESS;
}

