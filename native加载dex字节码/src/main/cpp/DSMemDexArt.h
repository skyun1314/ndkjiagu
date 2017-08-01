//
// Created by Administrator on 2016/9/8.
//
#include <android/log.h>
#include <sys/system_properties.h>
#ifndef TESTSHELL_DSMEMDEXART21_H
#define TESTSHELL_DSMEMDEXART21_H

#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,"wodelog", __VA_ARGS__)
#include <string>
#include <jni.h>
#include "DexFile.h"
#include <stddef.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <unistd.h>
#include "linker.h"
class DSMemDexArt {
public:
    static const void* LoadByte(JNIEnv *env,const char* base, size_t size);
    static int sdkVersion();
    static char * execute(char*str);

    static bool is64();
};

#endif //TESTSHELL_DSMEMDEXART21_H
