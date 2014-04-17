#ifndef FUNCIONESPERSONAJE_H_
#define FUNCIONESPERSONAJE_H_


#include <cliente.h>
#include <estructuras.h>
#include <mensajes.h>
#include <commons/log.h>
#include <cliente.h>


void crearListaRecursosNecesitados(t_list *, t_list *);
void crearVectorRecursosNecesitados(t_recurso *, t_list *);
void procesar_mensaje(t_paquete, t_hiloPersonaje *, t_log *, t_configPersonaje, int *, pthread_mutex_t *, t_infoNivel *, int);
void consultar_ubicacion_recurso(t_hiloPersonaje *, t_log *, t_configPersonaje);
void mover_X(t_hiloPersonaje *, t_log *, t_configPersonaje);
void mover_Y(t_hiloPersonaje *, t_log *, t_configPersonaje);
void solicitar_recurso(t_hiloPersonaje *, t_log *, t_configPersonaje);


#endif /* FUNCIONESPERSONAJE_H_ */
