#include "woody.h"
#include <string.h>

#define MESSAGE_SIZE 14

extern unsigned char __start_payload[];		// pointeur du debut de la fonction
extern unsigned char __stop_payload[];		// pointeur de la fin de la fonction
extern unsigned char _encrypted_start[];	// pointeur du debut de la partie a encrypter
extern unsigned char _encrypted_end[];		// pointeur de la fin de la partie a encrypter
extern unsigned char key_start[];


unsigned long	calc_jump(unsigned long from, unsigned long to, unsigned long offset)
{
	return to - from - offset;
}

size_t    ft_strlen(unsigned char *S)
{
    size_t    l = 0;
    while (S[l]) l++;
    return l;
}

void    RC4(unsigned char *text, size_t len, unsigned char *key, size_t L)
{
    unsigned char S[256] = {0};
    for (int i = 0; i < 256; i++)
        S[i] = i;
    for (int i = 0, j = 0; i < 256; i++ )
    {
        j = (j + S[i] + key[i % L]) % 256;
        S[i] ^= S[j];
        S[j] ^= S[i];
        S[i] ^= S[j];
    }

    for (size_t i = 0, j = 0, k = 0, t = 0; k < len; k++)
    {
        i = (i + 1) % 256 ;
        j = (j + S[i]) % 256 ;
        S[i] ^= S[j];
        S[j] ^= S[i];
        S[i] ^= S[j];
        t = (S[i] + S[j]) % 256 ;
        text[k] ^= S[t];
    }
}

unsigned char	*zero_extend(unsigned char *src, size_t len)
{
	unsigned char *dst;

	dst = malloc(sizeof(char) * len);
	bzero(dst, sizeof(char) * len);
	memcpy(dst, src, strlen((char*)src));
	return dst;
}

void	write_file(char *elf, long elf_size, int fd, unsigned long int text_end, unsigned long rel_jump, unsigned char *key) {
	size_t payload_len = __stop_payload - __start_payload;
	size_t jump_offest = _encrypted_start - __start_payload;
	size_t _encrypted_size = _encrypted_end - _encrypted_start;
	size_t key_offset = key_start - __start_payload;
	size_t key_len = __stop_payload - key_start;
	unsigned char payload[payload_len];


	key = zero_extend(key, key_len);
	memcpy(payload, __start_payload, payload_len);
	RC4(payload + (size_t)(_encrypted_start - __start_payload), _encrypted_size, key, key_len);

	memcpy(&payload[jump_offest - 4], &rel_jump, 4);
	memcpy(&payload[key_offset], key, key_len);
	write(fd, elf, text_end);
	write(fd, payload, payload_len);

	char b[4096] = {0};
	write(fd, b, PAGE_SIZE - payload_len);

	write(fd, elf + text_end, elf_size - text_end);
	close(fd);
	free(elf);
	free(key);
}

void	silvio_infect(int fd, char *elf, long elf_size, unsigned char* key) {

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
	write_file(elf, elf_size, fd, text_end, rel_jump, key);
}
