//  elf.h
//
//  Copyright (c) 2013 - 2020 by NuTyX team (http://nutyx.org)
// 
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, 
//  USA.
//

#ifndef ELF_H
#define ELF_H
#if __x86_64__ || __ppc64__
  #define ENV64BIT
#else
 #define ENV32BIT
#endif

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
   long    e_entry;
   long    e_phoff;
   long    e_shoff;
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

#endif /* ELF_H */

// vim:set ts=2 :
