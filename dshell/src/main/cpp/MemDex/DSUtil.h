//
// Created by Administrator on 2016/9/5.
//

#ifndef TESTSHELL_DSUTIL_CC_H
#define TESTSHELL_DSUTIL_CC_H


#include <jni.h>
#include "../common.h"
#include "../MSHook/Log.h"

#define SO_ART "libart.so"

namespace DShell {

    int lookupJniMethod(JNINativeMethod *table, const char *name, const char *sig, void (**fnPtrout)(u4 const *, union JValue *));

    int sdkVersion();

    bool isArtMode();
};


//extern void HookArtMethod(JNIEnv* env, jmethodID srcMethod, jmethodID targetMethod);

#endif //TESTSHELL_DSUTIL_CC_H
