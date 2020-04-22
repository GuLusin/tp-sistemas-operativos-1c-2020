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

t_log* iniciar_logger(void){
	t_log* log;
	if((log = log_create("broker.log","log",1,LOG_LEVEL_DEBUG)==NULL))
			perror("Error al crear log");
	return log;
}

t_config* iniciar_config(void){
	t_config* config;
	if((config = config_create("config"))== NULL)
		perror("Error al crear la config");
	return config;
}


void subscribirse_a_colas(t_log* logger, t_config* config){
	char *ip,*puerto;
	int wait_time,socket_broker, socket_cola_localized,socket_cola_caught,socket_cola_appeared;

	ip = config_get_string_value(config,"IP_BROKER");
	log_debug(logger,config_get_string_value(config,"IP_BROKER")); //pido y logueo ip
	puerto = config_get_string_value(config,"PUERTO_BROKER");
	log_debug(logger,config_get_string_value(config,"PUERTO_BROKER")); //pido y logueo puerto
	wait_time = config_get_int_value(config,"TIEMPO_RECONEXION");

	socket_broker = connect_to(ip,puerto,wait_time);

	void* stream = serializar_subscripcion(2);

	send(socket_broker, stream, sizeof(uint32_t)*2, 0);


}

void inicializar_team(){
	t_config* config;
	t_log* logger;

	logger = iniciar_logger();
	config = iniciar_config();

	subscribirse_a_colas(logger,config);




	enviar_mensaje(socket_cola_localized, "localized");
	log_debug(logger,"Mensaje %s enviado, al socket: %d.","localized",socket_cola_localized);	//Logueo envio de mensaje
	sleep(3);

	socket_cola_caught = connect_to(ip,puerto,wait_time);
	enviar_mensaje(socket_cola_caught, "caught");
	log_debug(logger,"Mensaje %s enviado, al socket: %d.","caught",socket_cola_caught);	//Logueo envio de mensaje
	sleep(3);

	socket_cola_appeared = connect_to(ip,puerto,wait_time);
	enviar_mensaje(socket_cola_appeared, "appeared");
	log_debug(logger,"Mensaje %s enviado, al socket: %d.","appeared",socket_cola_appeared);	//Logueo envio de mensaje
	sleep(3);
}


int main(void) {


	inicializar_team();

	return EXIT_SUCCESS;
}
