//
// Created by Administrator on 2016/9/8.
//

#include <fcntl.h>
#include "DSMemDexArt21.h"
#include "../../MSHook/Hooker.h"
#include "DexHeader.h"
#include <vector>
#include <bits/unique_ptr.h>
#include "../../JNIHelper/ScopedUtfChars.h"
#include "../DSJNIHook.h"

const void* (*org_artDexFileOpenMemory21)(const uint8_t* base, size_t size, const std::string& location, uint32_t location_checksum, void*/* MemMap* */ mem_map, std::string* error_msg) = NULL;

const void* ds_artDexFileOpenMemory21(const uint8_t* base, size_t size, const std::string& location, uint32_t location_checksum, void*/* MemMap* */ mem_map, std::string* error_msg) {

    return org_artDexFileOpenMemory21(base, size, location, location_checksum, mem_map, error_msg);
}

// 5.0、5.1 openDexFileNative
const jlong (*org_openDexFileNative_sdk21and22)(JNIEnv* env, jclass clazz, jstring javaSourceName, jstring javaOutputName, jint value) = NULL;
static jlong openDexFileNative_sdk21and22(JNIEnv* env, jclass clazz, jstring javaSourceName, jstring javaOutputName, jint value)
{
    std::unique_ptr<std::vector<const void*>> dex_files(new std::vector<const void*>());

    const void* cookie = DShell::DSMemDexArtBase::OpenDexFileNative(env, clazz, javaSourceName, javaOutputName, value);

    if (cookie)
    {
        dex_files.get()->push_back(cookie);
        return static_cast<jlong>(reinterpret_cast<uintptr_t>(dex_files.release()));
    }

    return org_openDexFileNative_sdk21and22(env, clazz, javaSourceName, javaOutputName, value);
}

void DShell::DSMemDexArt21::HookOpenDexFileNative(JNIEnv *env, OpenDexFileNativeCallback pOpenDexFileNativeCallback) {
    // 替换 art 的 art::DexFile::OpenMemory，应该可以直接使用dlsym，但hook之后可以在代码里加断点
    if (!org_artDexFileOpenMemory21) {
        Cydia::elfHookFunction("libart.so",
                               "_ZN3art7DexFile10OpenMemoryEPKhjRKNSt3__112basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEEjPNS_6MemMapEPS9_",
                               (void *) ds_artDexFileOpenMemory21,
                               (void **) &org_artDexFileOpenMemory21);
    }

    DSJNIHook::HookJNINative(env, "dalvik/system/DexFile", "openDexFileNative", "(Ljava/lang/String;Ljava/lang/String;I)J", (void*)openDexFileNative_sdk21and22, (void**)&org_openDexFileNative_sdk21and22);

    DSMemDexArtBase::HookOpenDexFileNative(env, pOpenDexFileNativeCallback);
}

static const void* DShell::DSMemDexArt21::LoadByte(const char* base, size_t size)
{
    std::string location = "";
    std::string err_msg;

    const Header* dex_header = reinterpret_cast<const Header*>(base);
    const void *p = ds_artDexFileOpenMemory21((const uint8_t *) base, size, location, dex_header->checksum_, NULL, &err_msg);
    if (!p) {
        LOGI("DSMemDex::LoadByte Failed %s", err_msg.c_str());
    }
    else {
        LOGI("DSMemDex::LoadByte : %x", p);
    }

    return p;
}
