//
// Created by zk on 2017/7/20.
//

#include "elfReade.h"
#define LOG_TAG "wodelog"
#define printf1(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))
#define ALOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))
#include <android/log.h>


bool ElfReade::readFile(const char *filename)
{

    struct stat sb;
    fd = open(filename, O_RDONLY);
    fstat(fd, &sb); /* 取得文件大小 */
    file_size = sb.st_size;
    start = (char*)mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if(start == MAP_FAILED) /* 判断是否映射成功 */
        return false;

    return true;
}

ElfReade::ElfReade(const char *filename)
{
    if( !readFile(filename))
    {
        printf("can not mmap the file %s\n", filename);
        exit(0);
    }

    memcpy(&elf_header, this->start, ELF_LENHT);
}

ElfReade::~ElfReade()
{
    munmap(start, file_size); /* 解除映射 */
    close(fd);

}


unsigned int ElfReade::getFileSize() const
{
    return this->file_size;
}


void ElfReade::printElfSymbol()
{

    //段表的总个数
    int SectionNumber = elf_header.e_shnum;
    //段表符号表的索引
    int str_index	  = elf_header.e_shstrndx;

    try{

        this->elf_section =(Elf32_Shdr*)(this->start + elf_header.e_shoff);
    }
    catch(exception& e)
    {
        cout << e.what() << "\n";
    }

    //段表字符串表开始地址, 文件映射首地址为start
    char *str = start + elf_section[str_index].sh_offset;

    int sym_struct_size = sizeof(Elf32_Sym);

    int tab_width = 8;

    Elf32_Sym *sym_addr = NULL;
    int sym_entry = 0;  //符号表的项数

    //默认字符表总是段表中的最后一个段
    //字符表首地址 strtab_addr
    char *strtab_addr = (char*)(start + elf_section[SectionNumber-1].sh_offset);

    for(int i=1; i<SectionNumber; i++)
    {
        char* tablename=   str + elf_section[i].sh_name;

        if((strstr(tablename, "sym") != NULL))
        {
            sym_entry = elf_section[i].sh_size / sym_struct_size;
            printf("\nSymbol table '%s' contains %d entries:\n", tablename, sym_entry);

            printf("Num:    Value  Size Type    Bind   Vis      Ndx Name\n");
            sym_addr = (Elf32_Sym*)(this->start + elf_section[i].sh_offset);

            for(int j=0; j<sym_entry; j++)
            {
               /* printf("%3d: %08x  %4d ", j, sym_addr[j].st_value, sym_addr[j].st_size);

                switch(ELF32_ST_TYPE(sym_addr[j].st_info))
                {
                    case 0:
                        printf("%-*s", tab_width, "NOTYPE");
                        break;
                    case 1:
                        printf("%-*s", tab_width, "OBJECT");
                        break;
                    case 2:
                        printf("%-*s", tab_width, "FUNC");
                        break;
                    case 3:
                        printf("%-*s", tab_width, "SECTION");
                        break;
                    case 4:
                        printf("%-*s", tab_width, "FILE");
                        break;
                    case 6:
                        printf("%-*s", tab_width, "TLS");
                        break;
                    case 7:
                        printf("%-*s", tab_width, "NUM");
                        break;
                    case 10:
                        printf("%-*s", tab_width, "LOOS");
                        break;
                    case 12:
                        printf("%-*s", tab_width, "HIOS");
                        break;
                }

                tab_width = 7;
                switch(ELF32_ST_BIND(sym_addr[j].st_info))
                {
                    case 0:
                        printf("%-*s", tab_width, "LOCAL");
                        break;
                    case 1:
                        printf("%-*s", tab_width, "GLOBAL");
                        break;
                    case 2:
                        printf("%-*s", tab_width, "WEAK");
                        break;
                    case 3:
                        printf("%-*s", tab_width, "NUM");
                        break;
                    case 10:
                        printf("%-*s", tab_width, "UNIQUE");
                        break;
                    case 12:
                        printf("%-*s", tab_width, "HIOS");
                        break;
                    case 13:
                        printf("%-*s", tab_width, "LOPROC");
                        break;
                    default:
                        printf("%-*s", tab_width," ");
                        break;
                }

                tab_width = 9;
                switch(ELF32_ST_VISIBILITY(sym_addr[j].st_other))
                {
                    case 0:
                        printf("%-*s", tab_width, "DEFAULT");
                        break;
                    case 1:
                        printf("%-*s", tab_width, "INTERNAL");
                        break;
                    case 2:
                        printf("%-*s", tab_width, "HIDDEN");
                        break;
                    case 3:
                        printf("%-*s", tab_width, "PROTECTED");
                        break;
                }

                tab_width = 3;
                switch(sym_addr[j].st_shndx)
                {
                    case SHN_ABS:
                        printf("%-*s", tab_width, "ABS");
                        break;
                    case SHN_COMMON:
                        printf("%-*s", tab_width, "COM");
                        break;
                    case SHN_UNDEF:
                        printf("%-*s", tab_width, "UND");
                        break;
                    case SHN_XINDEX:
                        printf("%-*s", tab_width, "COM");
                        break;
                    default:
                        printf("%3d", sym_addr[j].st_shndx);
                        break;
                }
*/
                tab_width = 15;

                if(strstr(strtab_addr + sym_addr[j].st_name, "_ZN3art7DexFile10OpenMemoryEPKhjRKNSt3__112basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEEjPNS_6M") != NULL){
                    printf1("---------%s-------","haha");
                    printf1(" %s",strtab_addr + sym_addr[j].st_name);

                }




                tab_width = 8;
                printf("\n");
            }

            printf("\n");
        }

    }
}



/*int main(int argc, char *argv[])
{
	ElfReader *read = NULL;

	if(argc <= 1)
	{
		printf("input the filename!\n");
		exit(0);
	}

	if(argc == 2)
	{
		printf("please usage:\n");
		printf("-h filename \t dump the file header\n");
		printf("-s filename \t dump the Section\n");
		exit(0);
	}

	if(argc == 3)
	{
		read = new ElfReader(argv[2]);

		if(!strcmp(argv[1], "-h"))
			read->printElfHeader();
		if(!strcmp(argv[1], "-S"))
			read->printElfSection();
		if(!strcmp(argv[1], "-s"))
			read->printElfSymbol();
		if(!strcmp(argv[1], "-r"))
			read->printElfRelSym();
		if(!strcmp(argv[1], "-d"))
			read->printElfDumpProgram();

	}

	return 0;
}*/








