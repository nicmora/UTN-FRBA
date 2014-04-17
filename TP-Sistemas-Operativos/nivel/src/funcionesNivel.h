#ifndef FUNCIONESNIVEL_H_
#define FUNCIONESNIVEL_H_


#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <strings.h>
#include <cliente.h>
#include <estructuras.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <nivel_gui.h>



void procesar_mensaje (int , int, t_paquete, t_list *, t_list *, t_configNivel, pthread_mutex_t *, pthread_mutex_t *, pthread_mutex_t *, t_log *);
void crearListaRecursosNecesitados(t_recurso *,t_list *);
void crearListaPersonajeTemporal(t_list *, t_list *);
void crearListaRecursosTemporal(t_list *, t_list *);
t_personaje *elegirPersonajeInterbloqueado(t_list *);


#endif /* FUNCIONESNIVEL_H_ */
