#include "DSMemDexArt21.h"
#include <sys/mman.h>
#include <stdlib.h>
#include <errno.h>
const void* (*org_artDexFileOpenMemory21)(const uint8_t* base, size_t size, const std::string& location, uint32_t location_checksum, void*/* MemMap* */ mem_map, std::string* error_msg) = NULL;




 const void* DSMemDexArt21::LoadByte(const char* base, size_t size)
{
    std::string location = "";
    std::string err_msg;


    void *handle = dlopen("libart.so", RTLD_NOW);
    org_artDexFileOpenMemory21= (const void *(*)(const uint8_t *, size_t, const std::string &,
                                                 uint32_t, void *, std::string *)) dlsym(handle, "_ZN3art7DexFile10OpenMemoryEPKhjRKNSt3__112basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEEjPNS_6MemMapEPS9_");


    const DexHeader* dex_header = reinterpret_cast<const DexHeader*>(base);
    const void *p = org_artDexFileOpenMemory21((const uint8_t *) base, size, location, dex_header->checksum, NULL, &err_msg);
    if (!p) {
        LOGD("DSMemDex::LoadByte Failed %s", err_msg.c_str());
    }
    else {
        LOGD("DSMemDex::LoadByte : %x", p);
    }

    return p;
}




/*
void modifySymbol(void* handle, const char *name, void* new_addr){
    if(!handle) return;
    soinfo *si = (soinfo*)handle;
    Elf32_Sym *symtab = (Elf32_Sym *) si->symtab;
    const char *strtab = si->strtab;
    Elf32_Rel *rel = si->rel;
    unsigned count = si->rel_count;
    unsigned idx;
    for(idx=0; idx<count; idx++){
        unsigned type = ELF32_R_TYPE(rel->r_info);
        unsigned sym = ELF32_R_SYM(rel->r_info);
        unsigned reloc = (unsigned)(rel->r_offset + si->base);
        char *sym_name = (char *)(strtab + symtab[sym].st_name);
        if(type==R_ARM_GLOB_DAT && strcmp(sym_name, name)==0){
            *((unsigned*)reloc) = (Elf32_Addr)new_addr;
            break;
        }
        rel++;
    }
}

*/
