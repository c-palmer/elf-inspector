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

    munmap(mmap_addr, file_info.st_size);
    close(fd);

    return 0;
}