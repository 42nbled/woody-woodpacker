#ifndef WOOODY_H
# define WOOODY_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <elf.h>
#include <sys/stat.h>

char	*read_file(const char *filename, int *size);
void	write_file(char *initial_code, long size);
long	filelen(const char *filename);
void	insert_str(char **buffer, const char *str, int index, size_t *size);

#endif
