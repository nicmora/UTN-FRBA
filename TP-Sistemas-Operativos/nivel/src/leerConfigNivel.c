#include "leerConfigNivel.h"


t_infoRecurso *crearNodoRecurso (t_config *, char *);

void leerConfigNivel (t_configNivel *datos_nivel, char *direccionConfirguracion)
{
	t_config *config_nivel = config_create (direccionConfirguracion);

	//Guarda el nombre.
	char *nombre = config_get_string_value (config_nivel,"nombre");
	datos_nivel->nombre = string_duplicate (nombre);

	//Guarda la lista de recursos.
	datos_nivel->recursos = list_create ();

	int n = 1;
	//Formo el string "cajaN", N es un numero.

	char *key = string_from_format ("caja%d",n);

	t_infoRecurso *datos_recurso;

	while (config_get_string_value (config_nivel,key) != NULL)
	{
		datos_recurso = crearNodoRecurso (config_nivel, key);

		list_add (datos_nivel->recursos, datos_recurso);

		++n;

		free (key);

		key = string_from_format ("caja%d",n);
	}

	free (key);

	//Guarda la direccion:puerto del orquestador.

	char *ip_puerto_aux = config_get_string_value (config_nivel,"IPPuertoOrquestador");

	char **v_ip_puerto_aux = string_split (ip_puerto_aux,":");

	datos_nivel->ip_orquestador = string_duplicate (v_ip_puerto_aux[0]);

	datos_nivel->puerto_orquestador = (uint16_t) atoi (v_ip_puerto_aux[1]);

	free (v_ip_puerto_aux[0]);
	free (v_ip_puerto_aux[1]);
	free (v_ip_puerto_aux);

	//Guarda la ip:puerto del nivel.

	ip_puerto_aux = config_get_string_value (config_nivel,"IPPuertoNivel");

	v_ip_puerto_aux = string_split (ip_puerto_aux,":");

	datos_nivel->ip_nivel = string_duplicate (v_ip_puerto_aux[0]);

	datos_nivel->puerto_nivel = (uint16_t) atoi (v_ip_puerto_aux[1]);

	free (v_ip_puerto_aux[0]);
	free (v_ip_puerto_aux[1]);
	free (v_ip_puerto_aux);

	//Guarda la cantidad de enemigos.
	char *enemigos = config_get_string_value (config_nivel, "enemigos");
	datos_nivel->enemigos = (uint8_t) atoi (enemigos);

	//Guarda el sleep de los enemigos.
	char *sleepEnemigos = config_get_string_value (config_nivel, "sleepEnemigos");
	datos_nivel->sleepEnemigos = (uint32_t) atoi (sleepEnemigos);


	//Guarda el tiempo de chequeo de deadlock.
	char *tiempoChequeoDeadlock = config_get_string_value (config_nivel,"tiempoChequeoDeadlock");
	datos_nivel->tiempoChequeoDeadlock = (uint32_t) atoi (tiempoChequeoDeadlock);

	//Guardo el indicador de recovery.
	char *recovery = config_get_string_value (config_nivel,"recovery");
	datos_nivel->recovery = (uint8_t) atoi (recovery);

	//Guarda el algoritmo de planificacion.
	char *algoritmo = config_get_string_value (config_nivel, "algoritmo");
	datos_nivel->algoritmo = *algoritmo;

	//Guarda el quantum para RR.
	char *quantum = config_get_string_value (config_nivel, "quantum");
	datos_nivel->quantum = (uint8_t) atoi (quantum);

	//Guarda el retardo de turno.
	char *retardo = config_get_string_value (config_nivel, "retardo");
	datos_nivel->retardo = (uint32_t) atoi (retardo);

	config_destroy(config_nivel);

	return;
}


t_infoRecurso *crearNodoRecurso (t_config *config_nivel, char *key)
{
	//Accedo al diccionario con key, y extraigo el string "Nombre,Simbolo,Instancia,PosX,PosY".

	char *s_datos_recurso_aux = config_get_string_value (config_nivel, key);

	//Separo los elementos del string, para recorrerlo por elementos.

	char **v_datos_recurso_aux = string_split (s_datos_recurso_aux, ",");

	//Armo la estructura del nodo.

	t_infoRecurso *datos_recurso;
	datos_recurso = (t_infoRecurso *) malloc (sizeof (t_infoRecurso));

	char *nombre = v_datos_recurso_aux[0];

	datos_recurso->nombre = string_duplicate (nombre);

	datos_recurso->recursoID = *v_datos_recurso_aux[1];

	uint8_t instancia = (uint8_t) atoi (v_datos_recurso_aux[2]);
	datos_recurso->instancia = instancia;

	uint8_t posx = (uint8_t) atoi (v_datos_recurso_aux[3]);
	datos_recurso->posx = posx;

	uint8_t posy = (uint8_t) atoi (v_datos_recurso_aux[4]);
	datos_recurso->posy = posy;

	free (v_datos_recurso_aux[0]);
	free (v_datos_recurso_aux[1]);
	free (v_datos_recurso_aux[2]);
	free (v_datos_recurso_aux[3]);
	free (v_datos_recurso_aux[4]);
	free (v_datos_recurso_aux);

	return datos_recurso;
}
