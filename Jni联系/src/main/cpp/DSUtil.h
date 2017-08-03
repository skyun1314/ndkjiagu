//
// Created by Administrator on 2016/9/5.
//

#ifndef TESTSHELL_DSUTIL_CC_H
#define TESTSHELL_DSUTIL_CC_H
#include <android/log.h>
#define LOGD(fmt,...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "[%s]" fmt, __FUNCTION__,##__VA_ARGS__)
#define LOGI(fmt,...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "[%s]" fmt, __FUNCTION__,##__VA_ARGS__)
#define LOGV(fmt,...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "[%s]" fmt, __FUNCTION__,##__VA_ARGS__)
#define LOGW(fmt,...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, "[%s]" fmt, __FUNCTION__,##__VA_ARGS__)
#define LOGE(fmt,...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "[%s]" fmt, __FUNCTION__,##__VA_ARGS__)
#define LOGF(fmt,...) __android_log_print(ANDROID_LOG_FATAL, LOG_TAG, "[%s]" fmt, __FUNCTION__,##__VA_ARGS__)

# define LOG_TAG "DShell"

#include <jni.h>
#include "common.h"


#define SO_ART "libart.so"

namespace DShell {

    int lookupJniMethod(JNINativeMethod *table, const char *name, const char *sig, void (**fnPtrout)(u4 const *, union JValue *));

    int sdkVersion();

    bool isArtMode();
};


//extern void HookArtMethod(JNIEnv* env, jmethodID srcMethod, jmethodID targetMethod);

#endif //TESTSHELL_DSUTIL_CC_H
