/*                                                  HITO 2
 * Objetivos:
Proceso Team: Permitir solamente planificar de forma FIFO un conjunto de entrenadores.

Proceso Broker: Implementación completa de la administración de las colas de mensajes.
Aceptar suscripciones a una cola de mensajes específica.

Proceso GameBoy: Permitir el envío de varios mensajes al proceso Broker y el mensaje Appeared Pokemon
al proceso Team.
*/

/*
 * team.c: In function ‘iniciar_logger’:
team.c:21:10: warning: assignment makes pointer from integer without a cast [-Wint-conversion]
  if((log = log_create("broker.log","log",1,LOG_LEVEL_DEBUG)==NULL))
          ^
team.c: In function ‘subscribirse_a_colas’:
team.c:46:17: warning: implicit declaration of function ‘serializar_subscripcion’ [-Wimplicit-function-declaration]
  void* stream = serializar_subscripcion(2);
                 ^~~~~~~~~~~~~~~~~~~~~~~
team.c:46:17: warning: initialization makes pointer from integer without a cast [-Wint-conversion]
/tmp/ccdsswnv.o: In function `subscribirse_a_colas':
team.c:(.text+0x705): undefined reference to `serializar_subscripcion'
collect2: error: ld returned 1 exit status
 *
 */



#include "team.h"

//#include<readline/readline.h>

//gcc team.c -lpthread -lcommons -o team
//./team

t_config* iniciar_config(void){
	t_config* config;
	if((config = config_create("config"))== NULL)
		perror("Error al crear la config");
	return config;
}

void subscribirse_a_colas(){
	char *ip,*puerto;
	int wait_time, socket_broker, socket_cola_localized, socket_cola_caught, socket_cola_appeared;

	//Obtiene los datos IP,PUERTO WAIT_TIME desde la config

	ip = config_get_string_value(config,"IP_BROKER");
	log_debug(logger,config_get_string_value(config,"IP_BROKER")); //pido y logueo ip
	puerto = config_get_string_value(config,"PUERTO_BROKER");
	log_debug(logger,config_get_string_value(config,"PUERTO_BROKER")); //pido y logueo puerto
	wait_time = config_get_int_value(config,"TIEMPO_RECONEXION");

	// Intenta conexion con el broker y envia la serializacion

	socket_broker = connect_to(ip,puerto,wait_time);

	void* stream = serializar_subscripcion(COLA_APPEARED_POKEMON);


	send(socket_broker, stream, sizeof(uint32_t)*3, 0);
	free(stream);
	//close(socket_broker);

	sleep(3);
	socket_broker = connect_to(ip,puerto,wait_time);
	stream = serializar_subscripcion(COLA_LOCALIZED_POKEMON);
	send(socket_broker, stream, sizeof(uint32_t)*3, 0);
	free(stream);
	//close(socket_broker);

	sleep(3);
	socket_broker = connect_to(ip,puerto,wait_time);
	stream = serializar_subscripcion(COLA_CAUGHT_POKEMON);
	send(socket_broker, stream, sizeof(uint32_t)*3, 0);
	free(stream);
	//close(socket_broker);

	//se subscribe a COLA_GET_POKEMON y sale
}

void inicializar_team(){



	subscribirse_a_colas();



	/*
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
	sleep(3);*/
}


int main(void) {

	logger = log_create("team.log","log",1,LOG_LEVEL_DEBUG);
	config = iniciar_config();
	inicializar_team();

	return EXIT_SUCCESS;
}
