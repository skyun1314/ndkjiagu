//
// Created by Administrator on 2016/9/8.
//

#include "DSMemDexArtBase.h"

static OpenDexFileNativeCallback DShell::DSMemDexArtBase::g_pOpenDexFileNativeCallback;

void DShell::DSMemDexArtBase::HookOpenDexFileNative(JNIEnv *env, OpenDexFileNativeCallback pOpenDexFileNativeCallback) {
    g_pOpenDexFileNativeCallback = pOpenDexFileNativeCallback;
}

const void *DShell::DSMemDexArtBase::OpenDexFileNative(JNIEnv *env, jclass clazz,
                                                       jstring javaSourceName,
                                                       jstring javaOutputName, jint value) {
    return g_pOpenDexFileNativeCallback(env, clazz, javaSourceName, javaOutputName, value);
}
