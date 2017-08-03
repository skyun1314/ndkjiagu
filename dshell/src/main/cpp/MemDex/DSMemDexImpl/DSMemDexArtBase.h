//
// Created by Administrator on 2016/9/8.
//

#ifndef TESTSHELL_DSMEMDEXARTBASE_H
#define TESTSHELL_DSMEMDEXARTBASE_H

#include <jni.h>
#include <stddef.h>

typedef const void* (*OpenDexFileNativeCallback)(JNIEnv* env, jclass clazz, jstring javaSourceName, jstring javaOutputName, jint value);

namespace DShell
{
class DSMemDexArtBase {
public:
//    static void HookOpenDexFileNative(JNIEnv* env, OpenDexFileNativeCallback pOpenDexFileNativeCallback);
//    static const void* LoadByte(const char* base, size_t size);
    static void HookOpenDexFileNative(JNIEnv* env, OpenDexFileNativeCallback pOpenDexFileNativeCallback);

    static const void* OpenDexFileNative(JNIEnv* env, jclass clazz, jstring javaSourceName, jstring javaOutputName, jint value);

protected:
    static OpenDexFileNativeCallback g_pOpenDexFileNativeCallback;
};
}

#endif //TESTSHELL_DSMEMDEXARTBASE_H
