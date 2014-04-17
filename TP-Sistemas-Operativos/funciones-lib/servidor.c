#include "servidor.h"

#define BACKLOG 10


int inicializarServidor(int puerto)
{
	int srv_sock;
	struct sockaddr_in srv_addr;
	int yes = 1;

	if ((srv_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket");
		exit(1);
	}

	if (setsockopt(srv_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
		perror("setsockopt");
		exit(1);
	}

	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	/*
	 * Funciones para cargar la IP en ordenacion de red
	 * inet_addr("IP");
	 * inet_aton("IP", &(my_addr.sin_addr)); //Devuelve 0 en caso de error
	 * Funcion inversa de inet_addr()
	 * inet_ntoa() //Si se usa 2 veces se pierde la IP anterior
	 */
	srv_addr.sin_port = htons(puerto);
	memset(&(srv_addr.sin_zero), '\0', 8);

	if (bind(srv_sock, (struct sockaddr *) &srv_addr, sizeof(struct sockaddr_in)) == -1)
	{
		perror("bind");
		exit(1);
	}


	if (listen (srv_sock, BACKLOG) == -1)
	{
		perror("listen");
		exit(1);
	}

	return srv_sock;
}


int aceptarCliente(int srv_sock)
{
	int cli_sock;
	struct sockaddr_in cli_addr;
	int sin_zero = sizeof(struct sockaddr_in);

	if ((cli_sock = accept(srv_sock, (struct sockaddr *) &cli_addr, &sin_zero)) == -1)
	{
		perror("accept");
		exit(1);
	}

	return cli_sock;
}


void agregarFD(int sockfd, fd_set *fdset, uint8_t *fdmax)
{
	FD_SET(sockfd, fdset);

	if (sockfd > (*fdmax))
	{
		(*fdmax) = sockfd;
	}

	return;
}


void quitarFD(int sockfd, fd_set *fdset, uint8_t *fdmax)
{
	FD_CLR(sockfd, fdset);

	if((*fdmax) == sockfd)
	{
		--(*fdmax);
	}

	return;
}
