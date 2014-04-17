#include "leerConfigNivel.h"
#include "funcionesNivel.h"
#include <pthread.h>
#include <sys/inotify.h>
#include <cliente.h>
#include <nivel_gui.h>
#include <tad_items.h>
#include <strings.h>
#include <unistd.h>
#include <commons/string.h>


//constantes
#define SLEEP_INOTIFY 3
#define RR 'R' //round robin
#define SRDF 'S' //algoritmo shortest remaining distance first
#define TAMANIO_EVENTO (sizeof(struct inotify_event) + 24)
#define TAMANIO_BUFFER (1024 * TAMANIO_EVENTO)


//variables globales
char *dirArchivoConfig;
t_configNivel datosNivel;
uint8_t nro_nivel;
int rows = 167;
int cols = 100;
t_list *ListaItems;
pthread_mutex_t mutexListaItems;
t_list *ListaPersonajes;
pthread_mutex_t mutexListaPersonajes;
pthread_mutex_t mutexHiloInterbloqueo;
pthread_mutex_t mutexEnemigos;
int sock_plataforma;
long int sleepEnemigos;
long int tiempoChequeoDeadlock;
t_log *logger;

//encabezados
void liberar_memoria();
void inicializar_recursos();
void inicializar_gui();
void inicializar_enemigos();
void inicializar_inotify();
void inicializar_interbloqueo();
void *funcion_inotify();
void *funcion_interbloqueo();
void *funcion_enemigo();


int main (int argc, char *argv[])
{
	if(argc > 1)
		dirArchivoConfig = argv[1];

	leerConfigNivel(&datosNivel, dirArchivoConfig);

	sleepEnemigos = datosNivel.sleepEnemigos * 1000;
	tiempoChequeoDeadlock = datosNivel.tiempoChequeoDeadlock * 1000;

	//crea el archivo de log
	char *nombreArchivoLog = string_from_format("/home/utnso/tp-2013-2c-volvimos-en-forma-de-sockets/logs/%s", datosNivel.nombre);
	logger = log_create(nombreArchivoLog, "Nivel", false, LOG_LEVEL_INFO);
	free(nombreArchivoLog);

	log_info(logger, "%s inicializado.", datosNivel.nombre);

	ListaPersonajes = list_create();
	ListaItems = list_create();

	pthread_mutex_init(&mutexListaItems,NULL);
	pthread_mutex_init(&mutexListaPersonajes,NULL);
	pthread_mutex_init(&mutexHiloInterbloqueo, NULL);
	pthread_mutex_init(&mutexEnemigos, NULL);

	inicializar_gui();
	log_info(logger, "GUI inicializada.");
	inicializar_recursos();
	log_info(logger, "Recursos creados.");
	inicializar_enemigos();
	log_info(logger, "Enemigos creados.");
	inicializar_inotify();
	log_info(logger, "Hilo Inotify creado.");
	inicializar_interbloqueo();
	log_info(logger, "Hilo Interbloqueo creado.");

	sock_plataforma = inicializarCliente(datosNivel.ip_orquestador,datosNivel.puerto_orquestador);

	t_nuevoNivel nuevoNivel;
	char *s_nro_nivel = string_substring_from (datosNivel.nombre, 5);
	nro_nivel = atoi (s_nro_nivel);
	free(s_nro_nivel);
	nuevoNivel.nivelID = nro_nivel;
	nuevoNivel.algoritmo = datosNivel.algoritmo;
	nuevoNivel.quantum = datosNivel.algoritmo == RR ? datosNivel.quantum : 0;
	nuevoNivel.retardo = datosNivel.retardo;

	t_paquete *paquete = empaquetar(NUEVO_NIVEL, sizeof(t_nuevoNivel), &nuevoNivel);

	if (send(sock_plataforma, paquete, sizeof(t_paquete), 0) == -1)
	{
		log_error(logger, "Error al enviar mensaje NUEVO_NIVEL.");
		exit(1);
	}

	free(paquete);

	log_info(logger, "Mensaje NUEVO_NIVEL enviado.");

	t_paquete paqueteAux;

	bzero(&paqueteAux.datos, TAMANIO_DATOS);

	if (recv(sock_plataforma, &paqueteAux, sizeof(t_paquete), MSG_WAITALL) == -1)
	{
		log_error(logger, "Error al recibir mensaje HANDSHAKE.");
		exit(1);
	}

	log_info(logger, "Mensaje HANDSHAKE recibido.");

	while(1)
	{
		bzero(&paqueteAux.datos, TAMANIO_DATOS);

		int nbytes = recv(sock_plataforma, &paqueteAux, sizeof(t_paquete), MSG_WAITALL);

		if (nbytes < 0)
		{
			log_error(logger, "Error al recibir el mensaje.");
			exit(1);
		}

		if (nbytes == 0)
		{
			close(sock_plataforma);

			liberar_memoria();

			return EXIT_SUCCESS; //finaliza el proceso
		}

		if(nbytes > 0)
		{
			pthread_mutex_lock(&mutexHiloInterbloqueo);
			procesar_mensaje(sock_plataforma, nro_nivel, paqueteAux, ListaItems, ListaPersonajes, datosNivel, &mutexListaItems, &mutexListaPersonajes, &mutexEnemigos, logger);
			pthread_mutex_unlock(&mutexHiloInterbloqueo);
		}
	}

	liberar_memoria();

	return EXIT_SUCCESS;
}


void liberar_memoria()
{
	list_destroy(ListaItems);

	void _funcDestruirPersonaje(t_personaje *personaje)
	{
		list_destroy(personaje->recursosAsignados);
		list_destroy(personaje->recursosNecesitados);
		free(personaje);

		return;
	}

	list_destroy_and_destroy_elements(ListaPersonajes, (void *) _funcDestruirPersonaje);

	free(datosNivel.ip_nivel);
	free(datosNivel.ip_orquestador);
	free(datosNivel.nombre);

	void _funcDestruirRecurso(t_infoRecurso *infoRecurso)
	{
		free(infoRecurso->nombre);
		free(infoRecurso);

		return;
	}

	list_destroy_and_destroy_elements(datosNivel.recursos, (void *) _funcDestruirRecurso);

	log_destroy(logger);

	return;
}


void inicializar_gui()
{
	rows = 167;
	cols = 100;
	nivel_gui_inicializar ();
	nivel_gui_get_area_nivel(&rows, &cols);

	pthread_mutex_lock(&mutexListaItems);
	nivel_gui_dibujar (ListaItems,datosNivel.nombre);
	pthread_mutex_unlock(&mutexListaItems);
}

void inicializar_recursos()
{
	t_infoRecurso *recurso;
	int i = 0;

	pthread_mutex_lock(&mutexListaItems);
	while((recurso = list_get(datosNivel.recursos,i))!= NULL)
	{
		CrearCaja (ListaItems, recurso->recursoID, recurso->posx, recurso->posy, recurso->instancia);
		i++;
	}

	nivel_gui_dibujar (ListaItems,datosNivel.nombre);
	pthread_mutex_unlock(&mutexListaItems);

	return;
}

void inicializar_enemigos()
{
	void _obtenerPosicionInicial(t_hiloEnemigo *hiloEnemigo)
	{
		int tempX = rand() % cols;
		int tempY = rand() % rows;

		bool _funcPosicionesRecursos(ITEM_NIVEL *item)
		{
			if(item->item_type == RECURSO_ITEM_TYPE)
			{
				return (((item->posx == tempX) && (item->posy == tempY)));
			}

			return false;
		}

		while((list_any_satisfy(ListaItems, (void *) _funcPosicionesRecursos)) || ((tempX == 0) && (tempY == 0)))
		{
			tempX = rand() % cols;
			tempY = rand() % rows;
		}
		hiloEnemigo->posX = tempX;
		hiloEnemigo->posY = tempY;

		return;
	}

	for(int p = 1; p <= datosNivel.enemigos; p++ )
	{
		t_hiloEnemigo *hiloEnemigo = malloc(sizeof(t_hiloEnemigo));
		hiloEnemigo->enemigoID = (char)(((int)'0')+p);
		hiloEnemigo->personajeID_victima = '\0';
		hiloEnemigo->posX_elegida = 0;
		hiloEnemigo->posY_elegida = 0;
		_obtenerPosicionInicial(hiloEnemigo);

		pthread_mutex_lock(&mutexListaItems);
		CrearEnemigo(ListaItems, hiloEnemigo->enemigoID, hiloEnemigo->posX, hiloEnemigo->posY);
		pthread_mutex_unlock(&mutexListaItems);

		if(pthread_create(&hiloEnemigo->hiloEnemigoID, NULL, (void *) funcion_enemigo, (void *) hiloEnemigo) == -1)
		{
			log_error(logger, "Error al crear el Hilo Enemigo.");
			exit(1);
		}
	}

	return;
}

void inicializar_inotify()
{
	pthread_t hiloInotify;

	if(pthread_create(&hiloInotify,NULL,(void *)funcion_inotify,NULL))
	{
		log_error(logger, "Error al crear el Hilo Inotify.");
		exit(1);
	}
	return;
}

void inicializar_interbloqueo()
{
	pthread_t hiloInterbloqueo;

	if(pthread_create(&hiloInterbloqueo,NULL,(void *)funcion_interbloqueo,NULL))
	{
		log_error(logger, "Error al crear el Hilo Interbloqueo.");
		exit(1);
	}
	return;
}


void elegirVictima(t_hiloEnemigo *hiloEnemigo)
{
	pthread_mutex_lock(&mutexListaItems);
	pthread_mutex_lock(&mutexListaPersonajes);
	bool _funcFiltrarPersonajes(ITEM_NIVEL *item) { return item->item_type == PERSONAJE_ITEM_TYPE; }
	t_list *ListaItemTemporal = list_filter(ListaItems, (void *) _funcFiltrarPersonajes);

	if(list_size(ListaItemTemporal) > 1)
	{
		bool _funcOrdenarPersonajesPorDistancia(ITEM_NIVEL *personaje, ITEM_NIVEL *otroPersonaje)
		{
			return (abs(personaje->posx - hiloEnemigo->posX) + abs(personaje->posy - hiloEnemigo->posY)) <
				    (abs(otroPersonaje->posx - hiloEnemigo->posX) + abs(otroPersonaje->posy - hiloEnemigo->posY));
		}
		list_sort(ListaItemTemporal, (void *) _funcOrdenarPersonajesPorDistancia);

		ITEM_NIVEL *personajeVictima;
		int i = 0;

		while((personajeVictima = list_get(ListaItemTemporal, i)) != NULL)
		{
			bool _funcBuscarPersonaje(t_personaje *personaje) { return personaje->personajeID == personajeVictima->id; }
			t_personaje *personaje = list_find(ListaPersonajes, (void *) _funcBuscarPersonaje);

			if(personaje->recursoBloqueante == '\0')
			{
				hiloEnemigo->personajeID_victima = personajeVictima->id;
				list_destroy(ListaItemTemporal);
				pthread_mutex_unlock(&mutexListaItems);
				pthread_mutex_unlock(&mutexListaPersonajes);
				return;
			}
			++i;
		}
	}
	else
	{
		ITEM_NIVEL *personajeVictima = list_get(ListaItemTemporal, 0);

		bool _funcBuscarPersonaje(t_personaje *personaje) { return personaje->personajeID == personajeVictima->id; }
		t_personaje *personaje = list_find(ListaPersonajes, (void *) _funcBuscarPersonaje);

		if(personaje->recursoBloqueante == '\0')
		{
			hiloEnemigo->personajeID_victima = personajeVictima->id;
			list_destroy(ListaItemTemporal);
			pthread_mutex_unlock(&mutexListaItems);
			pthread_mutex_unlock(&mutexListaPersonajes);
			return;
		}
	}

	list_destroy(ListaItemTemporal);
	pthread_mutex_unlock(&mutexListaItems);
	pthread_mutex_unlock(&mutexListaPersonajes);

	return;
}

void moverActivoX(t_hiloEnemigo *hiloEnemigo)
{
	bool _funcValidadNoPisarRecurso(ITEM_NIVEL *item)
	{
		if(item->item_type == RECURSO_ITEM_TYPE)
		{
			return (((item->posx == hiloEnemigo->posX) && (item->posy == hiloEnemigo->posY)));
		}
		return false;
	}

	usleep(sleepEnemigos);

	pthread_mutex_lock(&mutexListaItems);
	bool _funcBuscarPersonaje(ITEM_NIVEL *itemPersonaje) { return itemPersonaje->id == hiloEnemigo->personajeID_victima; }
	ITEM_NIVEL *itemPersonaje = list_find(ListaItems, (void *) _funcBuscarPersonaje);

	if(itemPersonaje != NULL)
	{
		if(hiloEnemigo->posX < itemPersonaje->posx)
		{
			++hiloEnemigo->posX;

			if(list_any_satisfy(ListaItems, (void *) _funcValidadNoPisarRecurso) || ((hiloEnemigo->posX == 0) && (hiloEnemigo->posY == 0)))
			{
				--hiloEnemigo->posX;
			}
		}

		if(hiloEnemigo->posX > itemPersonaje->posx)
		{
			--hiloEnemigo->posX;

			if(list_any_satisfy(ListaItems, (void *) _funcValidadNoPisarRecurso) || ((hiloEnemigo->posX == 0) && (hiloEnemigo->posY == 0)))
			{
				++hiloEnemigo->posX;
			}
		}

		MoverEnemigo(ListaItems, hiloEnemigo->enemigoID, hiloEnemigo->posX, hiloEnemigo->posY);
		nivel_gui_dibujar(ListaItems, datosNivel.nombre);
	}
	pthread_mutex_unlock(&mutexListaItems);

	return;
}

void moverActivoY(t_hiloEnemigo *hiloEnemigo)
{
	bool _funcValidadNoPisarRecurso(ITEM_NIVEL *item)
	{
		if(item->item_type == RECURSO_ITEM_TYPE)
		{
			return (((item->posx == hiloEnemigo->posX) && (item->posy == hiloEnemigo->posY)));
		}
		return false;
	}

	usleep(sleepEnemigos);

	pthread_mutex_lock(&mutexListaItems);
	bool _funcBuscarPersonaje(ITEM_NIVEL *itemPersonaje) { return itemPersonaje->id == hiloEnemigo->personajeID_victima; }
	ITEM_NIVEL *itemPersonaje = list_find(ListaItems, (void *) _funcBuscarPersonaje);

	if(itemPersonaje != NULL)
	{
		if(hiloEnemigo->posY < itemPersonaje->posy)
		{
			++hiloEnemigo->posY;

			if(list_any_satisfy(ListaItems, (void *) _funcValidadNoPisarRecurso) || ((hiloEnemigo->posX == 0) && (hiloEnemigo->posY == 0)))
			{
				--hiloEnemigo->posY;
			}
		}

		if(hiloEnemigo->posY > itemPersonaje->posy)
		{
			--hiloEnemigo->posY;
			if(list_any_satisfy(ListaItems, (void *) _funcValidadNoPisarRecurso) || ((hiloEnemigo->posX == 0) && (hiloEnemigo->posY == 0)))
			{
				++hiloEnemigo->posY;
			}
		}

		MoverEnemigo(ListaItems, hiloEnemigo->enemigoID, hiloEnemigo->posX, hiloEnemigo->posY);
		nivel_gui_dibujar(ListaItems, datosNivel.nombre);
	}
	pthread_mutex_unlock(&mutexListaItems);

	return;
}

void controlarPosicionMatar(t_hiloEnemigo *hiloEnemigo)
{
	pthread_mutex_lock(&mutexListaItems);
	bool _funcBuscarPersonaje(ITEM_NIVEL *itemPersonaje) { return itemPersonaje->id == hiloEnemigo->personajeID_victima; }
	ITEM_NIVEL *itemPersonaje = list_find(ListaItems, (void *) _funcBuscarPersonaje);

	if(itemPersonaje != NULL)
	{
		//  && (itemPersonaje->posx != 0) && itemPersonaje->posy != 0)
		if((itemPersonaje->posx == hiloEnemigo->posX) && (itemPersonaje->posy == hiloEnemigo->posY))
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
	}
	pthread_mutex_unlock(&mutexListaItems);

	return;
}


void generarCoordenadasMovimientoPasivo(t_hiloEnemigo *hiloEnemigo)
{
	int coorX;
	int coorY;

	void _generarCoordenadas()
	{
		coorX = rand() % 4;

		switch(coorX)
		{
			case 0:
				coorX = -2;
				break;
			case 1:
				coorX = -1;
				break;
			case 2:
				coorX = 1;
				break;
			case 3:
				coorX = 2;
				break;
		}

		if(abs(coorX) == 2)
			coorY = ((rand() % 2) == 0) ? -1 : 1;
		else
			coorY = ((rand() % 2) == 0) ? -2 : 2;

		return;
	}

	int posTempX = -1;
	int posTempY = -1;

	bool _funcPosicionesRecursos(ITEM_NIVEL *item)
	{
		if(item->item_type == RECURSO_ITEM_TYPE)
		{
			return (((item->posx == posTempX) && (item->posy == posTempY)));
		}

		return false;
	}

	while((list_any_satisfy(ListaItems, (void *) _funcPosicionesRecursos)) || ((posTempX < 0) || (posTempY < 0) || (posTempX > cols) || (posTempY > rows)))
	{
		_generarCoordenadas();
		posTempX = hiloEnemigo->posX + coorX;
		posTempY = hiloEnemigo->posY + coorY;
	}

	hiloEnemigo->posX_elegida = posTempX;
	hiloEnemigo->posY_elegida = posTempY;

	return;
}

void moverPasivoX(t_hiloEnemigo *hiloEnemigo)
{
	bool _funcValidadNoPisarRecurso(ITEM_NIVEL *item)
	{
		if(item->item_type == RECURSO_ITEM_TYPE)
		{
			return (((item->posx == hiloEnemigo->posX) && (item->posy == hiloEnemigo->posY)));
		}
		return false;
	}

	while(hiloEnemigo->posX != hiloEnemigo->posX_elegida)
	{
		usleep(sleepEnemigos);

		if(hiloEnemigo->posX < hiloEnemigo->posX_elegida)
		{
			++hiloEnemigo->posX;

			if(list_any_satisfy(ListaItems, (void *) _funcValidadNoPisarRecurso) || ((hiloEnemigo->posX == 0) && (hiloEnemigo->posY == 0)))
			{
				--hiloEnemigo->posX;
				generarCoordenadasMovimientoPasivo(hiloEnemigo);
				break;
			}
		}
		if(hiloEnemigo->posX > hiloEnemigo->posX_elegida)
		{
			--hiloEnemigo->posX;

			if(list_any_satisfy(ListaItems, (void *) _funcValidadNoPisarRecurso) || ((hiloEnemigo->posX == 0) && (hiloEnemigo->posY == 0)))
			{
				++hiloEnemigo->posX;
				generarCoordenadasMovimientoPasivo(hiloEnemigo);
				break;
			}
		}

		pthread_mutex_lock(&mutexListaItems);
		MoverEnemigo(ListaItems, hiloEnemigo->enemigoID, hiloEnemigo->posX, hiloEnemigo->posY);
		nivel_gui_dibujar(ListaItems, datosNivel.nombre);
		pthread_mutex_unlock(&mutexListaItems);
	}

	return;
}

void moverPasivoY(t_hiloEnemigo *hiloEnemigo)
{
	bool _funcValidadNoPisarRecurso(ITEM_NIVEL *item)
	{
		if(item->item_type == RECURSO_ITEM_TYPE)
		{
			return (((item->posx == hiloEnemigo->posX) && (item->posy == hiloEnemigo->posY)));
		}
		return false;
	}

	while(hiloEnemigo->posY != hiloEnemigo->posY_elegida)
	{
		usleep(sleepEnemigos);

		if(hiloEnemigo->posY < hiloEnemigo->posY_elegida)
		{
			++hiloEnemigo->posY;

			if(list_any_satisfy(ListaItems, (void *) _funcValidadNoPisarRecurso) || ((hiloEnemigo->posX == 0) && (hiloEnemigo->posY == 0)))
			{
				--hiloEnemigo->posY;
				generarCoordenadasMovimientoPasivo(hiloEnemigo);
				break;
			}
		}

		if(hiloEnemigo->posY > hiloEnemigo->posY_elegida)
		{
			--hiloEnemigo->posY;
			if(list_any_satisfy(ListaItems, (void *) _funcValidadNoPisarRecurso) || ((hiloEnemigo->posX == 0) && (hiloEnemigo->posY == 0)))
			{
				++hiloEnemigo->posY;
				generarCoordenadasMovimientoPasivo(hiloEnemigo);
				break;
			}
		}

		pthread_mutex_lock(&mutexListaItems);
		MoverEnemigo(ListaItems, hiloEnemigo->enemigoID, hiloEnemigo->posX, hiloEnemigo->posY);
		nivel_gui_dibujar(ListaItems, datosNivel.nombre);
		pthread_mutex_unlock(&mutexListaItems);
	}
	return;
}


void *funcion_enemigo(t_hiloEnemigo *hiloEnemigo)
{
	pthread_mutex_lock(&mutexListaItems);
	nivel_gui_dibujar(ListaItems, datosNivel.nombre);
	pthread_mutex_unlock(&mutexListaItems);

	while(1)
	{
		pthread_mutex_lock(&mutexListaPersonajes);
		bool _funcNoEstanBloqueados(t_personaje *personaje){ return personaje->recursoBloqueante == '\0'; }
		bool personajesActivos = (list_size(ListaPersonajes) && list_any_satisfy(ListaPersonajes,(void *)_funcNoEstanBloqueados));
		pthread_mutex_unlock(&mutexListaPersonajes);

		if(personajesActivos)
		{
			elegirVictima(hiloEnemigo);
			moverActivoX(hiloEnemigo);
			controlarPosicionMatar(hiloEnemigo);
			moverActivoY(hiloEnemigo);
			controlarPosicionMatar(hiloEnemigo);
		}
		else
		{
			generarCoordenadasMovimientoPasivo(hiloEnemigo);
			moverPasivoX(hiloEnemigo);
			moverPasivoY(hiloEnemigo);
		}

		pthread_mutex_lock(&mutexListaItems);
		nivel_gui_dibujar(ListaItems, datosNivel.nombre);
		pthread_mutex_unlock(&mutexListaItems);
	}
	return (void *) EXIT_SUCCESS;
}


void funcLiberarRecurso(t_infoRecurso *recurso)
{
	free(recurso->nombre);
	free(recurso);
}

void eliminar_configuracion_nivel()
{
	free(datosNivel.nombre);
	free(datosNivel.ip_nivel);
	free(datosNivel.ip_orquestador);
	list_destroy_and_destroy_elements(datosNivel.recursos, (void *) funcLiberarRecurso);
}

void *funcion_inotify()
{
	char buffer[TAMANIO_BUFFER];
	char anteriorAlgoritmo = datosNivel.algoritmo;
	uint8_t anteriorQuatum = datosNivel.quantum;
	uint8_t anteriorRetardo = datosNivel.retardo;
	int inotify_fd;
	bool notifico = false;

	if((inotify_fd = inotify_init()) < 0)
	{
		log_error(logger, "Error en la función inotify_init.");
		pthread_exit(NULL);
	}
	int watch_descriptor = inotify_add_watch(inotify_fd, dirArchivoConfig, IN_MODIFY);

	while(1)
	{
		int offset = 0;
		int bytes = read(inotify_fd, buffer, TAMANIO_BUFFER);
		if (bytes <= 0)
		{
			notifico = false;
			continue;
		}
		sleep(2);

		while(offset < bytes)
		{
			struct inotify_event *evento = (struct inotify_event *) &buffer[offset];

			if ((evento->mask & IN_MODIFY) && !notifico)
			{
				eliminar_configuracion_nivel();

				leerConfigNivel(&datosNivel, dirArchivoConfig);

				if(datosNivel.quantum != anteriorQuatum || datosNivel.retardo != anteriorRetardo || datosNivel.algoritmo != anteriorAlgoritmo)
				{
					if(datosNivel.algoritmo != RR && datosNivel.algoritmo != SRDF && datosNivel.algoritmo != 'r' && datosNivel.algoritmo != 's')
					{
						log_error(logger, "El algoritmo ingresado no existe.");
						exit(1);
					}

					anteriorAlgoritmo = datosNivel.algoritmo;
					anteriorQuatum = datosNivel.quantum;
					anteriorRetardo = datosNivel.retardo;

					if(sock_plataforma)
					{
						t_cambioAlgoritmo cambioAlgoritmo;
						cambioAlgoritmo.nivelID = nro_nivel;
						cambioAlgoritmo.algoritmo = datosNivel.algoritmo;
						cambioAlgoritmo.quantum = datosNivel.quantum;
						cambioAlgoritmo.retardo = datosNivel.retardo;

						t_paquete *paquete = empaquetar(CAMBIO_ALGORITMO, sizeof(t_cambioAlgoritmo), &cambioAlgoritmo);

						if(send(sock_plataforma, paquete, sizeof(t_paquete), 0) == -1)
						{
							log_error(logger, "Error al enviar mensaje CAMBIO_ALGORITMO.");
							exit(1);
						}

						free(paquete);
						log_info(logger, "Mensaje CAMBIO_ALGORITMO enviado.");
					}
				}
			}
			else
			{
				notifico = false;
				break;
			}
			offset += TAMANIO_EVENTO + evento->len;
			notifico = true;
		}
	}
    inotify_rm_watch(inotify_fd, watch_descriptor);
    close(inotify_fd);

	return (void *) EXIT_SUCCESS;
}


void loggearLista(t_list *lista)
{
	char *mensaje = calloc(1, sizeof(char));

	void _funcMostrarDatos(t_personaje *p)
	{
		string_append_with_format(&mensaje, "%c ", p->personajeID);
	}

	string_append(&mensaje, "Personajes Interbloqueados: ");

	list_iterate(lista, (void *) _funcMostrarDatos);

	log_info(logger, "%s", mensaje);

	free(mensaje);
}


void *funcion_interbloqueo()
{
	while(1)
	{
		usleep(tiempoChequeoDeadlock);

		pthread_mutex_lock(&mutexHiloInterbloqueo);
		if(list_size(ListaPersonajes) > 1)
		{
			pthread_mutex_lock(&mutexListaPersonajes);
			pthread_mutex_lock(&mutexListaItems);
			t_list *ListaInterbloqueados = list_create();
			crearListaPersonajeTemporal(ListaPersonajes, ListaInterbloqueados);

			t_list *ListaRecursosDisponiblesTemp = list_create();
			crearListaRecursosTemporal(ListaItems, ListaRecursosDisponiblesTemp);
			pthread_mutex_unlock(&mutexListaPersonajes);
			pthread_mutex_unlock(&mutexListaItems);

			//1) Arma la lista de personajes marcados y no marcados.

			t_personaje *personaje;
			int i = 0;

			while((personaje = list_get(ListaInterbloqueados, i)) != NULL)
			{
				if (list_size(personaje->recursosAsignados) == 0)
				{
					list_remove(ListaInterbloqueados, i);
					list_destroy(personaje->recursosAsignados);
					list_destroy(personaje->recursosNecesitados);
					free(personaje);
				}
				++i;
			}

			//2) Filtra personajes no marcados y los pasa a la lista de marcados.

			bool _funcPersonajesQuePuedenFinalizar(t_personaje *p)
			{
				t_recurso *recursoNecesitado;
				int i = 0;
				int j = 0;

				while ((recursoNecesitado = list_get(p->recursosNecesitados, i)) != NULL)
				{
					bool _funcBuscarRecurso(t_recurso *recurso) { return recurso->recursoID == recursoNecesitado->recursoID; }
					t_recurso *recursoDisponible = list_find(ListaRecursosDisponiblesTemp, (void *) _funcBuscarRecurso);

					if ((recursoDisponible != NULL) && (recursoDisponible->cantidad >= recursoNecesitado->cantidad))
					{
						++j; //Se cuenta si se puede satisfacer el recurso necesitado
					}

					++i;
				}
				return (j == list_size(p->recursosNecesitados));
			}

			while((personaje = list_remove_by_condition(ListaInterbloqueados, (void *) _funcPersonajesQuePuedenFinalizar)) != NULL)
			{
				t_recurso *recursoAsignado;
				int i = 0;

				while ((recursoAsignado = list_get (personaje->recursosAsignados, i)) != NULL)
				{
					bool _funcBuscarRecurso(t_recurso *recurso) { return recurso->recursoID == recursoAsignado->recursoID; }
					t_recurso *recursoDisponible = list_find(ListaRecursosDisponiblesTemp, (void *) _funcBuscarRecurso);

					recursoDisponible->cantidad += recursoAsignado->cantidad;

					++i;
				}

				list_destroy(personaje->recursosAsignados);
				list_destroy(personaje->recursosNecesitados);
				free(personaje);
			}

			//3) Informa los personajes en interbloqueo.

			bool _funcPersonajeBloqueado(t_personaje *p) { return ((p->recursoBloqueante) != ('\0')); }

			if((list_size(ListaInterbloqueados)) && list_all_satisfy(ListaInterbloqueados, (void *) _funcPersonajeBloqueado))
			{
				loggearLista(ListaInterbloqueados);

				if(datosNivel.recovery)
				{
					t_victimaInterbloqueo victimaInterbloqueo;
					victimaInterbloqueo.nivelID = nro_nivel;
					t_personaje *personaje = elegirPersonajeInterbloqueado(ListaInterbloqueados);
					victimaInterbloqueo.personajeID = personaje->personajeID;

					log_info(logger, "Víctima seleccionada para resolver interbloqueo: %c", personaje->personajeID);

					t_paquete *paquete = empaquetar(VICTIMA_INTERBLOQUEO, sizeof(t_victimaInterbloqueo), &victimaInterbloqueo);

					if(send(sock_plataforma, paquete, sizeof(t_paquete), 0) == -1)
					{
						log_error(logger, "Error al enviar mensaje VICTIMA_INTERBLOQUEO.");
						exit(1);
					}

					free(paquete);

					log_info(logger, "Mensaje VICTIMA_INTERBLOQUEO enviado.");
				}
			}

			void _funcDestruirPersonaje(t_personaje *personaje)
			{
				list_destroy(personaje->recursosAsignados);
				list_destroy(personaje->recursosNecesitados);
				free(personaje);

				return;
			}

			list_destroy_and_destroy_elements(ListaInterbloqueados, (void *) _funcDestruirPersonaje);
			list_destroy(ListaRecursosDisponiblesTemp);
		}
		pthread_mutex_unlock(&mutexHiloInterbloqueo);
	}

	return (void *) EXIT_SUCCESS;
}
