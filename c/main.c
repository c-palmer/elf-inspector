#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <assert.h>

#include "elf_inspector.h"

int main(int argc, char **argv) {
    printf("input file: %s\n", argv[1]);

    int fd = open(argv[1], O_RDONLY);
    assert (fd != -1);

    struct stat file_info;
    assert(fstat(fd, &file_info) != -1);

    char *mmap_addr = mmap(
        NULL,
        file_info.st_size,
        PROT_READ,
        MAP_PRIVATE,
        fd,
        0
    );
    assert(mmap_addr != MAP_FAILED);

    ElfFile elf_file = create_elf_file(mmap_addr);

    printf("0x%08X [START elf file]\n", 0);
    print_elf_header(elf_file);
    print_section_table(elf_file);
    print_sections(elf_file);
    printf("0x%08lX [END elf file]\n", file_info.st_size - 1);


    // Elf64_Ehdr *elf_header = (Elf64_Ehdr *) mmap_addr;
    // print_elf_header(elf_header);

    // Elf64_Shdr *section_table = (Elf64_Shdr *) (mmap_addr + elf_header->e_shoff);





















    // Elf64_Shdr symtab_header = elf_file.section_table[elf_file.symtab_shndx];

    // printf("symtab strings:\n");
    // char *section = mmap_addr + symtab_header.sh_offset;
    // for (int i = 0; i < elf_file.section_table[4].sh_size; i++) {
    //     if (section[i] == '\0') {
    //         printf("\\0");
    //         continue;
    //     }

    //     printf("%c", section[i]);
    // }
    // printf("\n\n");
    
    // printf("section 2 strings:\n");
    // section = mmap_addr + section_table[2].sh_offset;
    // for (int i = 0; i < section_table[2].sh_size; i++) {
    //     if (section[i] == '\0') {
    //         printf("\\0");
    //         continue;
    //     }

    //     printf("%c", section[i]);
    // }
    // printf("\n\n");


    // printf("section 3 symbol table:\n");
    // int symbol_str_section = section_table[3].sh_link;
    // printf(
    //     "associated string table section number: %d\n",
    //     section_table[3].sh_link
    // );
    // int symbol_count = section_table[3].sh_size / section_table[3].sh_entsize;
    // printf("number of symbol entries: %d\n", symbol_count);

    // Elf64_Sym *symbols = (Elf64_Sym *) (mmap_addr + section_table[3].sh_offset);
    // char *symbol_strs = mmap_addr + section_table[symbol_str_section].sh_offset;
    // for (int i = 0; i < symbol_count; i++) {
    //     printf("name: %s\n", symbol_strs + symbols[i].st_name);
    // }

    // // for (int i = 0; i < elf_header->e_shnum; i++) {
    // //     print_section(&section_table[i], elf_header, mmap_addr);
    // // }


    // // printf("0x%08X ... 0x%08X elf header\n", 0, elf_header->e_ehsize);
    // // printf("0x%08X ... 0x%08X program header table\n", 0, 0);
    // // printf("segments?\n");
    // // // printf("0x%08X ... 0x%08X segment 0\n", 0, 0);
    // // // printf("0x%08X ... 0x%08X segment 1\n", 0, 0);
    // // printf(
    // //     "0x%08X ... 0x%08X section header table\n",
    // //     elf_header->e_shoff,
    // //     elf_header->e_shoff + elf_header->e_shnum * elf_header->e_shentsize
    // // );

    // // for (int i = 0; i < elf_header->e_shnum; i++) {
    // //     if (section_table[i].sh_size == 0) continue;

    // //     printf(
    // //         "0x%08X ... 0x%08X section %d\n",
    // //         section_table[i].sh_offset,
    // //         section_table[i].sh_offset + section_table[i].sh_size,
    // //         i
    // //     );
    // // }

    // // print_elf_header_bytes(elf_header);

    // // printf("\nelf header (%d bytes):\n", elf_header->e_ehsize);
    // // printf("(from 0x%08x to 0x%08x)\n", 0, elf_header->e_ehsize);

    // // for (int i = 0; i < elf_header->e_ehsize; i++) {
    // //     if (i == 0) {
    // //         printf("0x%08x", i);
    // //     }

    // //     if (i % 8 == 0 && i != 0) {
    // //         printf("\n0x%08x", i - 8);

    // //         for (int j = i - 8; j < i; j++) {
    // //             printf("          %02d", j);
    // //         }

    // //         printf("\n\n0x%08x", i);
    // //     }

    // //         // if (i != 0) {
    // //         //     printf("\n0x%08x\n", i - 8);
    // //         // }

    // //     printf("        0x%02x", mmap_addr[i]);
    // // }
    // // printf("\n\n");

    // // printf("\n\n");

    // // printf("\n");

    // // printf(
    // //     "0x%x %c %c %c\n",
    // //     mmap_addr[EI_MAG0],
    // //     mmap_addr[EI_MAG1],
    // //     mmap_addr[EI_MAG2],
    // //     mmap_addr[EI_MAG3]
    // // );


    // // printf("class: %d\n", elf_header->e_ident[EI_CLASS]);

    // // printf("section header offset: %d\n", elf_header->e_shoff);
    // // printf("section count: %d\n", elf_header->e_shnum);

    // // Elf64_Shdr *section_headers = (Elf64_Shdr *) mmap_addr + elf_header->e_shoff;

    // // printf("");

    munmap(mmap_addr, file_info.st_size);
    close(fd);

    return 0;
}