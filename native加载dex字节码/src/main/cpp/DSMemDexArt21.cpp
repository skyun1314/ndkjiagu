#include "DSMemDexArt21.h"
#include <dlfcn.h>

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
