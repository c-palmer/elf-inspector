/*
 *  Code from: https://atakua.org/old-wp/wp-content/uploads/2015/03/libelf-by-example-20100112.pdf
 */

#include <err.h>
#include <fcntl.h>

#include <libelf.h> // 1
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>
#include <unistd.h>

int main(int argc, char **argv) {
    int fd;

    Elf *e; // 2
    char *k;

    Elf_Kind ek; // 3

    if (argc != 2) {
        errx(EX_USAGE, "usage: %s file-name", argv[0]);
    }

    if (elf_version(EV_CURRENT) == EV_NONE) { // 4
        errx(EX_SOFTWARE, "ELF library initialization failed: %s", elf_errmsg(-1));
    }

    if ((fd = open(argv[1], O_RDONLY, 0)) < 0) {
        err(EX_NOINPUT, "open \"%s\" failed", argv[1]);
    }

    //                     5
    if ((e = elf_begin(fd, ELF_C_READ, NULL)) == NULL) {
        errx(EX_SOFTWARE, "elf_begin() failed: %s.", elf_errmsg(-1)); // 6
    }

    ek = elf_kind(e); // 7

    switch (ek) {
        case ELF_K_AR:
            k = "ar(1) archive";
            break;
        
        case ELF_K_ELF:
            k = "elf object";
            break;
        
        case ELF_K_NONE:
            k = "data";
            break;
        
        default:
            k = "unrecognized";
    }

    (void) printf("%s: %s\n", argv[1], k);

    (void) elf_end(e); // 8
    (void) close(fd);

    exit(EX_OK);
}