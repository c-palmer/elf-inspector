#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>

#include "elf_inspector.h"

#define FLAGS_STR_LEN 14

/*
 *  prints the following template:
 *  "0xXXXXXXXX (X bytes) <struct.field>: <field value according to conv_spec>"
 *  
 *  - struct:    a struct
 *  - field:     a field of the struct
 *  - offset:    an int added to the offsetof value when printing address
 *  - conv_spec: a string containing a conversion specifier to print the field
 */
#define PRINT_FIELD(struct, field, offset, conv_spec) do {        \
    printf(                                                       \
        "0x%08lX (%lu bytes) %s: ",                               \
        offset + offsetof(typeof((struct)), field),               \
        sizeof((struct).field),                                   \
        #struct "." #field                                        \
    );                                                            \
                                                                  \
    printf(conv_spec, (struct).field);                            \
} while (0)

void populate_section_header_flags(Elf64_Xword bits, char *buf);
char *get_section_name(ElfFile elf_file, int shndx);
char *get_str_from_section(ElfFile elf_file, int shndx, int offset);
const char *e_type_str(Elf64_Half e_type);
const char *shn_str(ElfFile elf_file, Elf64_Half shn);
const char *sh_type_str(int sh_type);
const char *st_bind_str(int st_bind);
const char *st_type_str(int st_bind);
const char *st_visibility_str(int st_visibility);
bool is_valid_shndx(ElfFile elf_file, Elf64_Half shndx);

bool is_valid_shndx(ElfFile elf_file, Elf64_Half shndx) {
    return shndx != SHN_UNDEF && shndx < elf_file.header->e_shnum;
}

const char *st_visibility_str(int st_visibility) {
    switch (st_visibility) {
        case STV_DEFAULT: return "STV_DEFAULT";
        case STV_INTERNAL: return "STV_INTERNAL";
        case STV_HIDDEN: return "STV_HIDDEN";
        case STV_PROTECTED: return "STV_PROTECTED";
        case 4: return "STV_EXPORTED";
        case 5: return "STV_SINGLETON";
        case 6: return "STV_ELIMINATE";
    }

    return "ERROR unrecognized";
}

const char *st_type_str(int st_bind) {
    switch (st_bind) {
        case STT_NOTYPE: return "STT_NOTYPE";
        case STT_OBJECT: return "STT_OBJECT";
        case STT_FUNC: return "STT_FUNC";
        case STT_SECTION: return "STT_SECTION";
        case STT_FILE: return "STT_FILE";
        case STT_COMMON: return "STT_COMMON";
        case STT_TLS: return "STT_TLS";
    }

    if (STT_LOOS <= st_bind && st_bind <= STT_HIOS) return "os specific";
    if (STT_LOPROC <= st_bind && st_bind <= STT_HIPROC) return "processor specific";

    return "ERROR unrecognized";
}

const char *st_bind_str(int st_bind) {
    switch (st_bind) {
        case STB_LOCAL: return "STB_LOCAL";
        case STB_GLOBAL: return "STB_GLOBAL";
        case STB_WEAK: return "STB_WEAK";
    }

    if (STB_LOOS <= st_bind && st_bind <= STB_HIOS) return "os specific";
    if (STB_LOPROC <= st_bind && st_bind <= STB_HIPROC) return "processor specific";

    return "ERROR unrecognized";
}

const char *sh_type_str(int sh_type) {
    switch (sh_type) {
        case SHT_NULL: return "SHT_NULL";
        case SHT_PROGBITS: return "SHT_PROGBITS";
        case SHT_SYMTAB: return "SHT_SYMTAB";
        case SHT_STRTAB: return "SHT_STRTAB";
        case SHT_RELA: return "SHT_RELA";
        case SHT_HASH: return "SHT_HASH";
        case SHT_DYNAMIC: return "SHT_DYNAMIC";
        case SHT_NOTE: return "SHT_NOTE";
        case SHT_NOBITS: return "SHT_NOBITS";
        case SHT_REL: return "SHT_REL";
        case SHT_SHLIB: return "SHT_SHLIB";
        case SHT_DYNSYM: return "SHT_DYNSYM";
        case SHT_INIT_ARRAY: return "SHT_INIT_ARRAY";
        case SHT_FINI_ARRAY: return "SHT_FINI_ARRAY";
        case SHT_PREINIT_ARRAY: return "SHT_PREINIT_ARRAY";
        case SHT_GROUP: return "SHT_GROUP";
        case SHT_SYMTAB_SHNDX: return "SHT_SYMTAB_SHNDX";
        case SHT_RELR: return "SHT_RELR";
    }

    if (SHT_LOOS <= sh_type && sh_type <= SHT_HIOS) return "os specific";

    if (SHT_LOPROC <= sh_type && sh_type <= SHT_HIPROC)
        return "processor specific";

    if (SHT_LOUSER <= sh_type && sh_type <= SHT_LOUSER)
        return "application specific";

    return "ERROR unrecognized";
}

const char *shn_str(ElfFile elf_file, Elf64_Half shn) {
    if (shn == SHN_UNDEF) return "SHN_UNDEF";

    if (is_valid_shndx(elf_file, shn)) return "normal index";

    if (SHN_LOPROC <= shn && shn <= SHN_HIPROC) return "processor specific";

    if (SHN_LOOS <= shn && shn <= SHN_HIOS) return "os specific";

    switch (shn) {
        case SHN_ABS: return "SHN_ABS";
        case SHN_COMMON: return "SHN_COMMON";
        case SHN_XINDEX: return "SHN_XINDEX";
    }

    return "ERROR unrecognized";
}

const char *e_type_str(Elf64_Half e_type) {
    switch (e_type) {
        case ET_NONE: return "ET_NONE";
        case ET_REL: return "ET_REL";
        case ET_EXEC: return "ET_EXEC";
        case ET_DYN: return "ET_DYN";
        case ET_CORE: return "ET_CORE";
    };

    if (ET_LOOS <= e_type && e_type <= ET_HIOS) return "OS specific";

    if (ET_LOPROC <= e_type && e_type <= ET_HIPROC) return "processor specific";

    return "ERROR unrecognized";
}

char *get_str_from_section(ElfFile elf_file, int shndx, int offset) {
    if (elf_file.section_table[shndx].sh_type != SHT_STRTAB) return NULL;

    return elf_file.origin
         + elf_file.section_table[shndx].sh_offset
         + offset;
}

char *get_section_name(ElfFile elf_file, int shndx) {
    if (shndx >= elf_file.header->e_shnum) return NULL;

    return get_str_from_section(
        elf_file,
        elf_file.header->e_shstrndx,
        elf_file.section_table[shndx].sh_name
    );
}

void populate_section_header_flags(Elf64_Xword bits, char *buf) {
    typedef struct {
        char flag;
        Elf64_Xword mask;
    } FlagAndMask;

    FlagAndMask data[] = {
        { 'W', 0x1 },        // SHF_WRITE
        { 'A', 0x2 },        // SHF_ALLOC
        { 'X', 0x4 },        // SHF_EXECINSTR
        { 'M', 0x10 },       // SHF_MERGE
        { 'S', 0x20 },       // SHF_STRINGS
        { 'I', 0x40 },       // SHF_INFO_LINK
        { 'L', 0x80 },       // SHF_LINK_ORDER
        { 'O', 0x100 },      // SHF_OS_NONCONFORMING
        { 'G', 0x200 },      // SHF_GROUP
        { 'T', 0x400 },      // SHF_TLS
        { 'C', 0x800 },      // SHF_COMPRESSED
        { 'o', 0x0FF00000 }, // SHF_MASKOS
        { 'p', 0xF0000000 }  // SHF_MASKPROC
    };

    int buf_i = 0;
    for (int i = 0; i < sizeof(data) / sizeof(data[0]); i++) {
        if (data[i].mask & bits) {
            buf[buf_i++] = data[i].flag;
        }
    }
}

void print_section_symtab(ElfFile elf_file, Elf64_Half shndx) {
    if (elf_file.section_table[shndx].sh_type != SHT_SYMTAB) {
        printf("section header %d is not a symtab\n", shndx);
        return;
    }

    Elf64_Shdr symbol_header = elf_file.section_table[shndx];
    Elf64_Sym *symbol_table =
        (Elf64_Sym *) (elf_file.origin + symbol_header.sh_offset);
    int symbol_count = symbol_header.sh_size / symbol_header.sh_entsize;
    int strtab_ndx = symbol_header.sh_link;


    printf(
        "0x%08lX [START section %d (%s)]\n",
        symbol_header.sh_offset,
        shndx,
        get_section_name(elf_file, shndx)
    );

    for (int i = 0; i < symbol_count; i++) {
        Elf64_Sym symbol = symbol_table[i];
        Elf64_Off section_offset = symbol_header.sh_offset + i * sizeof(Elf64_Sym);

        printf("0x%08lX [START symbol %d]\n", section_offset, i);

        PRINT_FIELD(symbol, st_name, section_offset, "%d");
        printf(
            " (%s)",
            get_str_from_section(elf_file, strtab_ndx, symbol.st_name)
        );
        if (symbol.st_name == 0 && ELF64_ST_TYPE(symbol.st_info) == STT_SECTION) {
            printf(
                " (section %s (type is STT_SECTION))",
                get_section_name(elf_file, symbol.st_shndx)
            );
        }
        printf("\n");

        PRINT_FIELD(symbol, st_info, section_offset, "0x%X");
        int st_bind = ELF64_ST_BIND(symbol.st_info);
        int st_type = ELF64_ST_TYPE(symbol.st_info);
        printf(" (binding: %d (%s))", st_bind, st_bind_str(st_bind));
        printf(" (type: %d (%s))\n", st_type, st_type_str(st_type));

        PRINT_FIELD(symbol, st_other, section_offset, "0x%X");
        int st_visibility = ELF64_ST_VISIBILITY(symbol.st_other);
        printf(
            " (visibility: %d (%s))\n",
            st_visibility,
            st_visibility_str(st_visibility)
        );

        PRINT_FIELD(symbol, st_shndx, section_offset, "%d");
        printf(" (%s)", shn_str(elf_file, symbol.st_shndx));
        if (is_valid_shndx(elf_file, symbol.st_shndx)) {
            printf(" (%s)", get_section_name(elf_file, symbol.st_shndx));
        }
        printf("\n");

        const char *conv_spec = "0x%lX vaddress\n";

        if (elf_file.header->e_type == ET_REL) {
            switch (symbol.st_shndx) {
                case SHN_ABS:
                    conv_spec = "%ld absolute value (ET_REL && SHN_ABS)\n";
                    break;

                case SHN_COMMON:
                    conv_spec =  "%ld alignment (ET_REL && SHN_COMMON)\n";
                    break;

                default:
                    conv_spec = "%ld offset (ET_REL && normal index)\n";
                    break;
            }
        }

        PRINT_FIELD(symbol, st_value, section_offset, conv_spec);

        PRINT_FIELD(symbol, st_size, section_offset, "%lu bytes\n");

        printf(
            "0x%08lX [END symbol %d]\n\n",
            section_offset + sizeof(Elf64_Sym),
            i
        );
    }

    printf(
        "0x%08lX [END section %d (%s)]\n\n",
        symbol_header.sh_offset + symbol_count * sizeof(Elf64_Sym),
        shndx,
        get_section_name(elf_file, shndx)
    );
}

void print_sections(ElfFile elf_file) {
    Elf64_Half shnum = elf_file.header->e_shnum;

    printf("0x%08lX [START sections]\n", elf_file.section_table[1].sh_offset);

    for (int i = 0; i < shnum; i++) {
        print_section(elf_file, i);
    }

    printf(
        "0x%08lX [END sections]\n\n",
        elf_file.section_table[shnum - 1].sh_offset +
        elf_file.section_table[shnum - 1].sh_size
    );
}

void print_section(ElfFile elf_file, int shndx) {
    if (shndx == SHT_NULL) return;

    switch (elf_file.section_table[shndx].sh_type) {
        case SHT_PROGBITS:
            break;

        case SHT_SYMTAB:
            print_section_symtab(elf_file, shndx);
            return;

        case SHT_STRTAB:
            print_section_strtab(elf_file, shndx);
            return;

        case SHT_RELA:
        case SHT_HASH:
        case SHT_DYNAMIC:
        case SHT_NOTE:
        case SHT_NOBITS:
        case SHT_REL:
        case SHT_SHLIB:
        case SHT_DYNSYM:
        case SHT_INIT_ARRAY:
        case SHT_FINI_ARRAY:
        case SHT_PREINIT_ARRAY:
        case SHT_GROUP:
        case SHT_SYMTAB_SHNDX:
        case SHT_RELR:
            break;
    }
    
    printf(
        "not implemented: %s (%d)\n\n",
        get_section_name(elf_file, shndx), shndx
    );
}

void print_section_strtab(ElfFile elf_file, int shndx) {
    if (elf_file.section_table[shndx].sh_type != SHT_STRTAB) {
        printf("section header %d is not a strtab\n", shndx);
        return;
    }

    Elf64_Off sh_offset = elf_file.section_table[shndx].sh_offset;
    Elf64_Xword sh_size = elf_file.section_table[shndx].sh_size;
    const char *section_name = get_section_name(elf_file, shndx);
    const char *chars = elf_file.origin + sh_offset;
    const char *cur_str = chars;

    printf("0x%08lX [START section %d (%s)]\n", sh_offset, shndx, section_name);
    for (int i = 0; i < sh_size; i++) {
        if (chars[i] != '\0') continue;

        printf(
            "0x%08lX (%ld bytes) offset %ld: \"%s\\0\"\n",
            cur_str - elf_file.origin,
            chars + i + 1 - cur_str,
            cur_str - chars,
            cur_str
        );

        if (i < sh_size - 1) cur_str = chars + i + 1;
    }
    printf(
        "0x%08lX [END section %d (%s)]\n\n",
        sh_offset + sh_size,
        shndx,
        section_name
    );
}

void print_section_header(ElfFile elf_file, int shndx) {
    Elf64_Shdr header = elf_file.section_table[shndx];
    Elf64_Off header_offset = shndx * sizeof(Elf64_Shdr) + elf_file.header->e_shoff;

    printf("0x%08lX [START section header %d]\n", header_offset, shndx);

    PRINT_FIELD(header, sh_name, header_offset, "%d");
    printf(" (%s)\n", get_section_name(elf_file, shndx));

    PRINT_FIELD(header, sh_type, header_offset, "%d");
    printf(" (%s)\n", sh_type_str(elf_file.section_table[shndx].sh_type));

    PRINT_FIELD(header, sh_flags, header_offset, "0x%016lX");
    char flags[FLAGS_STR_LEN] = {0};
    populate_section_header_flags(header.sh_flags, (char *) &flags);
    if (flags[0] != '\0') printf(" (%s)", flags);
    printf("\n");

    PRINT_FIELD(header, sh_addr, header_offset, "0x%lX\n");

    PRINT_FIELD(header, sh_offset, header_offset, "0x%lX");
    printf(" (ends 0x%lX)\n", header.sh_offset + header.sh_size);

    PRINT_FIELD(header, sh_size, header_offset, "%lu bytes\n");

    const char *sh_link_desc = NULL;
    const char *sh_info_desc = NULL;
    // I wanted to use SHF_INFO_LINK but either it wasn't set correctly by the
    // assembler or I'm misunderstanding how it works
    bool sh_info_is_shndx = true;

    switch (header.sh_type) {
        case SHT_DYNAMIC:
            sh_link_desc = "string table shndx";
            sh_info_desc = "should be 0";
            break;
        
        case SHT_HASH:
            sh_link_desc = "symbol table shndx";
            sh_info_desc = "should be 0";
            break;

        case SHT_REL:
        case SHT_RELA:
            sh_link_desc = "symbol table shndx";
            sh_info_desc = "shndx to which relocation applies";
            break;

        case SHT_SYMTAB:
        case SHT_DYNSYM:
            sh_link_desc = "string table shndx";
            sh_info_desc = "stndx of last local symbol +1";
            sh_info_is_shndx = false;
            break;

        case SHT_GROUP:
            sh_link_desc = "symbol table shndx";
            sh_info_desc = "stndx of signature name";
            break;

        case SHT_SYMTAB_SHNDX:
            sh_link_desc = "symbol table shndx";
            sh_info_desc = "should be 0";
            break;
    }

    PRINT_FIELD(header, sh_link, header_offset, "%u");
    printf(" (%s)", shn_str(elf_file, header.sh_link));
    if (is_valid_shndx(elf_file, header.sh_link)) {
        printf(" (%s)", get_section_name(elf_file, header.sh_link));
    }
    if (sh_link_desc) printf(" (%s)", sh_link_desc);
    printf("\n");

    PRINT_FIELD(header, sh_info, header_offset, "%u");
    if (sh_info_is_shndx) {
        printf(" (%s)", shn_str(elf_file, header.sh_link));

        if (is_valid_shndx(elf_file, header.sh_info)) {
            printf(" (%s)", get_section_name(elf_file, header.sh_info));
        }
    }
    if (sh_info_desc) printf(" (%s)", sh_info_desc);
    printf("\n");
    
    PRINT_FIELD(header, sh_addralign, header_offset, "%ld bytes\n");

    PRINT_FIELD(header, sh_entsize, header_offset, "%ld bytes\n");

    printf(
        "0x%08lX [END section header %d]\n\n",
        header_offset + sizeof(Elf64_Shdr),
        shndx
    );
}

void print_section_table(ElfFile elf_file) {
    printf("0x%08lX [START section header table]\n", elf_file.header->e_shoff);

    for (int i = 0; i < elf_file.header->e_shnum; i++) {
        print_section_header(elf_file, i);
    }

    printf(
        "0x%08lX [END section header table]\n\n",
        elf_file.header->e_shoff + elf_file.header->e_shnum * sizeof(Elf64_Shdr)
    );
}

void print_elf_header(ElfFile elf_file) {
    Elf64_Ehdr header = *elf_file.header;

    printf("0x%08X [START elf header]\n", 0);

    PRINT_FIELD(header, e_ident[EI_MAG0], 0, "0x%X\n");
    PRINT_FIELD(header, e_ident[EI_MAG1], 0, "'%c'\n");
    PRINT_FIELD(header, e_ident[EI_MAG2], 0, "'%c'\n");
    PRINT_FIELD(header, e_ident[EI_MAG3], 0, "'%c'\n");

    PRINT_FIELD(header, e_ident[EI_CLASS], 0, "%d");
    printf(" (%s)\n",
        (char *[]) {
            "ELFCLASSNONE",
            "ELFCLASS32",
            "ELFCLASS64"
        }[header.e_ident[EI_CLASS]]
    );

    PRINT_FIELD(header, e_ident[EI_DATA], 0, "%d");
    printf(" (%s)\n",
        (char *[]) {
            "ELFDATANONE",
            "ELFDATA2LSB (2's complement little endian)",
            "ELFDATA2MSB (2's complement big endian)"
        }[header.e_ident[EI_DATA]]
    );

    PRINT_FIELD(header, e_ident[EI_VERSION], 0, "%d");
    printf(" (should be %d (EV_CURRENT))\n", EV_CURRENT);

    PRINT_FIELD(header, e_ident[EI_OSABI], 0, "%d");
    if (header.e_ident[EI_OSABI] == ELFOSABI_NONE) printf(" (no extensions)");
    printf("\n");

    PRINT_FIELD(header, e_ident[EI_ABIVERSION], 0, "%d\n");

    PRINT_FIELD(header, e_ident[EI_PAD], 0, "%d");
    printf(
        " (%ld bytes of padding)\n",
        EI_NIDENT - offsetof(Elf64_Ehdr, e_ident[EI_PAD])
    );

    printf("0x%08X ... 0x%08X\n", 0xA, 0xF);

    PRINT_FIELD(header, e_type, 0, "%d");
    printf(" (%s)\n", e_type_str(header.e_type));

    PRINT_FIELD(header, e_machine, 0, "%d");
    printf(" (should be %d (EM_X86_64))\n", EM_X86_64);

    PRINT_FIELD(header, e_version, 0, "%d");
    printf(" (should be %d (EV_CURRENT))\n", EV_CURRENT);

    PRINT_FIELD(header, e_entry, 0, "0x%lX\n");
    PRINT_FIELD(header, e_phoff, 0, "0x%lX\n");
    PRINT_FIELD(header, e_shoff, 0, "0x%lX\n");
    PRINT_FIELD(header, e_flags, 0, "0x%08X\n");
    PRINT_FIELD(header, e_ehsize, 0, "%d bytes\n");
    PRINT_FIELD(header, e_phentsize, 0, "%d bytes\n");
    PRINT_FIELD(header, e_phnum, 0, "%d\n");
    PRINT_FIELD(header, e_shentsize, 0, "%d bytes\n");
    PRINT_FIELD(header, e_shnum, 0, "%d\n");

    PRINT_FIELD(header, e_shstrndx, 0, "%d");
    printf(" (%s)", shn_str(elf_file, header.e_shstrndx));
    if (is_valid_shndx(elf_file, header.e_shstrndx)) {
        printf(" (%s)", get_section_name(elf_file, header.e_shstrndx));
    }
    printf("\n");

    printf(
        "0x%08lX [END elf header]\n\n",
        sizeof(Elf64_Ehdr)
    );
}

ElfFile create_elf_file(char *mmapped_file) {
    ElfFile elf_file;

    elf_file.origin = mmapped_file;

    elf_file.header = (Elf64_Ehdr *) elf_file.origin;

    elf_file.section_table =
        (Elf64_Shdr *) (elf_file.origin + elf_file.header->e_shoff);
    
    for (int i = 0; i < elf_file.header->e_shnum; i++) {
        if (elf_file.section_table[i].sh_type != SHT_SYMTAB) continue;

        elf_file.symtab_shndx = i;
    }

    return elf_file;
}