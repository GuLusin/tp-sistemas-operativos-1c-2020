/*                                                  HITO 2
 * Objetivos:
Proceso Team: Permitir solamente planificar de forma FIFO un conjunto de entrenadores.

Proceso Broker: Implementación completa de la administración de las colas de mensajes.
Aceptar suscripciones a una cola de mensajes específica.

Proceso GameBoy: Permitir el envío de varios mensajes al proceso Broker y el mensaje Appeared Pokemon
al proceso Team.
*/

#include "team.h"

//#include<readline/readline.h>

//gcc team.c -lpthread -lcommons -o team
//./team

t_log* iniciar_logger(void)
{
	return log_create("team.log","log",1,LOG_LEVEL_INFO);
}

void inicializar_team(){
	t_config* config;
	t_log* logger;
	char *ip,*puerto;
	int wait_time, socket_cola_localized,socket_cola_caught,socket_cola_appeared;

	logger = iniciar_logger();

	if((config = config_create("config"))== NULL) //creo config
		perror("Error al crear la config");

	ip = config_get_string_value(config,"IP_BROKER");
	log_info(logger,config_get_string_value(config,"IP_BROKER")); //pido y logueo ip

	puerto = config_get_string_value(config,"PUERTO_BROKER");
	log_info(logger,config_get_string_value(config,"PUERTO_BROKER")); //pido y logueo puerto

	wait_time = config_get_int_value(config,"TIEMPO_RECONEXION");

	socket_cola_localized = connect_to(ip,puerto,wait_time);
	enviar_mensaje(socket_cola_localized, "localized");
	log_info(logger,"Mensaje %s enviado, al socket: %d.","localized",socket_cola_localized);	//Logueo envio de mensaje
	sleep(3);

	socket_cola_caught = connect_to(ip,puerto,wait_time);
	enviar_mensaje(socket_cola_caught, "caught");
	log_info(logger,"Mensaje %s enviado, al socket: %d.","caught",socket_cola_caught);	//Logueo envio de mensaje
	sleep(3);

	socket_cola_appeared = connect_to(ip,puerto,wait_time);
	enviar_mensaje(socket_cola_appeared, "appeared");
	log_info(logger,"Mensaje %s enviado, al socket: %d.","appeared",socket_cola_appeared);	//Logueo envio de mensaje
	sleep(3);
}


int main(void) {
	t_log* logger; //creo log
	logger = iniciar_logger();
	log_info(logger,"Inicio conexion con el broker");
	inicializar_team();
	log_info(logger,"Termino conexion con el broker");
	log_destroy(logger);
	return EXIT_SUCCESS;
}
