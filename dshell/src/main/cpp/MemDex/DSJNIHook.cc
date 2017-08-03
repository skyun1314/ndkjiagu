//
// Created by Administrator on 2016/9/5.
//

#include <stddef.h>
#include <sys/system_properties.h>
#include <string.h>
#include <dlfcn.h>
#include "DSJNIHook.h"
#include "../MSHook/Log.h"
#include "DSUtil.h"

using namespace DShell;

extern "C" {
void mark() {
    // Do nothing
} ;
}

static void* ptr_art_work_around_app_jni_bugs()
{
    int sdkVer = sdkVersion();
    static void *art_work_around_app_jni_bugs = NULL;
    if (!art_work_around_app_jni_bugs && sdkVer >= 14 && sdkVer < 21)
    {
        void *vmHandle = dlopen(SO_ART, 0);
        art_work_around_app_jni_bugs = dlsym(vmHandle, "art_work_around_app_jni_bugs");
        dlclose(vmHandle);
    }
    return art_work_around_app_jni_bugs;
}

static JNINativeMethod gMarkMethods[] = {
        {"nativeMark", "()V", (void *) mark}
};

static int getJNINativeFuncOffset(JNIEnv *env)
{
    bool isArt = isArtMode();

    jclass javaClass = env->FindClass("com/test/dshell/DShell");

    env->RegisterNatives(javaClass, gMarkMethods, 1);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return -1;
    }

    jmethodID mtd_mark = env->GetStaticMethodID(javaClass, "nativeMark", "()V");
    if (!mtd_mark)
        return -1;

    size_t startAddress = (size_t) mtd_mark;
    size_t targetAddress = (size_t) mark;

    void* art_work_around_app_jni_bugs = ptr_art_work_around_app_jni_bugs();

//    if (art_work_around_app_jni_bugs) {
//        targetAddress = (size_t)art_work_around_app_jni_bugs;
//    }

    int offset = 0;
    bool found = false;
    size_t addr = 0;
    while (true) {

        addr = *((size_t *) (startAddress + offset));

        if (addr == targetAddress) {
            found = true;
            break;
        }
        offset += 4;
        if (offset >= 100) {
            LOGW("Ops: Unable to find the jni function.");
            break;
        }
    }

    env->UnregisterNatives(javaClass);
    env->DeleteLocalRef(javaClass);

    if (found) {
        if (!isArt) {
            offset += (sizeof(int) + sizeof(void *));
        }
        LOGW("Hoho, Get the offset : %d.", offset);

        return offset;
    }

    return -1;
}

static bool DShell::DSJNIHook::HookJNINative(JNIEnv *env, const char* className, const char* methodName, const char *methodSig, void *replace_func, void **old_func)
{
    int nativeFuncOffset = getJNINativeFuncOffset(env);

    LOGI("nativeFuncOffset %d", nativeFuncOffset);

    if (nativeFuncOffset >= 0) {
        jclass clazz = env->FindClass(className);

        jmethodID methodID = env->GetStaticMethodID(clazz, methodName, methodSig);

        if (methodID) {
            void **jniFuncPtr = (void **) ((size_t) methodID + nativeFuncOffset);
            *old_func = (void *) (*jniFuncPtr);
            *jniFuncPtr = (void *) replace_func;
            return true;
        }
    }

    return false;
}