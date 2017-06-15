#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <unistd.h>
#include "ecrypt-sync.h"
#include "bpm-file.h"

void iniciar_programa();
void abrir_imagen(bmpInfoHeader *, char **);
void elegir_opcion(int *);
void encriptar_imagen(char *, bmpInfoHeader);
void desencriptar_imagen(char *, bmpInfoHeader);
void simulacion(char *, bmpInfoHeader, int *);

char *dir = "./imagen.bmp";

int main() {

	int opcion = 0;
	int espera = 0;
	bmpInfoHeader header;
	char *body = NULL;

	iniciar_programa();
	elegir_opcion(&opcion);
	abrir_imagen(&header, &body);

	if(opcion == 1) {
		encriptar_imagen(body, header);
	} else if(opcion == 2) {
		desencriptar_imagen(body, header);
	} else if(opcion == 3) {
		simulacion(body, header, &espera);
	}

	exit;
}

void iniciar_programa() {

	printf("\n   ** CriptoApp - Grain128a **\n\n");

	sleep(1);
	printf(" Esta aplicacion trabaja con el archivo imagen.bmp ubicada en el\n mismo directorio de ejecucion.\n\n");
	sleep(3);
	printf(" Seleccione una opcion para proceder:\n");
	printf("  1 - Encriptar imagen.bmp\n");
	printf("  2 - Desencriptar imagen.bmp\n");
	printf("  3 - Realizar simulacion\n");
	printf(" Seleccion: ");
	return;
}


void abrir_imagen(bmpInfoHeader *header, char **body) {
	*body = LoadBMP(dir,header);
	return;
}

void elegir_opcion(int *opcion) {

	scanf("%u",opcion);
	printf("\n");

	while(*opcion != 1 && *opcion != 2 && *opcion != 3) {
		printf(" Elegir una opcion entre [1;2;3]: ");
		scanf("%u",opcion);
		printf("\n");
	}

	return;
}

void encriptar_imagen(char *plaintext, bmpInfoHeader header) {
	ECRYPT_ctx ctx;
	u8 key[16] = {0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0},
		IV[12] = {0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0x12,0x34,0x56,0x78};
	u32 msglen = header.imgsize;
	char ciphertext[msglen];
	ECRYPT_init();
	ECRYPT_keysetup(&ctx,key,128,96);
	ECRYPT_ivsetup(&ctx,IV);
	ECRYPT_encrypt_bytes(&ctx,plaintext,ciphertext,msglen);
	SaveBMP(dir, ciphertext, msglen);
}

void desencriptar_imagen(char *ciphertext, bmpInfoHeader header) {
	ECRYPT_ctx ctx;
	u8 key[16] = {0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0},
		IV[12] = {0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0x12,0x34,0x56,0x78};
	u32 msglen = header.imgsize;
	char plaintext[msglen];
	ECRYPT_init();
	ECRYPT_keysetup(&ctx,key,128,96);
	ECRYPT_ivsetup(&ctx,IV);
	ECRYPT_decrypt_bytes(&ctx,ciphertext,plaintext,msglen);
	SaveBMP(dir, plaintext, msglen);
	return;
}

void simulacion(char *body, bmpInfoHeader header, int *espera) {

	printf("\n * Simulacion de envio y recepcion de imagen * \n\n");
	sleep(2);
	printf(" Se procede a encriptar la imagen...\n");
	sleep(2);
	encriptar_imagen(body, header);
	printf(" La imagen se ha encriptado con exito.\n");
	sleep(1);
	printf("\n");
	printf(" Enviando la imagen a destino");sleep(2);printf(".");sleep(2);printf(".");sleep(2);printf(".\n");sleep(2);
	printf("\n");
	printf(" (Ingrese un valor para continuar...) ");
	scanf("%u",espera);
	printf("\n");
	printf(" La imagen a llegado a destino...\n\n"); sleep(2);
	printf(" Se procede a desencriptar la imagen...\n");
	abrir_imagen(&header, &body);
	desencriptar_imagen(body, header);
	printf(" La imagen se ha desencriptado con exito.\n"); sleep(2);
	printf(" \n Verifique la autenticidad de la imagen.");
	scanf("%u",espera);
	return;
}
