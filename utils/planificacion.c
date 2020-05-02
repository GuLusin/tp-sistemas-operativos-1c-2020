#include "planificacion.h"

int distancia_menor(t_list *new_entrenadores,int posicion_pokemon_x,int posicion_pokemon_y){

	int distancia(void* entrenador_parametro){
		t_entrenador entrenador = *((t_entrenador *) entrenador_parametro);
		return (abs(entrenador.posicion_x - posicion_x_pokemon) + abs(entrenador.posicion_y - posicion_y_pokemon))
	}

	bool es_mayor(void * numero1, void * numero2){
		if (*((int *) numero2) >= *((int *) numero1)){
			return true;
		}
		else
			return false;
	}

	t_list* distancia_de_entrenadores =  list_map(new_entrenadores, (void *)distancia);
    t_list* distancias_ordenadas = list_sort(distancia_de_entrenadores, (void *) es_mayor);
	int distancia_menor =  list_get(distancias_ordenadas, 0);
	return distancia_menor;
}

t_entrenador entrenador_mas_cerca(t_list *new_entrenadores,int posicion_pokemon_x,int posicion_pokemon_y){

	int distancia(void* entrenador_parametro){
		t_entrenador entrenador = *((t_entrenador *) entrenador_parametro);
		return (abs(entrenador.posicion_x - posicion_x_pokemon) + abs(entrenador.posicion_y - posicion_y_pokemon))
	}

	bool es_mayor(void * entrenador){
		if (destancia_menor == distancia(entrenador)){
			return true;
		}
		else
			return false;
	}

	int distancia_menor = distancia_menor(new_entrenadores,posicion_pokemon_x,posicion_pokemon_y);
	t_list* entrenadores_mas_cerca = list_filter(t_list* self, (void *) es_igual_distancia);
	t_entrenador entrenador_elegido = list_get(entrenadores_mas_cerca, 0);	//saco al primero por defecto si hay 2 a la misma distancia
	return entrenador_elegido;
}

void entrenador_a_ready(t_list *new_entrenadores,int posicion_pokemon_x,int posicion_pokemon_y){

	t_entrenador entrenador_elegido;
    int index;
	entrenador_elegido = entrenador_mas_cerca(new_entrenadores, posicion_pokemon_x, posicion_pokemon_y)
	index = entrenador_elegido.id;
	list_add(ready_entrenadores, entrenador);
	list_remove(new_entrenadores,index); //se puede asociar al index con la posicion aunque es una opcion poner el id
	//para asegurarse que se quiere seleccionar a uno especifico
}
