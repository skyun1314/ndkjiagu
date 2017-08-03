//
// Created by liam on 2016/8/30.
//

#include "XJniDShell.h"
#include "core/FastLoadDex.h"
#include <dlfcn.h>
#include "common.h"
#include "MSHook/Hooker.h"
#include "MemDex/DSMemDex.h"
#include "MemDex/DSJNIHook.h"

class DSEnv{
    JNIEnv* m_env;
    JavaVM* m_jvm;
public:
    DSEnv(JavaVM* jvm)
    {
        m_env = NULL;
        m_jvm = jvm;
        m_jvm->GetEnv((void **) &m_env, JNI_VERSION_1_4);
    }

    DSEnv(JNIEnv* env)
    {
        m_env = env;
        m_jvm = NULL;
    }

    operator JNIEnv*()
    {
        return getEnv();
    }

    JNIEnv* operator ->()
    {
        return getEnv();
    }

    JNIEnv* getEnv()
    {
        return m_env;
    }

    ~DSEnv()
    {
    }
};

//onLoad方法，在System.loadLibrary()执行时被调用
jint JNI_OnLoad(JavaVM* vm, void* reserved){
    LOGI("JNI_OnLoad startup~~!");
//    Cydia::elfEnumFunction("libart.so", [](const char *name)->bool {
//        if (strstr(name, "art7DexFile4Open"))
//        {
//            LOGI("libart.so function name %s", name);
//        }
//        return true;
//    });
//
//    Cydia::elfHookFunction("libart.so", "_ZN3art7DexFile4OpenEPKcS2_PNSt3__112basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEEPNS3_6vectorIPKS0_NS7_ISD_EEEE", (void*) my_open, (void**) &org_open);

    enableFastLoadDex();

    DSEnv env(vm);

    DShell::DSMemDex::HookOpenDexFileNative(env);

    //DShell::DSMemDex::LoadByte(NULL, 0);

    return JNI_VERSION_1_4;
}

//onUnLoad方法，在JNI组件被释放时调用
void JNI_OnUnload(JavaVM* vm, void* reserved){
    LOGI("call JNI_OnUnload ~~!!");
    closeFastLoadDex();
}

JNIEXPORT void JNICALL Java_com_test_dshell_DShell_nativeInit
        (JNIEnv *env, jobject thiz)
{

}
