#include "woody.h"

static char *NULL_str_ret(char *str, char *buffer, int fd) {
	if (str)
		perror(str);
	if (buffer)
		free(buffer);
	if (fd)
		close(fd);
	return NULL;
}

char *read_file(const char *filename, int *size) {
	int	fd;
	int filesize;
	char *buffer;
	struct stat st;
	fd = open(filename, O_RDONLY);
	if (fd == -1)
		return NULL_str_ret("file error", NULL, 0);

	if (fstat(fd, &st) == -1 )
		return NULL_str_ret("stat error", NULL, 0);

	filesize = st.st_size;

	buffer = malloc(filesize);
	if (!buffer)
		return NULL_str_ret("malloc error", NULL, fd);

	if ((read(fd, buffer, filesize) != filesize))
		return NULL_str_ret("read error", buffer, fd);

	*size = filesize;
	close(fd);
	return buffer;
}
