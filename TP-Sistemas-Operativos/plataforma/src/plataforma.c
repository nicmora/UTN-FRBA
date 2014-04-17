#include <servidor.h>
#include <estructuras.h>
#include <pthread.h>
#include <sys/time.h>
#include <commons/log.h>
#include "leerConfigPlataforma.h"
#include "funcionesPlataforma.h"


t_configPlataforma datosPlataforma;
t_log *logger;
t_list *planificadores;
t_list *personajesJugando;
t_list *personajesFinalizados;
pthread_mutex_t mutexPlanificadores;
pthread_mutex_t mutexPersonajes;
char *dirArchivoConfig;


void *funcion_orquestador(uint16_t);
void *funcion_planificador(uint8_t);
void liberar_memoria();

int main(int argc, char *argv[])
{
	if(argc > 1)
		dirArchivoConfig = argv[1];

	//Archivo de Log
	char *nombreArchivoLog = string_from_format("/home/utnso/tp-2013-2c-volvimos-en-forma-de-sockets/logs/%s", "Plataforma");
	logger = log_create(nombreArchivoLog, "Plataforma", true, LOG_LEVEL_INFO);
	free(nombreArchivoLog);

	log_info(logger, "Plataforma inicializada.");

	//Lectura de Configuracion
	leerConfigPlataforma(&datosPlataforma, dirArchivoConfig);

	log_info(logger, "Configuración cargada.");

	pthread_mutex_init(&mutexPlanificadores, NULL);
	pthread_mutex_init(&mutexPersonajes, NULL);

	//Creando hilo Orquestador
	pthread_t hiloOrquestador;
	pthread_create(&hiloOrquestador, NULL, (void *) funcion_orquestador, datosPlataforma.puerto);

	log_info(logger, "Hilo Orquestador creado.");

	pthread_join(hiloOrquestador, NULL);

	int pid = fork();
	if(pid == -1)
	{
		log_error(logger, "Error al invocar koopa");
		exit(1);

	}

	else if(pid == 0)
	{
		execl(datosPlataforma.koopa, datosPlataforma.koopa, datosPlataforma.montaje, datosPlataforma.script, NULL);
	}

	else
	{
		int codigo;
		wait(&codigo);
		log_info(logger, "Koopa devolvio el valor %d", codigo);
		exit(codigo);
	}

	liberar_memoria();

	return EXIT_SUCCESS;
}


void *funcion_orquestador(uint16_t puerto)
{
	int srv_sock = inicializarServidor(puerto);

	personajesJugando = list_create();
	personajesFinalizados = list_create();
	planificadores = list_create();

	while(1)
	{
		log_info(logger, "Escuchando conexiones...");

		int cli_sock = aceptarCliente(srv_sock);

		t_paquete paqueteAux;

		bzero(paqueteAux.datos, TAMANIO_DATOS);

		int nbytes = recv(cli_sock, &paqueteAux, sizeof(t_paquete), MSG_WAITALL);

		if (nbytes < 0)
		{
			log_error(logger, "Error al recibir el mensaje en orquestador.");
			exit(1);
		}

		if (nbytes == 0)
		{
			close(cli_sock);
		}

		if (nbytes > 0)
		{
			switch (paqueteAux.selector)
			{
				case NUEVO_NIVEL:
				{
					log_info(logger, "Nuevo Nivel conectado.");

					t_nuevoNivel *nuevoNivelAux = (t_nuevoNivel *) paqueteAux.datos;

					pthread_mutex_lock(&mutexPlanificadores);
					t_planificador *planificador = crear_planificador(nuevoNivelAux, cli_sock);

					list_add(planificadores, planificador);

					pthread_create(&planificador->hiloPlanificadorID, NULL, (void *) funcion_planificador, planificador->nivelID);
					pthread_mutex_unlock(&mutexPlanificadores);

					log_info(logger, "Hilo Planificador creado.");

					t_handshake handshake;
					handshake.handshake = 1;

					t_paquete *paquete = empaquetar(HANDSHAKE, sizeof(t_handshake), &handshake);

					if (send(cli_sock, paquete, sizeof(t_paquete), 0) == -1)
					{
						log_error(logger, "Error al enviar mensaje HANDSHAKE.");
						exit(1);
					}

					free(paquete);

					log_info(logger, "Mensaje HANDSHAKE enviado.");

					break;
				}

				case NUEVO_PERSONAJE:
				{
					log_info(logger, "Nuevo Personaje conectado.");

					t_nuevoPersonaje *nuevoPersonajeAux = (t_nuevoPersonaje *) paqueteAux.datos;

					pthread_mutex_lock(&mutexPlanificadores);
					pthread_mutex_lock(&mutexPersonajes);
					bool _funcBuscarPlanificador(t_planificador *planificador) { return planificador->nivelID == nuevoPersonajeAux->nivelID; }
					t_planificador *planificador = list_find(planificadores, (void *) _funcBuscarPlanificador);

					t_personaje *personaje = crear_personaje(nuevoPersonajeAux, cli_sock);

					list_add(planificador->personajesListos, personaje);
					agregarFD(cli_sock, &planificador->fdset, &planificador->fdmax);

					bool _funcBuscarPersonaje(t_personaje *personaje) { return personaje->personajeID == nuevoPersonajeAux->personajeID; }
					personaje = list_find(personajesJugando, (void *) _funcBuscarPersonaje);

					if(personaje == NULL)
					{
						personaje = crear_personaje(nuevoPersonajeAux, 0);
						list_add(personajesJugando, personaje);
					}

					++personaje->cantHilos;

					if (send(planificador->fdmin, &paqueteAux, sizeof(t_paquete), 0) == -1)
					{
						log_error(logger, "Error al enviar mensaje NUEVO_PERSONAJE.");
						exit(1);
					}

					log_info(logger, "Mensaje NUEVO_PERSONAJE enviado.");

					t_handshake handshake;
					handshake.handshake = 1;

					t_paquete *paquete = empaquetar(HANDSHAKE, sizeof(t_handshake), &handshake);

					if (send(cli_sock, paquete, sizeof(t_paquete), 0) == -1)
					{
						log_error(logger, "Error al enviar mensaje HANDSHAKE.");
						exit(1);
					}

					free(paquete);

					log_info(logger, "Mensaje HANDSHAKE enviado.");
					pthread_mutex_unlock(&mutexPlanificadores);
					pthread_mutex_unlock(&mutexPersonajes);

					break;
				}

				case PLAN_COMPLETADO:
				{
					log_info(logger, "Mensaje PLAN_COMPLETADO recibido.");

					t_planCompletado *planCompletadoAux = (t_planCompletado *) paqueteAux.datos;

					t_personaje *personaje = malloc(sizeof(t_personaje));

					personaje->personajeID = planCompletadoAux->personajeID;
					personaje->fd = cli_sock;
					personaje->nivelID = 0;
					personaje->posX = 0;
					personaje->posY = 0;
					personaje->recursoBloqueante = '\0';
					personaje->distancia = 0;
					personaje->quantum = 0;
					personaje->planificado = false;
					personaje->cantHilos = 0;
					personaje->recursosAsignados = list_create();
					personaje->recursosNecesitados = list_create();

					pthread_mutex_lock(&mutexPersonajes);
					list_add(personajesFinalizados, personaje);
					pthread_mutex_unlock(&mutexPersonajes);

					close(cli_sock);

					sleep(1);

					if((!list_size(personajesJugando)) && (list_size(personajesFinalizados) > 0))
					{
						return (void *) EXIT_SUCCESS;
					}

					break;
				}
			}
		}
	}

	return (void *) EXIT_SUCCESS;
}


void *funcion_planificador(uint8_t nivel)
{
	pthread_mutex_lock(&mutexPlanificadores);
	bool _funcBuscarPlanificador(t_planificador *planificador) { return planificador->nivelID == nivel; }
	t_planificador *planificador = list_find(planificadores, (void *) _funcBuscarPlanificador);
	pthread_mutex_unlock(&mutexPlanificadores);

	struct timeval tiempoUltimaPlanificacion;
	bool inicio = true;
	long int tiempoTranscurrido = 9999999;

	while (1)
	{
		fd_set fdset_aux;
		FD_ZERO(&fdset_aux);
		pthread_mutex_lock(&mutexPlanificadores);
		int fdmax = planificador->fdmax;
		fdset_aux = planificador->fdset;
		pthread_mutex_unlock(&mutexPlanificadores);

		struct timeval tiempoActual;
		gettimeofday(&tiempoActual, NULL);

		if(!inicio)
		{
			tiempoTranscurrido = (tiempoActual.tv_sec * 1000 + tiempoActual.tv_usec / 1000) - (tiempoUltimaPlanificacion.tv_sec * 1000 + tiempoUltimaPlanificacion.tv_usec / 1000);
		}

	    if ((tiempoTranscurrido >= planificador->retardo) && (list_size(planificador->personajesListos)))
	    {
	    	planificar_personajes(planificador->algoritmo, planificador, &tiempoUltimaPlanificacion, &inicio, &mutexPlanificadores, logger);
	    }

	    long int tiempoRetardo = tiempoTranscurrido > planificador->retardo ? planificador->retardo : (planificador->retardo - tiempoTranscurrido);

	    struct timeval retardo;
	    retardo.tv_sec = tiempoRetardo / 1000;
	    retardo.tv_usec = (tiempoRetardo % 1000) * 1000;

		if (select(fdmax + 1, &fdset_aux, NULL, NULL, &retardo) == -1)
		{
			log_error(logger, "Error en la función select.");
			exit(1);
		}

		for(int i = 0; i <= fdmax; i++)
		{
			if (FD_ISSET(i, &fdset_aux))
			{
				t_paquete paqueteAux;

				bzero(&paqueteAux.datos, TAMANIO_DATOS);

				int nbytes = recv(i, &paqueteAux, sizeof(t_paquete), MSG_WAITALL);

				if (nbytes < 0)
				{
					log_error(logger, "Error al recibir el mensaje en planificador.");
					exit(1);
				}

				if (nbytes == 0)
				{
					if (i == planificador->fdmin)
					{
						log_info(logger, "Nivel desconectado.");

						pthread_mutex_lock(&mutexPlanificadores);
						pthread_mutex_lock(&mutexPersonajes);
						eliminar_planificador(planificador, planificadores);
						pthread_mutex_unlock(&mutexPlanificadores);
						pthread_mutex_unlock(&mutexPersonajes);

						log_info(logger, "Hilo Planificador eliminado.");

						return (void *) EXIT_SUCCESS;
					}

					else
					{
						log_info(logger, "Personaje desconectado.");

						pthread_mutex_lock(&mutexPlanificadores);
						pthread_mutex_lock(&mutexPersonajes);
						eliminar_personaje(planificador, i, personajesJugando, logger);
						pthread_mutex_unlock(&mutexPersonajes);
						pthread_mutex_unlock(&mutexPlanificadores);

						log_info(logger, "Personaje eliminado.");
					}
				}

				if (nbytes > 0)
				{
					procesar_mensaje(i, paqueteAux, planificador, &mutexPlanificadores, personajesJugando, &mutexPersonajes, logger);
				}
			}
		}
	}

	return (void *) EXIT_SUCCESS;
}


void liberar_memoria()
{
//	void _funcDestruirPersonaje(t_personaje *personaje)
//	{
//		list_destroy(personaje->recursosAsignados);
//		list_destroy(personaje->recursosNecesitados);
//		free(personaje);
//
//		return;
//	}
//
//	list_destroy_and_destroy_elements(personajesJugando, (void *) _funcDestruirPersonaje);
//	list_destroy_and_destroy_elements(personajesFinalizados, (void *) _funcDestruirPersonaje);
//
//	void _funcDestruirPlanificador(t_planificador *planificador)
//	{
//		FD_ZERO(&planificador->fdset);
//		list_destroy_and_destroy_elements(planificador->personajesListos, (void *) _funcDestruirPersonaje);
//		list_destroy_and_destroy_elements(planificador->personajesBloqueados, (void *) _funcDestruirPersonaje);
//	}
//
//	list_destroy_and_destroy_elements(planificadores, (void *) _funcDestruirPlanificador);

	free(datosPlataforma.disco);
	free(datosPlataforma.koopa);
	free(datosPlataforma.montaje);
	free(datosPlataforma.script);

	log_destroy(logger);
}
