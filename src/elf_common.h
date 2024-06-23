/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

/**
 *  Based on the code of
	Fred Fish @ Cygnus Support, from information published
	in "UNIX System V Release 4, Programmers Guide: ANSI C and
	Programming Support Tools".
	*
	**/

/* Magic number signature */
#define EI_MAG0      0  /* File identification byte 0 index */
#define ELFMAG0         0x7F  /* Magic number byte 0 */
#define EI_MAG1      1  /* File identification byte 1 index */
#define ELFMAG1          'E'  /* Magic number byte 1 */
#define EI_MAG2      2  /* File identification byte 2 index */
#define ELFMAG2          'L'  /* Magic number byte 2 */
#define EI_MAG3      3  /* File identification byte 3 index */
#define ELFMAG3          'F'  /* Magic number byte 3 */

#define EI_CLASS  4  /* File class */
#define ELFCLASSNONE       0  /* Invalid class */
#define ELFCLASS32         1  /* 32-bit objects */
#define ELFCLASS64         2  /* 64-bit objects */

/* Operating System/ABI indication */
#define ELFOSABI_NONE         0  /* UNIX System V ABI */
#define ELFOSABI_GNU       3  /* GNU */

/* Values for e_type, which identifies the object file type.  */

#define ET_NONE      0  /* No file type */
#define ET_REL    1  /* Relocatable file */
#define ET_EXEC      2  /* Executable file */
#define ET_DYN    3  /* Shared object file */
#define ET_CORE      4  /* Core file */
#define ET_LOOS      0xFE00   /* Operating system-specific */
#define ET_HIOS      0xFEFF   /* Operating system-specific */
#define ET_LOPROC 0xFF00   /* Processor-specific */
#define ET_HIPROC 0xFFFF   /* Processor-specific */

/* Values for e_version.  */

#define EV_NONE      0     /* Invalid ELF version */
#define EV_CURRENT   1     /* Current version */

/* Values for program header, p_type field.  */

#define PT_NULL      0     /* Program header table entry unused */
#define PT_LOAD      1     /* Loadable program segment */
#define PT_DYNAMIC   2     /* Dynamic linking information */
#define PT_INTERP 3     /* Program interpreter */
#define PT_NOTE      4     /* Auxiliary information */
#define PT_SHLIB  5     /* Reserved, unspecified semantics */
#define PT_PHDR      6     /* Entry for header table itself */
#define PT_TLS    7     /* Thread local storage segment */
#define PT_LOOS      0x60000000  /* OS-specific */
#define PT_HIOS      0x6fffffff  /* OS-specific */
#define PT_LOPROC 0x70000000  /* Processor-specific */
#define PT_HIPROC 0x7FFFFFFF  /* Processor-specific */

#define PT_GNU_EH_FRAME (PT_LOOS + 0x474e550) /* Frame unwind information */
#define PT_SUNW_EH_FRAME PT_GNU_EH_FRAME      /* Solaris uses the same value */
#define PT_GNU_STACK (PT_LOOS + 0x474e551) /* Stack flags */
#define PT_GNU_RELRO (PT_LOOS + 0x474e552) /* Read-only after relocation */

/* Values for section header, sh_type field.  */

#define SHT_NULL        0               /* Section header table entry unused */
#define SHT_PROGBITS    1               /* Program specific (private) data */
#define SHT_SYMTAB      2               /* Link editing symbol table */
#define SHT_STRTAB      3               /* A string table */
#define SHT_RELA        4               /* Relocation entries with addends */
#define SHT_HASH        5               /* A symbol hash table */
#define SHT_DYNAMIC     6               /* Information for dynamic linking */
#define SHT_NOTE        7               /* Information that marks file */
#define SHT_NOBITS      8               /* Section occupies no space in file */
#define SHT_REL         9               /* Relocation entries, no addends */
#define SHT_SHLIB       10              /* Reserved, unspecified semantics */
#define SHT_DYNSYM      11              /* Dynamic linking symbol table */

#define SHT_INIT_ARRAY    14            /* Array of ptrs to init functions */
#define SHT_FINI_ARRAY    15            /* Array of ptrs to finish functions */
#define SHT_PREINIT_ARRAY 16            /* Array of ptrs to pre-init funcs */
#define SHT_GROUP         17            /* Section contains a section group */
#define SHT_SYMTAB_SHNDX  18            /* Indicies for SHN_XINDEX entries */

#define SHT_LOOS        0x60000000      /* First of OS specific semantics */
#define SHT_HIOS        0x6fffffff      /* Last of OS specific semantics */

#define SHT_GNU_INCREMENTAL_INPUTS 0x6fff4700   /* incremental build data */
#define SHT_GNU_ATTRIBUTES 0x6ffffff5   /* Object attributes */
#define SHT_GNU_HASH    0x6ffffff6      /* GNU style symbol hash table */
#define SHT_GNU_LIBLIST 0x6ffffff7      /* List of prelink dependencies */



/* Dynamic section tags.  */

#define DT_NULL         0
#define DT_NEEDED       1
#define DT_PLTRELSZ     2
#define DT_PLTGOT       3
#define DT_HASH         4
#define DT_STRTAB       5
#define DT_SYMTAB       6
#define DT_RELA         7
#define DT_RELASZ       8
#define DT_RELAENT      9
#define DT_STRSZ        10
#define DT_SYMENT       11
#define DT_INIT         12
#define DT_FINI         13
#define DT_SONAME       14
#define DT_RPATH        15
#define DT_SYMBOLIC     16
#define DT_REL          17
#define DT_RELSZ        18
#define DT_RELENT       19
#define DT_PLTREL       20
#define DT_DEBUG        21
#define DT_TEXTREL      22
#define DT_JMPREL       23
#define DT_BIND_NOW     24
#define DT_INIT_ARRAY   25
#define DT_FINI_ARRAY   26
#define DT_INIT_ARRAYSZ 27
#define DT_FINI_ARRAYSZ 28
#define DT_RUNPATH      29
#define DT_FLAGS        30
#define DT_ENCODING     32
#define DT_PREINIT_ARRAY   32
#define DT_PREINIT_ARRAYSZ 33

