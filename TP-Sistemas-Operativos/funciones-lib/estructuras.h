#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_

#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <commons/collections/list.h>
#include <sys/select.h>

#define NUEVO_NIVEL 'a'
#define NUEVO_PERSONAJE 'b'
#define HANDSHAKE 'c'
#define TURNO_CONCEDIDO 'd'
#define UBICACION_RECURSO 'e'
#define UBICACION_PERSONAJE 'f'
#define INSTANCIA_RECURSO 'g'
#define RESPUESTA 'h'
#define VICTIMA_INTERBLOQUEO 'i'
#define VICTIMA_ENEMIGO 'j'
#define NIVEL_COMPLETADO 'k'
#define PLAN_COMPLETADO 'l'
#define CAMBIO_ALGORITMO 'm'
#define SINCRONIZAR_DATOS 'n'
#define REINTENTAR_NIVEL 'o'
#define TAMANIO_DATOS 512
#define TAMANIO_VECTOR 6


//Estructuras para la administracion

typedef struct {
				char personajeID;
				uint8_t nivelID;
				uint8_t posX;
				uint8_t posY;
				uint8_t fd;
				t_list *recursosAsignados;
				t_list *recursosNecesitados;
				char recursoBloqueante;
				uint16_t distancia;
				uint8_t prioridad;
				uint8_t quantum;
				bool planificado;
				bool terminoQuantum;
				uint8_t cantHilos;
				} t_personaje;

typedef struct {
				pthread_t hiloPersonajeID;
				uint8_t srv_sock;
				uint8_t nivelID;
				char personajeID;
				uint8_t posX;
				uint8_t posY;
				uint8_t contRecurso;
				char recursoID;
				uint8_t recursoPosX;
				uint8_t recursoPosY;
				uint16_t distancia;
				bool ubicacionRecurso;
				bool turnoConcedido;
				bool seMovioEnX;
				bool murio;
				bool termino;
				} t_hiloPersonaje;

typedef struct {
				char recursoID;
				uint8_t cantidad;
				uint8_t posX;
				uint8_t posY;
				} t_recurso;

typedef struct {
				uint8_t nivelID;
				t_list *recursos;
				} t_nivel;

typedef struct {
				pthread_t hiloPlanificadorID;
				fd_set fdset;
				uint8_t fdmax;
				uint8_t fdmin;
				uint8_t nivelID;
				char algoritmo;
				uint8_t quantum;
				uint16_t retardo;
				t_list *personajesListos;
				t_list *personajesBloqueados;
				} t_planificador;

typedef struct {
				pthread_t hiloEnemigoID;
				char enemigoID;
				uint8_t posX;
				uint8_t posY;
				char personajeID_victima;
				uint8_t posX_elegida;
				uint8_t posY_elegida;
			   } t_hiloEnemigo;

//Estructuras para mensajes

typedef struct {
				char selector;
				uint32_t tamanio;
				char datos[TAMANIO_DATOS];
				} __attribute__((packed)) t_paquete;

typedef struct {
				uint8_t handshake;
				} __attribute__((packed)) t_handshake;

typedef struct {
				uint8_t turnoConcedido;
				} t_turnoConcedido;

typedef struct {
				uint8_t nivelID;
				char algoritmo;
				uint8_t quantum;
				uint16_t retardo;
				} t_nuevoNivel;

typedef struct {
				char personajeID;
				uint8_t nivelID;
				uint16_t distancia;
				t_recurso recursosNecesitados[TAMANIO_VECTOR];
				} t_nuevoPersonaje;

typedef struct {
				uint8_t nivelID;
				char recursoID;
				char personajeID;
				uint8_t posX;
				uint8_t posY;
				} t_ubicacionRecurso;

typedef struct {
				char personajeID;
				uint8_t nivelID;
				uint8_t posX;
				uint8_t posY;
				} t_ubicacionPersonaje;

typedef struct {
				char personajeID;
				uint8_t nivelID;
				char recursoID;
				} t_instanciaRecurso;

typedef struct {
				bool confirmacion;
				} t_respuesta;

typedef struct {
				char personajeID;
				uint8_t nivelID;
				} t_victimaInterbloqueo;

typedef struct {
				char personajeID;
				uint8_t nivelID;
				} t_victimaEnemigo;

typedef struct {
				char personajeID;
				uint8_t nivelID;
				} t_nivelCompletado;

typedef struct {
				char personajeID;
				} t_planCompletado;

typedef struct {
				uint8_t nivelID;
				char algoritmo;
				uint8_t quantum;
				uint16_t retardo;
			    } t_cambioAlgoritmo;

typedef struct {
				char personajeID;
				t_recurso recursosAsignados[TAMANIO_VECTOR];
				t_recurso recursosNecesitados[TAMANIO_VECTOR];
				char recursoBloqueante;
				} t_estadoPersonaje;
typedef struct {
				char personajeIDDesconectado;
				t_estadoPersonaje estadosPersonajes[TAMANIO_VECTOR];
				t_recurso recursosSobrantes[TAMANIO_VECTOR];
				} t_sincronizarDatos;

typedef struct {
				char personajeID;
				uint8_t nivelID;
				} t_reintentarNivel;


//Estructuras para leer el archivo de configuracion de personaje

typedef struct {
				char recursoID;
				} t_infoObjetivo;

typedef struct {
				uint8_t nivelID;
				t_list *objetivos;
				uint16_t distancia;
				} t_infoNivel;

typedef struct {
				char *nombre;
				char personajeID;
				t_list *niveles;
				uint8_t vidas;
				char *ip_orquestador;
				uint16_t puerto_orquestador;
				char *ip_personaje;
				uint16_t puerto_personaje;
				} t_configPersonaje;


//Estructuras para leer el archivo de configuracion de nivel

typedef struct {
				char *nombre;
				char recursoID;
				uint8_t instancia;
				uint8_t posx;
				uint8_t posy;
				} t_infoRecurso;

typedef struct {
				char *nombre;
				t_list *recursos;
				char *ip_orquestador;
				uint16_t puerto_orquestador;
				char *ip_nivel;
				uint16_t puerto_nivel;
				uint16_t tiempoChequeoDeadlock;
				uint8_t recovery;
				uint8_t enemigos;
				uint16_t sleepEnemigos;
				char algoritmo;
				uint8_t quantum;
				uint16_t retardo;
				} t_configNivel;

//Estructura para leer el archivo de configuracion de plataforma

typedef struct {
				uint16_t puerto;
				char *koopa;
				char *script;
				char *disco;
				char *montaje;
				} t_configPlataforma;


#endif /* ESTRUCTURAS_H_ */
