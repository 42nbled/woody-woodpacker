#ifndef WOOODY_H
# define WOOODY_H

# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <fcntl.h>
# include <elf.h>
# include <sys/stat.h>

# define PAGE_SIZE getpagesize()

char	*read_file(const char *filename, int *size);
void	silvio_infect(int fd, char *elf, long elf_size);

#endif
