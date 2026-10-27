# elf-inspector

I'm learning assembly and so I'm trying to learn more about how the assembly code I write eventually gets turned into a running program. My understanding is that an assembler basically uses the assembly code you write as instructions on how to create a relocatable elf file as its output (on Linux). The linker then converts many of these relocatable elf files (or just one) into a single executable elf file that the Linux kernel's loader can use to create a new process by mapping certain areas of content in the elf file directly into the new process's virtual address space, then setting the instruction pointer to the value of the entry point found in the elf header so that the cpu starts executing the new process.

(I think the loader also does some other stuff too like sets up command line arguments, environment variable key value pairs, the auxiliary vector, the stack and heap, the kernel section of the virtual memory space, etc. more on this [here](https://lwn.net/Articles/631631/).)

Before, I always thought of the assembler as simply turning assembly instructions into machine code through a very simple 1-1 translation process. My current understanding is that this is just one part of what the assembler does. Assembly code is more than just instructions. For example, in addition to the instructions, you usually also specify `.text` and `.data` sections, symbols and their visibility, data, etc.

All of these things I've mentioned (instructions, sections, symbols, and their visibility) are the components of elf files, which themselves are used as input for the linker. So right now I'm thinking of assembly as a kind of human readable text file version of an elf file that goes through a relatively simple transformation process done by the assembler into a binary elf file that can be understood by the linker, which in turn gets transformed into another binary elf file that can be understood by the kernel. So I felt really curious about what exactly those binary files contained, and hoped that understanding them would help me understand the assembly better (and eventually the assembler, the linker, and ultimately more about how processes work in general.)

`readelf` and `objdump` are great tools for this, but I was struggling at first to understand their outputs and felt like there was more in the elf file that they weren't showing me, so I started this project so I could examine every byte and interpret their meanings (I definitly need to get better at learning how to use existing tools instead of writing worse versions of them myself lol). But now having worked on this project, reading the output of readelf is no problem and everything seems so much simpler than it did before!

Reading the [elf specification](https://gabi.xinuos.com/index.html) has been incredibly helpful and I could not have understood the format without it. Instead of having to define all of the elf specific types and macros myself, there is a header called `elf.h` in glibc and everything is available in there (except I still need to implement functions to convert enum names to strings unfortunately).

This is my first "real" C project and I've already learned so much about C programming, assembly, assemblers, linkers, and most importantly elf files. Maybe I'm being naive but I'd like to eventually be able to write my own simple assembler after all of this. This project is kind of the first step into writing an assembler anyways, so maybe its not that out of the question. 

# Resources:
- https://gabi.xinuos.com/index.html
- https://www.airs.com/blog/archives/38
- https://www.toolchains.net/
- https://wiki.osdev.org/ELF
- https://wiki.osdev.org/ELF_Tutorial
- https://web.archive.org/web/20140130143820/http://www.robinhoksbergen.com/papers/howto_elf.html
- https://lwn.net/Articles/631631/
- https://www.nasm.us/
- https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html
- System V Application Binary Interface for AMD64 Architecture
- https://atakua.org/old-wp/wp-content/uploads/2015/03/libelf-by-example-20100112.pdf
- https://en.wikipedia.org/wiki/Executable_and_Linkable_Format
- https://en.wikipedia.org/wiki/DWARF
- https://github.com/aixoss/binutils/blob/master/binutils/readelf.c