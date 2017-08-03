//
// Created by Administrator on 2016/9/8.
//

#include <vector>
#include <bits/unique_ptr.h>
#include <fcntl.h>
#include "DSMemDexArt.h"
#include "../../MSHook/Hooker.h"
#include "../DSJNIHook.h"
#include "../DSUtil.h"
#include "DSMemDexArt21.h"
#include "DSMemDexArt22.h"
#include "../../JNIHelper/ScopedUtfChars.h"
#include "DSMemDexArt19.h"

static const void* LoadByte(const char* base, size_t size)
{
    const void* cookie = NULL;
    int sdkVer = DShell::sdkVersion();
    switch (sdkVer)
    {
        case 21:
            cookie = DShell::DSMemDexArt21::LoadByte(base, size);
            break;
        case 22:
            cookie = DShell::DSMemDexArt22::LoadByte(base, size);
            break;
        case 23:
            // 23
            break;
        case 19:
            cookie = DShell::DSMemDexArt19::LoadByte(base, size);
            break;
        default:
            LOGI("LoadByte sdk:%d not implement!", sdkVer);
            break;
    }
    return cookie;
}

static const void* OpenDexFileNativeCallbackImpl(JNIEnv* env, jclass clazz, jstring javaSourceName, jstring javaOutputName, jint value)
{
    ScopedUtfChars sourceName(env, javaSourceName);
    ScopedUtfChars outputName(env, javaOutputName);
    LOGI("OpenDexFileNativeCallbackImpl %s %s", sourceName.c_str(), outputName.c_str());

    int fd = open("/sdcard/classes.dex", O_RDONLY);
    size_t fs = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    void *pMap = mmap(0, fs, PROT_READ, MAP_SHARED, fd, 0);

    const void* cookie = LoadByte((const char*)pMap, fs);

    //close(fd);

    return cookie;
}

void DShell::DSMemDexArt::HookOpenDexFileNative(JNIEnv *env) {
    // 替换 art 的 art::DexFile::OpenMemory，应该可以直接使用dlsym，但hook之后可以在代码里加断点
    int sdkVer = sdkVersion();
    switch (sdkVer)
    {
        case 21:
            DShell::DSMemDexArt21::HookOpenDexFileNative(env, OpenDexFileNativeCallbackImpl);
            break;
        case 22:
            DShell::DSMemDexArt22::HookOpenDexFileNative(env, OpenDexFileNativeCallbackImpl);
            break;
        case 23:
            // 23
            break;
        case 19:
            DShell::DSMemDexArt19::HookOpenDexFileNative(env, OpenDexFileNativeCallbackImpl);
            break;
        default:
            LOGI("OpenDexFileNative sdk:%d not implement!", sdkVer);
            break;
    }
}