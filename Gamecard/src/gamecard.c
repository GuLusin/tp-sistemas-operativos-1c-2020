#include "gamecard.h"

int subscribirse_a_cola(cola_code cola){
	int socket_aux = connect_to(ip_broker,puerto_broker, tiempo_reintento_conexion);
	t_mensaje* mensaje = crear_mensaje(2, SUBSCRIPCION, cola);
	mensaje->id=ID_SUSCRIPCION;
	enviar_mensaje(socket_aux, mensaje);
	check_ack(socket_aux, ACK);
	return socket_aux;
}

void leer_config() {
	config = config_create("../config");
	tiempo_reintento_conexion = config_get_int_value(config,"TIEMPO_DE_REINTENTO_CONEXION");
	tiempo_reintento_operacion = config_get_int_value(config,"TIEMPO_DE_REINTENTO_OPERACION");
	ip_broker = config_get_string_value(config,"IP_BROKER");
	log_debug(logger,config_get_string_value(config,"IP_BROKER")); //pido y logueo ip
	puerto_broker = config_get_string_value(config,"PUERTO_BROKER");
	log_debug(logger,config_get_string_value(config,"PUERTO_BROKER")); //pido y logueo puerto
	punto_montaje = config_get_string_value(config,"PUNTO_MONTAJE_TALLGRASS");
	//log_debug(logger,config_get_string_value(config,"PUNTO_MONTAJE_TALLGRASS")); // Ver si es necesario loggear esto
}

void inicializar_gamecard() { // Suscribirse a NEW_POKEMON, CATCH_POKEMON, GET_POKEMON
	int socket_broker, socket_cola_new, socket_cola_catch, socket_cola_get;
	logger = log_create("gamecard.log","log",1,LOG_LEVEL_DEBUG);
	leer_config();
	socket_cola_new = subscribirse_a_cola(COLA_NEW_POKEMON);
	socket_cola_catch = subscribirse_a_cola(COLA_CATCH_POKEMON);
	socket_cola_get = subscribirse_a_cola(COLA_GET_POKEMON);
}

int main(void) {
	inicializar_gamecard();
	return EXIT_SUCCESS;
}