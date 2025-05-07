#include "woody.h"

int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <ELF file>\n", argv[0]);
		return 1;
	}
	int file_size;
	char *initial_code = read_file(argv[1], &file_size);
	if (!initial_code || file_size <= 0)
		return 1;

	write_file(initial_code, file_size);
	return 0;
}
