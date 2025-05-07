#include "woody.h"
#include "syscall.h"

void __attribute__((naked, section("payload"))) payload_write(void)
{
	asm __volatile__ (
		"push %rbp\n\t"
		"movq %rsp, %rbp\n\t"
		"sub $0x10, %rsp\n\t"
		"movabs $0xa21646c726f77, %rax\n\t"
		"movq %rax, -0x08(%rbp)\n\t"

		"movabs $0x77202c6f6c6c6568, %rax\n\t"
		"movq %rax, -0x0f(%rbp)\n\t"

		"lea -0x10(%rbp), %rsi\n\t"

		"movq $1, %rax\n\t"
		"movq $1, %rdi\n\t"
		"movq $15, %rdx\n\t"
		"syscall\n\t"
		"add $0x10, %rsp\n\t"
		"pop %rbp\n\t"
		"ret\n\t"
	);
}

long _syscall(long syscall_number, ...) {
	__builtin_va_list args;
	long result;

	long arg1, arg2, arg3, arg4, arg5, arg6;

	__builtin_va_start(args, syscall_number);
	arg1 = __builtin_va_arg(args, long);
	arg2 = __builtin_va_arg(args, long);
	arg3 = __builtin_va_arg(args, long);
	arg4 = __builtin_va_arg(args, long);
	arg5 = __builtin_va_arg(args, long);
	arg6 = __builtin_va_arg(args, long);
	__builtin_va_end(args);

	__asm__ volatile (
			"movq %1, %%rax	\n\t"
			"movq %2, %%rdi	\n\t"
			"movq %3, %%rsi	\n\t"
			"movq %4, %%rdx	\n\t"
			"movq %5, %%r10	\n\t"
			"movq %6, %%r8	\n\t"
			"movq %7, %%r9	\n\t"
			"syscall		\n\t"
			"movq %%rax, %0	\n\t"
			: "=r" (result)
			: "r" (syscall_number), "r" (arg1), "r" (arg2), "r" (arg3),
			"r" (arg4), "r" (arg5), "r" (arg6)
			: "rax", "rdi", "rsi", "rdx", "r10", "r8", "r9", "memory"
		);

	if (result < 0) {
		return -1;
	}

    return result;
}

extern unsigned char __start_payload[];
extern unsigned char __stop_payload[];

static void NULL_void_ret(char *str, char *buffer, int fd) {
	if (str)
		perror(str);
	if (buffer)
		free(buffer);
	if (fd)
		close(fd);
	return;
}

void write_file(char *buffer, long size) {
    const char *filename = "woody";
    char *ptr = buffer;
    int fd;
    size_t buffer_size = size;

    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)buffer;
    Elf64_Phdr *phdr = (Elf64_Phdr *)(buffer + ehdr->e_phoff);
    Elf64_Shdr *shdr = (Elf64_Shdr *)(buffer + ehdr->e_shoff);

    (void)shdr;

    if (memcmp(ehdr->e_ident, ELFMAG, SELFMAG) != 0)
        {NULL_void_ret("not a valid ELF file.", ptr, 0); return;}

    Elf64_Phdr *target_phdr = NULL;
    for (int i = 0; i < ehdr->e_phnum; i++) {
        if (phdr[i].p_type == PT_LOAD && (phdr[i].p_flags & PF_X)) {
            target_phdr = &phdr[i];
            break;
        }
    }

    if (!target_phdr)
        {NULL_void_ret("no available phdr found", ptr, 0); return;}

    fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    printf("%d\n", fd);
    if (fd == -1)
        {NULL_void_ret("open error", ptr, 0); return;}

    size_t payload_len = __stop_payload - __start_payload;
    unsigned char payload[payload_len];
	memcpy(payload, __start_payload, payload_len);

    size_t insert_offset = target_phdr->p_offset;

    ehdr = (Elf64_Ehdr *)buffer;

    if (ehdr->e_shoff >= insert_offset) {
        ehdr->e_shoff += payload_len;
    }

    phdr = (Elf64_Phdr *)(buffer + ehdr->e_phoff);
    for (int i = 0; i < ehdr->e_phnum; i++) {
        if (phdr[i].p_offset > insert_offset) {
            phdr[i].p_offset += payload_len;
        }
        if (phdr[i].p_offset <= insert_offset &&
            phdr[i].p_offset + phdr[i].p_filesz >= insert_offset) {
            phdr[i].p_filesz += payload_len;
            phdr[i].p_memsz += payload_len;
        }
    }

    _syscall(SYS_write, fd, buffer, insert_offset);
    _syscall(SYS_write, fd, payload, payload_len);
    _syscall(SYS_write, fd, buffer + insert_offset, buffer_size - insert_offset);

    close(fd);
    free(buffer);
}
