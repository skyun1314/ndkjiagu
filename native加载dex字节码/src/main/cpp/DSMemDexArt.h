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
    static const void *LoadByte(JNIEnv *env,const jobject pJobject);

    static int sdkVersion();

    static char *execute(char *str);

    static bool is64();


    static int sdk_int;//6.0

    static void replace_classloader_cookie(JNIEnv *pEnv, jobject pJobject);

    static jlongArray ConvertNativeToJavaArray(JNIEnv *env, void *buff);

    static void replace_resouce(JNIEnv *env, jobject pJobject);

    static const char *mPackageName;
    static jstring thePackagename;

    static char *copyDexToByte(JNIEnv *env, jobject application_obj, char *name, int &dexlen);

    static void copyFile(JNIEnv *env,const jobject obj);

    static const jbyte *readDexFileFromApk(JNIEnv *env, jobject cxt, int &size);

    static char *getSizeByoff(char *DexFileBytes, int lenth, int DexFileBytesIndex);

    static int bytesToInt(char *bytes);

    static int32_t swapInt32(int32_t value);

    static char *decrpt(char *srcdata, int len);

    static char *the_dex_byte;
    static int the_dex_size;

    static void _onCreate(JNIEnv *env,jobject context);
};

#endif //TESTSHELL_DSMEMDEXART21_H
