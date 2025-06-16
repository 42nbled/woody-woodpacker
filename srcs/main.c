#include "woody.h"

int main(int argc, char *argv[]) {
	const char		filename[] = "woody";
	unsigned char	*key;
	char			*initial_code;
	int				file_size;
	int				fd;

	if (argc < 2 || argc > 3) {
		fprintf(stderr, "Usage: %s <ELF file> [key]\n", argv[0]);
		return 1;
	}
	if (argc == 3)
		key = (unsigned char *)argv[2];
	else
	 	key = (unsigned char*)"\x42\x42\x42\x42";

	initial_code = read_file(argv[1], &file_size);
	if (!initial_code || file_size <= 0)
		return 1;

	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)initial_code;
	if (ehdr->e_ident[EI_CLASS] != ELFCLASS64) {
		fprintf(stderr, "Error: Not a 64-bit ELF file.\n");
		free(initial_code);
		return 1;
	}

	fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0777);
	if (fd == -1) {
		fprintf(stderr, "Error: open error");
		free(initial_code);
		return 1;
	}

	silvio_infect(fd, initial_code, file_size, key);
	return 0;
}
