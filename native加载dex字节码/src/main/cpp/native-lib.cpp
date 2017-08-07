#include <jni.h>
#include <string>
#include <dlfcn.h>
#include "Common.h"
#include "Object.h"
#include "DSMemDexArt.h"
# include <stdlib.h>
#include <zlib.h>
#include <android/log.h>
#include <sys/types.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>


char *sings = "300201dd30820146020101300d06092a864886f70d010105050030373116301406035504030c0d416e64726f69642044656275673110300e060355040a0c07416e64726f6964310b3009060355040613025553301e170d3137303531393136303730335a170d3437303531323136303730335a30373116301406035504030c0d416e64726f69642044656275673110300e060355040a0c07416e64726f6964310b300906035504061302555330819f300d06092a864886f70d010101050003818d0030818902818100e334dfc6411e99544a8104e8980678489efa7b1ae3f01982de52f346321d803c6a03cdd4ddeaf63ee89b121cb4d18a452be03839357be83411d03fddb591a63ef524a1619b9623856bc6a29b2e9eb672f972c21d7314a598cf035312af32d4a57a1569d00f5466b8d823a6dcbf07c2ef968401f91b9718823193fac2386ad2890203010001300d06092a864886f70d0101050500038181007a071abf02671a80d13d6188ccb781a2ca0c78b7ffc57b5a9d44a0e022ead87294062229f8667e69b46929aa291d9c387be2d0e579a316f955ed4f404ffa18f425a94e426721531f1de91e1d41ea13f85548ea1556112d053c9ab3e572e05d802d2914680b8bd6da626206b16eb102142736c275ebfbf5285b198b77c8026e49";

typedef void (*OPEN_DEX_FILE)(const u4 *args, JValue *pResult);

OPEN_DEX_FILE open_dex_file = NULL;
bool isArt;
using namespace std;
char *strsings = (char *) malloc(962);


#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,"wodelog", __VA_ARGS__)
#define  be_attached_check()\
{\
    try\
    {\
        const int bufsize = 1024;\
        char filename[bufsize];\
        char line[bufsize];\
        int pid = getpid();\
        sprintf(filename, "/proc/%d/status", pid);\
        FILE* fd = fopen(filename, "r");\
        if (fd != NULL)\
        {\
            while (fgets(line, bufsize, fd))\
            {\
                if (strncmp(line, "TracerPid", 9) == 0)\
                {\
                    int statue = atoi(&line[10]);\
                    if (statue != 0)\
                    {\
                        LOGD("be attached !! kill %d", pid);\
                        fclose(fd);\
                    }\
                    break;\
                }\
            }\
            fclose(fd);\
        } else\
        {\
            LOGD("open %s fail...", filename);\
        }\
    } catch (...)\
    {\
    }\
}



extern "C" {



jbyte *GetbyteArrayElements(JNIEnv *env, jbyteArray array, jboolean *isCopy) {
    //be_attached_check();
    return env->GetByteArrayElements(array, NULL);
}

int strCmp(const char *str1, const char *str2) {
    //be_attached_check();
    return strcmp(str1, str2);
}

void *GetFunAddr(char *methodName, char *sig) {
    void *handle = dlopen("libdvm.so", RTLD_NOW);
    JNINativeMethod *jniNativeMethod = (JNINativeMethod *) dlsym(handle,
                                                                 "dvm_dalvik_system_DexFile");

    //be_attached_check();
    while (jniNativeMethod++) {
        if (strCmp(jniNativeMethod->name, methodName) == 0
            && strCmp(jniNativeMethod->signature, sig) == 0
                ) {
            dlclose(handle);
            return jniNativeMethod->fnPtr;;
        }
    }


}

jint loadDavlikDex(JNIEnv *env) {
    open_dex_file = (OPEN_DEX_FILE) GetFunAddr("openDexFile", "([B)I");
    u1 *mybyte = (u1 *) DSMemDexArt::the_dex_byte;
    ArrayObject *arrayObject = (ArrayObject *) malloc(sizeof(ArrayObject) + DSMemDexArt::the_dex_size);

    int arLen = sizeof(ArrayObject);

    arrayObject->length = DSMemDexArt::the_dex_size;
    memcpy(arrayObject->contents, mybyte, DSMemDexArt::the_dex_size);
    u4 args[] = {*(u4 *) &arrayObject};
    JValue jResult = {};

    open_dex_file(args, &jResult);
    return jResult.i;
}





void *loadDex(JNIEnv *env, jobject jobject1,  jobject obj) {



    DSMemDexArt::copyFile(env, obj);

    //jsize alen = env->GetArrayLength(jbyteArray1); //获取长度
   // jbyte *jbyte1 = GetbyteArrayElements(env, jbyteArray1, NULL);
    if (isArt) {

        return (void *) DSMemDexArt::LoadByte(env, obj);

    } else {
        return (void *) loadDavlikDex(env);
    }

}


static JNINativeMethod method[] = {

        {"attachBaseContext",
                "(Landroid/content/Context;)V",
                (void *) loadDex
        }

};


jobject getApplication(JNIEnv *env) {
    jobject application = NULL;
    jclass activity_thread_clz = env->FindClass("android/app/ActivityThread");
    if (activity_thread_clz != NULL) {
        jmethodID currentApplication = env->GetStaticMethodID(
                activity_thread_clz, "currentApplication", "()Landroid/app/Application;");
        if (currentApplication != NULL) {
            application = env->CallStaticObjectMethod(activity_thread_clz, currentApplication);
        } else {
            //  LOGE("Cannot find method: currentApplication() in ActivityThread.");
        }
        env->DeleteLocalRef(activity_thread_clz);
    } else {
        // LOGE("Cannot find class: android.app.ActivityThread");
    }

    return application;
}

jmethodID GetMethodId(JNIEnv *env, jclass jclass1, char *jstring1, char *jstring2) {
    //be_attached_check();
    return env->GetMethodID(jclass1, jstring1, jstring2);
}


void callStaticVoidMethodd(JNIEnv *env, const char *str, jlong jlong1) {
    jclass systemZ = env->FindClass("java/lang/System");
    jmethodID systemM = env->GetStaticMethodID(systemZ, "exit", "(I)V");
    //be_attached_check();
    if (jlong1 != NULL) {
        if (jlong1 != 678830132) {
            //env->CallStaticVoidMethod(systemZ,systemM,0);
        }
    } else {
        if (strCmp(str, strsings) == 0)
            printf("str_1 is equal to str_2. \n");
        else {
            //env->CallStaticVoidMethod(systemZ,systemM,0);
        }
        //be_attached_check();
    }
}

jint RegisterNative(JNIEnv *env, jclass jclass1, JNINativeMethod method1[], int Nnum) {
    //be_attached_check();
    // Application object
    jobject application = getApplication(env);
    if (application != NULL) {
        // be_attached_check();
// Context(ContextWrapper) class
        jclass context_clz = env->GetObjectClass(application);
// getPackageManager()方法
        jmethodID getPackageManager = GetMethodId(env, context_clz, "getPackageManager",
                                                  "()Landroid/content/pm/PackageManager;");
// 获取PackageManager实例
        jobject package_manager = env->CallObjectMethod(application, getPackageManager);
        strsings[2] = '8';
// PackageManager class
        jclass package_manager_clz = env->GetObjectClass(package_manager);
// getPackageInfo()方法
        jmethodID getPackageInfo = GetMethodId(env, package_manager_clz, "getPackageInfo",
                                               "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");
// getPackageName()方法
        jmethodID getPackageName = GetMethodId(env, context_clz, "getPackageName",
                                               "()Ljava/lang/String;");
// 调用getPackageName()
        jstring package_name = (jstring) (env->CallObjectMethod(application, getPackageName));
        // be_attached_check();
// PackageInfo实例
        jobject package_info = env->CallObjectMethod(package_manager, getPackageInfo, package_name,
                                                     64);
// PackageInfo class
        jclass package_info_clz = env->GetObjectClass(package_info);
// signatures字段
        jfieldID signatures_field = env->GetFieldID(package_info_clz, "signatures",
                                                    "[Landroid/content/pm/Signature;");
        //  be_attached_check();
        jobject signatures = env->GetObjectField(package_info, signatures_field);
        jobjectArray signatures_array = (jobjectArray) signatures;
        jobject signature0 = env->GetObjectArrayElement(signatures_array, 0);
// Signature class
        jclass signature_clz = env->GetObjectClass(signature0);
// toCharsString()方法
        jmethodID toCharsString = GetMethodId(env, signature_clz, "toCharsString",
                                              "()Ljava/lang/String;");
// 调用toCharsString()
        jstring signature_str = (jstring) (env->CallObjectMethod(signature0, toCharsString));
// 最终的签名串
        const char *sign = env->GetStringUTFChars(signature_str, NULL);
        callStaticVoidMethodd(env, sign, NULL);
        ///be_attached_check();
    }
    //be_attached_check();
    return env->RegisterNatives(jclass1, method, Nnum);
}


//检查dex合法性
jclass FindCLass(JNIEnv *env, char *str) {
    // be_attached_check();
    // Application object
    jobject application = getApplication(env);
    if (application != NULL) {

// Context(ContextWrapper) class
        jclass context_clz = env->GetObjectClass(application);
// getPackageCodePath()方法
        jmethodID getPackageCodePath = GetMethodId(env, context_clz, "getPackageCodePath",
                                                   "()Ljava/lang/String;");
// 获取getPackageCodePath实例
        jobject getPackage_CodePath = env->CallObjectMethod(application, getPackageCodePath);

        //  be_attached_check();
        const char *CodePath = env->GetStringUTFChars((jstring) getPackage_CodePath, NULL);

        jclass ZipFile = env->FindClass("java/util/zip/ZipFile");
        jmethodID id_ZipFile = GetMethodId(env, ZipFile, "<init>", "(Ljava/lang/String;)V");
        jobject Zip_File = env->NewObject(ZipFile, id_ZipFile, env->NewStringUTF(CodePath));
        //  be_attached_check();
        jclass getEntry = env->GetObjectClass(Zip_File);
        //  be_attached_check();
        jmethodID ZipEntry = GetMethodId(env, getEntry, "getEntry",
                                         "(Ljava/lang/String;)Ljava/util/zip/ZipEntry;");
        jobject Zip_Entry = env->CallObjectMethod(Zip_File, ZipEntry,
                                                  env->NewStringUTF("classes.dex"));
        // be_attached_check();
        jclass Zip_Entry_class = env->GetObjectClass(Zip_Entry);
        jmethodID getCrc = GetMethodId(env, Zip_Entry_class, "getCrc", "()J");
        jlong get_Crc = (jlong) env->CallLongMethod(Zip_Entry, getCrc);
        callStaticVoidMethodd(env, NULL, get_Crc);
        //be_attached_check();

        printf("%s", "haha");
    }
    return env->FindClass(str);
}


char *c1(JNIEnv *env, jstring jstring1) {

    return (char *) env->GetStringUTFChars(jstring1, NULL);
}

char *c2(JNIEnv *env, jstring jstring1) {
    return (char *) env->GetStringUTFChars(jstring1, NULL);
}

int itoa(char *h) {
    return atoi(h);
}


void sub_F0D(unsigned char *v5, unsigned char *v7) {

}

void *sub_F0C(const char *a1, char *a2) {
    char v2; // ST14_1@3
    int v3 = 0; // r1@3
    signed int v5; // [sp+28h] [bp-18h]@1
    char *v6; // [sp+2Ch] [bp-14h]@1
    size_t v7; // [sp+30h] [bp-10h]@1
    signed int v8; // [sp+34h] [bp-Ch]@1
    const char *v9; // [sp+38h] [bp-8h]@1
    char *s; // [sp+3Ch] [bp-4h]@1

    s = (char *) a1;
    v9 = a2;
    v8 = strlen(a1);
    v7 = strlen(v9);
    v6 = (char *) malloc(v8 + 2);
    v5 = 0;
    memset(v6, 0, v8 + 2);
    while (v5 < v8) {
        v2 = s[v5];

        *(v6 + v5) = v2 ^ v9[v3];
        if (!*(v6 + v5))
            *(v6 + v5) = s[v8 - v5];
        ++v5;
        v3++;
        if (v3 == v7) {
            v3 == 0;
        }
    }
    *((unsigned char *) v6 + v8 + 1) = 0;
    return v6;
}


jobject getGlobalContext(JNIEnv *env) {
    jclass activityThread = env->FindClass("android/app/ActivityThread");

    jmethodID currentActivityThread = env->GetStaticMethodID(activityThread,
                                                             "currentActivityThread",
                                                             "()Landroid/app/ActivityThread;");

    jobject at = env->CallStaticObjectMethod(activityThread, currentActivityThread);

    jmethodID getApplication = env->GetMethodID(activityThread, "getApplication",
                                                "()Landroid/app/Application;");

    jobject context = env->CallObjectMethod(at, getApplication);

    return context;
}



char *jstringTostring(JNIEnv *env, jstring str) {
    char *rtn = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("utf-8");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr = (jbyteArray) env->CallObjectMethod(str, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte *ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0) {
        rtn = (char *) malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->DeleteLocalRef(clsstring);
    env->DeleteLocalRef(strencode);
    env->ReleaseByteArrayElements(barr, ba, 0);
    return rtn;
}


static void init(JNIEnv *env) {
    jclass jclazz = env->FindClass("android/os/Build$VERSION");
    jfieldID SDK_INT = env->GetStaticFieldID(jclazz, "SDK_INT", "I");

    jclass System = env->FindClass("java/lang/System");
    jmethodID System_getProperty = env->GetStaticMethodID(System, "getProperty",
                                                          "(Ljava/lang/String;)Ljava/lang/String;");

    jstring vm_version_name = env->NewStringUTF("java.vm.version");
    jstring vm_version_value = static_cast<jstring>(env->CallStaticObjectMethod(System,
                                                                                System_getProperty,
                                                                                vm_version_name));
    char *cvm_version_value = jstringTostring(env, vm_version_value);
    env->DeleteLocalRef(vm_version_name);
    env->DeleteLocalRef(vm_version_value);
    double version = atof(cvm_version_value);
    free(cvm_version_value);
    if (version >= 2) {
        isArt = true;
    } else {
        isArt = false;
    }


}


jint JNI_OnLoad(JavaVM *vm, void *reserved) {

    /*char* str= DSMemDexArt::execute("nm -D libart.so | grep OpenMemory");

     if(strstr(str,"") == NULL){

     }*/





    for (int i = 0; i < strlen(sings); ++i) {
        strsings[i] = sings[i];
    }


    JNIEnv *env = NULL;
    jint result = -1;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return result;
    }
    getApplication(env);
    jclass jclass1 = FindCLass(env, "com/example/nativedex/MyAppLication");
    int ret = RegisterNative(env, jclass1, method, 1);
    init(env);

    if (ret < 0) {
        return result;
    }
    return JNI_VERSION_1_6;

}


}







