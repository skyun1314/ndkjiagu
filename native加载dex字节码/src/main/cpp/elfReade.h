//
// Created by zk on 2017/7/20.
//
#ifndef __ELFREADER_H_
#define __ELFREADER_H_

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <exception>
#include "elf.h"

using namespace std;


//Elf 头文件长度
//#define ELF_LENHT 		sizeof(Elf32_Ehdr)



class ElfReade {
public:

    //读取elf文件
    bool readFile(const char *filename);


    char * printElfSymbol();



    ElfReade(const char *filename);

    ~ElfReade();

private:

    Elf32_Ehdr	elf_header_32;
    Elf32_Shdr 		*elf_section_32;
    Elf64_Ehdr	elf_header_64;
    Elf64_Shdr 		*elf_section_64;

    char *start;
    int fd;     //打开文件描述符
    unsigned int pos; //文件当前读过的字节数
    unsigned int file_size;
};


#endif //新NDK_ELFREADE_H
