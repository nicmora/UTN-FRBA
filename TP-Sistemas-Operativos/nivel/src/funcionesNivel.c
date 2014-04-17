#include "funcionesNivel.h"


int prioridad = 0;

void procesar_mensaje (int sock_plataforma, int nro_nivel, t_paquete paqueteAux, t_list *ListaItems, t_list *ListaPersonajes, t_configNivel datosNivel, pthread_mutex_t *mutexListaItems, pthread_mutex_t *mutexListaPersonajes, pthread_mutex_t *mutexEnemigos, t_log *logger)
{
	switch(paqueteAux.selector)
	{
		case NUEVO_PERSONAJE:
		{
			t_nuevoPersonaje *nuevoPersonaje = (t_nuevoPersonaje *) paqueteAux.datos;

			pthread_mutex_lock(mutexListaItems);
			pthread_mutex_lock(mutexListaPersonajes);
			t_personaje *personaje = (t_personaje *) malloc(sizeof(t_personaje));
			personaje->personajeID = nuevoPersonaje->personajeID;
			personaje->nivelID = nuevoPersonaje->nivelID;
			personaje->recursosAsignados = list_create();
			personaje->recursosNecesitados = list_create();
			personaje->posX = 0;
			personaje->posY = 0;
			personaje->recursoBloqueante = '\0';
			personaje->prioridad = prioridad;
			++prioridad;
			crearListaRecursosNecesitados(nuevoPersonaje->recursosNecesitados,personaje->recursosNecesitados);

			ITEM_NIVEL *itemPersonaje = malloc(sizeof(ITEM_NIVEL));
			itemPersonaje->id = nuevoPersonaje->personajeID;
			itemPersonaje->posx = 0;
			itemPersonaje->posy = 0;
			itemPersonaje->item_type = PERSONAJE_ITEM_TYPE;

			list_add(ListaItems,itemPersonaje);
			list_add(ListaPersonajes,personaje);
			nivel_gui_dibujar(ListaItems,datosNivel.nombre);
			pthread_mutex_unlock(mutexListaItems);
			pthread_mutex_unlock(mutexListaPersonajes);

			log_info(logger, "Personaje: %c conectado.", personaje->personajeID);

			break;
		}

		case UBICACION_RECURSO:
		{
			log_info(logger, "Mensaje UBICACION_RECURSO recibido.");

			t_ubicacionRecurso *ubicacionRecurso = (t_ubicacionRecurso *) paqueteAux.datos;

			pthread_mutex_lock(mutexListaItems);
			bool _funcBuscarRecurso(ITEM_NIVEL *item){ return item->id == ubicacionRecurso->recursoID; }
			ITEM_NIVEL *itemBuscado = list_find(ListaItems,(void *)_funcBuscarRecurso);
			pthread_mutex_unlock(mutexListaItems);

			ubicacionRecurso->posX = itemBuscado->posx;
			ubicacionRecurso->posY = itemBuscado->posy;

			t_paquete *paquete = empaquetar(UBICACION_RECURSO, sizeof(t_ubicacionRecurso), ubicacionRecurso);

			if(send(sock_plataforma,paquete,sizeof(t_paquete),0) == -1)
			{
				log_error(logger, "Error al enviar mensaje UBICACION_RECURSO.");
				exit(1);
			}

			free(paquete);

			log_info(logger, "Mensaje UBICACION_RECURSO enviado.");

			break;
		}

		case UBICACION_PERSONAJE:
		{
			log_info(logger, "Mensaje UBICACION_PERSONAJE recibido.");

			t_ubicacionPersonaje *ubicacionPersonaje = (t_ubicacionPersonaje *) paqueteAux.datos;

			pthread_mutex_lock(mutexListaItems);
			pthread_mutex_lock(mutexListaPersonajes);
			bool _funcBuscarEnListaItems(ITEM_NIVEL *personaje){ return personaje->id == ubicacionPersonaje->personajeID; }
			ITEM_NIVEL *itemPersonaje = list_find(ListaItems,(void *)_funcBuscarEnListaItems);
			bool _funcBuscarEnListaPersonajes(t_personaje *personaje){ return personaje->personajeID == ubicacionPersonaje->personajeID; }
			t_personaje *personaje = list_find(ListaPersonajes,(void *)_funcBuscarEnListaPersonajes);

			if((itemPersonaje != NULL) && (personaje != NULL))
			{
				itemPersonaje->posx = ubicacionPersonaje->posX;
				itemPersonaje->posy = ubicacionPersonaje->posY;

				personaje->posX = ubicacionPersonaje->posX;
				personaje->posY = ubicacionPersonaje->posY;

				bool _funcPisarEnemigo(ITEM_NIVEL *itemEnemigo)
				{
					if(itemEnemigo->item_type == ENEMIGO_ITEM_TYPE)
					{
						return ((itemPersonaje->posx == itemEnemigo->posx) && (itemPersonaje->posy == itemEnemigo->posy));
					}

					return false;
				}

				bool muertePorEnemigo = list_any_satisfy(ListaItems, (void *) _funcPisarEnemigo);

				if(muertePorEnemigo)
				{
					t_victimaEnemigo victimaEnemigo;
					victimaEnemigo.personajeID = itemPersonaje->id;
					victimaEnemigo.nivelID = nro_nivel;

					t_paquete *paquete = empaquetar(VICTIMA_ENEMIGO, sizeof(t_victimaEnemigo), &victimaEnemigo);

					if(send(sock_plataforma, paquete, sizeof(t_paquete), 0) == -1)
					{
						log_error(logger, "Error al enviar mensaje VICTIMA_ENEMIGO.");
						exit(1);
					}

					free(paquete);

					log_info(logger, "Mensaje VICTIMA_ENEMIGO enviado.");
				}

				nivel_gui_dibujar(ListaItems,datosNivel.nombre);
				pthread_mutex_unlock(mutexListaItems);
				pthread_mutex_unlock(mutexListaPersonajes);
			}

			break;
		}

		case INSTANCIA_RECURSO:
		{
			log_info(logger, "Mensaje INSTANCIA_RECURSO recibido.");

			t_instanciaRecurso *instanciaRecurso = (t_instanciaRecurso *) paqueteAux.datos;

			pthread_mutex_lock(mutexListaItems);
			pthread_mutex_lock(mutexListaPersonajes);
			bool _funcBuscarRecursoEnItems(ITEM_NIVEL *rec){ return rec->id == instanciaRecurso->recursoID;	}
			ITEM_NIVEL *itemRecurso = list_find(ListaItems,(void *)_funcBuscarRecursoEnItems);

			bool _funcBuscarPersonaje(t_personaje *pers){ return pers->personajeID == instanciaRecurso->personajeID; }
			t_personaje *personaje = list_find(ListaPersonajes,(void *)_funcBuscarPersonaje);

			if(personaje != NULL)
			{
				if(itemRecurso->quantity)
				{
					--itemRecurso->quantity;

					bool _funcBuscarRecurso(t_recurso *recurso) { return recurso->recursoID == instanciaRecurso->recursoID; }
					t_recurso *recursoAsignado = list_find(personaje->recursosAsignados, (void *) _funcBuscarRecurso);
					t_recurso *recursoNecesitado = list_find(personaje->recursosNecesitados, (void *) _funcBuscarRecurso);

					if(recursoAsignado != NULL)
					{
						++recursoAsignado->cantidad;
					}
					else
					{
						t_recurso *recurso = malloc(sizeof(t_recurso));
						recurso->recursoID = instanciaRecurso->recursoID;
						recurso->cantidad = 1;
						list_add (personaje->recursosAsignados, recurso);
					}

					--recursoNecesitado->cantidad;

					if(recursoNecesitado->cantidad == 0)
					{
						list_remove_by_condition(personaje->recursosNecesitados, (void *) _funcBuscarRecurso);
					}

					if (send(sock_plataforma,&paqueteAux,sizeof(t_paquete),0) == -1)
					{
						log_error(logger, "Error al enviar mensaje RESPUESTA.");
						exit(1);
					}

					log_info(logger, "Mensaje INSTANCIA_RECURSO enviado.");

				}
				else
				{
					personaje->recursoBloqueante = instanciaRecurso->recursoID;
				}
			}

			nivel_gui_dibujar(ListaItems,datosNivel.nombre);
			pthread_mutex_unlock(mutexListaItems);
			pthread_mutex_unlock(mutexListaPersonajes);


			break;
		}

		case SINCRONIZAR_DATOS:
		{
			log_info(logger, "Mensaje SINCRONIZAR_DATOS recibido.");

			t_sincronizarDatos *sincronizarDatos = (t_sincronizarDatos *) paqueteAux.datos;

			pthread_mutex_lock(mutexListaItems);
			pthread_mutex_lock(mutexListaPersonajes);
			//1) Eliminar Personaje desconectado
			bool _funcBuscarItemPersonaje(ITEM_NIVEL *personaje) { return personaje->id == sincronizarDatos->personajeIDDesconectado; }
			list_remove_by_condition(ListaItems, (void *) _funcBuscarItemPersonaje);
			bool _funcBuscarPersonaje(t_personaje *personaje) { return personaje->personajeID == sincronizarDatos->personajeIDDesconectado; }
			t_personaje *personajeDesconectado = list_remove_by_condition(ListaPersonajes, (void *) _funcBuscarPersonaje);

			if(personajeDesconectado != NULL)
			{
				list_destroy(personajeDesconectado->recursosAsignados);
				list_destroy(personajeDesconectado->recursosNecesitados);
				free(personajeDesconectado);

				//2) Actualizar Item Recursos disponibles
				int i = 0;

				while(sincronizarDatos->recursosSobrantes[i].recursoID != '\0')
				{
					bool _funcBuscarItemRecurso(ITEM_NIVEL *recurso) { return recurso->id == sincronizarDatos->recursosSobrantes[i].recursoID; }
					ITEM_NIVEL *itemRecurso = list_find(ListaItems, (void *) _funcBuscarItemRecurso);
					itemRecurso->quantity += sincronizarDatos->recursosSobrantes[i].cantidad;
					++i;
				}

				i = 0;

				while(sincronizarDatos->estadosPersonajes[i].personajeID != '\0')
				{
					bool _funcBuscarPersonajeParaSincronizar(t_personaje *personaje) { return personaje->personajeID == sincronizarDatos->estadosPersonajes[i].personajeID; }
					t_personaje *personaje = list_find(ListaPersonajes, (void *) _funcBuscarPersonajeParaSincronizar);

					if(personaje != NULL)
					{
						personaje->recursoBloqueante = sincronizarDatos->estadosPersonajes[i].recursoBloqueante;

						list_clean(personaje->recursosAsignados);
						list_clean(personaje->recursosNecesitados);

						int j = 0;

						while(sincronizarDatos->estadosPersonajes[i].recursosAsignados[j].recursoID != '\0')
						{
							t_recurso *recursoAsignado = (t_recurso *) malloc(sizeof(t_recurso));
							recursoAsignado->recursoID = sincronizarDatos->estadosPersonajes[i].recursosAsignados[j].recursoID;
							recursoAsignado->cantidad = sincronizarDatos->estadosPersonajes[i].recursosAsignados[j].cantidad;
							recursoAsignado->posX = 0;
							recursoAsignado->posY = 0;
							list_add(personaje->recursosAsignados, recursoAsignado);
							++j;
						}

						j = 0;

						while(sincronizarDatos->estadosPersonajes[i].recursosNecesitados[j].recursoID != '\0')
						{
							t_recurso *recursoNecesitado = (t_recurso *) malloc(sizeof(t_recurso));
							recursoNecesitado->recursoID = sincronizarDatos->estadosPersonajes[i].recursosNecesitados[j].recursoID;
							recursoNecesitado->cantidad = sincronizarDatos->estadosPersonajes[i].recursosNecesitados[j].cantidad;
							recursoNecesitado->posX = 0;
							recursoNecesitado->posY = 0;
							list_add(personaje->recursosNecesitados, recursoNecesitado);
							++j;
						}
					}

					++i;
				}
			}

			nivel_gui_dibujar(ListaItems, datosNivel.nombre);
			pthread_mutex_unlock(mutexListaItems);
			pthread_mutex_unlock(mutexListaPersonajes);

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


void crearListaPersonajeTemporal(t_list *ListaPrincipal, t_list *ListaDuplicada)
{
	int i = 0;
	t_personaje *p;

	while((p = list_get(ListaPrincipal, i)) != NULL)
	{
		t_personaje *personajeTemporal = (t_personaje *) malloc(sizeof(t_personaje));
		personajeTemporal->personajeID = p->personajeID;
		personajeTemporal->recursoBloqueante = p->recursoBloqueante;
		personajeTemporal->prioridad = p->prioridad;
		personajeTemporal->recursosAsignados = list_create();
		personajeTemporal->recursosNecesitados = list_create();

		t_recurso *r;
		int j = 0;

		while((r = list_get(p->recursosAsignados, j)) != NULL)
		{
			t_recurso *recursoAsignado = (t_recurso *) malloc (sizeof(t_recurso));
			recursoAsignado->recursoID = r->recursoID;
			recursoAsignado->cantidad = r->cantidad;
			list_add(personajeTemporal->recursosAsignados, recursoAsignado);
			++j;
		}

		j = 0;

		while((r = list_get(p->recursosNecesitados, j)) != NULL)
		{
			t_recurso *recursoNecesitado = (t_recurso *) malloc (sizeof(t_recurso));
			recursoNecesitado->recursoID = r->recursoID;
			recursoNecesitado->cantidad = r->cantidad;
			list_add(personajeTemporal->recursosNecesitados, recursoNecesitado);
			++j;
		}

		list_add(ListaDuplicada, personajeTemporal);

		++i;
	}

	return;
}


void crearListaRecursosTemporal(t_list *ListaPrincipal, t_list *ListaDuplicada)
{
	int i = 0;
	ITEM_NIVEL *r;

	while((r = list_get(ListaPrincipal, i)) != NULL)
	{
		if(r->item_type == RECURSO_ITEM_TYPE)
		{
			t_recurso *recurso = (t_recurso *) malloc(sizeof(t_recurso));
			recurso->recursoID = r->id;
			recurso->cantidad = r->quantity;
			list_add(ListaDuplicada, recurso);
		}

		++i;
	}

	return;
}


t_personaje *elegirPersonajeInterbloqueado(t_list *ListaPersonajes)
{
	bool _funcOrdenarPersonajesPorPrioridad(t_personaje *personaje, t_personaje *otroPersonaje) { return personaje->prioridad < otroPersonaje->prioridad; }
	if(list_size(ListaPersonajes) > 1)
	{
		list_sort(ListaPersonajes, (void *) _funcOrdenarPersonajesPorPrioridad);
	}

	t_personaje *personaje = list_get(ListaPersonajes, 0);

	return personaje;
}


