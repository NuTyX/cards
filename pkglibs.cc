#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "elf_common.h"
#include "elf_internal.h"

int error ( const char* message);

static unsigned long dynamic_addr;
static unsigned long dynamic_size_32bits;
static unsigned long long dynamic_size_64bits;
static unsigned int dynamic_nent;

static char * dynamic_strings;
static unsigned long dynamic_strings_length;
static char * string_table;
static unsigned long string_table_length;

static Elf_Ehdr_Begin * buffer;
static Elf_Ehdr_End * buffer_end;

static Elf_Ehdr_Begin elf_header_begin;
static Elf_Ehdr_End	elf_header_end;
static Elf_Ehdr_32Bit elf_header_32bits;
static Elf_Ehdr_64Bit elf_header_64bits;

static Elf_Shdr_32 * section_headers_32bits = NULL;
static Elf_Phdr_32 * program_headers_32bits = NULL;
static Elf_Shdr_64 * section_headers_64bits = NULL;
static Elf_Phdr_64 * program_headers_64bits = NULL;
static Elf_Dyn_32 *  dynamics_section_32bits = NULL;
static Elf_Dyn_64 *  dynamics_section_64bits = NULL;

static FILE * file = NULL;

static size_t result;

static void * get_data ( void * var, FILE * file, long offset, size_t size, size_t nmemb)
{
	void * mvar;
	if (size == 0 || nmemb == 0)
		return NULL;
	if (fseek (file, offset, SEEK_SET))
	{
		printf("Cannot seek\n");
		return NULL;
	}
	mvar = var;
	if (mvar == NULL)
	{
	/* Check for overflow.  */
		if (nmemb < (~(size_t) 0 - 1) / size)
			/* + 1 so that we can '\0' terminate invalid string table sections.  */
			mvar = malloc (size * nmemb + 1);
		if (mvar == NULL)
		{
			printf("Out of memory\n");
			return NULL;
		}
		((char *) mvar)[size * nmemb] = '\0';
	}
	if (fread (mvar, size, nmemb, file) != nmemb)
	{
		printf("Unable to read in 0x%lx bytes", (unsigned long)(size * nmemb));
		return NULL;
	}
	return mvar;
}
int get_32bit_elf_header_part(FILE *file)
{
	Elf_Ehdr_32Bit * buffer_32bits_part;
	buffer_32bits_part = (Elf_Ehdr_32Bit*)malloc(sizeof(elf_header_32bits));
	result = fread (buffer_32bits_part,1,sizeof(elf_header_32bits),file);
	if (result != sizeof(elf_header_32bits))
		return error ("Reading error at 32bit_elf_header_part"); 
	elf_header_32bits=*buffer_32bits_part;
	return 0;
}
int get_64bit_elf_header_part(FILE *file)
{
	Elf_Ehdr_64Bit * buffer_64bits_part;
	buffer_64bits_part =(Elf_Ehdr_64Bit*)malloc(sizeof(elf_header_64bits));
	result = fread (buffer_64bits_part,1,sizeof(elf_header_64bits),file);
	if (result != sizeof(elf_header_64bits))
		return error("Reading error at 64bit_elf_header_part");
	elf_header_64bits=*buffer_64bits_part;
	return 0;
}
void end()
{
	if (file !=NULL)
		fclose(file);
	if (buffer != NULL)
		free(buffer);
	if (buffer_end != NULL)
		free(buffer_end);
  if (section_headers_32bits != NULL)
    free(section_headers_32bits);
  if (section_headers_64bits != NULL)
    free(section_headers_64bits);
  if (program_headers_32bits != NULL)
    free(program_headers_32bits);
  if (program_headers_64bits != NULL)
    free(program_headers_64bits);
	if (dynamics_section_32bits != NULL)
		free(dynamics_section_32bits);
	if (dynamics_section_64bits != NULL)
		free(dynamics_section_64bits);
}
int error ( const char* message)
{
	printf("%s\n");
	end();
	return 1;
}
int main(int argc, char *argv[])
{

	const char *file_name;
	struct stat statbuf;

	file_name = argv[1];

	if (stat (file_name, &statbuf) < 0)
		return error ("Cannot find file");

	file = fopen (file_name, "rb" );
	if (file == NULL)
		return error("Cannot open the file");

	/* Check Magic number */
	buffer = (Elf_Ehdr_Begin *)malloc( sizeof(elf_header_begin));

	// copy the file into the buffer:
	result = fread (buffer,1,sizeof(elf_header_begin),file);
	if (result != sizeof(elf_header_begin))
	{
		fputs ("Reading error",stderr); 
		exit (3);
	}

	elf_header_begin = *buffer;

	if (   elf_header_begin.e_ident[EI_MAG0] != ELFMAG0
		||  elf_header_begin.e_ident[EI_MAG1] != ELFMAG1
		||  elf_header_begin.e_ident[EI_MAG2] != ELFMAG2
		||  elf_header_begin.e_ident[EI_MAG3] != ELFMAG3)
	{
		error(" Not an ELF file \n");
	}
	/* Get the Arch specific part of the ELF Header */
	if ( elf_header_begin.e_ident[EI_CLASS] == ELFCLASS32 )
	{
		get_32bit_elf_header_part(file);
		
	}
	else
	{
		get_64bit_elf_header_part(file);
	}
	/* Get the end part of the ELF Header */
	buffer_end = (Elf_Ehdr_End *)malloc( sizeof(elf_header_end));
	result = fread (buffer_end,1,sizeof(elf_header_end),file);

	if (result != sizeof(elf_header_end))
	{
		fputs ("Reading error elf_header_end",stderr);
		exit (3);
	}

	elf_header_end = *buffer_end;	

	if (elf_header_end.e_shnum == 0)
		error ("There are no Sections in this file");

	/* Main check if it's a 32 bits or 64 bits elf files */
	if ( elf_header_begin.e_ident[EI_CLASS] == ELFCLASS32 )
	{
/* Start with the section header */
		unsigned int i;
		unsigned long offset;
		long str_tab_len;
		char * name;

		section_headers_32bits = (Elf_Shdr_32 *)get_data (NULL, file,
				elf_header_32bits.e_shoff,
				elf_header_end.e_shentsize,
				elf_header_end.e_shnum);

		if (!section_headers_32bits)
		  return error ("There are no Sections in this file\n");

		for (i = 0;i < elf_header_end.e_shnum;i++)
		{
/* Only the dynamic section we are concern */
			if ( section_headers_32bits[i].sh_type == SHT_DYNAMIC )
				printf("[%d] sh_name: %u sh_type: %u sh_addr: %x\n",i,
					section_headers_32bits[i].sh_name,
					section_headers_32bits[i].sh_type,
					section_headers_32bits[i].sh_addr);
		}
/* Program header */
		program_headers_32bits  = (Elf_Phdr_32 *) get_data(NULL, file,
			elf_header_32bits.e_phoff,
			elf_header_end.e_phentsize,
			elf_header_end.e_phnum);

		if (!program_headers_32bits)
			return error("Out of mem\n");

		for (i = 0;i < elf_header_end.e_phnum;i++)
		{
/* Only the dynamic Programm header we are concern */
			if ( program_headers_32bits[i].p_type == PT_DYNAMIC)
			{
				/* save the offset and the size of the dynamic info */
				dynamic_addr = program_headers_32bits[i].p_offset;
				dynamic_size_32bits = program_headers_32bits[i].p_filesz;
			}
		}
/* Only the dynamic section we are concern for Dynamic libraries */
		Elf_Dyn_32 * edyn;
		dynamics_section_32bits = (Elf_Dyn_32 *) get_data ( NULL,file,dynamic_addr,
			1,dynamic_size_32bits);

		if (!dynamics_section_32bits)
			error("Out of mem");
		/* first pass to catch the index offset of the strings */
		for (edyn = dynamics_section_32bits , i = 0;
				edyn < dynamics_section_32bits + dynamic_size_32bits;
				edyn++,i++)
		{
			if (edyn->d_tag == DT_STRTAB)
				offset = edyn->d_un.d_val; /* store the offset of the dynamic libraries */
			if (edyn->d_tag == DT_NULL)
				break;
		}

		if (fseek (file, 0, SEEK_END))
			return error ("Error to seek to end of the file");

		str_tab_len = ftell (file) - offset;

		if (str_tab_len < 1)
			return error("Impossible to determine the size of the dynamic table");

		dynamic_strings = (char *) get_data (NULL, file, offset, 1,
				str_tab_len);
		dynamic_strings_length = dynamic_strings == NULL ? 0 : str_tab_len;

		for (edyn = dynamics_section_32bits;
				edyn < dynamics_section_32bits + dynamic_size_32bits;
				edyn ++)
		{
			if (edyn->d_tag == DT_NEEDED) /* if it's a lib */
			{
				if ( ( dynamic_strings != NULL ) && ( offset <  dynamic_strings_length) )
				{
					name = dynamic_strings + edyn->d_un.d_val;
					printf("%s ",name);
				}
			}
		}
	}
/* 64 bits
   If we are in case of a 64 bits elf file */ 
	else
	{
/* Start with the section header */
		unsigned int i;
		unsigned long offset;
		long str_tab_len;
		char * name;

		section_headers_64bits = (Elf_Shdr_64  *)get_data (NULL, file,
				elf_header_64bits.e_shoff,
				elf_header_end.e_shentsize,
				elf_header_end.e_shnum);

		if (!section_headers_64bits)
			return error ("There are no Sections in this file\n");

		for (i = 0;i < elf_header_end.e_shnum;i++)
		{
/* Only the dynamic section we are concern */
			if ( section_headers_64bits[i].sh_type == SHT_DYNAMIC )
				printf("[%d] sh_name: %u sh_type: %u sh_addr: %x\n",i,
					section_headers_64bits[i].sh_name,
					section_headers_64bits[i].sh_type,
					section_headers_64bits[i].sh_addr);
		}
/* Program header */
		program_headers_64bits = (Elf_Phdr_64 *) get_data(NULL, file,
			elf_header_64bits.e_phoff,
			elf_header_end.e_phentsize,
			elf_header_end.e_phnum);

		if (!program_headers_64bits)
			return error("Out of mem\n");

		for (i = 0;i < elf_header_end.e_phnum;i++)
		{
/* Only the dynamic Programm header we are concern */
			if ( program_headers_64bits[i].p_type == PT_DYNAMIC)
			{
				/* save the offset and the size of the dynamic info */ 
				dynamic_addr = program_headers_64bits[i].p_offset;
				dynamic_size_64bits = program_headers_64bits[i].p_filesz;
			}
		}	
/* Only the dynamic section we are concern for Dynamic libraries */
		Elf_Dyn_64  * edyn;
		dynamics_section_64bits = (Elf_Dyn_64 *) get_data ( NULL,file,dynamic_addr,
			 1,dynamic_size_64bits);

		if (!dynamics_section_64bits)
			return error("Out of mem\n");
		/* first pass to catch the index offset of the strings */
		for (edyn = dynamics_section_64bits , i = 0;
			edyn < dynamics_section_64bits + dynamic_size_64bits;
			edyn++,i++)
		{
			if (edyn->d_tag == DT_STRTAB) /* store the offset of the dynamic libraries */
				offset = edyn->d_un.d_val;
			 if (edyn->d_tag ==  DT_NEEDED)
					printf("%u %u \n ",edyn->d_tag,edyn->d_un.d_val);

			if (edyn->d_tag == DT_NULL)
				break;
		}

		if (fseek (file, 0, SEEK_END))
			return error ("Error to seek to end of the file");

		str_tab_len = ftell (file) - offset;

		if (str_tab_len < 1)
			return error ("Impossible to determine the size of the dynamic table");

		dynamic_strings = (char *) get_data (NULL, file, offset, 1,
				str_tab_len);
		dynamic_strings_length = dynamic_strings == NULL ? 0 : str_tab_len;

		for (edyn = dynamics_section_64bits;
			edyn < dynamics_section_64bits + dynamic_size_64bits;
			edyn ++)
		{
			if (edyn->d_tag == DT_NEEDED) /* if it's a lib */
			{
				if ( ( dynamic_strings != NULL ) && ( offset <	dynamic_strings_length) )
				{
					name = dynamic_strings + edyn->d_un.d_val;
					printf("%s ",name);
				}
			}
		}
	}
printf("\n");
end();
return 0;
}
// vim:set ts=2 :
