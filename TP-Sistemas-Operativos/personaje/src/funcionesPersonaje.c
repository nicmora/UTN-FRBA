#include "funcionesPersonaje.h"


void crearListaRecursosNecesitados(t_list *recursosNecesitados, t_list *lista_objetivos)
{
	t_list *lista_objetivos_aux = list_create();
	t_infoObjetivo *infoObjetivo_aux;
	int i = 0;

	while((infoObjetivo_aux = list_get(lista_objetivos, i)) != NULL)
	{
		t_infoObjetivo *infoObjetivo = malloc(sizeof(t_infoObjetivo));
		infoObjetivo->recursoID = infoObjetivo_aux->recursoID;
		list_add(lista_objetivos_aux, infoObjetivo);
		++i;
	}

	int cantidad;
	i = 0;

	while((infoObjetivo_aux = list_get(lista_objetivos_aux, i)) != NULL)
	{
		bool _funcBuscarRecurso(t_recurso *recurso) { return recurso->recursoID == infoObjetivo_aux->recursoID; }
		t_recurso *recurso = list_find(recursosNecesitados, (void *) _funcBuscarRecurso);

		if(recurso == NULL)
		{
			bool _funcIgualRecurso(t_infoObjetivo *infoObjetivo) {return infoObjetivo->recursoID == infoObjetivo_aux->recursoID;}
			cantidad = list_count_satisfying(lista_objetivos_aux, (void *) _funcIgualRecurso);

			t_recurso *recursoCantidad = malloc (sizeof(t_recurso));
			recursoCantidad->recursoID = infoObjetivo_aux->recursoID;
			recursoCantidad->cantidad = cantidad;
			recursoCantidad->posX = 0;
			recursoCantidad->posY = 0;

			list_add(recursosNecesitados, recursoCantidad);
		}

		++i;
	}

	list_clean(lista_objetivos_aux);
	list_destroy(lista_objetivos_aux);

	return;
}


void crearVectorRecursosNecesitados(t_recurso *recursosNecesitados, t_list *lista_objetivos)
{
	t_list *listaRecursosNecesitados = list_create();
	crearListaRecursosNecesitados(listaRecursosNecesitados, lista_objetivos);

	int n = 0;
	t_recurso *recurso;

	while((recurso = list_get(listaRecursosNecesitados, n)) != NULL)
	{
		recursosNecesitados[n].recursoID = recurso->recursoID;
		recursosNecesitados[n].cantidad = recurso->cantidad;
		recursosNecesitados[n].posX = 0;
		recursosNecesitados[n].posY = 0;
		free(recurso);
		++n;
	}

	while(n < TAMANIO_VECTOR)
	{
		recursosNecesitados[n].recursoID = '\0';
		recursosNecesitados[n].cantidad = 0;
		recursosNecesitados[n].posX = 0;
		recursosNecesitados[n].posY = 0;
		++n;
	}

	list_destroy(listaRecursosNecesitados);

	return;
}


void procesar_mensaje(t_paquete paqueteAux, t_hiloPersonaje *hiloPersonaje, t_log *logger, t_configPersonaje datosPersonaje, int *vidas, pthread_mutex_t *mutexVidas, t_infoNivel *infoNivel, int contReintentos)
{
	switch(paqueteAux.selector)
	{
		case TURNO_CONCEDIDO:
		{
			log_debug(logger, "Hilo - Personaje: %s, Nivel: %d - [RECIBIO MENSAJE turnoConcedido]", datosPersonaje.nombre, hiloPersonaje->nivelID);

			hiloPersonaje->turnoConcedido = true;

			if(!hiloPersonaje->ubicacionRecurso)
			{
				consultar_ubicacion_recurso(hiloPersonaje, logger, datosPersonaje);
			}

			if(hiloPersonaje->ubicacionRecurso)
			{
				if((hiloPersonaje->posX != hiloPersonaje->recursoPosX) || (hiloPersonaje->posY != hiloPersonaje->recursoPosY))
				{
					if((!hiloPersonaje->seMovioEnX) && hiloPersonaje->turnoConcedido)
					{
						mover_X(hiloPersonaje, logger, datosPersonaje);

						if(hiloPersonaje->posY != hiloPersonaje->recursoPosY)
						{
							hiloPersonaje->seMovioEnX = true;
						}
						hiloPersonaje->turnoConcedido = false;
					}

					if(hiloPersonaje->seMovioEnX && hiloPersonaje->turnoConcedido)
					{
						mover_Y(hiloPersonaje, logger, datosPersonaje);

						if(hiloPersonaje->posX != hiloPersonaje->recursoPosX)
						{
							hiloPersonaje->seMovioEnX = false;
						}
						hiloPersonaje->turnoConcedido = false;
					}
				}

				else
				{
					solicitar_recurso(hiloPersonaje, logger, datosPersonaje);
				}
			}

			break;
		}

		case UBICACION_RECURSO:
		{
			log_debug(logger, "Hilo - Personaje: %s, Nivel: %d - [RECIBIO MENSAJE ubicacionRecurso]", datosPersonaje.nombre, hiloPersonaje->nivelID);

			t_ubicacionRecurso *ubicacionRecursoAux = (t_ubicacionRecurso *) paqueteAux.datos;
			hiloPersonaje->recursoPosX = ubicacionRecursoAux->posX;
			hiloPersonaje->recursoPosY = ubicacionRecursoAux->posY;

			hiloPersonaje->ubicacionRecurso = true;

			log_info(logger, "El recurso: %c se encuentra en la posición: (%d, %d).", ubicacionRecursoAux->recursoID, ubicacionRecursoAux->posX, ubicacionRecursoAux->posY);

			break;
		}

		case INSTANCIA_RECURSO:
		{
			log_debug(logger, "Hilo - Personaje: %s, Nivel: %d - [RECIBIO MENSAJE instanciaRecurso]", datosPersonaje.nombre, hiloPersonaje->nivelID);

			++hiloPersonaje->contRecurso;
			hiloPersonaje->ubicacionRecurso = false;
			hiloPersonaje->turnoConcedido = false;
			hiloPersonaje->seMovioEnX = false;

			t_infoObjetivo *infoObjetivo = list_get(infoNivel->objetivos, hiloPersonaje->contRecurso);

			if(infoObjetivo == NULL)
			{
				hiloPersonaje->termino = true;
			}

			break;
		}

		case VICTIMA_INTERBLOQUEO:
		{
			pthread_mutex_lock(mutexVidas);
			--(*vidas);
			pthread_mutex_unlock(mutexVidas);

			log_info(logger, "%s perdió una vida - [Motivo: Víctima de interbloqueo]", datosPersonaje.nombre);
			log_info(logger, "Vidas restantes: %d - Reintentos realizados: %d", *vidas, contReintentos);

			close(hiloPersonaje->srv_sock);

			hiloPersonaje->murio = true;

			break;
		}

		case VICTIMA_ENEMIGO:
		{
			pthread_mutex_lock(mutexVidas);
			--(*vidas);
			pthread_mutex_unlock(mutexVidas);

			log_info(logger, "%s perdió una vida - [Motivo: Víctima de un enemigo]", datosPersonaje.nombre);
			log_info(logger, "Vidas restantes: %d - Reintentos realizados: %d", *vidas, contReintentos);

			close(hiloPersonaje->srv_sock);

			hiloPersonaje->murio = true;

			break;
		}

		default:
		{
			log_error(logger, "Mensaje desconocido.");
			exit(1);

			break;
		}
	}

	return;
}


void consultar_ubicacion_recurso(t_hiloPersonaje *hiloPersonaje, t_log *logger, t_configPersonaje datosPersonaje)
{
	t_ubicacionRecurso ubicacionRecurso;
	ubicacionRecurso.nivelID = hiloPersonaje->nivelID;
	ubicacionRecurso.recursoID = hiloPersonaje->recursoID;
	ubicacionRecurso.personajeID = hiloPersonaje->personajeID;
	ubicacionRecurso.posX = 0;
	ubicacionRecurso.posY = 0;

	t_paquete *paquete = empaquetar(UBICACION_RECURSO, sizeof(t_ubicacionRecurso), &ubicacionRecurso);

	if(send(hiloPersonaje->srv_sock, paquete, sizeof(t_paquete), 0) == -1)
	{
		log_error(logger, "Error al enviar mensaje UBICACION_RECURSO.");
		exit(1);
	}

	free(paquete);

	log_info(logger, "%s solicitó la ubicación del recurso: %c del Nivel: %d.", datosPersonaje.nombre, ubicacionRecurso.recursoID, hiloPersonaje->nivelID);
	log_debug(logger, "Hilo - Personaje: %s, Nivel: %d - [ENVIO MENSAJE ubicacionRecurso]", datosPersonaje.nombre, hiloPersonaje->nivelID);

	return;
}


void mover_X(t_hiloPersonaje *hiloPersonaje, t_log *logger, t_configPersonaje datosPersonaje)
{
	if(hiloPersonaje->posX < hiloPersonaje->recursoPosX)
	{
		++hiloPersonaje->posX;
	}
	if(hiloPersonaje->posX > hiloPersonaje->recursoPosX)
	{
		--hiloPersonaje->posX;
	}

	t_ubicacionPersonaje ubicacionPersonaje;
	ubicacionPersonaje.nivelID = hiloPersonaje->nivelID;
	ubicacionPersonaje.personajeID = hiloPersonaje->personajeID;
	ubicacionPersonaje.posX = hiloPersonaje->posX;
	ubicacionPersonaje.posY = hiloPersonaje->posY;

	t_paquete *paquete = empaquetar(UBICACION_PERSONAJE, sizeof(t_ubicacionPersonaje), &ubicacionPersonaje);

	if(send(hiloPersonaje->srv_sock, paquete, sizeof(t_paquete), 0) == -1)
	{
		log_error(logger, "Error al enviar mensaje UBICACION_PERSONAJE.");
		exit(1);
	}

	free(paquete);

	log_debug(logger, "%s se encuentra en la posición: (%d, %d) del Nivel: %d.", datosPersonaje.nombre, ubicacionPersonaje.posX, ubicacionPersonaje.posY, hiloPersonaje->nivelID);
	log_debug(logger, "Hilo - Personaje: %s, Nivel: %d - [ENVIO MENSAJE ubicacionPersonaje]", datosPersonaje.nombre, hiloPersonaje->nivelID);

	return;
}


void mover_Y(t_hiloPersonaje *hiloPersonaje, t_log *logger, t_configPersonaje datosPersonaje)
{
	if(hiloPersonaje->posY < hiloPersonaje->recursoPosY)
	{
		++hiloPersonaje->posY;
	}
	if(hiloPersonaje->posY > hiloPersonaje->recursoPosY)
	{
		--hiloPersonaje->posY;
	}

	t_ubicacionPersonaje ubicacionPersonaje;
	ubicacionPersonaje.nivelID = hiloPersonaje->nivelID;
	ubicacionPersonaje.personajeID = hiloPersonaje->personajeID;
	ubicacionPersonaje.posX = hiloPersonaje->posX;
	ubicacionPersonaje.posY = hiloPersonaje->posY;

	t_paquete *paquete = empaquetar(UBICACION_PERSONAJE, sizeof(t_ubicacionPersonaje), &ubicacionPersonaje);

	if(send(hiloPersonaje->srv_sock, paquete, sizeof(t_paquete), 0) == -1)
	{
		log_error(logger, "Error al enviar mensaje UBICACION_PERSONAJE.");
		exit(1);
	}

	free(paquete);

	log_debug(logger, "%s se encuentra en la posición: (%d, %d) del Nivel: %d.", datosPersonaje.nombre, ubicacionPersonaje.posX, ubicacionPersonaje.posY, hiloPersonaje->nivelID);
	log_debug(logger, "Hilo - Personaje: %s, Nivel: %d - [ENVIO MENSAJE ubicacionPersonaje]", datosPersonaje.nombre, hiloPersonaje->nivelID);

	return;
}


void solicitar_recurso(t_hiloPersonaje *hiloPersonaje, t_log *logger, t_configPersonaje datosPersonaje)
{
	t_instanciaRecurso instanciaRecurso;
	instanciaRecurso.personajeID = hiloPersonaje->personajeID;
	instanciaRecurso.nivelID = hiloPersonaje->nivelID;
	instanciaRecurso.recursoID = hiloPersonaje->recursoID;

	t_paquete *paquete = empaquetar(INSTANCIA_RECURSO, sizeof(t_instanciaRecurso), &instanciaRecurso);

	if(send(hiloPersonaje->srv_sock, paquete, sizeof(t_paquete), 0) == -1)
	{
		log_error(logger, "Error al enviar mensaje INSTANCIA_RECURSO.");
		exit(1);
	}

	free(paquete);

	log_info(logger, "%s solicitó el recurso: %c del Nivel: %d.", datosPersonaje.nombre, instanciaRecurso.recursoID, hiloPersonaje->nivelID);
	log_debug(logger, "Hilo - Personaje: %s, Nivel: %d - [ENVIO MENSAJE instanciaRecurso]", datosPersonaje.nombre, hiloPersonaje->nivelID);

	return;
}
