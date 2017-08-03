//
// Created by zk on 2017/7/20.
//

#include "elfReade.h"
#include "DSMemDexArt.h"

#define LOG_TAG "wodelog"
#define printf1(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))
#define ALOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))

#include <android/log.h>


bool ElfReade::readFile(const char *filename) {

    struct stat sb;
    fd = open(filename, O_RDONLY);
    fstat(fd, &sb); /* 取得文件大小 */
    file_size = sb.st_size;
    start = (char *) mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (start == MAP_FAILED) /* 判断是否映射成功 */
        return false;

    return true;
}

ElfReade::ElfReade(const char *filename) {
    if (!readFile(filename)) {
        printf("can not mmap the file %s\n", filename);
        exit(0);
    }

    if (DSMemDexArt::is64()) {
        memcpy(&elf_header_64, this->start, sizeof(Elf64_Ehdr));
    } else {
        memcpy(&elf_header_32, this->start, sizeof(Elf32_Ehdr));
    }


}

ElfReade::~ElfReade() {
    munmap(start, file_size); /* 解除映射 */
    close(fd);

}


char *ElfReade::printElfSymbol() {

    //段表的总个数
    int SectionNumber;// = elf_header.e_shnum;
    //段表符号表的索引
    int str_index;// = elf_header.e_shstrndx;
    //this->elf_section =(Elf32_Shdr*)(this->start + elf_header.e_shoff);


    //段表字符串表开始地址, 文件映射首地址为start
    char *str;// = start + elf_section[str_index].sh_offset;

    //默认字符表总是段表中的最后一个段
    //字符表首地址 strtab_addr
    char *strtab_addr;//= (char*)(start + elf_section[SectionNumber-1].sh_offset);

    int sym_struct_size;//= sizeof(Elf32_Sym);


    Elf32_Sym *sym_addr_32 = NULL;
    Elf64_Sym *sym_addr_64 = NULL;

    if (DSMemDexArt::is64()) {
        SectionNumber = elf_header_64.e_shnum;
        str_index = elf_header_64.e_shstrndx;
        this->elf_section_64 = (Elf64_Shdr *) (this->start + elf_header_64.e_shoff);
        str = start + elf_section_64[str_index].sh_offset;
        strtab_addr = (char *) (start + elf_section_64[SectionNumber - 1].sh_offset);
        sym_struct_size = sizeof(Elf64_Sym);
    } else {
        SectionNumber = elf_header_32.e_shnum;
        str_index = elf_header_32.e_shstrndx;
        this->elf_section_32 = (Elf32_Shdr *) (this->start + elf_header_32.e_shoff);
        str = start + elf_section_32[str_index].sh_offset;
        strtab_addr = (char *) (start + elf_section_32[SectionNumber - 1].sh_offset);
        sym_struct_size = sizeof(Elf32_Sym);
    }


    int sym_entry = 0;  //符号表的项数



    for (int i = 1; i < SectionNumber; i++) {
        char *tablename ;//= str + elf_section[i].sh_name;
        if (DSMemDexArt::is64()) {
            tablename = str + elf_section_64[i].sh_name;
        }else{
           tablename = str + elf_section_32[i].sh_name;
        }




        if ((strstr(tablename, "sym") != NULL)) {



            if (DSMemDexArt::is64()) {
                sym_entry = elf_section_64[i].sh_size / sym_struct_size;
                sym_addr_64 = (Elf64_Sym *) (this->start + elf_section_64[i].sh_offset);
            }else{
                sym_entry = elf_section_32[i].sh_size / sym_struct_size;
                sym_addr_32 = (Elf32_Sym *) (this->start + elf_section_32[i].sh_offset);
            }


            for (int j = 0; j < sym_entry; j++) {

                char*xx00;
                if (DSMemDexArt::is64()) {
                    xx00=strtab_addr + sym_addr_64[j].st_name;
                }else{
                    xx00=strtab_addr + sym_addr_32[j].st_name;
                }
                if (strstr(xx00, "RKNSt3__112basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEEjPNS_6MemMapEP") !=
                    NULL&&(strstr(xx00,"OpenMemory")!=NULL)&&(strstr(xx00,"_ZN3art7Dex")!=NULL)) {
                    printf1(" %s", xx00);
                    return xx00;
                }

            }

        }

    }
    return "";
}

