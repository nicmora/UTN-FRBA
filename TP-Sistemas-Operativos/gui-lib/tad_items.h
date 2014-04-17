#ifndef TAD_ITEMS_H_
#define TAD_ITEMS_H_

#include "nivel_gui.h"
#include <commons/collections/list.h>


void BorrarItem(t_list* items, char id);
void restarRecurso(t_list* items, char id);
void MoverPersonaje(t_list* items, char personaje, int x, int y);
void MoverEnemigo(t_list* items, char personaje, int x, int y);
void CrearPersonaje(t_list* items, char id, int x , int y);
void CrearEnemigo(t_list* items, char id, int x , int y);
void CrearCaja(t_list* items, char id, int x , int y, int cant);
void CrearItem(t_list* items, char id, int x, int y, char tipo, int cant);


#endif /* TAD_ITEMS_H_ */
