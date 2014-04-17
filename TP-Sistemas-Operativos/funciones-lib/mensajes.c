#include "mensajes.h"


t_paquete *empaquetar(char selector, int tamanio, void *estructura)
{
	t_paquete *paquete = (t_paquete *) malloc(sizeof(t_paquete));

	paquete->selector = selector;
	paquete->tamanio = tamanio;
	bzero(paquete->datos, TAMANIO_DATOS);
	memcpy(paquete->datos, estructura, tamanio);

	return paquete;
}
