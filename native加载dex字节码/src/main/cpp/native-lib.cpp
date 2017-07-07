#include <jni.h>
#include <string>
#include <dlfcn.h>
#include "Common.h"
#include "Object.h"
#include "../../../../尼古拉斯_赵四/src/main/cpp/common.h"

using namespace std;

extern "C" {


typedef void (*OPEN_DEX_FILE)(const u4 *args, JValue *pResult);
OPEN_DEX_FILE open_dex_file = NULL;

void *GetFunAddr(char *methodName, char *sig) {
    void *handle = dlopen("libdvm.so", RTLD_NOW);
    JNINativeMethod *jniNativeMethod = (JNINativeMethod *) dlsym(handle,
                                                                 "dvm_dalvik_system_DexFile");


    while (jniNativeMethod++) {
        if (strcmp(jniNativeMethod->name, methodName) == 0
            && strcmp(jniNativeMethod->signature, sig) == 0
                ) {
            dlclose(handle);
            return jniNativeMethod->fnPtr;;
        }
    }


}


jint loadDex(JNIEnv *env, jobject jobject1, jbyteArray jbyteArray1) {
    jbyte *jbyte1 = env->GetByteArrayElements(jbyteArray1, NULL);
    jsize alen = env->GetArrayLength(jbyteArray1); //获取长度
    open_dex_file = (OPEN_DEX_FILE) GetFunAddr("openDexFile", "([B)I");
    u1 *mybyte = (u1 *) jbyte1;
    ArrayObject *arrayObject = (ArrayObject *) malloc(sizeof(ArrayObject) + alen);

    int arLen = sizeof(ArrayObject);

    arrayObject->length = alen;
    memcpy(arrayObject->contents, mybyte, alen);
    u4 args[] = {*(u4 *) &arrayObject};
    JValue jResult = {};

    open_dex_file(args, &jResult);
    return jResult.i;
}


static JNINativeMethod method[] = {

        {"loadDex",
                "([B)I",
                (void *) loadDex
        }

};


jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    jint result = -1;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return result;
    }

    jclass jclass1 = env->FindClass("com/example/nativedex/MyDexClassLoader");
    int ret = env->RegisterNatives(jclass1, method, 1);
    if (ret < 0) {
        return result;
    }
    return JNI_VERSION_1_6;

}


}







