#ifndef  _ELF_INTERNAL_H
#define _ELF_INTERNAL_H

/* ELF Header */
#define EI_NIDENT 16    /* Size of e_ident[] */

typedef struct elf_internal_ehdr_begin {
	unsigned char      e_ident[EI_NIDENT]; /* ELF "magic number" */
	unsigned short		 e_type;
	unsigned short     e_machine;
	unsigned int       e_version;
} Elf_Ehdr_Begin;
typedef struct elf_internal_ehdr_32b {
	unsigned int     e_entry;
  unsigned int     e_phoff;
	unsigned int     e_shoff;
} Elf_Ehdr_32Bit;
typedef struct elf_internal_ehdr_64b {
   unsigned long long    e_entry;
   unsigned long long    e_phoff;
   unsigned long long    e_shoff;
} Elf_Ehdr_64Bit;

typedef struct elf_internal_ehdr_end {
	unsigned int       e_flags;
	unsigned short     e_ehsize;
	unsigned short     e_phentsize;
	unsigned short     e_phnum;
	unsigned short     e_shentsize;
	unsigned short     e_shnum;
	unsigned short     e_shstrndx;
} Elf_Ehdr_End;



/* dynamic section structure */
typedef struct elf_internal_dyn_32 {
	unsigned long d_tag;
	union { 
		unsigned long d_val;
		unsigned long d_ptr;
	 } d_un;
} Elf_Dyn_32;

typedef struct elf_internal_dyn_64 {
  unsigned long d_tag;
  union {
    unsigned long   d_val;
    unsigned long   d_ptr;
   } d_un;
} Elf_Dyn_64;

/* Section header */

typedef struct elf_internal_shdr_32 {
	unsigned int  sh_name;
	unsigned int  sh_type;
	unsigned long sh_flags;
	unsigned long sh_addr;
	unsigned long sh_offset;
	unsigned long sh_size;
	unsigned int  sh_link;
	unsigned int  sh_info;
	unsigned long sh_addralign;
	unsigned long sh_entsize;
} Elf_Shdr_32;

typedef struct elf_internal_shdr_64 {
	unsigned int   sh_name;
	unsigned int   sh_type;
	unsigned long  sh_flags;
	unsigned long  sh_addr;
	unsigned long  sh_offset;
	unsigned long  sh_size;
	unsigned int   sh_link;
	unsigned int   sh_info;
	unsigned long  sh_addralign;
	unsigned long  sh_entsize;
} Elf_Shdr_64;

/* Program header */

typedef struct elf_internal_phdr_32 {
	unsigned int p_type;
	unsigned long p_offset;
	unsigned long p_vaddr;
	unsigned long p_paddr;
	unsigned long p_filesz;
	unsigned long p_memsz;
	unsigned int p_flags;
	unsigned int p_align;
} Elf_Phdr_32;

typedef struct elf_internal_phdr_64 {
	unsigned int p_type;
	unsigned int p_flags;
	unsigned long p_offset;
	unsigned long p_vaddr;
	unsigned long p_paddr;
	unsigned long p_filesz;
	unsigned long p_memsz;
	unsigned long p_align;
} Elf_Phdr_64;
	
#endif /* _ELF_INTERNAL_H */

// vim:set ts=2 :
