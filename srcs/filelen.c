#include "woody.h"

static long NULL_long_ret(char *str, char *buffer, FILE *fd) {
	if (str)
		perror(str);
	if (buffer)
		free(buffer);
	if (fd)
		fclose(fd);
	return -1;
}

long filelen(const char *filename) {
	FILE *fd;
	long filesize;

	fd = fopen(filename, "rb");
	if (fd == NULL)
		return NULL_long_ret("file error", NULL, NULL);

	if (fseek(fd, 0, SEEK_END) != 0)
		return NULL_long_ret("fseek error", NULL, fd);

	filesize = ftell(fd);
	if (filesize == -1)
		return NULL_long_ret("ftell error", NULL, fd);

	fclose(fd);
	return filesize;
}
