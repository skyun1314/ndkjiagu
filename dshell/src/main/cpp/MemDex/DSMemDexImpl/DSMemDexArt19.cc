//
// Created by Administrator on 2016/9/8.
//

#include <fcntl.h>
#include "DSMemDexArt19.h"
#include "../../MSHook/Hooker.h"
#include "DexHeader.h"
#include "../DSJNIHook.h"
#include "../../JNIHelper/ScopedUtfChars.h"
#include <vector>
#include <bits/unique_ptr.h>

const void* (*org_artDexFileOpenMemory19)(unsigned char const*/*byte*/ base, unsigned int size, std::string const& location, unsigned int location_checksum, void*/* MemMap* */ mem_map) = NULL;
typedef const void* (*org1_artDexFileOpenMemory19)(unsigned char const*/*byte*/ base, unsigned int size, std::string const& location, unsigned int location_checksum, void*/* MemMap* */ mem_map);
typedef const void* (*org2_artDexFileOpenMemory19)(int a);
const void* ds_artDexFileOpenMemory19(unsigned char const*/*byte*/ base, unsigned int size, std::string const& location, unsigned int location_checksum, void*/* MemMap* */ mem_map)
{
    return org_artDexFileOpenMemory19((const unsigned char *) base, size, location, location_checksum, NULL);
}

// 4.4 openDexFileNative
const jint (*org_openDexFileNative_sdk19)(JNIEnv* env, jclass clazz, jstring javaSourceName, jstring javaOutputName, jint value) = NULL;
static jint openDexFileNative_sdk19(JNIEnv* env, jclass clazz, jstring javaSourceName, jstring javaOutputName, jint value)
{
    ScopedUtfChars sourceName(env, javaSourceName);
    ScopedUtfChars outputName(env, javaOutputName);
    LOGI("OpenDexFileNativeCallbackImpl %s %s", sourceName.c_str(), outputName.c_str());

    const void* cookie = DShell::DSMemDexArtBase::OpenDexFileNative(env, clazz, javaSourceName, javaOutputName, value);

    if (cookie)
    {
        return static_cast<jint>(reinterpret_cast<uintptr_t>(cookie));
    }

    return org_openDexFileNative_sdk19(env, clazz, javaSourceName, javaOutputName, value);
}

void*  (*org_mmap)(void *, size_t, int, int, int, off_t) = NULL;
void*  ds_mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset)
{
    return org_mmap(addr, len, prot, flags, fd, offset);
}

void DShell::DSMemDexArt19::HookOpenDexFileNative(JNIEnv *env, OpenDexFileNativeCallback pOpenDexFileNativeCallback) {
    // 替换 art 的 art::DexFile::OpenMemory，应该可以直接使用dlsym，但hook之后可以在代码里加断点
    if (!org_artDexFileOpenMemory19) {
        Cydia::elfHookFunction("libart.so",
                               "_ZN3art7DexFile10OpenMemoryEPKhjRKSsjPNS_6MemMapE",
                               (void *) ds_artDexFileOpenMemory19,
                               (void **) &org_artDexFileOpenMemory19);
    }

//    if (!org_mmap) {
//        Cydia::elfHookFunction("libc.so",
//                               "mmap",
//                               (void *) ds_mmap,
//                               (void **) &org_mmap);
//    }

    DSJNIHook::HookJNINative(env, "dalvik/system/DexFile", "openDexFileNative", "(Ljava/lang/String;Ljava/lang/String;I)I", (void*)openDexFileNative_sdk19, (void**)&org_openDexFileNative_sdk19);

//    int fd = open("/sdcard/classes.dex", O_RDONLY);
//    size_t fs = lseek(fd, 0, SEEK_END);
//    lseek(fd, 0, SEEK_SET);
//
//    void *pMap = mmap(0, fs, PROT_READ, MAP_SHARED, fd, 0);
//    LoadByte((const char*)pMap, fs);

    DSMemDexArtBase::HookOpenDexFileNative(env, pOpenDexFileNativeCallback);
}

static const void* DShell::DSMemDexArt19::LoadByte(const char* base, size_t size)
{
    std::string location = "";
    std::string err_msg;

//    void* so = dlopen("libart.so", 0);
//
//    void* abc = dlsym(so, "_ZN3art7DexFile10OpenMemoryEPKhjRKSsjPNS_6MemMapE");
//    org1_artDexFileOpenMemory19 func = (org1_artDexFileOpenMemory19)abc;

    const Header* dex_header = reinterpret_cast<const Header*>(base);
    const void *p = ds_artDexFileOpenMemory19((const uint8_t *)base, size, location, dex_header->checksum_, NULL);
    //const void* p = func((const unsigned char *) base, size, location, dex_header->checksum_, NULL);
    if (!p) {
        LOGI("DSMemDex::LoadByte Failed %s", err_msg.c_str());
    }
    else {
        LOGI("DSMemDex::LoadByte : %x", p);
    }

    return p;
}
