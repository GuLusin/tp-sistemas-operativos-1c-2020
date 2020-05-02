/*
 * planificacion.h
 *
 *  Created on: Apr 20, 2020
 *      Author: madd
 */

#ifndef PLANIFICACION_H
#define PLANIFICACION_H

#include <stdio.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <stdbool.h>

typedef struct {
   int id;
   int posicion_x;
   int posicion_y;
   t_list *pokemones;
   t_list *objetivos;

}t_entrenador;


int distancia_menor(t_list *new_entrenadores,int posicion_pokemon_x,int posicion_pokemon_y);
t_entrenador entrenador_mas_cerca(t_list *new_entrenadores,int posicion_pokemon_x,int posicion_pokemon_y);

//Funciones para mover entrenadores
void entrenador_a_ready(t_list *new_entrenadores,int posicion_pokemon_x,int posicion_pokemon_y);

#endif /* PLANIFICACION_H */
