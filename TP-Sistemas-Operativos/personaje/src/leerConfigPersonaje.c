#include "leerConfigPersonaje.h"


t_infoNivel *crearNodoNivel (t_config *, char **, int);

void leerConfigPersonaje (t_configPersonaje *datos_personaje, char *direccionConfirguracion)
{
	t_config *config_personaje = config_create (direccionConfirguracion);

	//Guarda el nombre.

	char *nombre = config_get_string_value (config_personaje,"nombre");

	datos_personaje->nombre = string_duplicate (nombre);

	//Guarda el simbolo.

	datos_personaje->personajeID = *config_get_string_value (config_personaje,"simbolo");

	//Guarda la lista de niveles.

	datos_personaje->niveles = list_create ();

	int n = 0;

	//Transforma el string "[Nivel1,Nivel2,Nivel3]" en el Array ["Nivel1","Nivel2","Nivel3"].

	char **v_niveles_aux = config_get_array_value (config_personaje,"planDeNiveles");

	t_infoNivel *datos_nivel;

	while (v_niveles_aux[n] != NULL)
	{
		datos_nivel = crearNodoNivel (config_personaje, v_niveles_aux, n);
		list_add (datos_personaje->niveles, datos_nivel);
		++n;
	}

	int i = 0;

	while (i <= n)
	{
		free (v_niveles_aux[i]);
		++i;
	}

	free (v_niveles_aux);

	//Guarda las vidas.

	char *vidas = config_get_string_value (config_personaje,"vidas");
	datos_personaje->vidas = (uint8_t) atoi (vidas);

	//Guarda la direccion:puerto del orquestador.

	char *ip_puerto_aux = config_get_string_value (config_personaje,"IPPuertoOrquestador");

	char **v_ip_puerto_aux = string_split (ip_puerto_aux,":");

	datos_personaje->ip_orquestador = string_duplicate (v_ip_puerto_aux[0]);

	datos_personaje->puerto_orquestador = (uint16_t) atoi (v_ip_puerto_aux[1]);

	free (v_ip_puerto_aux[0]);
	free (v_ip_puerto_aux[1]);
	free (v_ip_puerto_aux);

	//Guarda la ip:puerto local del personaje.

	ip_puerto_aux = config_get_string_value (config_personaje, "IPPuertoPersonaje");

	v_ip_puerto_aux = string_split (ip_puerto_aux, ":");

	datos_personaje->ip_personaje = string_duplicate (v_ip_puerto_aux[0]);

	datos_personaje->puerto_personaje = (uint16_t) atoi (v_ip_puerto_aux[1]);

	free (v_ip_puerto_aux[0]);
	free (v_ip_puerto_aux[1]);
	free (v_ip_puerto_aux);

	config_destroy (config_personaje);

	return;
}


t_infoNivel *crearNodoNivel (t_config *config_personaje, char **v_niveles_aux, int n)
{
	//Forma el string "obj[Niveln]".

	char *key = string_from_format ("obj[%s]",v_niveles_aux[n]);
	char *key2 = string_from_format ("distancia[%s]",v_niveles_aux[n]);

	//Del string "Nivel1", trunco y formo el string "1".

	char *nro_nivel_aux = string_substring_from (v_niveles_aux[n], 5);

	//Arma la estructura del nodo.

	t_infoNivel *datos_nivel;
	datos_nivel = (t_infoNivel *) malloc (sizeof (t_infoNivel));

	datos_nivel->nivelID = (uint8_t) atoi (nro_nivel_aux);
	datos_nivel->distancia = (uint16_t) atoi (config_get_string_value (config_personaje,key2));
	datos_nivel->objetivos = list_create();

	char **recursos_aux = config_get_array_value (config_personaje,key);

	int i = 0;

	while (recursos_aux[i] != NULL)
	{
		t_infoObjetivo *objetivo = (t_infoObjetivo *) malloc (sizeof (t_infoObjetivo));
		objetivo->recursoID = *recursos_aux[i];
		list_add (datos_nivel->objetivos, objetivo);
		free(recursos_aux[i]);
		++i;
	}

	free (recursos_aux);

	free (key);
	free (key2);
	free (nro_nivel_aux);

	return datos_nivel;
}
