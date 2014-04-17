#ifndef FUNCIONESPLATAFORMA_H_
#define FUNCIONESPLATAFORMA_H_


#include <stdbool.h>
#include <strings.h>
#include <pthread.h>
#include <servidor.h>
#include <estructuras.h>
#include <commons/collections/list.h>
#include <commons/log.h>


t_planificador *crear_planificador(t_nuevoNivel *, int);
t_personaje *crear_personaje(t_nuevoPersonaje *, int);
void eliminar_planificador(t_planificador *, t_list *);
void eliminar_personaje(t_planificador *, int, t_list *, t_log *);
void asignarRecursosLiberados(t_personaje *, t_planificador *, t_log *);
void sincronizarEstadoPersonajesRecursos(t_personaje *, t_planificador *, t_log *);
void planificar_personajes(char, t_planificador *, struct timeval *, bool *, pthread_mutex_t *, t_log *);
void procesar_mensaje(int, t_paquete, t_planificador *, pthread_mutex_t *, t_list *, pthread_mutex_t *, t_log *);
void crearListaRecursosNecesitados(t_recurso *,t_list *);


#endif /* FUNCIONESPLATAFORMA_H_ */
