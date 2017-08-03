//
// Created by Administrator on 2016/9/8.
//

#ifndef TESTSHELL_DSMEMDEXART22_H
#define TESTSHELL_DSMEMDEXART22_H

#include <jni.h>
#include "DSMemDexArt21.h"

namespace DShell
{
class DSMemDexArt22 : public DSMemDexArt21 {
public:
    static void HookOpenDexFileNative(JNIEnv* env, OpenDexFileNativeCallback pOpenDexFileNativeCallback);
    static const void* LoadByte(const char* base, size_t size);
};
}

#endif //TESTSHELL_DSMEMDEXART22_H
