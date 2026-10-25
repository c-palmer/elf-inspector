#!/bin/sh

# nasm -f elf64 -gDWARF asm/main.s -o asm/main.o
nasm -f elf64 asm/main.s -o asm/main.o
ld asm/main.o -o asm/main

compile() {
    gcc -Wall -Werror -g \
        -Iinclude \
        c/main.c c/elf_inspector.c \
        -lelf \
        -o c/main
}

compile || exit 1

# if ! compile; then
#     exit 1
# fi


printf "assembly output:\n"

./asm/main

printf "\nelf inspector:\n"

./c/main asm/main.o