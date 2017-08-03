//
// Created by Administrator on 2016/9/8.
//

#ifndef TESTSHELL_DSMEMDEXART19_H
#define TESTSHELL_DSMEMDEXART19_H

#include <jni.h>
#include "DSMemDexArtBase.h"

namespace DShell
{
class DSMemDexArt19 : DSMemDexArtBase {
public:
    static void HookOpenDexFileNative(JNIEnv* env, OpenDexFileNativeCallback pOpenDexFileNativeCallback);
    static const void* LoadByte(const char* base, size_t size);
};
}

#endif //TESTSHELL_DSMEMDEXART19_H
