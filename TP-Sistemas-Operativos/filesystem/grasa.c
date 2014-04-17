#define FUSE_USE_VERSION 27
#define _FILE_OFFSET_BITS 64


#include "grasa.h"


//#define FILEPATH "/home/utnso/Escritorio/disco_grande.bin"
#define CUSTOM_FUSE_OPT_KEY(t, p, v) { t, offsetof(struct t_runtime_options, p), v }
#define FILE_MAX_SIZE 4187593113,6
#define DIRECTORIO_RAIZ "/"


void* data_disco;
GHeader* header;
t_bitarray *bitmap;
GFile* nodos;
long int tamanio_disco;
long int tamanio_bitmap;
int bitmap_cant_bloques;
int bits_disponibles;
pthread_mutex_t mutex;

void obtener_tamanio_disco(const char* path)
{
	FILE* file = fopen(path, "r");
	fseek(file, 0L, SEEK_END);
	tamanio_disco = ftell(file);
	fclose(file);

	return;
}


void mapeo_archivo(const char* path)
{
	FILE* file = fopen(path, "r+"); //O_RDWR
	int fd;

	if ((fd = fileno(file)) == -1)
	{
		exit(1);
	}

	if ((data_disco = mmap(0, tamanio_disco, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED)
	{
		close(fd);
		exit(1);
	}

	return;
}


void inicio_header()
{
	header = data_disco;

	return;
}


void inicio_tabla_nodos()
{
	long int offset = BLOCK_SIZE + (bitmap_cant_bloques * BLOCK_SIZE);
	nodos = data_disco + offset;

	return;
}


void obtener_bitmap_cant_bloques()
{
	int cant_bloques = (tamanio_disco / BLOCK_SIZE) + (tamanio_disco % BLOCK_SIZE != 0);
	tamanio_bitmap = cant_bloques / 8; //en Bytes
	bitmap_cant_bloques = (tamanio_bitmap / BLOCK_SIZE) + (tamanio_bitmap % BLOCK_SIZE != 0);

	return;
}


void inicio_bitmap()
{
	bitmap = bitarray_create(data_disco + BLOCK_SIZE, tamanio_bitmap);

	return;
}


char**  armar_vector_path(char* text)
{
    int length_value = strlen(text) - 1;
    char* value_without_brackets = string_substring(text, 1, length_value);
    char **array_values = string_split(value_without_brackets, "/");

    free(value_without_brackets);

    return array_values;
}


int obtener_bloque_archivo(const char* path)
{
   char** vector_path = armar_vector_path(path);

   int parent_dir = 0;
   int pos_archivo = -1;

   int j;
   int i=0;

   while (vector_path[i] != NULL)
   {
	   for (j=0;j<1024;j++)
	   {
		   if ((strcmp(nodos[j].fname, vector_path[i]) == 0) && (nodos[j].parent_dir_block == parent_dir))
		   {
			   parent_dir = j + 1;
			   pos_archivo = j;
			   break;
		   }
	   }
	   i++;
   }

   if (j == 1024)
   {
	   return -1;
   }
   return pos_archivo;
}


int obtener_bloque_padre (const char* path)
{
	char** vector_path = armar_vector_path(path);
	char *file_name = strrchr (path, '/') + 1;

	int parent_dir = 0;

	if ( strcmp (path, strrchr(path, '/')) !=0 )
	{
		int i = 0;
		while (vector_path[i] != NULL)
		{
			int j;
			for (j = 0; j < 1024; j++)
			{
				if ((strcmp(nodos[j].fname, vector_path[i]) == 0) && (nodos[j].parent_dir_block == parent_dir))
				{
					if ((i == 0) && (parent_dir == 0))
					{
						parent_dir = j + 1;
					}
					if ((i > 0) && (parent_dir != 0))
					{
						parent_dir = j + 1;
					}
				}
			}
			i++;
		}
	}

	return parent_dir;
}


int buscar_nodo_vacio ()
{
	int i;
	for (i = 0; i < 1024; i++)
	{
		if (nodos[i].state == 0)
		{
			return i;
		}
	}
	return -1;
}


void reservar_bloque_directo(int pos_archivo, int i)
{
	int pos_bit = bits_disponibles;

	while(bitarray_test_bit(bitmap, pos_bit))
	{
		pos_bit++;
	}
	bitarray_set_bit(bitmap, pos_bit);
	nodos[pos_archivo].blk_indirect[i] = pos_bit;
	ptrGBloque* blk_direct = (ptrGBloque *) (data_disco + nodos[pos_archivo].blk_indirect[i] * BLOCK_SIZE);
	int k;
	for(k = 0; k < 1024; k++)
	{
		blk_direct[k] = 0;
	}
	return;
}


void reservar_bloque_datos(ptrGBloque *blk_direct, int j)
{
	int pos_bit = bits_disponibles;
	while(bitarray_test_bit(bitmap, pos_bit))
	{
		pos_bit++;
	}
	bitarray_set_bit(bitmap, pos_bit);
	blk_direct[j] = pos_bit;
	char *bloque_dato = (char *) data_disco + blk_direct[j] * BLOCK_SIZE;
	bzero(bloque_dato, BLOCK_SIZE);
	return;
}


/*----------------------------------- FUSE -------------------------------*/

static int grasa_open(const char *path, struct fuse_file_info *fi)
{
	return 0;
}


static int grasa_getattr(const char *path, struct stat *stbuf)
{
	int res = 0;

	memset(stbuf, 0, sizeof(struct stat));

	if (strcmp(path, DIRECTORIO_RAIZ) == 0)
	{
		stbuf->st_mode = S_IFDIR | 0777;
		stbuf->st_nlink = 2;
	}
	else
	{
		pthread_mutex_lock(&mutex);
		int pos_archivo = obtener_bloque_archivo(path);
		if (pos_archivo != -1)
		{
			if (nodos[pos_archivo].state == 1)
			{
				stbuf->st_mode = S_IFREG | 0777;
				stbuf->st_nlink = 1;
				stbuf->st_size = nodos[pos_archivo].file_size;
			}
			else if (nodos[pos_archivo].state == 2)
			{
				stbuf->st_mode = S_IFDIR | 0777;
				stbuf->st_nlink = 1;
			}
		}
		else
		{
			res = -ENOENT;
		}
		pthread_mutex_unlock(&mutex);
	}

	return res;
}


static int grasa_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
	(void) offset;
	(void) fi;

	int i;

	pthread_mutex_lock(&mutex);
	if (strcmp(path, DIRECTORIO_RAIZ) == 0)
	{
		for (i = 0; i < 1024; i++)
		{
			if ((nodos[i].parent_dir_block == 0) && (nodos[i].state != 0))
			{
				filler(buffer, nodos[i].fname, NULL, 0);
			}
		}
	}
	else
	{
		int pos_directorio = obtener_bloque_archivo (path);
		for (i = 0; i < 1024; i++)
		{
			if (((nodos[i].parent_dir_block - 1) == pos_directorio) && (nodos[i].state != 0))
			{
				filler(buffer, nodos[i].fname, NULL, 0);
			}
		}
	}
	pthread_mutex_unlock(&mutex);

	return 0;
}


static int grasa_mkdir (const char* path, mode_t mode)
{
    mode = S_IFDIR | 0777;

    pthread_mutex_lock(&mutex);
    int i = buscar_nodo_vacio();
    pthread_mutex_unlock(&mutex);

    if (i != -1)
    {
    	pthread_mutex_lock(&mutex);
    	nodos[i].state = 2;
    	strcpy (nodos[i].fname, strrchr (path, '/') + 1);
    	nodos[i].parent_dir_block = obtener_bloque_padre(path);
    	pthread_mutex_unlock(&mutex);

    	return 0;
    }
    else
    	return -1;

    return 0;
}


static int grasa_create (const char* path, mode_t mode, struct fuse_file_info * fi)
{
	pthread_mutex_lock(&mutex);
	int i = buscar_nodo_vacio();
	pthread_mutex_unlock(&mutex);

	if (i != -1)
	{
		pthread_mutex_lock(&mutex);
		nodos[i].state = 1;
		strcpy (nodos[i].fname, strrchr (path, '/') + 1);
		nodos[i].file_size = 0;
		nodos[i].parent_dir_block = obtener_bloque_padre (path);
		int j;
		for (j = 0; j < 1000; j++)
		{
			nodos[i].blk_indirect[j] = 0;
		}
		pthread_mutex_unlock(&mutex);
	}

	else
		return -1;

	return 0;
}


static int grasa_utimens(const char * path, const struct timespec ts[2])
{
	pthread_mutex_lock(&mutex);
	int pos_archivo = obtener_bloque_archivo(path);
	pthread_mutex_unlock(&mutex);

	if (pos_archivo != -1)
	{
		pthread_mutex_lock(&mutex);
		nodos[pos_archivo].m_date = ts[1].tv_nsec;
		pthread_mutex_unlock(&mutex);

		return 0;
	}
	else
	{
		return -ENOENT;
	}

	return 0;
}


static int grasa_rmdir(const char* path)
{
	pthread_mutex_lock(&mutex);
	int pos_directorio = obtener_bloque_archivo(path);
	pthread_mutex_unlock(&mutex);

	int dir_vacio = 0;

	if (pos_directorio != -1)
	{
		pthread_mutex_lock(&mutex);
		int i;
		for (i = 0; i < 1024; i++)
		{
			if ((nodos[i].parent_dir_block - 1) == pos_directorio)
			{
				dir_vacio = -1;
			}
		}

		if (dir_vacio == 0)
		{
			bzero(nodos[pos_directorio].fname, 71);
			nodos[pos_directorio].state = 0;
			nodos[pos_directorio].parent_dir_block = 0;
		}
		pthread_mutex_unlock(&mutex);
	}
	else
		return -ENOENT;

	return 0;
}


static int grasa_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	pthread_mutex_lock(&mutex);
	int pos_archivo = obtener_bloque_archivo(path);
	pthread_mutex_unlock(&mutex);

	if(nodos[pos_archivo].file_size)
	{
		int cant_bloques = (offset / BLOCK_SIZE);
		int i = (cant_bloques / 1024); //Subindice Array[1000]
		int j = (cant_bloques % 1024); //Subindice Array[1024]
		int off_bloque = (offset % BLOCK_SIZE);

		pthread_mutex_lock(&mutex);
		ptrGBloque *blk_direct = (ptrGBloque*) (data_disco + nodos[pos_archivo].blk_indirect[i] * BLOCK_SIZE); //blk_direct = Array[1024]
		pthread_mutex_unlock(&mutex);

		bzero(buf, size); //Limpio el buffer
		int bytes_leidos = 0;

		if ((BLOCK_SIZE - off_bloque) >= size)
		{
			pthread_mutex_lock(&mutex);
			memcpy(buf, (char *) (data_disco + blk_direct[j] * BLOCK_SIZE + off_bloque), size);
			pthread_mutex_unlock(&mutex);
			bytes_leidos = size;
		}
		else
		{
			pthread_mutex_lock(&mutex);
			memcpy(buf, (char *) (data_disco + blk_direct[j] * BLOCK_SIZE + off_bloque), BLOCK_SIZE - off_bloque);
			pthread_mutex_unlock(&mutex);
			bytes_leidos += BLOCK_SIZE - off_bloque;
			size -= BLOCK_SIZE - off_bloque;
			j++;

			if (j == 1024)
			{
				j = 0;
				i++;
				pthread_mutex_lock(&mutex);
				blk_direct = (data_disco + nodos[pos_archivo].blk_indirect[i] * BLOCK_SIZE);
				pthread_mutex_unlock(&mutex);
			}

			int cant_bloques_por_leer = size / BLOCK_SIZE;
			int bytes_por_leer = size % BLOCK_SIZE;

			if (cant_bloques_por_leer == 0)
			{
				pthread_mutex_lock(&mutex);
				memcpy(buf + bytes_leidos, (char *) (data_disco + blk_direct[j] * BLOCK_SIZE), bytes_por_leer);
				pthread_mutex_unlock(&mutex);
				bytes_leidos += bytes_por_leer;
				size -= bytes_por_leer;
			}
			else
			{
				int k;
				for (k = 1; k <= cant_bloques_por_leer; k++)
				{
					pthread_mutex_lock(&mutex);
					memcpy(buf + bytes_leidos, (char *) (data_disco + (blk_direct[j] * BLOCK_SIZE)), BLOCK_SIZE);
					pthread_mutex_unlock(&mutex);
					bytes_leidos += BLOCK_SIZE;
					size -= BLOCK_SIZE;
					++j;

					if (j == 1024)
					{
						j = 0;
						i++;
						pthread_mutex_lock(&mutex);
						blk_direct = (data_disco + nodos[pos_archivo].blk_indirect[i] * BLOCK_SIZE);
						pthread_mutex_unlock(&mutex);
					}
				}

				if(bytes_por_leer > 0)
				{
					pthread_mutex_lock(&mutex);
					memcpy(buf + bytes_leidos, (char *) (data_disco + (blk_direct[j] * BLOCK_SIZE)), bytes_por_leer);
					pthread_mutex_unlock(&mutex);
					bytes_leidos += bytes_por_leer;
					size -= bytes_por_leer;
				}
			}
		}

		return bytes_leidos;
	}
	return 0;
}


static int grasa_truncate(const char *path, off_t offset)
{
	pthread_mutex_lock(&mutex);
	int pos_archivo = obtener_bloque_archivo(path);
	pthread_mutex_unlock(&mutex);

	//Achicar archivo
	if(nodos[pos_archivo].file_size > offset)
	{
		int cant_bloques = (offset / BLOCK_SIZE);
		int i = (cant_bloques / 1024); //Subindice Array[1000]
		int j = (cant_bloques % 1024); //Subindice Array[1024]
		int off_bloque = (offset % BLOCK_SIZE);

		if(offset == 0) // (cant_bloques == 0) && (off_bloque == 0) //Borrar archivo completo
		{
			while((nodos[pos_archivo].blk_indirect[i] != 0) && (i < 1000))
			{
				pthread_mutex_lock(&mutex);
				ptrGBloque *blk_direct = (ptrGBloque *) (data_disco + nodos[pos_archivo].blk_indirect[i] * BLOCK_SIZE);//blk_direct = Array[1024]
				pthread_mutex_unlock(&mutex);

				while((blk_direct[j] != 0) && (j < 1024))
				{
					pthread_mutex_lock(&mutex);
					bitarray_clean_bit(bitmap, blk_direct[j]);
					char *bloque_dato = (char *) data_disco + blk_direct[j] * BLOCK_SIZE;
					bzero(bloque_dato, BLOCK_SIZE);
					blk_direct[j] = 0;
					pthread_mutex_unlock(&mutex);
					++j;
				}

				pthread_mutex_lock(&mutex);
				bitarray_clean_bit(bitmap, nodos[pos_archivo].blk_indirect[i]);//Borra Array[1024]
				nodos[pos_archivo].blk_indirect[i] = 0;
				pthread_mutex_unlock(&mutex);

				++i;
			}

			pthread_mutex_lock(&mutex);
			nodos[pos_archivo].file_size = 0;
			pthread_mutex_unlock(&mutex);

			return 0;
		}

		if((cant_bloques == 0) && (off_bloque > 0)) //Menos de un bloque
		{
			while((nodos[pos_archivo].blk_indirect[i] != 0) && (i < 1000))
			{
				pthread_mutex_lock(&mutex);
				ptrGBloque *blk_direct = (ptrGBloque *) (data_disco + nodos[pos_archivo].blk_indirect[i] * BLOCK_SIZE);
				pthread_mutex_unlock(&mutex);

				++j;
				while((blk_direct[j] != 0) && (j < 1024))
				{
					pthread_mutex_lock(&mutex);
					bitarray_clean_bit(bitmap, blk_direct[j]);
					char *bloque_dato = (char *) data_disco + blk_direct[j] * BLOCK_SIZE;
					bzero(bloque_dato, BLOCK_SIZE);
					blk_direct[j] = 0;
					pthread_mutex_unlock(&mutex);
					++j;
				}
				if(i > 0)
				{
					pthread_mutex_lock(&mutex);
					bitarray_clean_bit(bitmap, nodos[pos_archivo].blk_indirect[i]);
					nodos[pos_archivo].blk_indirect[i] = 0;
					pthread_mutex_unlock(&mutex);
				}

				++i;
			}

			return 0;
		}

		if(cant_bloques > 0) //1 Bloque entero y un poco mas || N Bloques enteros y un poco mas
		{
			if(off_bloque > 0) //Valido si estoy en la mitad del bloque, si es asi, paso al siguiente
			{
				++j;
				if(j == 1024)
				{
					++i;
					if(i == 1000)
					{
						return -1;
					}
					j = 0;
				}
			}

			while((nodos[pos_archivo].blk_indirect[i] != 0) && (i < 1000))
			{
				pthread_mutex_lock(&mutex);
				ptrGBloque *blk_direct = (ptrGBloque *) (data_disco + nodos[pos_archivo].blk_indirect[i] * BLOCK_SIZE);
				pthread_mutex_unlock(&mutex);

				while((blk_direct[j] != 0) && (j < 1024))
				{
					pthread_mutex_lock(&mutex);
					bitarray_clean_bit(bitmap, blk_direct[j]);
					char *bloque_dato = (char *) data_disco + blk_direct[j] * BLOCK_SIZE;
					bzero(bloque_dato, BLOCK_SIZE);
					blk_direct[j] = 0;
					pthread_mutex_unlock(&mutex);

					++j;
				}
				if(blk_direct[0] == 0) //Si tengo un bloque de datos en la posicion 0, no borro el array[1024]
				{
					pthread_mutex_lock(&mutex);
					bitarray_clean_bit(bitmap, nodos[pos_archivo].blk_indirect[i]);
					nodos[pos_archivo].blk_indirect[i] = 0;
					pthread_mutex_unlock(&mutex);
				}
					++i;
			}

			return 0;
		}
	} //Hasta aca chequeado

	//Agrandar archivo
	if(nodos[pos_archivo].file_size < offset)
	{
		pthread_mutex_lock(&mutex);
		int cant_bloques = (nodos[pos_archivo].file_size / BLOCK_SIZE);
		int i = (cant_bloques / 1024); //Subindice Array[1000]
		int j = (cant_bloques % 1024); //Subindice Array[1024]
		int off_bloque = (nodos[pos_archivo].file_size % BLOCK_SIZE);
		pthread_mutex_unlock(&mutex);

		if (nodos[pos_archivo].file_size == 0) //el archivo es nuevo
		{
			int bytes_por_reservar = offset;
			int bytes_reservados = 0;
			pthread_mutex_lock(&mutex);
			reservar_bloque_directo(pos_archivo, i); //Array[1024]
			ptrGBloque *blk_direct = (ptrGBloque *) (data_disco + nodos[pos_archivo].blk_indirect[i] * BLOCK_SIZE);
			pthread_mutex_unlock(&mutex);

			while(bytes_por_reservar > bytes_reservados)
			{
				pthread_mutex_lock(&mutex);
				reservar_bloque_datos(blk_direct, j);
				pthread_mutex_unlock(&mutex);
				bytes_reservados += BLOCK_SIZE;
				bytes_por_reservar -= bytes_reservados;
				++j;
				if(j == 1024)
				{
					++i;
					if(i < 1000)
					{
						pthread_mutex_lock(&mutex);
						reservar_bloque_directo(pos_archivo, i);
						blk_direct = (ptrGBloque *) (data_disco + nodos[pos_archivo].blk_indirect[i] * BLOCK_SIZE);
						pthread_mutex_unlock(&mutex);
						j = 0;
					}
					else
					{
						return -1;
					}
				}
			}

			return 0;
		}

		else //filesize > 0 El archivo ya tiene datos grabados
		{
			int bytes_por_reservar = offset - nodos[pos_archivo].file_size;
			int bytes_reservados = 0;

			while(bytes_por_reservar > bytes_reservados)
			{
				if(nodos[pos_archivo].blk_indirect[i] == 0)
				{
					pthread_mutex_lock(&mutex);
					reservar_bloque_directo(pos_archivo, i);
					pthread_mutex_unlock(&mutex);
					j = 0;
				}

				pthread_mutex_lock(&mutex);
				ptrGBloque *blk_direct = (ptrGBloque *) (data_disco + nodos[pos_archivo].blk_indirect[i] * BLOCK_SIZE);
				pthread_mutex_unlock(&mutex);

				if(blk_direct[j] == 0)
				{
					pthread_mutex_lock(&mutex);
					reservar_bloque_datos(blk_direct, j);
					pthread_mutex_unlock(&mutex);
					bytes_reservados += BLOCK_SIZE;
					bytes_por_reservar -= bytes_reservados;
				}

				++j;

				if(j == 1024)
				{
					++i;
					if(i < 1000)
					{
						pthread_mutex_lock(&mutex);
						reservar_bloque_directo(pos_archivo, i);
						blk_direct = (ptrGBloque *) (data_disco + nodos[pos_archivo].blk_indirect[i] * BLOCK_SIZE);
						pthread_mutex_unlock(&mutex);
						j = 0;
					}
					else
					{
						return -1;
					}
				}
				else
				{
					bytes_reservados += (offset - bytes_por_reservar);
					bytes_por_reservar -= bytes_reservados;
				}

			}
		}

		return 0;
	}

	return 0;
}


static int grasa_unlink(const char* path)
{
	pthread_mutex_lock(&mutex);
	int pos_archivo = obtener_bloque_archivo (path);
	pthread_mutex_unlock(&mutex);

	if	(pos_archivo != -1)
	{
		grasa_truncate(path, 0); //No semaforear

		pthread_mutex_lock(&mutex);
		bzero(nodos[pos_archivo].fname, 71);
		nodos[pos_archivo].state = 0;
		nodos[pos_archivo].parent_dir_block = 0;
		pthread_mutex_unlock(&mutex);
	}

	return 0;
}


static int grasa_write(const char* path, const char* buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	pthread_mutex_lock(&mutex);
	int pos_archivo = obtener_bloque_archivo(path);
	pthread_mutex_unlock(&mutex);

	int cant_bloques = (offset / BLOCK_SIZE);
	int i = (cant_bloques / 1024); //Subindice Array[1000]
	int j = (cant_bloques % 1024); //Subindice Array[1024]
	int off_bloque = (offset % BLOCK_SIZE);

	grasa_truncate(path, size + offset); //No semaforear

	pthread_mutex_lock(&mutex);
	ptrGBloque *blk_direct = (ptrGBloque *) (data_disco + nodos[pos_archivo].blk_indirect[i] * BLOCK_SIZE); //blk_direct = Array[1024]
	pthread_mutex_unlock(&mutex);

	long int bytes_escritos = 0;

	if ((BLOCK_SIZE - off_bloque) >= size)
	{
		pthread_mutex_lock(&mutex);
		memcpy((char *) (data_disco + blk_direct[j] * BLOCK_SIZE + off_bloque), buf, size);
		pthread_mutex_unlock(&mutex);
		bytes_escritos += size;
		size -= bytes_escritos;
		pthread_mutex_lock(&mutex);
		nodos[pos_archivo].file_size += bytes_escritos;
		pthread_mutex_unlock(&mutex);
	}
	else
	{
		pthread_mutex_lock(&mutex);
		memcpy((char *) (data_disco + blk_direct[j] * BLOCK_SIZE + off_bloque), buf, BLOCK_SIZE - off_bloque);
		pthread_mutex_unlock(&mutex);
		bytes_escritos += (BLOCK_SIZE - off_bloque);
		size -= bytes_escritos;
		pthread_mutex_lock(&mutex);
		nodos[pos_archivo].file_size += bytes_escritos;
		pthread_mutex_unlock(&mutex);
		j++;

		if (j == 1024)
		{
			j = 0;
			i++;
			pthread_mutex_lock(&mutex);
			blk_direct = (data_disco + nodos[pos_archivo].blk_indirect[i] * BLOCK_SIZE);
			pthread_mutex_unlock(&mutex);
		}

		int cant_bloques_por_escribir = size / BLOCK_SIZE;
		int bytes_por_escribir = size % BLOCK_SIZE;

		if (cant_bloques_por_escribir == 0)
		{
			pthread_mutex_lock(&mutex);
			memcpy((char *) (data_disco + blk_direct[j] * BLOCK_SIZE), buf + bytes_escritos, bytes_por_escribir);
			pthread_mutex_unlock(&mutex);
			bytes_escritos += bytes_por_escribir;
			size -= bytes_por_escribir;
			pthread_mutex_lock(&mutex);
			nodos[pos_archivo].file_size += bytes_escritos;
			pthread_mutex_unlock(&mutex);
		}
		else
		{
			int k;
			for (k = 0; k < cant_bloques_por_escribir; k++)
			{
				pthread_mutex_lock(&mutex);
				memcpy((char *) (data_disco + (blk_direct[j] * BLOCK_SIZE)), buf + bytes_escritos, BLOCK_SIZE);
				pthread_mutex_unlock(&mutex);
				bytes_escritos += BLOCK_SIZE;
				size -= BLOCK_SIZE;
				pthread_mutex_lock(&mutex);
				nodos[pos_archivo].file_size += bytes_escritos;
				pthread_mutex_unlock(&mutex);
				++j;

				if (j == 1024)
				{
					j = 0;
					i++;
					pthread_mutex_lock(&mutex);
					blk_direct = (data_disco + nodos[pos_archivo].blk_indirect[i] * BLOCK_SIZE);
					pthread_mutex_unlock(&mutex);
				}
			}

			if(bytes_por_escribir > 0)
			{
				pthread_mutex_lock(&mutex);
				memcpy((char *) (data_disco + (blk_direct[j] * BLOCK_SIZE)), buf + bytes_escritos, bytes_por_escribir);
				pthread_mutex_unlock(&mutex);
				bytes_escritos += bytes_por_escribir;
				size -= bytes_por_escribir;
				pthread_mutex_lock(&mutex);
				nodos[pos_archivo].file_size += bytes_escritos;
				pthread_mutex_unlock(&mutex);
			}
		}
	}

	return bytes_escritos;
}


static struct fuse_operations grasa_oper = {
											.getattr = grasa_getattr,
											.readdir = grasa_readdir,
											.mkdir = grasa_mkdir,
											.create = grasa_create,
											.rmdir = grasa_rmdir,
											.unlink = grasa_unlink,
											.open = grasa_open,
											.read = grasa_read,
											.truncate = grasa_truncate,
											.write = grasa_write,
											.utimens = grasa_utimens,
											};

enum {
		KEY_VERSION, KEY_HELP,
		};


static struct fuse_opt fuse_options[] = {
   // Estos son parametros por defecto que ya tiene FUSE
   FUSE_OPT_KEY("-V", KEY_VERSION),
   FUSE_OPT_KEY("--version", KEY_VERSION),
   FUSE_OPT_KEY("-h", KEY_HELP),
   FUSE_OPT_KEY("--help", KEY_HELP),
   FUSE_OPT_END,
};

int main(int argc, char* argv[])
{
	char* filepath = argv[1];
	//struct fuse_args args = FUSE_ARGS_INIT(argc, argv);



	obtener_tamanio_disco(filepath);
	obtener_bitmap_cant_bloques();
	mapeo_archivo(filepath);
	inicio_header();
	inicio_tabla_nodos();
	obtener_bitmap_cant_bloques();
	inicio_bitmap();
	bits_disponibles = 1 + 1024 + bitmap_cant_bloques;

	pthread_mutex_init(&mutex, NULL);

	puts("Se inicio correctamente el filesystem");

	return fuse_main(argc - 1, &argv[1], &grasa_oper, NULL);
}
