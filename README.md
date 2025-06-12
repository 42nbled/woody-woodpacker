# Woody Woodpacker

Woody Woodpacker is an ELF file infector/packer for 64-bit Linux ELF binaries. It injects a custom payload into the target ELF file, encrypts part of the payload, and writes the modified binary as `woody`.

## Project Structure

- `srcs/` - Source code (C and assembly)
  - `main.c` - Entry point
  - `read_file.c` - File reading logic
  - `write_file.c` - ELF infection and writing logic
  - `payload.asm` - Assembly payload injected into binaries
- `header/woody.h` - Header file
- `Makefile` - Build instructions
- `tests_binaries/` - Test binaries and test source

## Build

To build the project, run:

```sh
make
```

This will produce the `woody_woodpacker` executable.

## Usage

```sh
./woody_woodpacker <target_elf>
```

- `<target_elf>`: Path to a 64-bit ELF binary to infect.
- The infected binary will be written as `woody` in the current directory.

## Notes

- Only 64-bit ELF files are supported.
- The payload prints "....WOODY...." when executed.
- Test binaries are provided in `tests_binaries/` for experimentation.
