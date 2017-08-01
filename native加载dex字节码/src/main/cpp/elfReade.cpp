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





char * ElfReade::printElfSymbol()
{

    //段表的总个数
    int SectionNumber = elf_header.e_shnum;
    //段表符号表的索引
    int str_index  = elf_header.e_shstrndx;
    this->elf_section =(Elf32_Shdr*)(this->start + elf_header.e_shoff);


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
            sym_addr = (Elf32_Sym*)(this->start + elf_section[i].sh_offset);

            for(int j=0; j<sym_entry; j++)
            {

                if(strstr(strtab_addr + sym_addr[j].st_name, "RKNSt3__112basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEEjPNS_6MemMapEPKNS_10OatDexFileEPS9_") != NULL){
                    printf1(" %s",strtab_addr + sym_addr[j].st_name);
                    return strtab_addr + sym_addr[j].st_name;
                }

            }

        }

    }
}

