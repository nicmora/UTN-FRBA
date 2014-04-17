#include "cliente.h"


int inicializarCliente(char *ip, uint16_t puerto)
{
	int sockfd;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		 perror("socket");
		 exit(1);
	}

	struct sockaddr_in srv_addr;
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = htonl(INADDR_ANY); //TODO Maquina remota inet_addr(ip)
	srv_addr.sin_port = htons(puerto);
	memset(&(srv_addr.sin_zero), '\0', 8);

	if ((connect(sockfd, (struct sockaddr *) &srv_addr, sizeof(struct sockaddr))) == -1)
	{
		perror("connect");
		exit(1);
	}

	return sockfd;
}
