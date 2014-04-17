#include "funcionesPlataforma.h"


t_planificador *crear_planificador(t_nuevoNivel *nuevoNivel, int cli_sock)
{
	t_planificador *planificador = malloc(sizeof(t_planificador));

	FD_ZERO(&planificador->fdset);
	FD_SET (cli_sock, &planificador->fdset);
	planificador->fdmax = cli_sock;
	planificador->fdmin = cli_sock;
	planificador->nivelID = nuevoNivel->nivelID;
	planificador->algoritmo = nuevoNivel->algoritmo;
	planificador->quantum = nuevoNivel->quantum;
	planificador->retardo = nuevoNivel->retardo;
	planificador->personajesListos = list_create();
	planificador->personajesBloqueados = list_create();

	return planificador;
}


t_personaje *crear_personaje(t_nuevoPersonaje *nuevoPersonaje, int cli_sock)
{
	t_personaje *personaje = malloc(sizeof(t_personaje));

	personaje->personajeID = nuevoPersonaje->personajeID;
	personaje->fd = cli_sock;
	personaje->nivelID = nuevoPersonaje->nivelID;
	personaje->posX = 0;
	personaje->posY = 0;
	personaje->recursoBloqueante = '\0';
	personaje->distancia = nuevoPersonaje->distancia;
	personaje->quantum = 0;
	personaje->planificado = false;
	personaje->cantHilos = 0;
	personaje->recursosAsignados = list_create();
	personaje->recursosNecesitados = list_create();
	personaje->terminoQuantum = false;
	crearListaRecursosNecesitados(nuevoPersonaje->recursosNecesitados,personaje->recursosNecesitados);

	return personaje;
}


void eliminar_planificador(t_planificador *planificador, t_list *planificadores)
{
	void *_funcEliminarPersonaje(t_personaje *personaje)
	{
		close(personaje->fd);
		list_destroy(personaje->recursosAsignados);
		list_destroy(personaje->recursosNecesitados);
		free(personaje);

		return (void *) EXIT_SUCCESS;
	}

	list_destroy_and_destroy_elements(planificador->personajesListos, (void *) _funcEliminarPersonaje);
	list_destroy_and_destroy_elements(planificador->personajesBloqueados, (void *) _funcEliminarPersonaje);

	close(planificador->fdmin);

	FD_ZERO(&planificador->fdset);

	bool _funcBuscarPlanificador(t_planificador *planificador_aux) { return planificador_aux->nivelID == planificador->nivelID; }
	list_remove_by_condition(planificadores, (void *) _funcBuscarPlanificador);

	return;
}


void eliminar_personaje(t_planificador *planificador, int cli_sock, t_list *personajesJugando, t_log *logger)
{
	bool _funcBuscarPersonajePorFd(t_personaje *personaje) { return personaje->fd == cli_sock; }
	t_personaje *personaje = list_remove_by_condition(planificador->personajesListos, (void *) _funcBuscarPersonajePorFd);

	if(personaje == NULL)
	{
		personaje = list_remove_by_condition(planificador->personajesBloqueados, (void *) _funcBuscarPersonajePorFd);
	}

	asignarRecursosLiberados(personaje, planificador, logger);

	sincronizarEstadoPersonajesRecursos(personaje, planificador, logger);

	bool _funcBuscarPersonajePorID(t_personaje *personajeJugando) { return personajeJugando->personajeID == personaje->personajeID; }
	t_personaje *personajeJugando = list_find(personajesJugando, (void *) _funcBuscarPersonajePorID);

	if(personajeJugando != NULL)
	{
		--personajeJugando->cantHilos;

		if(!personajeJugando->cantHilos)
		{
			list_remove_by_condition(personajesJugando, (void *) _funcBuscarPersonajePorID);
		}
	}

	close(cli_sock);
	quitarFD(cli_sock, &planificador->fdset, &planificador->fdmax);
	list_destroy(personaje->recursosAsignados);
	list_destroy(personaje->recursosNecesitados);
	free(personaje);

	return;
}


void asignarRecursosLiberados(t_personaje *personajeDesconectado, t_planificador *planificador, t_log *logger)
{
	void _funcAsignarRecursosLiberados(t_recurso *recursoLiberado, t_personaje *personajeBloqueado)
	{
		bool _funcBuscarRecurso(t_recurso *recurso) { return recurso->recursoID == recursoLiberado->recursoID; }
		t_recurso *recursoAsignado = list_find(personajeBloqueado->recursosAsignados, (void *) _funcBuscarRecurso);
		t_recurso *recursoNecesitado = list_find(personajeBloqueado->recursosNecesitados, (void *) _funcBuscarRecurso);

		if(recursoAsignado != NULL)
		{
			++recursoAsignado->cantidad;
			--recursoNecesitado->cantidad;
			--recursoLiberado->cantidad;
		}

		else
		{
			t_recurso *recurso = malloc (sizeof(t_recurso));
			recurso->recursoID = recursoLiberado->recursoID;
			recurso->cantidad = 1;
			--recursoNecesitado->cantidad;
			--recursoLiberado->cantidad;
			list_add(personajeBloqueado->recursosAsignados, recurso);
		}

		if(recursoNecesitado->cantidad == 0)
		{
			list_remove_by_condition(personajeBloqueado->recursosNecesitados, (void *) _funcBuscarRecurso);
		}

		if(recursoLiberado->cantidad == 0)
		{
			list_remove_by_condition(personajeDesconectado->recursosAsignados, (void *) _funcBuscarRecurso);
		}

		bool _funcBuscarPersonajeBloqueado(t_personaje *personajeBloqueado) { return personajeBloqueado->recursoBloqueante == recursoLiberado->recursoID; }
		personajeBloqueado = list_remove_by_condition(planificador->personajesBloqueados, (void *) _funcBuscarPersonajeBloqueado);
		personajeBloqueado->recursoBloqueante = '\0';
		list_add(planificador->personajesListos, personajeBloqueado);

		t_instanciaRecurso instanciaRecurso;
		instanciaRecurso.nivelID = planificador->nivelID;
		instanciaRecurso.personajeID = personajeBloqueado->personajeID;
		instanciaRecurso.recursoID = recursoLiberado->recursoID;

		t_paquete *paquete = empaquetar(INSTANCIA_RECURSO, sizeof(t_instanciaRecurso), &instanciaRecurso);

		if(send(personajeBloqueado->fd, paquete, sizeof(t_paquete), 0) == -1)
		{
			log_error(logger, "Error al enviar mensaje INSTANCIA_RECURSO.");
			exit(1);
		}

		free(paquete);

		log_info(logger, "Mensaje INSTANCIA_RECURSO enviado.");

		return;
	}

	int i = 0;
	t_recurso *recursoLiberado;

	while((recursoLiberado = list_get(personajeDesconectado->recursosAsignados, i)) != NULL)
	{
		t_personaje *personajeBloqueado;
		bool _funcBuscarPersonajeBloqueado(t_personaje *personajeBloqueado) { return personajeBloqueado->recursoBloqueante == recursoLiberado->recursoID; }

		while((recursoLiberado->cantidad > 0) && ((personajeBloqueado = list_find(planificador->personajesBloqueados, (void *) _funcBuscarPersonajeBloqueado)) != NULL))
		{
			_funcAsignarRecursosLiberados(recursoLiberado, personajeBloqueado);
		}

		++i;
	}

	return;
}


void sincronizarEstadoPersonajesRecursos(t_personaje *personajeDesconectado, t_planificador *planificador, t_log *logger)
{
	t_sincronizarDatos sincronizarDatos;
	sincronizarDatos.personajeIDDesconectado = personajeDesconectado->personajeID;

	int i = 0;
	t_recurso *recursoSobrante;

	while((recursoSobrante = list_get(personajeDesconectado->recursosAsignados, i)) != NULL)
	{
		sincronizarDatos.recursosSobrantes[i].recursoID = recursoSobrante->recursoID;
		sincronizarDatos.recursosSobrantes[i].cantidad = recursoSobrante->cantidad;
		++i;
	}

	while(i < TAMANIO_VECTOR)
	{
		sincronizarDatos.recursosSobrantes[i].recursoID = '\0';
		sincronizarDatos.recursosSobrantes[i].cantidad = 0;
		++i;
	}

	i = 0;
	t_personaje *personaje;

	while((personaje = list_get(planificador->personajesListos, i)) != NULL)
	{
		t_recurso *recursoAsignado;
		sincronizarDatos.estadosPersonajes[i].personajeID = personaje->personajeID;
		sincronizarDatos.estadosPersonajes[i].recursoBloqueante = personaje->recursoBloqueante;

		int j = 0;

		while((recursoAsignado = list_get(personaje->recursosAsignados, j)) != NULL)
		{
			sincronizarDatos.estadosPersonajes[i].recursosAsignados[j].recursoID = recursoAsignado->recursoID;
			sincronizarDatos.estadosPersonajes[i].recursosAsignados[j].cantidad = recursoAsignado->cantidad;
			++j;
		}

		while(j < TAMANIO_VECTOR)
		{
			sincronizarDatos.estadosPersonajes[i].recursosAsignados[j].recursoID = '\0';
			sincronizarDatos.estadosPersonajes[i].recursosAsignados[j].cantidad = 0;
			++j;
		}

		j = 0;
		t_recurso *recursoNecesitado;

		while((recursoNecesitado = list_get(personaje->recursosNecesitados, j)) != NULL)
		{
			sincronizarDatos.estadosPersonajes[i].recursosNecesitados[j].recursoID = recursoNecesitado->recursoID;
			sincronizarDatos.estadosPersonajes[i].recursosNecesitados[j].cantidad = recursoNecesitado->cantidad;
			++j;
		}

		while(j < TAMANIO_VECTOR)
		{
			sincronizarDatos.estadosPersonajes[i].recursosNecesitados[j].recursoID = '\0';
			sincronizarDatos.estadosPersonajes[i].recursosNecesitados[j].cantidad = 0;
			++j;
		}

		++i;
	}

	int k = 0;

	while((personaje = list_get(planificador->personajesBloqueados, k)) != NULL)
	{
		t_recurso *recursoAsignado;
		sincronizarDatos.estadosPersonajes[i].personajeID = personaje->personajeID;
		sincronizarDatos.estadosPersonajes[i].recursoBloqueante = personaje->recursoBloqueante;

		int j = 0;

		while((recursoAsignado = list_get(personaje->recursosAsignados, j)) != NULL)
		{
			sincronizarDatos.estadosPersonajes[i].recursosAsignados[j].recursoID = recursoAsignado->recursoID;
			sincronizarDatos.estadosPersonajes[i].recursosAsignados[j].cantidad = recursoAsignado->cantidad;
			++j;
		}

		while(j < TAMANIO_VECTOR)
		{
			sincronizarDatos.estadosPersonajes[i].recursosAsignados[j].recursoID = '\0';
			sincronizarDatos.estadosPersonajes[i].recursosAsignados[j].cantidad = 0;
			++j;
		}

		j = 0;
		t_recurso *recursoNecesitado;

		while((recursoNecesitado = list_get(personaje->recursosNecesitados, j)) != NULL)
		{
			sincronizarDatos.estadosPersonajes[i].recursosNecesitados[j].recursoID = recursoNecesitado->recursoID;
			sincronizarDatos.estadosPersonajes[i].recursosNecesitados[j].cantidad = recursoNecesitado->cantidad;
			++j;
		}

		while(j < TAMANIO_VECTOR)
		{
			sincronizarDatos.estadosPersonajes[i].recursosNecesitados[j].recursoID = '\0';
			sincronizarDatos.estadosPersonajes[i].recursosNecesitados[j].cantidad = 0;
			++j;
		}

		++i;
		++k;
	}

	while(i < TAMANIO_VECTOR)
	{
		sincronizarDatos.estadosPersonajes[i].personajeID = '\0';
		sincronizarDatos.estadosPersonajes[i].recursoBloqueante = '\0';
		++i;
	}

	t_paquete *paquete = empaquetar(SINCRONIZAR_DATOS, sizeof(t_sincronizarDatos), &sincronizarDatos);

	if(send(planificador->fdmin, paquete, sizeof(t_paquete), 0) == -1)
	{
		log_error(logger, "Error al enviar mensaje SINCRONIZAR_DATOS.");
		exit(1);
	}

	free(paquete);

	log_info(logger, "Mensaje SINCRONIZAR_DATOS enviado.");

}


void planificar_personajes(char algoritmo, t_planificador *planificador, struct timeval *tiempoUltimaPlanificacion, bool *inicio, pthread_mutex_t *mutexPlanificadores, t_log *logger)
{
	t_personaje *personajeElegido = NULL;
	t_personaje *personaje = NULL;

	switch (algoritmo)
	{
		case 'R':
		{
			pthread_mutex_lock(mutexPlanificadores);
			personaje = list_get(planificador->personajesListos, 0);
			pthread_mutex_unlock(mutexPlanificadores);

			if(personaje != NULL)
			{
				if(!personaje->planificado) //No Esta Planificado
				{
					if(!personaje->quantum)
					{
						if(personaje->terminoQuantum)
						{
							pthread_mutex_lock(mutexPlanificadores);
							personaje = list_remove(planificador->personajesListos, 0);
							personaje->planificado = false;
							list_add(planificador->personajesListos, personaje);

							personajeElegido = list_get(planificador->personajesListos, 0);
							personajeElegido->quantum = planificador->quantum;
							personajeElegido->planificado = true;
							personajeElegido->terminoQuantum = false;
							pthread_mutex_unlock(mutexPlanificadores);
						}
						else
						{
							pthread_mutex_lock(mutexPlanificadores);
							personajeElegido = list_get(planificador->personajesListos, 0);
							pthread_mutex_unlock(mutexPlanificadores);
							personajeElegido->quantum = planificador->quantum;
							personajeElegido->planificado = true;
						}
					}
					else
					{
						pthread_mutex_lock(mutexPlanificadores);
						personajeElegido = list_get(planificador->personajesListos, 0);
						pthread_mutex_unlock(mutexPlanificadores);
						personajeElegido->planificado = true;
					}
				}
			}

			break;
		}

		case 'S':
		{
			pthread_mutex_lock(mutexPlanificadores);
			personaje = list_get(planificador->personajesListos, 0);
			pthread_mutex_unlock(mutexPlanificadores);

			if(personaje != NULL)
			{
				if(!personaje->planificado)
				{
					if(!personaje->quantum)
					{
						if(list_size(planificador->personajesListos) > 1)
						{
							pthread_mutex_lock(mutexPlanificadores);
							bool _funcOrdenarMinimaDistancia(t_personaje *personaje, t_personaje *otroPersonaje) { return personaje->distancia < otroPersonaje->distancia; }
							list_sort(planificador->personajesListos, (void *) _funcOrdenarMinimaDistancia);
							pthread_mutex_unlock(mutexPlanificadores);
						}

						pthread_mutex_lock(mutexPlanificadores);
						personajeElegido = list_get(planificador->personajesListos, 0);
						pthread_mutex_unlock(mutexPlanificadores);
						personajeElegido->planificado = true;
						personajeElegido->quantum = 1;

					}
					else
					{
						pthread_mutex_lock(mutexPlanificadores);
						bool _funcBuscarPersonajeSinDistancia(t_personaje *personajeSinDistancia) { return personajeSinDistancia->distancia == 999; }
						t_personaje *personajeSinDistancia = list_find(planificador->personajesListos, (void *) _funcBuscarPersonajeSinDistancia);
						pthread_mutex_unlock(mutexPlanificadores);

						if(personajeSinDistancia != NULL)
						{
							t_turnoConcedido turnoConcedido;
							turnoConcedido.turnoConcedido = 1;

							t_paquete *paquete = empaquetar(TURNO_CONCEDIDO, sizeof(t_turnoConcedido), &turnoConcedido);

							if (send(personajeSinDistancia->fd, paquete, sizeof(t_paquete), 0) == -1)
							{
								log_error(logger, "Error al enviar mensaje TURNO_CONCEDIDO.");
								exit(1);
							}

							free(paquete);
						}

						pthread_mutex_lock(mutexPlanificadores);
						personajeElegido = list_get(planificador->personajesListos, 0);
						pthread_mutex_unlock(mutexPlanificadores);
						personajeElegido->planificado = true;
					}
				}
			}

			break;
		}

		default:
		{
			log_error(logger, "Algoritmo desconocido.");
			exit(1);

			break;
		}
	}

	if (personajeElegido != NULL)
	{
		t_turnoConcedido turnoConcedido;
		turnoConcedido.turnoConcedido = 1;

		t_paquete *paquete = empaquetar(TURNO_CONCEDIDO, sizeof(t_turnoConcedido), &turnoConcedido);

		if (send(personajeElegido->fd, paquete, sizeof(t_paquete), 0) == -1)
		{
			log_error(logger, "Error al enviar mensaje TURNO_CONCEDIDO.");
			exit(1);
		}

		free(paquete);

		log_info(logger, "Mensaje TURNO_CONCEDIDO enviado.");

		gettimeofday(tiempoUltimaPlanificacion, NULL);
		*inicio = false;
	}

	return;
}


void procesar_mensaje(int cli_sock, t_paquete paqueteAux, t_planificador *planificador, pthread_mutex_t *mutexPlanificadores, t_list *personajesJugando, pthread_mutex_t *mutexPersonajes, t_log *logger)
{
	switch (paqueteAux.selector)
	{
		case UBICACION_RECURSO:
		{
			log_info(logger, "Mensaje UBICACION_RECURSO recibido.");

			if(cli_sock != planificador->fdmin)
			{
				if (send(planificador->fdmin, &paqueteAux, sizeof(t_paquete), 0) == -1)
				{
					log_error(logger, "Error al enviar mensaje UBICACION_RECURSO.");
					exit(1);
				}

				log_info(logger, "Mensaje UBICACION_RECURSO enviado.");

				pthread_mutex_lock(mutexPlanificadores);
				bool _funcBuscarPersonajePorFD(t_personaje *personaje) { return personaje->fd == cli_sock; }
				t_personaje *personaje = list_find(planificador->personajesListos, (void *) _funcBuscarPersonajePorFD);
				pthread_mutex_unlock(mutexPlanificadores);

				if(personaje != NULL)
				{
					personaje->planificado = false;
				}
			}

			else
			{
				t_ubicacionRecurso *ubicacionRecursoAux = (t_ubicacionRecurso *) paqueteAux.datos;

				pthread_mutex_lock(mutexPlanificadores);
				bool _funcBuscarPersonajePorID(t_personaje *personaje) { return personaje->personajeID == ubicacionRecursoAux->personajeID; }
				t_personaje *personaje = list_find(planificador->personajesListos, (void *) _funcBuscarPersonajePorID);
				pthread_mutex_unlock(mutexPlanificadores);

				if(personaje != NULL)
				{
					personaje->distancia = abs(ubicacionRecursoAux->posX - personaje->posX) + abs(ubicacionRecursoAux->posY - personaje->posY);

					if (send(personaje->fd, &paqueteAux, sizeof(t_paquete), 0) == -1)
					{
						log_error(logger, "Error al enviar mensaje UBICACION_RECURSO.");
						exit(1);
					}

					log_info(logger, "Mensaje UBICACION_RECURSO enviado.");
				}
			}

			break;
		}

		case UBICACION_PERSONAJE:
		{
			log_info(logger, "Mensaje UBICACION_PERSONAJE recibido.");

			t_ubicacionPersonaje *ubicacionPersonaje = (t_ubicacionPersonaje *) paqueteAux.datos;

			pthread_mutex_lock(mutexPlanificadores);
			bool _funcBuscarPersonajePorFD(t_personaje *personaje) { return personaje->fd == cli_sock; }
			t_personaje *personaje = list_find(planificador->personajesListos, (void *) _funcBuscarPersonajePorFD);
			pthread_mutex_unlock(mutexPlanificadores);

			if(personaje != NULL)
			{
				personaje->posX = ubicacionPersonaje->posX;
				personaje->posY = ubicacionPersonaje->posY;

				if (send(planificador->fdmin, &paqueteAux, sizeof(t_paquete), 0) == -1)
				{
					log_error(logger, "Error al enviar mensaje UBICACION_PERSONAJE.");
					exit(1);
				}

				log_info(logger, "Mensaje UBICACION_PERSONAJE enviado.");

				if(planificador->algoritmo == 'R')
				{
					--personaje->quantum;
				}

				if(!personaje->quantum)
				{
					personaje->terminoQuantum = true;
				}

				personaje->planificado = false;
			}

			break;
		}

		case INSTANCIA_RECURSO:
		{
			log_info(logger, "Mensaje INSTANCIA_RECURSO recibido.");

			if(cli_sock != planificador->fdmin)
			{
				t_instanciaRecurso *instanciaRecursoAux = (t_instanciaRecurso *) paqueteAux.datos;

				pthread_mutex_lock(mutexPlanificadores);
				bool _funcBuscarPersonajePorFD(t_personaje *personaje) { return personaje->fd == cli_sock; }
				t_personaje *personaje = list_remove_by_condition(planificador->personajesListos, (void *) _funcBuscarPersonajePorFD);
				pthread_mutex_unlock(mutexPlanificadores);

				if(personaje != NULL)
				{
					personaje->recursoBloqueante = instanciaRecursoAux->recursoID;
					personaje->quantum = 0;
					personaje->planificado = false;
					personaje->distancia = 999;
					personaje->terminoQuantum = false;

					pthread_mutex_lock(mutexPlanificadores);
					list_add (planificador->personajesBloqueados, personaje);
					pthread_mutex_unlock(mutexPlanificadores);

					if (send(planificador->fdmin, &paqueteAux, sizeof(t_paquete), 0) == -1)
					{
						log_error(logger, "Error al enviar mensaje INSTANCIA_RECURSO.");
						exit(1);
					}

					log_info(logger, "Mensaje INSTANCIA_RECURSO enviado.");
				}
			}

			else
			{
				t_instanciaRecurso *instanciaRecursoAux = (t_instanciaRecurso *) paqueteAux.datos;

				pthread_mutex_lock(mutexPlanificadores);
				bool _funcBuscarPersonajePorID(t_personaje *personaje) { return personaje->personajeID == instanciaRecursoAux->personajeID; }
				t_personaje *personaje = list_remove_by_condition(planificador->personajesBloqueados, (void *) _funcBuscarPersonajePorID);
				pthread_mutex_unlock(mutexPlanificadores);

				if(personaje != NULL)
				{
					personaje->recursoBloqueante = '\0';

					bool _funcBuscarRecurso(t_recurso *recurso) { return recurso->recursoID == instanciaRecursoAux->recursoID; }
					t_recurso *recursoAsignado = list_find(personaje->recursosAsignados, (void *) _funcBuscarRecurso);
					t_recurso *recursoNecesitado = list_find(personaje->recursosNecesitados, (void *) _funcBuscarRecurso);

					if(recursoAsignado != NULL)
					{
						++recursoAsignado->cantidad;
					}

					else
					{
						t_recurso *recurso = malloc(sizeof(t_recurso));
						recurso->recursoID = instanciaRecursoAux->recursoID;
						recurso->cantidad = 1;
						list_add (personaje->recursosAsignados, recurso);
					}

					--recursoNecesitado->cantidad;

					if(recursoNecesitado->cantidad == 0)
					{
						list_remove_by_condition(personaje->recursosNecesitados, (void *) _funcBuscarRecurso);
					}

					pthread_mutex_lock(mutexPlanificadores);
					list_add (planificador->personajesListos, personaje);
					pthread_mutex_unlock(mutexPlanificadores);

					if (send(personaje->fd, &paqueteAux, sizeof(t_paquete), 0) == -1)
					{
						log_error(logger, "Error al enviar mensaje INSTANCIA_RECURSO.");
						exit(1);
					}

					log_info(logger, "Mensaje INSTANCIA_RECURSO enviado.");
				}
			}

			break;
		}

		case CAMBIO_ALGORITMO:
		{
			log_info(logger, "Mensaje CAMBIO_ALGORITMO recibido.");

			t_cambioAlgoritmo *cambioAlgoritmo = (t_cambioAlgoritmo *) paqueteAux.datos;

			pthread_mutex_lock(mutexPlanificadores);
			planificador->algoritmo = cambioAlgoritmo->algoritmo;
			planificador->retardo = cambioAlgoritmo->retardo;
			planificador->quantum = cambioAlgoritmo->quantum;
			pthread_mutex_unlock(mutexPlanificadores);

			break;
		}

		case NIVEL_COMPLETADO:
		{
			log_info(logger, "Mensaje NIVEL_COMPLETADO recibido.");

			pthread_mutex_lock(mutexPlanificadores);
			pthread_mutex_lock(mutexPersonajes);
			eliminar_personaje(planificador, cli_sock, personajesJugando, logger);
			pthread_mutex_unlock(mutexPlanificadores);
			pthread_mutex_unlock(mutexPersonajes);

			break;
		}

		case VICTIMA_INTERBLOQUEO:
		{
			log_info(logger, "Mensaje VICTIMA_INTERBLOQUEO recibido.");

			t_victimaInterbloqueo *victimaInterbloqueo = (t_victimaInterbloqueo *) paqueteAux.datos;

			pthread_mutex_lock(mutexPlanificadores);
			bool _funcBuscarPersonaje(t_personaje *personaje) { return personaje->personajeID == victimaInterbloqueo->personajeID; }
			t_personaje *personaje = list_find(planificador->personajesBloqueados, (void *) _funcBuscarPersonaje);
			pthread_mutex_unlock(mutexPlanificadores);

			if(personaje != NULL)
			{
				if(send(personaje->fd,  &paqueteAux, sizeof(t_paquete), 0) == -1)
				{
					log_error(logger, "Error al enviar mensaje VICTIMA_INTERBLOQUEO.");
					exit(1);
				}

				log_info(logger, "Mensaje VICTIMA_INTERBLOQUEO enviado.");
			}

			break;
		}

		case VICTIMA_ENEMIGO:
		{
			log_info(logger, "Mensaje VICTIMA_ENEMIGO recibido.");

			t_victimaEnemigo *victimaEnemigo = (t_victimaEnemigo *) paqueteAux.datos;

			pthread_mutex_lock(mutexPlanificadores);
			bool _funcBuscarPersonaje(t_personaje *personaje) { return personaje->personajeID == victimaEnemigo->personajeID; }
			t_personaje *personaje = list_find(planificador->personajesListos, (void *) _funcBuscarPersonaje);
			pthread_mutex_unlock(mutexPlanificadores);

			if(personaje != NULL)
			{
				if(send(personaje->fd, &paqueteAux, sizeof(t_paquete), 0) == -1)
				{
					log_error(logger, "Error al enviar mensaje VICTIMA_ENEMIGO.");
					exit(1);
				}

				log_info(logger, "Mensaje VICTIMA_ENEMIGO enviado.");
			}

			break;
		}

		default:
		{
			log_error(logger, "Error: Mensaje desconocido.");
			exit(1);

			break;
		}
	}

}


void crearListaRecursosNecesitados(t_recurso *vectorRecursosNecesitados,t_list *listaRecursosNecesitados)
{
	int i = 0;

	while(vectorRecursosNecesitados[i].recursoID != '\0')
	{
		t_recurso *recurso = malloc(sizeof(t_recurso));
		recurso->cantidad = vectorRecursosNecesitados[i].cantidad;
		recurso->recursoID= vectorRecursosNecesitados[i].recursoID;
		list_add(listaRecursosNecesitados,recurso);
		i++;
	}
}
