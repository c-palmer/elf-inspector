# elf-inspector

I'm learning assembly and am trying to learn more about how the assembly code I write eventually gets turned into a running program. My understanding is that an assembler basically converts assembly code into a relocatable elf file (on Linux). The linker then converts this relocatable elf file into an executable elf file that the Linux kernel's loader can use to create a new process.

Before I always thought of the assembler as simply turning assembly instructions into machine code through a very simple 1-1 translation process. My current understanding is that this is just one part of what the assembler does. Assembly code is more than just instructions. For example, you usually specify `.text` and `.data` sections, symbols and their visibility, and data in addition to the instructions.

All of these things I've mentioned (instructions, sections, symbols, and their visibility) are the components of elf files, which themselves are used as input for the linker. So right now I'm thinking of assembly as a kind of human readable text file version of an elf file that goes through a relatively simple transformation done by the assembler into a binary file that can be understood by the linker, which in turn gets transformed into another binary file that can be understood by the kernel. So I felt really curious about what exactly those binary files contained, and hoped that understanding them would help me understand the assembly better.

`readelf` and `objdump` are great tools for this, but I was struggling to understand their outputs and felt like there was more in the elf file that I wasn't seeing, so I started this project so I could examine every byte and interpret their meanings (I definitly need to get better at learning how to use existing tools instead of writing worse versions of them myself lol). But now having worked on this project, reading the output of readelf is no problem and everything seems so much simpler than it did before!

Reading the [elf specification](https://gabi.xinuos.com/index.html) has been incredibly helpful and I could not have understood the format without it. Instead of having to define all of the elf specific types and macros myself, there is a header called `elf.h` in glibc and everything is available in there (except I still need to implement functions to convert enum names to strings unfortunately).

This is my first "real" C project and I've already learned so much about C programming, assembly, assemblers, linkers, and most importantly elf files. Maybe I'm being naive but I'd like to eventually be able to write my own simple assembler after all of this. This project is kind of a first step into writing an assembler anyways, so maybe its not that out of the question. 

# Resources:
- [https://gabi.xinuos.com/index.html]
- [https://www.airs.com/blog/archives/38]
- [https://www.toolchains.net/]
- [https://wiki.osdev.org/ELF]
- [https://wiki.osdev.org/ELF_Tutorial]
- [https://web.archive.org/web/20140130143820/http://www.robinhoksbergen.com/papers/howto_elf.html]
- [https://www.nasm.us/]
- [https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html]
- System V Application Binary Interface for AMD64 Architecture
- [https://atakua.org/old-wp/wp-content/uploads/2015/03/libelf-by-example-20100112.pdf]
- [https://en.wikipedia.org/wiki/Executable_and_Linkable_Format]
- [https://en.wikipedia.org/wiki/DWARF]
- [https://github.com/aixoss/binutils/blob/master/binutils/readelf.c]