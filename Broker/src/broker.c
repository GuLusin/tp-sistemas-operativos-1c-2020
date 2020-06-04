/*                                                  HITO 2
 * Objetivos:
Proceso Team: Permitir solamente planificar de forma FIFO un conjunto de entrenadores.

Proceso Broker: Implementación completa de la administración de las colas de mensajes.
Aceptar suscripciones a una cola de mensajes específica.

Proceso GameBoy: Permitir el envío de varios mensajes al proceso Broker y el mensaje Appeared Pokemon
al proceso Team.

    delimitador[] = ",";
    string_split(cadena, delimitador);

*/

#include "broker.h"

bool id_validation(int socket_cliente) {
	send_ack(socket_cliente, ACK); //Broker envia confirmacion del mensaje recibido
	if(check_ack(socket_cliente, ACK))// Broker espera al team para que confirme la confirmacion
		send_ack(socket_cliente, id_mensajes_globales++); // Broker le manda el id del mensaje
		return check_ack(socket_cliente, id_mensajes_globales - 1); //Broker espera la confirmacion del id y sale si es correcta.}

}
//#include<readline/readline.h>

//gcc broker.c -lpthread -lcommons -o broker
//./broker

void manejar_subscripcion(cola_code cola,int socket_cliente){

	switch(cola){
		case COLA_APPEARED_POKEMON:
			pthread_mutex_lock(&mutex_cola_appeared);
			list_add(sockets_cola_appeared, (void*)socket_cliente);
			pthread_mutex_unlock(&mutex_cola_appeared);
			break;
		case COLA_CAUGHT_POKEMON:
			pthread_mutex_lock(&mutex_cola_caught);
			list_add(sockets_cola_caught, (void*)socket_cliente);
			pthread_mutex_unlock(&mutex_cola_caught);
			break;
		case COLA_LOCALIZED_POKEMON:
			pthread_mutex_lock(&mutex_cola_localized);
			list_add(sockets_cola_localized, (void*)socket_cliente);
			pthread_mutex_unlock(&mutex_cola_localized);
			break;
		case COLA_CATCH_POKEMON:
			pthread_mutex_lock(&mutex_cola_catch);
			list_add(sockets_cola_catch, (void*)socket_cliente);
			pthread_mutex_unlock(&mutex_cola_catch);
			break;
		case COLA_GET_POKEMON:
			pthread_mutex_lock(&mutex_cola_get);
			list_add(sockets_cola_get, (void*)socket_cliente);
			pthread_mutex_unlock(&mutex_cola_get);
			break;
		case COLA_NEW_POKEMON:
			pthread_mutex_lock(&mutex_cola_new);
			list_add(sockets_cola_new, (void*)socket_cliente);
			pthread_mutex_unlock(&mutex_cola_new);
			break;
	}
	puts("Mensaje recibido con exito!");
	send_ack(socket_cliente,ACK);
	puts("Aviso de retorno con exito!");
	//id_validation(socket_cliente);
	return;

}

bool manejar_mensaje(t_mensaje* mensaje){
    switch(mensaje->codigo_operacion){
    	case GET_POKEMON:;
    		break;








    }
}


/* recibir_mensaje
 * socket_cliente = socket del cual hace recv para obtener el mensaje que llega.
 *
 * Primero delega a serializar buffer el cual devuelve un t_mensaje* el cual luego es pasado a
 * manejar mensaje que se encarga de procesarlo.
 */



void recibir_mensaje(int *socket_cliente){
	//printf("a recibir mensaje le llega el socket %d\n",*socket_cliente);
	uint32_t codigo_operacion;

	if(recv(*socket_cliente, &(codigo_operacion),sizeof(uint32_t), MSG_WAITALL)==-1){
		perror("Falla recv() op_code");
	}

	//printf("op_code: %d\n", codigo_operacion);

	uint32_t id;

	if(recv(*socket_cliente, &(id), sizeof(uint32_t), MSG_WAITALL) == -1){
		perror("Falla recv() id");
	}

	//printf("id:%d\n", id);

	uint32_t size_contenido_mensaje;

	if(recv(*socket_cliente, &(size_contenido_mensaje), sizeof(uint32_t), MSG_WAITALL) == -1){
		perror("Falla recv() size_contenido_mensaje");
	}


	//printf("size contenido:%d\n", size_contenido_mensaje);

	if(codigo_operacion==SUBSCRIPCION){
		int cola;
		recv(*socket_cliente, &(cola),size_contenido_mensaje, MSG_WAITALL);
		printf("cola:%d\n", cola);
		manejar_subscripcion(cola, *socket_cliente);
		//send_ack(socket_cliente);
		return;
	}

	void* stream = malloc(size_contenido_mensaje);

	if(recv(*socket_cliente, stream, size_contenido_mensaje, MSG_WAITALL) == -1){
		perror("Falla recv() contenido");
	}
	t_mensaje* mensaje = deserializar_mensaje(codigo_operacion, stream);
	mensaje->id=id_mensajes_globales++;
	manejar_mensaje(mensaje);
}


/* recibir_cliente
 * socket_servidor = socket del cual se esperara la solicitud de conexion
 */

void recibir_cliente(int *socket_servidor){
	while(1){
		esperar_cliente(*socket_servidor,recibir_mensaje);
	}
}

void remover_socket(t_list* lista, int un_socket){
	int i=-1;
	bool es_igual(void* otro_socket){
		i++;
		return un_socket==(int)otro_socket;
	}
	list_find(lista, (void*)es_igual);
	printf("i:%d\n",i);
	int socket_desechado = list_remove(lista, i);
	printf("socket_desechado:%d\n", socket_desechado);
	close(socket_desechado);
}

void notificar_mensaje(t_mensaje* mensaje){
	bool r;
	int *n;
	int num;
	switch(mensaje->codigo_operacion){
		case APPEARED_POKEMON:;
			void enviar_appeared(void* socket_cola){
				num = socket_cola;
				//num = *n;
				printf("SOCKET: %d",num);
				puts("MENSAJE");
				enviar_mensaje((int)socket_cola, mensaje);
				puts("CHECK");
				r = check_ack((int)socket_cola, ACK);
				if(r){
					puts("todo bien");
				}else{
					puts("envio fallo");
					remover_socket(sockets_cola_appeared,(int)socket_cola);
				}
			}
			list_iterate(sockets_cola_appeared,enviar_appeared);
			break;
		case LOCALIZED_POKEMON:;
			void enviar_localized(void* socket_cola){
				enviar_mensaje((int)socket_cola, mensaje);
				r = check_ack((int)socket_cola, ACK);
				if(r){
					puts("todo bien");
				}else{
					puts("envio fallo");
					remover_socket(sockets_cola_localized,(int)socket_cola);
				}
		}
		list_iterate(sockets_cola_localized,enviar_localized);
			break;
	}

}

void enviar_appeared_pokemon(t_pokemon* pokemon){
	puts("envia appeared");
	int id_correlativo = 455;
	t_mensaje* mensaje = crear_mensaje(5, APPEARED_POKEMON,pokemon->nombre,pokemon->pos_x,pokemon->pos_y,id_correlativo);
	int socket_aux = (int)list_get(sockets_cola_appeared,0);
	printear_mensaje(mensaje);
	enviar_mensaje(socket_aux,mensaje);
	printf("Se envio el mensaje al socket:%d\n", socket_aux);
	check_ack(socket_aux,ACK);
	puts("ACK con exito\n");
}

void printear_lista(t_list* lista){
	void printear_elemento(void* elem){
		printf("%d,",(int)elem);
	}
	list_iterate(lista,printear_elemento);
}

void printear_estado(){
	printf("sockets_cola_localized:");
	printear_lista(sockets_cola_localized);
	printf("\nsockets_cola_caught:");
	printear_lista(sockets_cola_caught);
	printf("\nsockets_cola_appeared:");
	printear_lista(sockets_cola_appeared);
	printf("\n");
}

void inicializar_broker(){

	char *ip,*puerto;

	id_mensajes_globales=0;

	pthread_t pthread_atender_cliente;
	pthread_mutex_init(&mutex_cola_new, NULL);
	pthread_mutex_init(&mutex_cola_get, NULL);
	pthread_mutex_init(&mutex_cola_catch, NULL);
	pthread_mutex_init(&mutex_cola_localized, NULL);
	pthread_mutex_init(&mutex_cola_caught, NULL);
	pthread_mutex_init(&mutex_cola_appeared, NULL);

	logger = log_create("broker.log", "log", true, LOG_LEVEL_DEBUG);
	config = config_create("../config");

	sockets_cola_new = list_create();
	sockets_cola_get = list_create();
	sockets_cola_catch = list_create();
	sockets_cola_localized = list_create();
	sockets_cola_caught = list_create();
	sockets_cola_appeared = list_create();

	ip = config_get_string_value(config,"IP_BROKER");
	log_debug(logger,ip); //pido y logueo ip

    puerto = config_get_string_value(config,"PUERTO_BROKER");
	log_debug(logger,puerto); //pido y logueo puerto

	//revisar esto:
	if((socket_broker = listen_to(ip,puerto)) == -1)
		return;

	log_debug(logger,"Socket: %d, escuchando",socket_broker);	//Socket queda escuchado

	//.............................

	puts("Espera de 5s antes de comenzar a recibir sockets:\n");
	sleep(2);

	pthread_create(&pthread_atender_cliente, NULL,(void*)recibir_cliente, &socket_broker);
	pthread_detach(pthread_atender_cliente);
	log_debug(logger,"Recibi al cliente");	//Recibi al cliente

	//getchar();
    //-------------------------------------------
}

void envio_mensaje(){
	t_pokemon* pokemon;
	t_pokemon* pokemon1;
	t_pokemon* pokemon2;
	t_pokemon* pokemon3;
	t_pokemon* pokemon4;
	t_mensaje* mensaje_aux;
	t_pokemon_especie* especie_pikachu;
	while(true){
		int msg = getchar();
		switch(msg){
			case 'p':
				pokemon = crear_pokemon("Pikachu",-1,2);
				mensaje_aux = crear_mensaje(5, APPEARED_POKEMON,pokemon->nombre,pokemon->pos_x,pokemon->pos_y,455);
				puts("envia appeared pokemon");
				notificar_mensaje(mensaje_aux);
				break;
			case 'P':
				puts("Localized");
				//------SE CREA UN LOCALIZED----------
				pokemon1 = crear_pokemon("Pikachu",-1,2);
				pokemon2 = crear_pokemon("Pikachu",9,5);
				pokemon3 = crear_pokemon("Pikachu",-1,2);
				pokemon4 = crear_pokemon("Pikachu",-1,4);
				especie_pikachu = crear_pokemon_especie("Pikachu");
				agregar_pokemon_a_especie(especie_pikachu,pokemon1);
				agregar_pokemon_a_especie(especie_pikachu,pokemon2);
				agregar_pokemon_a_especie(especie_pikachu,pokemon3);
				agregar_pokemon_a_especie(especie_pikachu,pokemon4);

				mensaje_aux = crear_mensaje(3, LOCALIZED_POKEMON, ID_DEFAULT, especie_pikachu);
				mensaje_aux->id=ID_DEFAULT;

				notificar_mensaje(mensaje_aux);
				break;

			case 'S':
				puts("Localized");
				//------SE CREA UN LOCALIZED----------

				pokemon1 = crear_pokemon("Squirtle",-12,12);
				pokemon2 = crear_pokemon("Squirtle",-12,12);
				pokemon3 = crear_pokemon("Squirtle",-12,12);
				pokemon4 = crear_pokemon("Squirtle",-12,12);
				especie_pikachu = crear_pokemon_especie("Squirtle");
				agregar_pokemon_a_especie(especie_pikachu,pokemon1);
				agregar_pokemon_a_especie(especie_pikachu,pokemon2);
				agregar_pokemon_a_especie(especie_pikachu,pokemon3);
				agregar_pokemon_a_especie(especie_pikachu,pokemon4);

				mensaje_aux = crear_mensaje(3, LOCALIZED_POKEMON, 455, especie_pikachu);
				mensaje_aux->id=ID_DEFAULT;

				notificar_mensaje(mensaje_aux);
				break;
			case 'B':
				puts("Localized");
				//------SE CREA UN LOCALIZED----------
				pokemon1 = crear_pokemon("Bulbasaur",-2,7);
				pokemon2 = crear_pokemon("Bulbasaur",6,3);
				pokemon3 = crear_pokemon("Bulbasaur",-2,7);
				pokemon4 = crear_pokemon("Bulbasaur",5,4);
				especie_pikachu = crear_pokemon_especie("Bulbasaur");
				agregar_pokemon_a_especie(especie_pikachu,pokemon1);
				agregar_pokemon_a_especie(especie_pikachu,pokemon2);
				agregar_pokemon_a_especie(especie_pikachu,pokemon3);
				agregar_pokemon_a_especie(especie_pikachu,pokemon4);

				mensaje_aux = crear_mensaje(3, LOCALIZED_POKEMON, 455, especie_pikachu);
				mensaje_aux->id=ID_DEFAULT;

				notificar_mensaje(mensaje_aux);
				break;
			case 'b':
				pokemon = crear_pokemon("Bulbasaur",9,5);
				mensaje_aux = crear_mensaje(5, APPEARED_POKEMON,pokemon->nombre,pokemon->pos_x,pokemon->pos_y,455);
				puts("envia appeared pokemon");
				notificar_mensaje(mensaje_aux);
				break;
			case 's':
				pokemon = crear_pokemon("Squirtle",4,7);
				mensaje_aux = crear_mensaje(5, APPEARED_POKEMON,pokemon->nombre,pokemon->pos_x,pokemon->pos_y,455);
				puts("envia appeared pokemon");
				notificar_mensaje(mensaje_aux);
				break;
			case 'r':
				printear_estado();
				break;

			default:
				break;
		}

	}

}


int main(void) {



	inicializar_broker();
	sleep(2);

	/*
	t_pokemon* pokemon;
	pokemon = crear_pokemon("Pikachu",-1,2);
	puts("envia appeared pokemon");
	enviar_appeared_pokemon(pokemon);

	pokemon = crear_pokemon("Bulbasaur",9,5);
	puts("envia appeared pokemon");
	enviar_appeared_pokemon(pokemon);

	pokemon = crear_pokemon("Squirtle",4,7);
	puts("envia appeared pokemon");
	enviar_appeared_pokemon(pokemon);


	puts("Localized");
	//------SE CREA UN LOCALIZED----------
	t_pokemon* pokemon1;
	t_pokemon* pokemon2;
	t_pokemon* pokemon3;
	t_pokemon* pokemon4;
	pokemon1 = crear_pokemon("Pikachu",-1,2);
	pokemon2 = crear_pokemon("Pikachu",9,5);
	pokemon3 = crear_pokemon("Pikachu",-1,2);
	pokemon4 = crear_pokemon("Pikachu",-1,4);
	t_pokemon_especie* especie_pikachu = crear_pokemon_especie("Pikachu");
	agregar_pokemon_a_especie(especie_pikachu,pokemon1);
	agregar_pokemon_a_especie(especie_pikachu,pokemon2);
	agregar_pokemon_a_especie(especie_pikachu,pokemon3);
	agregar_pokemon_a_especie(especie_pikachu,pokemon4);

	t_mensaje* mensaje_aux = crear_mensaje(3, LOCALIZED_POKEMON, 3, especie_pikachu);
	mensaje_aux->id=ID_DEFAULT;

	int socket_aux =(int)list_get(sockets_cola_localized,0);
	printear_mensaje(mensaje_aux);
	enviar_mensaje(socket_aux,mensaje_aux);
	printf("Se envio el mensaje al socket:%d\n", socket_aux);
	check_ack(socket_aux,ACK);
	puts("ACK con exito\n");
	*/
	pthread_t envio_mensaje_t;
	pthread_create(&envio_mensaje_t, NULL, (void*)envio_mensaje, NULL);
	sem_t esperar;
	sem_init(&esperar, 0,0);
	sem_wait(&esperar);
	printf("cola appeared: %d\ncola caught: %d\ncola localized: %d\n", (int)list_get(sockets_cola_appeared,0),(int)list_get(sockets_cola_caught,0),(int)list_get(sockets_cola_localized,0));
	close(socket_broker);


	return EXIT_SUCCESS;
}
