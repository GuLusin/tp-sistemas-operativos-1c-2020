#ifndef BACK_END_BROCKER_H
#define BACK_END_BROCKER_H

#include "../../utils/conexiones.c"
#include "../../utils/mensajes.c"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/list.h>

void recibir_mensaje(int *socket_servidor);
void recibir_cliente(int socket_servidor);





#endif /* BACK_END_BROCKER_H */





