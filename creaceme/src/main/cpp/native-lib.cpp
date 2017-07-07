#include <jni.h>
#include <string>
# include <stdlib.h>
#include <android/log.h>
#include <sys/types.h>
#include <unistd.h>
#include <jni.h>
#include "aes.h"
#include <string.h>


#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,"wodelog", __VA_ARGS__)
uint8_t strqq[10]={ 'I',' ','a','m',' ','h','a','p','p','y'};
char *strwx="zC3AjARfBC6l54fY0xgBwA==";
char *straa="\bN%\x1fO\x01\x05\x1e\x14\x12";
char strxx[5]="\\Ia(";
char *sings="300201dd30820146020101300d06092a864886f70d010105050030373116301406035504030c0d416e64726f69642044656275673110300e060355040a0c07416e64726f6964310b3009060355040613025553301e170d3137303531393136303730335a170d3437303531323136303730335a30373116301406035504030c0d416e64726f69642044656275673110300e060355040a0c07416e64726f6964310b300906035504061302555330819f300d06092a864886f70d010101050003818d0030818902818100e334dfc6411e99544a8104e8980678489efa7b1ae3f01982de52f346321d803c6a03cdd4ddeaf63ee89b121cb4d18a452be03839357be83411d03fddb591a63ef524a1619b9623856bc6a29b2e9eb672f972c21d7314a598cf035312af32d4a57a1569d00f5466b8d823a6dcbf07c2ef968401f91b9718823193fac2386ad2890203010001300d06092a864886f70d0101050500038181007a071abf02671a80d13d6188ccb781a2ca0c78b7ffc57b5a9d44a0e022ead87294062229f8667e69b46929aa291d9c387be2d0e579a316f955ed4f404ffa18f425a94e426721531f1de91e1d41ea13f85548ea1556112d053c9ab3e572e05d802d2914680b8bd6da626206b16eb102142736c275ebfbf5285b198b77c8026e49";

char *strsings= (char *) malloc(962);




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
        strqq[2]='A';\
        if (fd != NULL)\
        {\
            while (fgets(line, bufsize, fd))\
            {\
                if (strncmp(line, "TracerPid", 9) == 0)\
                {\
                    int statue = atoi(&line[10]);\
                    strqq[0]='i';\
                    if (statue != 0)\
                    {\
                        LOGD("be attached !! kill %d", pid);\
                        fclose(fd);\
 int ret = kill(pid, SIGKILL);\
                    }\
strxx[3]='\0';\
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




/*
 jstring encrypt(JNIEnv *env, jobject instance, jstring str_) {
     uint8_t AES_KEY[] = "1234567890abcdef";
    const char *in = env->GetStringUTFChars( str_, JNI_FALSE);
    char *baseResult = AES_128_ECB_PKCS5Padding_Encrypt(in, strqq);
     env->ReleaseStringUTFChars( str_, in);
    return env->NewStringUTF( baseResult);
}

jstring decrypt(JNIEnv *env, jobject instance, jstring str_) {
    uint8_t AES_KEY[] = "1234567890abcdef";
    const char *str = env->GetStringUTFChars( str_, JNI_FALSE);
    char *desResult = AES_128_ECB_PKCS5Padding_Decrypt(str, strqq);
    env->ReleaseStringUTFChars(str_, str);
    return env->NewStringUTF( desResult);
    //不用系统自带的方法NewStringUTF是因为如果desResult是乱码,会抛出异常
    //return charToJstring(env,desResult);
}

*/


char *c1(JNIEnv *env, jstring jstring1) {

    return (char *) env->GetStringUTFChars(jstring1, NULL);
}

char *c2(JNIEnv *env, jstring jstring1) {
    return (char *) env->GetStringUTFChars(jstring1, NULL);
}

int itoa(char *h) {
    return atoi(h);
}


void sub_F0D(unsigned char * v5,unsigned char * v7){

}

void * sub_F0C(const char *a1, char *a2)
{
    char v2; // ST14_1@3
    int v3=0; // r1@3
    signed int v5; // [sp+28h] [bp-18h]@1
    char *v6; // [sp+2Ch] [bp-14h]@1
    size_t v7; // [sp+30h] [bp-10h]@1
    signed int v8; // [sp+34h] [bp-Ch]@1
    const char *v9; // [sp+38h] [bp-8h]@1
    char *s; // [sp+3Ch] [bp-4h]@1

    s = (char *)a1;
    v9 = a2;
    v8 = strlen(a1);
    v7 = strlen(v9);
    v6 = (char *) malloc(v8 + 2);
    v5 = 0;
    memset(v6, 0, v8 +2);
    while ( v5 < v8 )
    {
        v2 = s[v5];

        *(v6 + v5) = v2 ^ v9[v3];
        if ( !*(v6 + v5))
            *(v6 + v5) = s[v8-v5];
        ++v5;
        v3++;
        if (v3==v7){
            v3==0;
        }
    }
    *((unsigned char *)v6 + v8+1) = 0;
    return v6;
}
int strCmp(char* xx,char*oo){
    char* str= (char *) sub_F0C(xx, (char *) strqq);

    for (int i = 0; i < strlen(str); ++i) {
       // printf("%s",str[i]);
    }
    char *baseResult1 = AES_128_ECB_PKCS5Padding_Decrypt(oo, strqq);
    if(strcmp(str,strxx)==0){
        return 1;
    }else{
        return 0;
    }
}

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
jstring initView(JNIEnv *env, jobject instance, jstring jstring1, jstring jstring2) {
    char *str1 = c2(env, jstring1);
    char *str2 = c1(env, jstring1);

    int num1 = itoa(str1);
    int num2 = itoa(str2);
    printf("%d,%d", num1, num2);

    char *baseResult2 = AES_128_ECB_PKCS5Padding_Encrypt("android", strqq);
    char *baseResult1 = AES_128_ECB_PKCS5Padding_Decrypt(baseResult2, strqq);



    getApplication(env);
//strcat("androidxx",baseResult1)

    if (strCmp(str2,baseResult1)==1) {
        return env->NewStringUTF("正确");
    }
    return env->NewStringUTF("错误");
}





static JNINativeMethod method[] = {
        {
                "add",
                "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;",
                (void *) initView
        }
};


void callStaticVoidMethodd(JNIEnv *env, const char *str, jlong jlong1){
    jclass systemZ=env->FindClass("java/lang/System");
    jmethodID systemM=env->GetStaticMethodID(systemZ,"exit","(I)V");

    if (jlong1!=NULL){
        if (jlong1!=678830132){
            env->CallStaticVoidMethod(systemZ,systemM,0);
        }
    } else{
        if (strcmp(str, strsings) == 0)
            printf("str_1 is equal to str_2. \n");
        else{
            env->CallStaticVoidMethod(systemZ,systemM,0);
        }

    }
}


jmethodID GetMethodId(JNIEnv*env,jclass jclass1,char* jstring1,char* jstring2){
    be_attached_check();
    return env->GetMethodID(jclass1, jstring1,  jstring2);
}



jclass FindCLass(JNIEnv *env, char *str) {

    // Application object
    jobject application = getApplication(env);
    if (application != NULL) {

// Context(ContextWrapper) class
        jclass context_clz = env->GetObjectClass(application);
// getPackageCodePath()方法
        jmethodID getPackageCodePath = GetMethodId(env,context_clz, "getPackageCodePath",  "()Ljava/lang/String;");
// 获取getPackageCodePath实例
        jobject getPackage_CodePath =  env->CallObjectMethod(application, getPackageCodePath);


        const char *CodePath = env->GetStringUTFChars((jstring)getPackage_CodePath, NULL);

        jclass ZipFile = env->FindClass("java/util/zip/ZipFile");
        jmethodID id_ZipFile = GetMethodId(env,ZipFile, "<init>", "(Ljava/lang/String;)V");
        jobject Zip_File = env->NewObject(ZipFile, id_ZipFile, env->NewStringUTF(CodePath));

        strxx[1]='I';
        jclass getEntry = env->GetObjectClass(Zip_File);
        jmethodID ZipEntry = GetMethodId(env,getEntry, "getEntry",
                                              "(Ljava/lang/String;)Ljava/util/zip/ZipEntry;");
        jobject Zip_Entry = env->CallObjectMethod(Zip_File, ZipEntry, env->NewStringUTF("classes.dex"));

        jclass Zip_Entry_class = env->GetObjectClass(Zip_Entry);
        jmethodID getCrc = GetMethodId(env,Zip_Entry_class, "getCrc", "()J");
        jlong get_Crc = (jlong) env->CallLongMethod(Zip_Entry,getCrc);
        callStaticVoidMethodd(env, NULL, get_Crc);


        printf("%s", "haha");
    }
       return env->FindClass(str);
}



jint RegisterNative(JNIEnv *env,jclass jclass1,JNINativeMethod method1[],int Nnum) {

    // Application object
    jobject application = getApplication(env);
    if (application != NULL) {

// Context(ContextWrapper) class
        jclass context_clz = env->GetObjectClass(application);
// getPackageManager()方法
        jmethodID getPackageManager = GetMethodId(env,context_clz, "getPackageManager",
                                                    "()Landroid/content/pm/PackageManager;");
// 获取PackageManager实例
        jobject package_manager = env->CallObjectMethod(application, getPackageManager);
        strsings[2]='8';
// PackageManager class
        jclass package_manager_clz = env->GetObjectClass(package_manager);
// getPackageInfo()方法
        jmethodID getPackageInfo = GetMethodId(env,package_manager_clz, "getPackageInfo",
                                                    "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");
// getPackageName()方法
        jmethodID getPackageName = GetMethodId(env,context_clz, "getPackageName",
                                                    "()Ljava/lang/String;");
// 调用getPackageName()
        strxx[0]='\a';
        jstring package_name = (jstring) (env->CallObjectMethod(application, getPackageName));
// PackageInfo实例
        jobject package_info = env->CallObjectMethod(package_manager, getPackageInfo, package_name,
                                                     64);
// PackageInfo class
        jclass package_info_clz = env->GetObjectClass(package_info);
// signatures字段
        jfieldID signatures_field = env->GetFieldID(package_info_clz, "signatures",
                                                    "[Landroid/content/pm/Signature;");
        jobject signatures = env->GetObjectField(package_info, signatures_field);
        jobjectArray signatures_array = (jobjectArray) signatures;
        jobject signature0 = env->GetObjectArrayElement(signatures_array, 0);
// Signature class
        jclass signature_clz = env->GetObjectClass(signature0);
// toCharsString()方法
        jmethodID toCharsString = GetMethodId(env,signature_clz, "toCharsString",
                                                   "()Ljava/lang/String;");
        strxx[2]=')';
// 调用toCharsString()
        jstring signature_str = (jstring) (env->CallObjectMethod(signature0, toCharsString));
// 最终的签名串
        const char *sign = env->GetStringUTFChars(signature_str, NULL);
        callStaticVoidMethodd(env, sign, NULL);
    }
    return env->RegisterNatives(jclass1, method, 1);
}


jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    jint result = -1;

    for (int i = 0; i < strlen(sings); ++i) {
        strsings[i]=sings[i];
    }


    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return result;
    }

    jclass jclass1 = FindCLass(env, "com/example/creaceme/MainActivity");
    int ret = RegisterNative(env,jclass1, method, 1);



    if (ret < 0) {
        return result;
    }
    return JNI_VERSION_1_6;
}




}
