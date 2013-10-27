#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "elf_common.h"
#include "elf.h"
#include "dependencies_utils.h"
#include "file_utils.h"

int error ( const char* message);
void dependencies_utils_end (void);

Elf_Ehdr_Begin * buffer = NULL;
Elf_Ehdr_32Bit * buffer_32bits_part = NULL;
Elf_Ehdr_64Bit * buffer_64bits_part = NULL;
Elf_Ehdr_End * buffer_end = NULL;

Elf_Ehdr_Begin elf_header_begin;
Elf_Ehdr_End	elf_header_end;
Elf_Ehdr_32Bit elf_header_32bits;
Elf_Ehdr_64Bit elf_header_64bits;

Elf_Shdr_32 * section_headers_32bits = NULL;
Elf_Phdr_32 * program_headers_32bits = NULL;
Elf_Shdr_64 * section_headers_64bits = NULL;
Elf_Phdr_64 * program_headers_64bits = NULL;
Elf_Dyn_32 *  dynamics_section_32bits = NULL;
Elf_Dyn_64 *  dynamics_section_64bits = NULL;

FILE * file = NULL;

int get_32bit_elf_header_part(FILE *file)
{
	size_t result;
	buffer_32bits_part = (Elf_Ehdr_32Bit*)malloc(sizeof(elf_header_32bits));
	result = fread (buffer_32bits_part,1,sizeof(elf_header_32bits),file);
	if (result != sizeof(elf_header_32bits))
		return error ("Reading error at 32bit_elf_header_part"); 
	elf_header_32bits=*buffer_32bits_part;
	return 0;
}
int get_64bit_elf_header_part(FILE *file)
{
	size_t result;
	buffer_64bits_part =(Elf_Ehdr_64Bit*)malloc(sizeof(elf_header_64bits));
	result = fread (buffer_64bits_part,1,sizeof(elf_header_64bits),file);
	if (result != sizeof(elf_header_64bits))
		return error("Reading error at 64bit_elf_header_part");
	elf_header_64bits=*buffer_64bits_part;
	return 0;
}
int getRuntimeLibrairiesList(set<string>& runtimeLibrairiesList, const string& fileName)
{
#ifndef NDEBUG
	printf("sizeof unsigned char: %d\n",sizeof(unsigned char));
	printf("sizeof unsigned short int: %d\n",sizeof(unsigned short int));
	printf("sizeof short int: %d\n",sizeof(short int));
	printf("sizeof int: %d\n",sizeof(int));
	printf("sizeof unsigned int: %d\n",sizeof(unsigned int));
	printf("sizeof int long: %d\n",sizeof(int long));
	printf("sizeof double: %d\n",sizeof(double));

#endif
	unsigned long dynamic_addr;
	unsigned long dynamic_size_32bits;
	unsigned long long dynamic_size_64bits;
	long dynamic_strings_length;
	size_t result;

	const	char * file_name = fileName.c_str();

	char * dynamic_strings = NULL;

	if ( ! checkFileExist(file_name) || checkFileEmpty(file_name) )
		return error ("Cannot find file");

	file = fopen (file_name, "rb" );
	if (file == NULL)
		return error("Cannot open the file");
#ifndef NDEBUG
 printf("File open: %s\n",file_name);
#endif


	/* Magic number */
	buffer = (Elf_Ehdr_Begin *)malloc( sizeof(elf_header_begin));
#ifndef NDEBUG
	printf("sizeof elf_header_begin :%d\n",sizeof(elf_header_begin));
#endif
	// copy the file into the buffer:
	result = fread (buffer,1,sizeof(elf_header_begin),file);
	if (result != sizeof(elf_header_begin))
		return error ("Reading error"); 
	elf_header_begin = *buffer;

  if (  elf_header_begin.e_ident[EI_MAG0] != ELFMAG0
    ||  elf_header_begin.e_ident[EI_MAG1] != ELFMAG1
    ||  elf_header_begin.e_ident[EI_MAG2] != ELFMAG2
    ||  elf_header_begin.e_ident[EI_MAG3] != ELFMAG3)
    return error("Not an ELF file");
#ifndef NDEBUG
  printf("Magic Number: %d %c %c %c\n",
  elf_header_begin.e_ident[EI_MAG0],
  elf_header_begin.e_ident[EI_MAG1],
  elf_header_begin.e_ident[EI_MAG2],
  elf_header_begin.e_ident[EI_MAG3]);
#endif

 if ( elf_header_begin.e_ident[EI_CLASS] == ELFCLASS32 )
/* 32bit elf file */
    get_32bit_elf_header_part(file);
  else
/* 64 bit elf file */
    get_64bit_elf_header_part(file);

/* Get the end part of the ELF Header */
	buffer_end = (Elf_Ehdr_End *)malloc( sizeof(elf_header_end));
	result = fread (buffer_end,1,sizeof(elf_header_end),file);
	if (result != sizeof(elf_header_end))
		return error("Reading error elf_header_end");

	elf_header_end = *buffer_end;

	if (elf_header_end.e_shnum == 0)
		return error ("There are no Sections in this file");
#ifndef NDEBUG
  printf("\nNumber of Section header: %d 0x%x\n",
	elf_header_end.e_shnum,elf_header_end.e_shnum);
	printf("Size of Section header entity : %d 0x%x\n",
	elf_header_end.e_shentsize,elf_header_end.e_shentsize);
	printf("Number of Program header: %d 0x%x\n",
	elf_header_end.e_phnum,elf_header_end.e_phnum);
	printf("Size of Program header entity: %d 0x%x\n\n",
	elf_header_end.e_phentsize,elf_header_end.e_phentsize);
#endif
/* Main check if it's a 32 bits or 64 bits elf files */
	if ( elf_header_begin.e_ident[EI_CLASS] == ELFCLASS32 )
	{
/* Start with the section header */
		long offset = 0;
		long str_tab_len = 0;
		char * name = NULL;

		section_headers_32bits = (Elf_Shdr_32 *)get_data (NULL, file,
				elf_header_32bits.e_shoff,
				elf_header_end.e_shentsize,
				elf_header_end.e_shnum);

		if (!section_headers_32bits)
		  return error ("There are no Sections in this file\n");

		for (int i = 0;i < elf_header_end.e_shnum;i++)
		{
/* Only the dynamic section we are concern */
			if ( section_headers_32bits[i].sh_type == SHT_STRTAB )
			{
					offset = section_headers_32bits[i].sh_offset;
#ifndef NDEBUG
		printf("section_headers_32bits[%d].sh_offset: %u 0x%x\n",
	i,section_headers_32bits[i].sh_offset,section_headers_32bits[i].sh_offset);
#endif
				
					if ( i ==  elf_header_end.e_shnum - 1 )
					{
						dependencies_utils_end();
#ifndef NDEBUG
	printf("No dynamics found\n");
		printf("offset : %u 0x%x\n",
	offset,offset);
#endif
						return 0;
					}
					break;
			}
		}
/* Program header */
		program_headers_32bits  = (Elf_Phdr_32 *) get_data(NULL, file,
			elf_header_32bits.e_phoff,
			elf_header_end.e_phentsize,
			elf_header_end.e_phnum);

		if (!program_headers_32bits)
			return error("Out of mem");

		for (unsigned int i = 0;i < elf_header_end.e_phnum;i++)
		{
/* Only the dynamic Programm header we are concern */
			if ( program_headers_32bits[i].p_type == PT_DYNAMIC)
			{
				/* save the offset and the size of the dynamic info */
				dynamic_addr = program_headers_32bits[i].p_offset;
				dynamic_size_32bits = program_headers_32bits[i].p_filesz;
#ifndef NDEBUG
		printf("program_headers_32bits[%d].p_offset: %u 0x%x\n",
	i,program_headers_32bits[i].p_offset,program_headers_32bits[i].p_offset);
		printf("program_headers_32bits[%d].p_filesz: %u 0x%x\n",
	i,program_headers_32bits[i].p_filesz,program_headers_32bits[i].p_filesz);
#endif
			}
		}
/* Only the dynamic section we are concern for Dynamic libraries */
		Elf_Dyn_32 * edyn;
		dynamics_section_32bits = (Elf_Dyn_32 *) get_data ( NULL,file,dynamic_addr,
			1,dynamic_size_32bits);

		if (!dynamics_section_32bits)
			error("Out of mem");

		if (fseek (file, 0, SEEK_END))
			return error ("Error to seek to end of the file");

#ifndef NDEBUG
		printf("File length: %u 0x%x\n",
	ftell (file),ftell (file));
#endif
		str_tab_len = ftell (file) - offset;
#ifndef NDEBUG
		printf("offset : %u 0x%x\n",
	offset,offset);
		printf("str_tab_len: %u 0x%x\n",
	str_tab_len,str_tab_len);
#endif
		if (str_tab_len < 1)
			return error("Impossible to determine the size of the dynamic table");

		dynamic_strings = (char *) get_data (NULL, file, offset, 1,
				str_tab_len);
		dynamic_strings_length = dynamic_strings == NULL ? 0 : str_tab_len;
		for (edyn = dynamics_section_32bits;
				edyn < dynamics_section_32bits + dynamic_size_32bits;
				edyn ++)
		{
			if (edyn->d_tag != DT_NEEDED)
				break;
			
			if (edyn->d_tag == DT_NEEDED) /* if it's a lib */
			{
				if ( ( dynamic_strings != NULL ) && ( offset <  dynamic_strings_length) )
				{
					name = dynamic_strings + edyn->d_un.d_val;
					if ( name != NULL )
						runtimeLibrairiesList.insert(name);
				}
			}
		}
		if (dynamics_section_32bits != NULL)
		{
			free(dynamics_section_32bits);
			dynamics_section_32bits=NULL;
		}
		if (dynamic_strings != NULL)
		{
			free(dynamic_strings);
			dynamic_strings=NULL;
		}
	}
/* 64 bits
   If we are in case of a 64 bits elf file */ 
	else
	{
/* Start with the section header */
		long offset = 0;
		long str_tab_len = 0;
		char * name = NULL;

		section_headers_64bits = (Elf_Shdr_64  *)get_data (NULL, file,
				elf_header_64bits.e_shoff,
				elf_header_end.e_shentsize,
				elf_header_end.e_shnum);

		if (!section_headers_64bits)
			return error ("There are no Sections in this file\n");

		for (int i = 0;i < elf_header_end.e_shnum;i++)
		{
/* Only the dynamic section we are concern */
			if ( section_headers_64bits[i].sh_type == SHT_STRTAB )
			{
				offset = section_headers_64bits[i].sh_offset;
#ifndef NDEBUG
		printf("section_headers_64bits[%d].sh_offset: %u 0x%x\n",
	i,section_headers_64bits[i].sh_offset,section_headers_64bits[i].sh_offset);
#endif
				if ( i ==  elf_header_end.e_shnum - 1 )
				{
					dependencies_utils_end();
#ifndef NDEBUG
	printf("No dynamics found\n");
		printf("offset : %u 0x%x\n",
	offset,offset);
#endif
					return 0;
				}
				break;
			}
		}
    if ( offset == elf_header_end.e_shnum - 1)
    {
      dependencies_utils_end();
#ifndef NDEBUG
  printf("No dynamics found\n");
    printf("offset : %u 0x%x\n",
  offset,offset);
#endif
      return 0;
    }
/* Program header */
		program_headers_64bits = (Elf_Phdr_64 *) get_data(NULL, file,
			elf_header_64bits.e_phoff,
			elf_header_end.e_phentsize,
			elf_header_end.e_phnum);

		if (!program_headers_64bits)
			return error("Out of mem");

		for (unsigned int i = 0;i < elf_header_end.e_phnum;i++)
		{
/* Only the dynamic Programm header we are concern */
			if ( program_headers_64bits[i].p_type == PT_DYNAMIC)
			{
				/* save the offset and the size of the dynamic info */ 
				dynamic_addr = program_headers_64bits[i].p_offset;
				dynamic_size_64bits = program_headers_64bits[i].p_filesz;
#ifndef NDEBUG
		printf("program_headers_64bits[%d].p_offset: %u 0x%x\n",
	i,program_headers_64bits[i].p_offset,program_headers_64bits[i].p_offset);
		printf("program_headers_64bits[%d].p_filesz: %u 0x%x\n",
	i,program_headers_64bits[i].p_filesz,program_headers_64bits[i].p_filesz);
#endif
			}
		}
/* Only the dynamic section we are concern for Dynamic libraries */
		Elf_Dyn_64  * edyn;
		dynamics_section_64bits = (Elf_Dyn_64 *) get_data ( NULL,file,dynamic_addr,
			 1,dynamic_size_64bits);

		if (!dynamics_section_64bits)
			return error("Out of mem");

		if (fseek (file, 0, SEEK_END))
			return error ("Error to seek to end of the file");
#ifndef NDEBUG
    printf("File length: %u 0x%x\n",
  ftell (file),ftell (file));
#endif
		str_tab_len = ftell (file) - offset;
#ifndef NDEBUG
		printf("offset : %u 0x%x\n",
	offset,offset);
		printf("str_tab_len: %u 0x%x\n",
	str_tab_len,str_tab_len);
#endif
		if (str_tab_len < 1)
			return error ("Impossible to determine the size of the dynamic table");
		dynamic_strings = (char *) get_data (NULL, file, offset, 1,
				str_tab_len);
		dynamic_strings_length = dynamic_strings == NULL ? 0 : str_tab_len;
		for (edyn = dynamics_section_64bits;
			edyn < dynamics_section_64bits + dynamic_size_64bits;
			edyn ++)
		{
			if (edyn->d_tag != DT_NEEDED)
				break;
			if (edyn->d_tag == DT_NEEDED) /* if it's a lib */
			{
				if ( ( dynamic_strings != NULL ) && ( offset <	dynamic_strings_length) )
				{
					name = dynamic_strings + edyn->d_un.d_val;
					runtimeLibrairiesList.insert(name);
				}
			}
		}
		if (dynamics_section_64bits != NULL)
		{
			free(dynamics_section_64bits);
			dynamics_section_64bits=NULL;
		}
		if (dynamic_strings != NULL)
		{
			free(dynamic_strings);
			dynamic_strings = NULL;
		}
	}
dependencies_utils_end();
return 0;
}

void dependencies_utils_end()
{
	if (file != NULL)
	{
		fclose(file);
		file=NULL;
	}
	if (buffer_32bits_part != NULL)
	{
		free(buffer_32bits_part);
		buffer_32bits_part=NULL;
	}

	if (buffer_64bits_part != NULL)
	{
		free(buffer_64bits_part);
		buffer_64bits_part=NULL;
	}
	if (buffer != NULL)
	{
		free(buffer);
		buffer=NULL;
	}
	if (buffer_end != NULL)
	{
		free(buffer_end);
		buffer_end=NULL;
	}
	if (section_headers_32bits != NULL)
	{
		free(section_headers_32bits);
		section_headers_32bits=NULL;
	}
	if (section_headers_64bits != NULL)
	{
		free(section_headers_64bits);
		section_headers_64bits=NULL;
	}
	if (program_headers_32bits != NULL)
	{
		free(program_headers_32bits);
		program_headers_32bits=NULL;
	}

  if (program_headers_64bits != NULL)
	{
    free(program_headers_64bits);
		program_headers_64bits=NULL;
	}
	if (dynamics_section_32bits != NULL)
	{
		free(dynamics_section_32bits);
		dynamics_section_32bits=NULL;
	}
  if (dynamics_section_64bits != NULL)
	{
    free(dynamics_section_64bits);
		dynamics_section_64bits=NULL;
	}
}
int error ( const char* message)
{
#ifndef NDEBUG
  fprintf(stderr,"%s\n",message);
#endif
  dependencies_utils_end();
#ifndef NDEBUG
  return 1;
#else
	return 0;
#endif
}
// vim:set ts=2 :
