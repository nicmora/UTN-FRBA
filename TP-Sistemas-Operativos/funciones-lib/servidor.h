#ifndef SERVIDOR_H_
#define SERVIDOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include "mensajes.h"


int inicializarServidor(int);
int aceptarCliente(int);
void agregarFD(int, fd_set *, uint8_t *);
void quitarFD(int, fd_set *, uint8_t *);


#endif /* SERVIDOR_H_ */
