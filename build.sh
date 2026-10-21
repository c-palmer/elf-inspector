#!/bin/sh

nasm -f elf64 -gDWARF asm/main.s -o asm/main.o
ld asm/main.o -o asm/main

gcc -g c/main.c -o c/main


printf "assembly output:\n"

./asm/main

printf "\nelf contents:\n"

./c/main asm/main.o