#include "woody.h"

#define MESSAGE_SIZE 14

extern unsigned char __start_payload[];		// pointeur du debut de la fonction
extern unsigned char __stop_payload[];		// pointeur de la fin de la fonction
extern unsigned char _encrypted_start[];	// pointeur du debut de la partie a encrypter
extern unsigned char _encrypted_end[];		// pointeur de la fin de la partie a encrypter

unsigned long	calc_jump(unsigned long from, unsigned long to, unsigned long offset)
{
	return to - from - offset;
}

void	XOR(unsigned char *payload, size_t payload_len, char key) {
	for (size_t i = 0; i < payload_len; i++)
		payload[i] ^= key;
}

void	write_file(char *elf, long elf_size, int fd, unsigned long int text_end, unsigned long rel_jump) {
	size_t payload_len = __stop_payload - __start_payload;
	size_t jump_offest = _encrypted_start - __start_payload;
	size_t _encrypted_size = _encrypted_end - _encrypted_start;
	unsigned char payload[payload_len];

	memcpy(payload, __start_payload, payload_len);
	XOR(payload + (size_t)(_encrypted_start - __start_payload), _encrypted_size, 0x42);

	memcpy(&payload[jump_offest - 4], &rel_jump, 4);

	write(fd, elf, text_end);
	write(fd, payload, payload_len);

	char b[4096] = {0};
	write(fd, b, PAGE_SIZE - payload_len);

	write(fd, elf + text_end, elf_size - text_end);
	close(fd);
	free(elf);
}

void	silvio_infect(int fd, char *elf, long elf_size) {

	size_t payload_len = __stop_payload - __start_payload;
	size_t jump_offest = _encrypted_start - __start_payload;

	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)elf;
	Elf64_Phdr *phdr = (Elf64_Phdr *)(elf + ehdr->e_phoff);
	Elf64_Shdr *shdr = (Elf64_Shdr *)(elf + ehdr->e_shoff);

	unsigned long int text_end, payload_vaddr;
	unsigned long int entry = ehdr->e_entry;

	ehdr->e_shoff += PAGE_SIZE;

	for (size_t i = 0; i < ehdr->e_phnum; ++i) {
		if (phdr[i].p_type == PT_LOAD && phdr[i].p_flags == (PF_R | PF_X)) {
			text_end = phdr[i].p_offset + phdr[i].p_filesz;
			payload_vaddr = phdr[i].p_vaddr + phdr[i].p_memsz;

			ehdr->e_entry = payload_vaddr;
			phdr[i].p_filesz += payload_len;
			phdr[i].p_memsz += payload_len;

			for (size_t j=i+1; j < ehdr->e_phnum; ++j)
				phdr[j].p_offset += PAGE_SIZE;

			break ;
		}
	}

	for (size_t i=0; i < ehdr->e_shnum; ++i) {
		if (shdr[i].sh_offset > text_end)
			shdr[i].sh_offset += PAGE_SIZE;
		else if (shdr[i].sh_addr + shdr[i].sh_size == payload_vaddr)
			shdr[i].sh_size += payload_len;
	}

	unsigned long rel_jump = calc_jump(payload_vaddr, entry, jump_offest);
	write_file(elf, elf_size, fd, text_end, rel_jump);
}
