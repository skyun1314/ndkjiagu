//
// Created by Administrator on 2016/9/8.
//
#include <android/log.h>
#ifndef TESTSHELL_DSMEMDEXART21_H
#define TESTSHELL_DSMEMDEXART21_H
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,"wodelog", __VA_ARGS__)
#include <string>
#include <jni.h>
#include "DexFile.h"
#include <stddef.h>
class DSMemDexArt21 {
public:
    static const void* LoadByte(const char* base, size_t size);
};

#endif //TESTSHELL_DSMEMDEXART21_H
