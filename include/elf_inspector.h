#ifndef ELF_INSPECTOR_H
#define ELF_INSPECTOR_H

#include <elf.h>

typedef struct {
    char *origin;
    Elf64_Ehdr *header;
    Elf64_Shdr *section_table;
    Elf64_Half symtab_shndx;
} ElfFile;

ElfFile create_elf_file(char *mmapped_file);
void print_elf_header(ElfFile elf_file);
void print_section_table(ElfFile elf_file);
void print_section_header(ElfFile elf_file, int shndx);
void print_section_strtab(ElfFile elf_file, int shndx);
void print_section(ElfFile elf_file, int shndx);
void print_sections(ElfFile elf_file);
void print_section_symtab(ElfFile elf_file, Elf64_Half shndx);




// const int screen_width = 107;
// const int ehdr_offset_count = 23;

// void print_section(
//     Elf64_Shdr *section_table,
//     Elf64_Ehdr *elf_header,
//     char *elf_origin
// );





// void print_elf_header_bytes(Elf64_Ehdr *elf_header);

#endif /* elf_inspector.h */