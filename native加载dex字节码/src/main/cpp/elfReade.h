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

#define offsetof(s, m) (size_t)&(((s*)0)->m)

#define get_data(base, s, index) (*(s*)(base+index))
/*  --定义的头文件
#define EI_NIDENT 16
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Word;
typedef uint32_t Elf32_Off;
typedef uint32_t Elf32_Addr;
*/
typedef struct
{
    unsigned char   e_ident[EI_NIDENT];
    Elf32_Half      e_type;     //elf文件类型 1 可重定位；2 可执行； 3 共享目标文件
    Elf32_Half      e_machine;  //机器相关
    Elf32_Word      e_version;  //版本信息，一般为1
    Elf32_Addr      e_entry;    //入口地址
    Elf32_Off       e_phoff;    //程序头偏移地址
    Elf32_Off       e_shoff;    //段表在文件中的偏移地址
    Elf32_Word      e_flags;
    Elf32_Half      e_ehsize;   //elf文件头的大小
    Elf32_Half      e_phentsize;
    Elf32_Half      e_phnum;
    Elf32_Half      e_shentsize; //段表描述符的大小，一般为sizeof(Elf32_Shdr)
    Elf32_Half      e_shnum;    //段表描述符数量
    Elf32_Half      e_shstrndx; //段表字符串表在段表中的下标
} Elf32_header;


//Elf 头文件长度
#define ELF_LENHT 		sizeof(Elf32_header)
#define Section_Lenth sizeof(Elf32_Shdr)
//段表



class ElfReade {
public:

    //读取elf文件
    bool readFile(const char *filename);
/*
    Elf32_Ehdr& getHeader() const;

    void printElfHeader() const;

	void printElfSection();
    void printElfRelSym();

    void printElfDumpProgram();*/
    char * printElfSymbol();



    unsigned int  getFileSize() const;


    ElfReade(const char *filename);

    ~ElfReade();

private:

    Elf32_header	elf_header;
    Elf32_Shdr 		*elf_section;

    char *start;
    int fd;     //打开文件描述符
    unsigned int pos; //文件当前读过的字节数
    unsigned int file_size;
};


#endif //新NDK_ELFREADE_H
