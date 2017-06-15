#include "bpm-file.h"

unsigned char *LoadBMP(char *filename, bmpInfoHeader *bInfoHeader) {

	FILE *f;
	bmpFileHeader header; /* cabecera */
	unsigned char *imgdata; /* datos de imagen */
	uint16_t type; /* 2 bytes identificativos */

	f = fopen(filename, "r");
	if (!f)
		return NULL; /* Si no podemos leer, no hay imagen*/

	/* Leemos los dos primeros bytes */
	fread(&type, sizeof(uint16_t), 1, f);
	if (type != 0x4D42) { /* Comprobamos el formato */
		fclose(f);
		return NULL;
	}

	/* Leemos la cabecera de fichero completa */
	fread(&header, sizeof(bmpFileHeader), 1, f);

	/* Leemos la cabecera de información completa */
	fread(bInfoHeader, sizeof(bmpInfoHeader), 1, f);

	/* Reservamos memoria para la imagen, ¿cuánta?
	 Tanto como indique imgsize */
	imgdata = (unsigned char*) malloc(bInfoHeader->imgsize);

	/* Nos situamos en el sitio donde empiezan los datos de imagen,
	 nos lo indica el offset de la cabecera de fichero*/
	fseek(f, header.offset, SEEK_SET);

	/* Leemos los datos de imagen, tantos bytes como imgsize */
	fread(imgdata, bInfoHeader->imgsize, 1, f);

	/* Cerramos */
	fclose(f);

	/* Devolvemos la imagen */
	return imgdata;
}

unsigned char *SaveBMP(char *filename, char *data, uint32_t msglen) {

	FILE *f;
	bmpFileHeader header; /* cabecera */
	bmpInfoHeader bInfoHeader;
	uint16_t type; /* 2 bytes identificativos */

	f = fopen(filename, "r+");
	if (!f)
		return NULL; /* Si no podemos leer, no hay imagen*/

	/* Leemos los dos primeros bytes */
	fread(&type, sizeof(uint16_t), 1, f);
	if (type != 0x4D42) { /* Comprobamos el formato */
		fclose(f);
		return NULL;
	}

	/* Leemos la cabecera de fichero completa */
	fread(&header, sizeof(bmpFileHeader), 1, f);

	/* Leemos la cabecera de información completa */
	fread(&bInfoHeader, sizeof(bmpInfoHeader), 1, f);

	/* Nos situamos en el sitio donde empiezan los datos de imagen,
	 nos lo indica el offset de la cabecera de fichero*/
	fseek(f, header.offset, SEEK_SET);

	/* Leemos los datos de imagen, tantos bytes como imgsize */
	fwrite(data, sizeof(char), msglen, f);

	/* Cerramos */
	fclose(f);

	/* Devolvemos la imagen */
	return 0;
}

void DisplayInfo(bmpInfoHeader *info) {
	printf("Tamaño de la cabecera: %u\n", info->headersize);
	printf("Anchura: %d\n", info->width);
	printf("Altura: %d\n", info->height);
	printf("Planos (1): %d\n", info->planes);
	printf("Bits por pixel: %d\n", info->bpp);
	printf("Compresión: %d\n", info->compress);
	printf("Tamaño de datos de imagen: %u\n", info->imgsize);
	printf("Resolucón horizontal: %u\n", info->bpmx);
	printf("Resolucón vertical: %u\n", info->bpmy);
	printf("Colores en paleta: %d\n", info->colors);
	printf("Colores importantes: %d\n", info->imxtcolors);
}
