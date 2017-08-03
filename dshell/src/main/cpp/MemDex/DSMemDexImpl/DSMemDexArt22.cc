//
// Created by Administrator on 2016/9/8.
//

#include <fcntl.h>
#include "DSMemDexArt22.h"
#include "../../MSHook/Hooker.h"
#include "DexHeader.h"
#include <vector>
#include <bits/unique_ptr.h>

const void* (*org_artDexFileOpenMemory22)(const uint8_t* base, size_t size, const std::string& location, uint32_t location_checksum, void*/* MemMap* */ mem_map, const void*/* OatFile* */ oat_file, std::string* error_msg) = NULL;

const void* ds_artDexFileOpenMemory22(const uint8_t* base, size_t size, const std::string& location, uint32_t location_checksum, void*/* MemMap* */ mem_map, const void*/* OatFile* */ oat_file, std::string* error_msg) {

    return org_artDexFileOpenMemory22(base, size, location, location_checksum, mem_map, oat_file, error_msg);
}

void DShell::DSMemDexArt22::HookOpenDexFileNative(JNIEnv *env, OpenDexFileNativeCallback pOpenDexFileNativeCallback)
{
    if (!org_artDexFileOpenMemory22) {
        // samsung
        Cydia::elfHookFunction("libart.so",
                               "_ZN3art7DexFile10OpenMemoryEPKhjRKNSt3__112basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEEjPNS_6MemMapEPKNS_7OatFileES2_PS9_.constprop.183",
                               (void *) ds_artDexFileOpenMemory22,
                               (void **) &org_artDexFileOpenMemory22);
    }

    if (!org_artDexFileOpenMemory22) {
        Cydia::elfHookFunction("libart.so",
                               "_ZN3art7DexFile10OpenMemoryEPKhjRKNSt3__112basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEEjPNS_6MemMapEPKNS_7OatFileES2_PS9_",
                               (void *) ds_artDexFileOpenMemory22,
                               (void **) &org_artDexFileOpenMemory22);
    }

    DSMemDexArt21::HookOpenDexFileNative(env, pOpenDexFileNativeCallback);
}

static const void* DShell::DSMemDexArt22::LoadByte(const char* base, size_t size)
{
    std::string location = "";
    std::string err_msg;

    const Header* dex_header = reinterpret_cast<const Header*>(base);
    const void *p = ds_artDexFileOpenMemory22((const uint8_t *) base, size, location, dex_header->checksum_, NULL, NULL, &err_msg);
    if (!p) {
        LOGI("DSMemDex::LoadByte Failed %s", err_msg.c_str());
    }
    else {
        LOGI("DSMemDex::LoadByte : %x", p);
    }

    return  p;
}
