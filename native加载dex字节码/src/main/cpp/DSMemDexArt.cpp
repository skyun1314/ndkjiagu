#include "DSMemDexArt.h"
#include "elfReade.h"
#include <sys/mman.h>
#include <stdlib.h>
#include <errno.h>

const void *
(*org_artDexFileOpenMemory)(const uint8_t *base, size_t size, const std::string &location,
                            uint32_t location_checksum, void */* MemMap* */ mem_map,
                            std::string *error_msg) = NULL;

const void
(*org_open_dex_file)(char *, int, int, int, int) = NULL;


const void *
(*org_artDexFileOpenMemory_6_0)(const uint8_t *base,
                                size_t size,
                                const std::string &location,
                                //uint32_t location_checksum,
                                int *location_checksum,
                                void */* MemMap* */ mem_map,
                                void *oat_dex_file,
                                std::string *error_msg) = NULL;




int DSMemDexArt::sdkVersion() {

    static bool done = false;
    static int sdkVersion = 0;
    if (!done) {
        done = true;
        char propValue[PROP_VALUE_MAX] = {0};
        __system_property_get("ro.build.version.sdk", propValue);
        sdkVersion = atoi(propValue);
    }

    return sdkVersion;
}


char *DSMemDexArt::execute(char *str) {
    FILE *fstream = NULL;
    char buff[1024];
    memset(buff, 0, sizeof(buff));
    if (NULL == (fstream = popen(str, "r"))) {
        fprintf(stderr, "execute command failed: %s", strerror(errno));
        return NULL;
    }
    if (NULL != fgets(buff, sizeof(buff), fstream)) {
        printf("%s", buff);
    } else {
        pclose(fstream);
        return NULL;
    }
    pclose(fstream);
    return buff;
}


const void *DSMemDexArt::LoadByte(const char *base, size_t size) {
    std::string location = "";
    std::string err_msg;
    char *OpenMemoryname = "/data/data/com.example.nativedex/files/classes.dex";
    int error_message;

    void *handle1 = dlopen("libart.so", RTLD_NOW);
    void *open_dex_file = dlsym(handle1, "_ZN3art7DexFile8OpenFileEiPKcbPNSt3__112basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEE");
    org_open_dex_file= (const void (*)(char *, int, int, int, int)) open_dex_file;

  char*  buffer2 = (char *)malloc(256);
    memset(buffer2,0,256);
    int   odex_handle = open(OpenMemoryname, 0);
    org_open_dex_file(buffer2, odex_handle, error_message, 0, error_message);




    ElfReade *read = new ElfReade("/system/lib/libart.so");
    OpenMemoryname = read->printElfSymbol();
    void *handle = dlopen("libart.so", RTLD_NOW);
    int sdkVer = DSMemDexArt::sdkVersion();

    void *OpenMemoryname_sys = dlsym(handle, OpenMemoryname);




    const void *p = NULL;
    const DexHeader *dex_header = reinterpret_cast<const DexHeader *>(base);
    if (sdkVer >= 23) {
        org_artDexFileOpenMemory_6_0 = (const void *(*)(const uint8_t *, size_t, const string &, int *,
                                                        void *, void *,
                                                        string *)) OpenMemoryname_sys;


        const DexStringId *xx1= reinterpret_cast<const DexStringId*>(base + dex_header->stringIdsOff);
        const DexTypeId *xx2= reinterpret_cast<const DexTypeId*>(base + dex_header->typeIdsOff);
        const DexFieldId *xx3=            reinterpret_cast<const DexFieldId*>(base + dex_header->fieldIdsOff);
        const DexMethodId *xx4=           reinterpret_cast<const DexMethodId*>(base + dex_header->methodIdsOff);
        const DexProtoId *xx5=           reinterpret_cast<const DexProtoId*>(base + dex_header->protoIdsOff);
        const DexClassDef *xx6=           reinterpret_cast<const DexClassDef*>(base + dex_header->classDefsOff);

      /*  int fd = open("/data/data/com.example.nativedex/files/classes.dex", O_RDONLY);
        size_t fs = lseek(fd, 0, SEEK_END);
        lseek(fd, 0, SEEK_SET);

        void *pMap = mmap(0, fs, PROT_READ, MAP_SHARED, fd, 0);*/
        int a=4237652352;
        int *xxoo;
        xxoo=&a;
       p= org_artDexFileOpenMemory_6_0((const uint8_t *) base, size, location,
                                      xxoo
                                       ,
                                       NULL,
                                       NULL, &err_msg);

        printf("%s","hani");

    } else {
        org_artDexFileOpenMemory = (const void *(*)(const uint8_t *, size_t, const string &,
                                                    uint32_t, void *, string *)) OpenMemoryname_sys;

        p = org_artDexFileOpenMemory((const uint8_t *) base, size, location,
                                     dex_header->checksum, NULL, &err_msg);
    }


    if (!p) {
        LOGD("DSMemDex::LoadByte Failed %s", err_msg.c_str());
    } else {
        LOGD("DSMemDex::LoadByte : %x", p);
    }

    return p;
}

void HH::haha(int a) {
printf("%d",a);
}

void HH::aa() {
   haha(90);
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
