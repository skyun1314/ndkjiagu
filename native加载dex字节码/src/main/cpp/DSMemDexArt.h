//
// Created by Administrator on 2016/9/8.
//

#include <sys/system_properties.h>
#ifndef TESTSHELL_DSMEMDEXART21_H
#define TESTSHELL_DSMEMDEXART21_H
#include <android/log.h>
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,"wodelog", __VA_ARGS__)
#include <string>
#include <jni.h>
#include "DexFile.h"
#include <stddef.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <bits/unique_ptr.h>
#include "linker.h"
class DSMemDexArt {
public:
    static const void *LoadByte(JNIEnv *env, const char *base, jsize size, jobject pJobject);
    static int sdkVersion();
    static char * execute(char*str);

    static bool is64();


    static int sdk_int;//6.0

    static void replace_classloader_cookie(JNIEnv *pEnv,  jobject pJobject);

    static jlongArray ConvertNativeToJavaArray(JNIEnv* env,  void* buff);
   // static jlongArray ConvertNativeToJavaArray(JNIEnv* env,  std::vector<std::unique_ptr<const DexFile>>& vec);
   static void replace_resouce(JNIEnv *env, jobject pJobject);
};

#endif //TESTSHELL_DSMEMDEXART21_H
