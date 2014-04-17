#include "leerConfigPlataforma.h"


void leerConfigPlataforma (t_configPlataforma *datos_plataforma, char *direccionConfiguracion)
{
	t_config *config_plataforma = config_create (direccionConfiguracion);

	//Guarda el puerto de plataforma.

	char *puerto = config_get_string_value (config_plataforma, "puerto");
	datos_plataforma->puerto = (uint16_t) atoi (puerto);

	//Guarda la direccion del binario koopa.

	char *koopa = config_get_string_value (config_plataforma, "koopa");
	datos_plataforma->koopa = string_duplicate (koopa);

	//Guarda la direccion del script.

	char *script = config_get_string_value (config_plataforma, "script");
	datos_plataforma->script = string_duplicate (script);

	//Guarda la direccion del disco.

	char *disco = config_get_string_value (config_plataforma, "disco");
	datos_plataforma->disco = string_duplicate (disco);

	//Guarda la direccion del punto de montaje.

	char *montaje = config_get_string_value (config_plataforma, "montaje");
	datos_plataforma->montaje = string_duplicate (montaje);

	config_destroy (config_plataforma);
}
