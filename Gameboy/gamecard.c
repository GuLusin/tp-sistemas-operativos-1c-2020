//gcc broker.c -lpthread -lcommons -o broker
//./broker

t_log* iniciar_logger(void)
{
	return log_create("team.log","log",1,LOG_LEVEL_INFO);
}

void inicializar_broker(){
	t_config* config;
    t_log* logger;
	int socket_broker;
	char * ip,*puerto;

	logger = iniciar_logger();

	if((config = config_create("config"))== NULL)
		perror("Error al crear la config");

	ip = config_get_string_value(config,"IP_BROKER");
	log_info(logger,config_get_string_value(config,"IP_BROKER")); //pido y logueo ip

    puerto = config_get_string_value(config,"PUERTO_BROKER");
	log_info(logger,config_get_string_value(config,"PUERTO_BROKER")); //pido y logueo puerto

	socket_broker = listen_to(ip,puerto);
	log_info(logger,"Socket: %s, escuchando",socket_broker);	//Socket queda escuchado

	recibir_cliente(socket_broker);
	log_info(logger,"Recibi al cliente");	//Recibi al cliente
}
