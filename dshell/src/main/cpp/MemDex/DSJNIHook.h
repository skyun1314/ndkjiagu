//
// Created by Administrator on 2016/9/5.
//

#ifndef TESTSHELL_DSJNIHOOK_H
#define TESTSHELL_DSJNIHOOK_H


#include <jni.h>

namespace DShell {
    class DSJNIHook {
    public:
        static bool HookJNINative(JNIEnv *env, const char* className, const char* methodName, const char *methodSig, void *replace_func, void **old_func);
    };
}



#endif //TESTSHELL_DSJNIHOOK_H
