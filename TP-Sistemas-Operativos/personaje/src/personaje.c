#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <commons/log.h>
#include <commons/string.h>
#include <cliente.h>
#include <estructuras.h>
#include <mensajes.h>
#include "funcionesPersonaje.h"
#include "leerConfigPersonaje.h"

char *dirArchivoConfig;
t_configPersonaje datosPersonaje;
t_list *listaHilosPersonaje;
pthread_mutex_t mutexlistaHilosPersonaje;
int contReintentos = 0;
int vidas;
pthread_mutex_t mutexVidas;
t_log *logger;

void *funcion_personaje(t_infoNivel *);
void manejoDeSeniales(int);


int main(int argc, char *argv[])
{
	bool reintentar = true;

	while(reintentar)
	{
		if(argc > 1)
			dirArchivoConfig = argv[1];

		//lee el archivo de configuracion de personaje
		leerConfigPersonaje(&datosPersonaje, dirArchivoConfig);

		//inicializacion de los semaforos
		pthread_mutex_init(&mutexVidas, NULL);
		pthread_mutex_init(&mutexlistaHilosPersonaje, NULL);

		vidas = datosPersonaje.vidas;

		//crea el archivo de log
		char *nombreArchivoLog = string_from_format("/home/utnso/tp-2013-2c-volvimos-en-forma-de-sockets/logs/%s", datosPersonaje.nombre);
		logger = log_create(nombreArchivoLog, "Personaje", true, LOG_LEVEL_INFO);
		free(nombreArchivoLog);

		log_info(logger, "Personaje: %s - [INICIALIZADO]", datosPersonaje.nombre);

		//inicializacion de las señales
		signal(SIGUSR1, &manejoDeSeniales);
		signal(SIGTERM, &manejoDeSeniales);
		log_debug(logger, "Señales activadas.");

		listaHilosPersonaje = list_create();
		int contNivel = 0;
		t_infoNivel *nivel;

		//lanza un hilo por cada nivel a completar
		while((nivel = list_get(datosPersonaje.niveles, contNivel)) != NULL)
		{
			t_hiloPersonaje *hiloPersonaje = (t_hiloPersonaje *) malloc(sizeof(t_hiloPersonaje));
			hiloPersonaje->nivelID = nivel->nivelID;
			hiloPersonaje->personajeID = datosPersonaje.personajeID;

			pthread_mutex_lock(&mutexlistaHilosPersonaje);
			list_add(listaHilosPersonaje, hiloPersonaje);
			pthread_create(&hiloPersonaje->hiloPersonajeID, NULL, (void *) funcion_personaje, nivel);
			pthread_mutex_unlock(&mutexlistaHilosPersonaje);

			log_debug(logger, "Hilo - Personaje: %s, Nivel: %d - [CREADO]", datosPersonaje.nombre, nivel->nivelID);

			++contNivel;
		}

		//espera a que terminen todos los hilos
		int contHilosPersonaje = 0;
		t_hiloPersonaje *hiloPersonajeAux;

		while((hiloPersonajeAux = list_get(listaHilosPersonaje, contHilosPersonaje)) != NULL)
		{
			pthread_join(hiloPersonajeAux->hiloPersonajeID, NULL);
			++contHilosPersonaje;
		}

		sleep(1);

		if(vidas > 0)
		{
			int srv_sock = inicializarCliente(datosPersonaje.ip_orquestador, datosPersonaje.puerto_orquestador);

			t_planCompletado planCompletado;
			planCompletado.personajeID = datosPersonaje.personajeID;

			t_paquete *paquete = empaquetar(PLAN_COMPLETADO, sizeof(t_planCompletado), &planCompletado);

			if(send(srv_sock, paquete, sizeof(t_paquete), 0) == -1)
			{
				log_error(logger, "Error al enviar mensaje PLAN_COMPLETADO.");
				exit(1);
			}

			log_info(logger, "%s completó el plan de niveles.", datosPersonaje.nombre);
			log_info(logger, "Vidas restantes: %d - Reintentos realizados: %d", vidas, contReintentos);
			log_debug(logger, "Personaje: %s - [ENVIO MENSAJE planCompletado]", datosPersonaje.nombre);

			free(paquete);

			reintentar = false;
		}

		else
		{
			printf("GAME OVER - ¿Desea reintentar? (s/n) - Reintentos realizados: %d\n", contReintentos);

			int opcion = getchar();

			while((opcion != 115) && (opcion != 110))
				opcion = getchar();

			if(opcion == 110)
				reintentar = false;
			else
				++contReintentos;
		}
	}

	//TODO: liberar_memoria();
	log_destroy(logger);

	return EXIT_SUCCESS;
}


void *funcion_personaje(t_infoNivel *infoNivel)
{
	pthread_mutex_lock(&mutexlistaHilosPersonaje);
	bool _funcBuscarHiloPersonaje(t_hiloPersonaje *hiloPersonaje) { return hiloPersonaje->nivelID == infoNivel->nivelID; }
	t_hiloPersonaje *hiloPersonaje = list_find(listaHilosPersonaje, (void *) _funcBuscarHiloPersonaje);
	pthread_mutex_unlock(&mutexlistaHilosPersonaje);

	while(vidas)
	{
		hiloPersonaje->posX = 0;
		hiloPersonaje->posY = 0;
		hiloPersonaje->contRecurso = 0;
		hiloPersonaje->recursoID = '\0';
		hiloPersonaje->recursoPosX = 0;
		hiloPersonaje->recursoPosY = 0;
		hiloPersonaje->distancia = infoNivel->distancia;
		hiloPersonaje->turnoConcedido = false;
		hiloPersonaje->ubicacionRecurso = false;
		hiloPersonaje->seMovioEnX = false;
		hiloPersonaje->murio = false;
		hiloPersonaje->termino = false;

		//inicializa al personaje como cliente
		hiloPersonaje->srv_sock = inicializarCliente(datosPersonaje.ip_orquestador, datosPersonaje.puerto_orquestador);

		t_nuevoPersonaje nuevoPersonaje;
		nuevoPersonaje.personajeID = hiloPersonaje->personajeID;
		nuevoPersonaje.nivelID = hiloPersonaje->nivelID;
		nuevoPersonaje.distancia = hiloPersonaje->distancia;
		crearVectorRecursosNecesitados(nuevoPersonaje.recursosNecesitados, infoNivel->objetivos);

		t_paquete *paquete = empaquetar(NUEVO_PERSONAJE, sizeof(t_nuevoPersonaje), &nuevoPersonaje);

		if(send(hiloPersonaje->srv_sock, paquete, sizeof(t_paquete), 0) == -1)
		{
			log_error(logger, "Error al enviar mensaje NUEVO_PERSONAJE.");
			exit(1);
		}

		log_debug(logger, "Hilo - Personaje: %s, Nivel: %d - [ENVIO MENSAJE nuevoPersonaje]", datosPersonaje.nombre, nuevoPersonaje.nivelID);

		free(paquete);

		t_paquete paqueteAux;

		bzero(paqueteAux.datos, TAMANIO_DATOS);

		if(recv(hiloPersonaje->srv_sock, &paqueteAux, sizeof(t_paquete), MSG_WAITALL) == -1)
		{
			log_error(logger, "Error al recibir mensaje HANDSHAKE.");
			exit(1);
		}

		log_debug(logger, "Hilo - Personaje: %s, Nivel: %d - [RECIBIO MENSAJE handshake]", datosPersonaje.nombre, nuevoPersonaje.nivelID);

		t_infoObjetivo *infoObjetivo;

		while(((infoObjetivo = list_get(infoNivel->objetivos, hiloPersonaje->contRecurso)) != NULL) && (!hiloPersonaje->murio) && (vidas))
		{
			hiloPersonaje->recursoID = infoObjetivo->recursoID;

			bzero(paqueteAux.datos, TAMANIO_DATOS);

			int cantbytes = recv(hiloPersonaje->srv_sock, &paqueteAux, sizeof(t_paquete), MSG_WAITALL);

			if (cantbytes < 0)
			{
				log_error(logger, "Error al recibir el mensaje.");
				exit(1);
			}

			if (cantbytes == 0)
			{
				close(hiloPersonaje->srv_sock);
				return (void *) EXIT_SUCCESS;
			}

			if (cantbytes > 0)
			{
				procesar_mensaje(paqueteAux, hiloPersonaje, logger, datosPersonaje, &vidas, &mutexVidas, infoNivel, contReintentos);
			}
		}

		if(hiloPersonaje->termino)
		{
			t_nivelCompletado nivelCompletado;
			nivelCompletado.personajeID = hiloPersonaje->personajeID;
			nivelCompletado.nivelID = hiloPersonaje->nivelID;

			paquete = empaquetar(NIVEL_COMPLETADO, sizeof(t_nivelCompletado), &nivelCompletado);

			if(send(hiloPersonaje->srv_sock, paquete, sizeof(t_paquete), 0) == -1)
			{
				log_error(logger, "Error al enviar mensaje NIVEL_COMPLETADO.");
				exit(1);
			}

			log_info(logger, "%s completó el Nivel %d.", datosPersonaje.nombre, nuevoPersonaje.nivelID);
			log_debug(logger, "Hilo - Personaje: %s, Nivel: %d - [ENVIO MENSAJE nivelCompletado]", datosPersonaje.nombre, nuevoPersonaje.nivelID);

			free(paquete);

			close(hiloPersonaje->srv_sock);

			return (void *) EXIT_SUCCESS;
		}
	}

	close(hiloPersonaje->srv_sock);

	return (void *) EXIT_SUCCESS;
}


void manejoDeSeniales(int senial)
{
	switch(senial)
	{
		case SIGUSR1:
		{
			++vidas;
			log_info(logger, "%s recibió una vida - [Motivo: Señal SIGUSR1]", datosPersonaje.nombre);
			log_info(logger, "Vidas restantes: %d - Reintentos realizados: %d", vidas, contReintentos);
			signal(SIGUSR1, &manejoDeSeniales);
			signal(SIGTERM, &manejoDeSeniales);

			break;
		}

		case SIGTERM:
		{
			--vidas;
			log_info(logger, "%s perdió una vida - [Motivo: Señal SIGTERM]", datosPersonaje.nombre);
			log_info(logger, "Vidas restantes: %d - Reintentos realizados: %d", vidas, contReintentos);
			signal(SIGUSR1, &manejoDeSeniales);
			signal(SIGTERM, &manejoDeSeniales);

			if(!vidas)
			{
				exit(0);
			}

			break;
		}
	}

	return;
}
