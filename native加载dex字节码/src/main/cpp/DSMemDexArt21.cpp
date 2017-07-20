#include "DSMemDexArt21.h"
#include <sys/mman.h>
#include <stdlib.h>
#include <errno.h>
const void* (*org_artDexFileOpenMemory21)(const uint8_t* base, size_t size, const std::string& location, uint32_t location_checksum, void*/* MemMap* */ mem_map, std::string* error_msg) = NULL;

int DSMemDexArt21::sdkVersion()
{
    static bool done = false;
    static int sdkVersion = 0;
    if (!done)
    {
        done = true;
        char propValue[PROP_VALUE_MAX] = {0};
        __system_property_get("ro.build.version.sdk", propValue);
        sdkVersion = atoi(propValue);
    }

    return sdkVersion;
}


char* DSMemDexArt21::execute(char*str){
    FILE *fstream=NULL;
    char buff[1024];
    memset(buff,0,sizeof(buff));
    if(NULL==(fstream=popen(str,"r")))
    {
        fprintf(stderr,"execute command failed: %s",strerror(errno));
        return NULL;
    }
    if(NULL!=fgets(buff, sizeof(buff), fstream))
    {
        printf("%s",buff);
    }
    else
    {
        pclose(fstream);
        return NULL;
    }
    pclose(fstream);
    return buff;
}



const void* DSMemDexArt21::LoadByte(const char* base, size_t size)
{
    std::string location = "";
    std::string err_msg;

/*
char* OpenMemoryname="";


    int sdkVer = DSMemDexArt21::sdkVersion();
    switch (sdkVer)
    {
        case 21:
            OpenMemoryname="_ZN3art7DexFile10OpenMemoryEPKhjRKNSt3__112basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEEjPNS_6MemMapEPS9_";
           break;
        case 22:
            OpenMemoryname="_ZN3art7DexFile10OpenMemoryEPKhjRKNSt3__112basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEEjPNS_6MemMapEPKNS_7OatFileES2_PS9_";
                            _ZN3art7DexFile10OpenMemoryERKNSt3__112basic_stringIcNS1_11char_traitsIcEENS1_9allocatorIcEEEEjPNS_6MemMapEPS7_
            break;
        case 23:
            // 23
            break;
        default:
            //LOGI("OpenDexFileNative sdk:%d not implement!", sdkVer);
            break;
    }
*/

  /*  ElfReade *read = new ElfReade("/system/lib/libart.so");
    read->printElfSymbol();*/


    void *handle = dlopen("libart.so", RTLD_NOW);
    org_artDexFileOpenMemory21= (const void *(*)(const uint8_t *, size_t, const std::string &,
                                                 uint32_t, void *, std::string *)) dlsym(handle, OpenMemoryname);


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
