#include <jni.h>
#include <android/log.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "mycommom.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <dlfcn.h>
#include <sys/mman.h>


void native_attachContextBaseContext(JNIEnv *env, jclass clazz, jobject ctx);

void native_onCreate(JNIEnv *env, jclass clazz);

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved);

#define  TAG    "info"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

# define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))
#define JNIREG_CLASS "com/example/unpack/StubApplication"
//#define JNIREG_CLASS "com/ali/mobisecenhance/StubApplication"


jclass Build_version;
jint sdk_int;
jclass ActivityThread;
jfieldID mPackages;
jclass myArrayMap;
jmethodID ArrayMap_get;
jfieldID mBoundApplication;
jfieldID mInitialApplication;
jfieldID mAllApplications;
jmethodID currentActivityThread;

jclass AppBindData;
jfieldID AppBindData_info;

jclass myArrayList;
jmethodID arraylist_size;
jmethodID arraylist_get;
jmethodID arraylist_set;


jclass myContext;
jmethodID context_getPackageName;
jmethodID context_getApplicationInfo;
jmethodID context_getClassLoader;
jmethodID context_getAssets;
jmethodID context_getPackageResourePath;

jclass myWeakReference;
jmethodID WeakReference_get;

jclass myLoadedApk;
jfieldID LoadedApk_mClassLoader;
jfieldID LoadedApk_mApplication;

jclass myApplicationInfo;
jfieldID ApplicationInfo_dataDir;
jfieldID ApplicationInfo_nativeLibraryDir;
jfieldID ApplicationInfo_sourceDir;

jclass myApplication;
jmethodID Application_onCreate;
jmethodID Application_attach;

jclass myContextWrapper;
jmethodID ContextWrapper_attachBaseContext;

jclass myPathClassLoader;

jclass myBaseDexClassLoader;
jfieldID BaseDexClassLoader_pathList;

jclass myDexPathList;
jfieldID DexPathList_dexElements;

jclass myElement;
jfieldID DexPathList_Element_dexFile;
jfieldID DexPathList_Element_file;

jclass myFile;
jmethodID myFile_getAbsolutePath;

jclass myDexFile;
jfieldID mCookie;
jmethodID myOpenDexFile;

jclass myClassLoader;
jmethodID classLoader_findClass;

jclass mySystem;


jmethodID system_getProperty;
jclass status;

jmethodID SystemProperties_get;
jclass mySystemProperties;

int isDalvik;
int isArt;

jobject onCreateObj;
const char *mPackageName;
const char *mAbsolutePath_str;

JNINativeMethod *dvm_dalvik_system_DexFile;

void (*openDexFile)(const u4 *args, union JValue *pResult);

typedef int (*pArtFun)(int, int);

int dvm_Cookie = 0;//4.4
jlong art_Cookie = 0;//5.0-5.1
jobject art_MarCookie = 0;//6.0

jclass myFindClass(JNIEnv *env, jclass *ptr, char *name) {
    jobject g_cls_string;
    jclass clazz = env->FindClass(name);
    if (clazz) {
        g_cls_string = env->NewGlobalRef(clazz);
        *ptr = (jclass) g_cls_string;
        return (jclass) g_cls_string;
    } else {
        return 0;
    }

}

void init_class(JNIEnv *env, jobject obj, jobject ctx) {
    if (!myFindClass(env, &Build_version, "android/os/Build$VERSION")) {
        LOGI("ERROR:Build$VERSION");
        return;
    }

    jfieldID fieldID = (env->GetStaticFieldID)(Build_version, "SDK_INT", "I");
    sdk_int = env->GetStaticIntField(Build_version, fieldID);
    LOGI("sdk_int %d\n", sdk_int);


    if (!myFindClass(env, &ActivityThread, "android/app/ActivityThread")) {
        LOGI("ERROR:ActivityThread");
        return;
    }

    if (sdk_int > 18) {
        mPackages = env->GetFieldID(ActivityThread, "mPackages", "Landroid/util/ArrayMap;");
        if (!myFindClass(env, &myArrayMap, "android/util/ArrayMap")) {
            LOGI("ERROR:myArrayMap");
            return;
        }

        ArrayMap_get = env->GetMethodID(myArrayMap, "get",
                                        "(Ljava/lang/Object;)Ljava/lang/Object;");


        mBoundApplication = env->GetFieldID(
                ActivityThread,
                "mBoundApplication",
                "Landroid/app/ActivityThread$AppBindData;");
        mInitialApplication = env->GetFieldID(
                ActivityThread,
                "mInitialApplication",
                "Landroid/app/Application;");
        mAllApplications = env->GetFieldID(
                ActivityThread,
                "mAllApplications",
                "Ljava/util/ArrayList;");
        currentActivityThread = env->GetStaticMethodID(
                ActivityThread,
                "currentActivityThread",
                "()Landroid/app/ActivityThread;");
        //LOGI("ActivityThread:%p,%p,%p,%p",mBoundApplication,mInitialApplication,mAllApplications,currentActivityThread);

        if (!myFindClass(env, &AppBindData, "android/app/ActivityThread$AppBindData")) {
            LOGI("ERROR:AppBindData");
            return;
        }
        AppBindData_info = env->GetFieldID(AppBindData, "info", "Landroid/app/LoadedApk;");


        if (!myFindClass(env, &myArrayList, "java/util/ArrayList")) {
            LOGI("ERROR:myArrayList");
            return;
        }
        arraylist_size = env->GetMethodID(myArrayList, "size", "()I");
        arraylist_get = env->GetMethodID(myArrayList, "get", "(I)Ljava/lang/Object;");
        arraylist_set = env->GetMethodID(myArrayList, "set",
                                         "(ILjava/lang/Object;)Ljava/lang/Object;");


        if (!myFindClass(env, &myContext, "android/content/Context")) {
            LOGI("ERROR:myContext");
            return;
        }
        context_getPackageName = env->GetMethodID(myContext, "getPackageName",
                                                  "()Ljava/lang/String;");
        context_getApplicationInfo = env->GetMethodID(
                myContext,
                "getApplicationInfo",
                "()Landroid/content/pm/ApplicationInfo;");
        context_getClassLoader = env->GetMethodID(
                myContext,
                "getClassLoader",
                "()Ljava/lang/ClassLoader;");
        context_getAssets = env->GetMethodID(
                myContext,
                "getAssets",
                "()Landroid/content/res/AssetManager;");
        context_getPackageResourePath = env->GetMethodID(
                myContext,
                "getPackageResourcePath",
                "()Ljava/lang/String;");


        if (!myFindClass(env, &myWeakReference, "java/lang/ref/WeakReference")) {
            LOGI("ERROR:myWeakReference");
            return;
        }
        WeakReference_get = env->GetMethodID(myWeakReference, "get", "()Ljava/lang/Object;");


        if (!myFindClass(env, &myLoadedApk, "android/app/LoadedApk")) {
            LOGI("ERROR:myLoadedApk");
            return;
        }
        LoadedApk_mClassLoader = env->GetFieldID(
                myLoadedApk,
                "mClassLoader",
                "Ljava/lang/ClassLoader;");
        LoadedApk_mApplication = env->GetFieldID(
                myLoadedApk,
                "mApplication",
                "Landroid/app/Application;");


        if (!myFindClass(env, &myApplicationInfo, "android/content/pm/ApplicationInfo")) {
            LOGI("ERROR:myApplicationInfo");
            return;
        }
        ApplicationInfo_dataDir = env->GetFieldID(
                myApplicationInfo,
                "dataDir",
                "Ljava/lang/String;");
        ApplicationInfo_nativeLibraryDir = env->GetFieldID(
                myApplicationInfo,
                "nativeLibraryDir",
                "Ljava/lang/String;");
        ApplicationInfo_sourceDir = env->GetFieldID(
                myApplicationInfo,
                "sourceDir",
                "Ljava/lang/String;");


        if (!myFindClass(env, &myApplication, "android/app/Application")) {
            LOGI("ERROR:myApplication");
            return;
        }
        Application_onCreate = env->GetMethodID(myApplication, "onCreate", "()V");
        Application_attach = env->GetMethodID(
                myApplication,
                "attach",
                "(Landroid/content/Context;)V");


        if (!myFindClass(env, &myContextWrapper, "android/content/ContextWrapper")) {
            LOGI("ERROR:myContextWrapper");
            return;
        }
        ContextWrapper_attachBaseContext = env->GetMethodID(
                myContextWrapper,
                "attachBaseContext",
                "(Landroid/content/Context;)V");


        LOGI("PathClassLoader start");
        if (!myFindClass(env, &myPathClassLoader, "dalvik/system/PathClassLoader")) {
            LOGI("ERROR:myPathClassLoader");
            return;
        }
        if (sdk_int > 13) {

            if (!myFindClass(env, &myBaseDexClassLoader, "dalvik/system/BaseDexClassLoader")) {
                LOGI("ERROR:myBaseDexClassLoader");
                return;
            }

            BaseDexClassLoader_pathList = env->GetFieldID(
                    myBaseDexClassLoader,
                    "pathList",
                    "Ldalvik/system/DexPathList;");
            if (!myFindClass(env, &myDexPathList, "dalvik/system/DexPathList")) {
                LOGI("ERROR:myDexPathList");
                return;
            }

            DexPathList_dexElements = env->GetFieldID(
                    myDexPathList,
                    "dexElements",
                    "[Ldalvik/system/DexPathList$Element;");

            if (!myFindClass(env, &myElement, "dalvik/system/DexPathList$Element")) {
                LOGI("ERROR:myElement");
                return;
            }
            DexPathList_Element_dexFile = env->GetFieldID(
                    myElement,
                    "dexFile",
                    "Ldalvik/system/DexFile;");
            if (sdk_int > 22)//6.0
                DexPathList_Element_file = env->GetFieldID(myElement, "dir", "Ljava/io/File;");
            else
                DexPathList_Element_file = env->GetFieldID(myElement, "file", "Ljava/io/File;");
            if (!myFindClass(env, &myFile, "java/io/File")) {
                LOGI("ERROR:myFile");
                return;
            }
            myFile_getAbsolutePath = env->GetMethodID(
                    myFile,
                    "getAbsolutePath",
                    "()Ljava/lang/String;");
            LOGI("PathClassLoader end");

            if (!myFindClass(env, &myDexFile, "dalvik/system/DexFile")) {
                LOGI("ERROR:myDexFile");
                return;
            }
            if (sdk_int > 22) {
                mCookie = env->GetFieldID(myDexFile, "mCookie", "Ljava/lang/Object;");
                myOpenDexFile = env->GetStaticMethodID(myDexFile, "openDexFile",
                                                       "(Ljava/lang/String;Ljava/lang/String;I)Ljava/lang/Object;");
            } else if (sdk_int > 19) {
                mCookie = env->GetFieldID(myDexFile, "mCookie", "J");

                myOpenDexFile = env->GetStaticMethodID(myDexFile, "openDexFile",
                                                       "(Ljava/lang/String;Ljava/lang/String;I)J");

            } else {
                mCookie = env->GetFieldID(myDexFile, "mCookie", "I");
                myOpenDexFile = env->GetStaticMethodID(myDexFile, "openDexFile",
                                                       "(Ljava/lang/String;Ljava/lang/String;I)I");

            }

            if (!myFindClass(env, &myClassLoader, "java/lang/ClassLoader")) {
                LOGI("ERROR:myClassLoader");
                return;
            }
            //android 5+以上无法用findClass找到android.app.Application类
            classLoader_findClass = env->GetMethodID(
                    myClassLoader,
                    "loadClass",
                    "(Ljava/lang/String;)Ljava/lang/Class;");

            LOGI("System start");
            if (!myFindClass(env, &mySystem, "java/lang/System")) {
                LOGI("ERROR:myClassLoader");
                return;
            }
            system_getProperty = env->GetStaticMethodID(
                    mySystem,
                    "getProperty",
                    "(Ljava/lang/String;)Ljava/lang/String;");

            LOGI("SystemProperties start");

            status = myFindClass(env,
                                 &mySystemProperties,
                                 "android/os/SystemProperties");

            if (status) {
                /*            SystemProperties_get = env->GetStaticMethodID(
                                    env,
                                    mySystemProperties,
                                    "get",
                                    "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");*/
                SystemProperties_get = env->GetStaticMethodID(
                        mySystemProperties,
                        "get",
                        "(Ljava/lang/String;)Ljava/lang/String;");

                jstring vmname = env->NewStringUTF("java.vm.name");
                jobject tmp = env->CallStaticObjectMethod(mySystem, system_getProperty, vmname);
                const char *v22 = env->GetStringUTFChars((jstring) tmp, 0);
                LOGI("------- vmNameStr:%s", v22);
                env->ReleaseStringUTFChars((jstring) tmp, v22);

                // persist.sys.dalvik.vm.lib
                // persist.sys.dalvik.vm.lib.2
                jstring vmlibstr = env->NewStringUTF("persist.sys.dalvik.vm.lib.2");

                //这里反编译出错
                jobject runtime = env->CallStaticObjectMethod(mySystemProperties,
                                                              SystemProperties_get, vmlibstr);

                const char *v28 = env->GetStringUTFChars((jstring) runtime, 0);


                env->ReleaseStringUTFChars((jstring) runtime, v28);

                jstring vm_version = env->NewStringUTF("java.vm.version");
                jobject v32 = env->CallStaticObjectMethod(mySystem, system_getProperty, vm_version);
                const char *runtime_version = env->GetStringUTFChars((jstring) v32, 0);
                LOGI("---- vmVersionStr:%s", runtime_version);
                double d = atof(runtime_version);

                if (d > 2)
                    isDalvik = 0;
                else
                    isDalvik = 1;
                env->ReleaseStringUTFChars((jstring) v32, runtime_version);

                return;
            }

        }
    }
}


int lookup(JNINativeMethod *table, const char *name, const char *sig,
           void (**fnPtrout)(u4 const *, union JValue *)) {
    int i = 0;
    while (table[i].name != NULL) {
        LOGI("lookup %d %s", i, table[i].name);
        if ((strcmp(name, table[i].name) == 0)
            && (strcmp(sig, table[i].signature) == 0)) {
            *fnPtrout = (void (*)(const u4 *, JValue *)) table[i].fnPtr;
            return 1;
        }
        i++;
    }
    return 0;
}

void myExtractFile(JNIEnv *env, jobject application_obj, const char *szPath) {
    AAssetManager *mgr;

    if (access(szPath, R_OK)) {
        jclass v19 = env->GetObjectClass(application_obj);
        jmethodID v20 = env->GetMethodID(v19, "getAssets", "()Landroid/content/res/AssetManager;");
        jobject v22 = env->CallObjectMethod(application_obj, v20);

        mgr = AAssetManager_fromJava(env, v22);
        if (mgr == NULL) {
            LOGI(" %s", "AAssetManager==NULL");
            return;
        }
        AAsset *asset = AAssetManager_open(mgr, "dump.dex", AASSET_MODE_STREAMING);
        FILE *file;
        void *buffer;
        int numBytesRead;
        if (asset) {
            file = fopen(szPath, "w");
            //       int bufferSize = AAsset_getLength(asset);
            //       LOGI("buffersize is %d",bufferSize);
            buffer = malloc(1024);
            while (1) {
                numBytesRead = AAsset_read(asset, buffer, 1024);
                if (numBytesRead <= 0)
                    break;
                fwrite(buffer, numBytesRead, 1, file);
            }
            free(buffer);
            fclose(file);
            AAsset_close(asset);
        } else {
            LOGI("Error AAssetManager_open");
            return;
        }

    } else {
        LOGI("dump.dex existed");
    }
}

void myLoadDex_dvm(JNIEnv *env, char *szPath) {

    void *ldvm = (void *) dlopen("libdvm.so", 1);
    JNINativeMethod *dvm_dalvik_system_DexFile = (JNINativeMethod *) dlsym(ldvm,
                                                                           "dvm_dalvik_system_DexFile");
    if (0 == lookup(dvm_dalvik_system_DexFile, "openDexFile", "([B)I", &openDexFile)) {
        openDexFile = NULL;
        LOGI("method does not found ");
        return;
    } else {
        LOGI("openDexFile method found ! HAVE_BIG_ENDIAN");
    }
    int handle;
    struct stat buf = {0};

    handle = open(szPath, 0);
    LOGI("handle:%X\n", handle);
    if (!handle) {
        LOGI("open dump.dex failed");
        return;
    }
    int status = fstat(handle, &buf);
    if (status) {
        LOGI("fstat failed");
        return;
    }
    int dexLen = buf.st_size;
    LOGI("dexLen:%d,st_blksize:%d", dexLen, (int) buf.st_blksize);
    //#define PROT_READ 0x1     /* Page can be read.  */
    //#define PROT_WRITE  0x2     /* Page can be written.  */
    //#define PROT_EXEC 0x4     /* Page can be executed.  */
    //#define PROT_NONE 0x0     /* Page can not be accessed.  */

    //#define MAP_SHARED  0x01    /* Share changes.  */
    //#define MAP_PRIVATE 0x02    /* Changes are private.  */

    char *dexbase = (char *) mmap(0, dexLen, 3, 2, handle, 0);
    LOGI("dex magic %c %c %c %c %c %c %c",
         *dexbase,
         *(dexbase + 1),
         *(dexbase + 2),
         *(dexbase + 3),
         *(dexbase + 4),
         *(dexbase + 5),
         *(dexbase + 6));

    //将dex映射进入内存
    char *arr;
    arr = (char *) malloc(16 + dexLen);
    ArrayObject *ao = (ArrayObject *) arr;
    LOGI("sizeof ArrayObject:%d", sizeof(ArrayObject));
    ao->length = dexLen;
    memcpy(arr + 16, dexbase, dexLen);
    munmap(dexbase, dexLen);

    union JValue pResult;
    u4 args[] = {*(u4 *) &ao};
   // jint result;
    if (openDexFile != NULL) {
        openDexFile(args, &pResult);
        //result = (jint) pResult.l;
        dvm_Cookie = (jint) pResult.i;
        LOGI("Dalvik Cookie :%X",  pResult.i);
    }
}

void myLoadDex_art(JNIEnv *env, char *szPath) {

    jstring inPath = env->NewStringUTF(szPath);

    if (sdk_int > 22) {
        art_MarCookie = env->CallStaticObjectMethod(myDexFile, myOpenDexFile, inPath, 0, 0);
        LOGI("----MarCookie:%p", art_MarCookie);
    } else {
        art_Cookie = env->CallStaticLongMethod(myDexFile, myOpenDexFile, inPath, 0, 0);
        LOGI("----artCookie:%llx", art_Cookie);
    }

    void *dlart = dlopen("libart.so", 1);
    pArtFun pArtDexFileFindClassDef = (pArtFun) dlsym(dlart, "_ZNK3art7DexFile12FindClassDefEt");
    LOGI("pArtDexFileFindClassDef:%p", pArtDexFileFindClassDef);


}

void replace_classloader_cookie(JNIEnv *env, jobject classLoader) {
    if (sdk_int > 13) {
        // "java/lang/ClassLoader"
        jobject v28 = env->GetObjectField(classLoader, BaseDexClassLoader_pathList);
        //获取Elements数组
        jobject v15 = env->GetObjectField(v28, DexPathList_dexElements);
        int count = env->GetArrayLength((jarray) v15);
        //count一直是1
        LOGI("element count: %d", count);
        int i = 0;
        while (i < count) {
            jobject Element = env->GetObjectArrayElement((jobjectArray) v15, i);

            jobject fileclazz = env->GetObjectField(Element, DexPathList_Element_file);// 获取file类
            jobject v20 = env->CallObjectMethod(fileclazz,
                                                myFile_getAbsolutePath);// file.getAbsolutePath()
            const char *str = env->GetStringUTFChars((jstring) v20, 0);
            //android 6.0下str为:/
            LOGI("element is %s", str);
            //
            /*int length = (env->GetStringUTFLength)( v20);
            int cmpstatus = strncasecmp("apk", (length - 3 + str), 3);
            (env->ReleaseStringUTFChars)( v20, str);*/
            jobject DexFile = env->GetObjectField(Element,
                                                  DexPathList_Element_dexFile);// 获取DexFile类

            if (sdk_int <= 19) {
                LOGI("SetIntField");
                LOGI("---dvm_Cookie:%X", dvm_Cookie);
                env->SetIntField(DexFile, mCookie, dvm_Cookie);
            } else if (sdk_int > 22) {
                LOGI("SetObjectField");
                LOGI("---art_MarCookie:%X", art_MarCookie);
                env->SetObjectField(DexFile, mCookie, art_MarCookie);
            } else {
                LOGI("SetLongField");
                LOGI("----artCookie:%llx", art_Cookie);
                env->SetLongField(DexFile, mCookie, art_Cookie);
            }
            /*if(!cmpstatus)
            {

                break;
            }*/
            i++;
        }
        LOGI("exit replace_classloader_cookie");
    }
}

void native_attachContextBaseContext(JNIEnv *env, jobject obj, jobject ctx) {
    jobject application_obj = obj;
    init_class(env, obj, ctx);
    LOGI("arg:application_obj:%p, myContextWrapper:%p, ContextWrapper_attachBaseContext:%p",
         application_obj, myContextWrapper, ContextWrapper_attachBaseContext);

    //这个地方ida反编译出错 少了参数ctx
    env->CallNonvirtualVoidMethod(application_obj, myContextWrapper,
                                  ContextWrapper_attachBaseContext, ctx);
    jclass v12 = env->GetObjectClass(application_obj);// 获取StubApplication类
    jmethodID v13 = env->GetMethodID(v12, "getFilesDir", "()Ljava/io/File;");


    jobject file_obj = env->CallObjectMethod(obj, v13);// File getFilesDir()

    //   LOGI("file_obj:%p",file_obj);
    jclass file_classz = env->GetObjectClass(file_obj);
    jmethodID v18 = env->GetMethodID(file_classz, "getAbsolutePath", "()Ljava/lang/String;");
    jobject mAbsolutePath = env->CallObjectMethod(file_obj, v18);

    //6.0下为/data/user/0/packagename/files/目录
    mAbsolutePath_str = env->GetStringUTFChars((jstring) mAbsolutePath, 0);
    LOGI("global files path is %s", mAbsolutePath_str);

    //return ApplicationInfo 
    jobject ApplicationInfo = env->CallObjectMethod(application_obj, context_getApplicationInfo);
    jobject v24 = env->GetObjectField(ApplicationInfo, ApplicationInfo_nativeLibraryDir);
    const char *mNativeLibraryDir = env->GetStringUTFChars((jstring) v24, 0);
    //LOGI("mNativeLibraryDir is %s",mNativeLibraryDir);

    jobject v32 = env->CallObjectMethod(application_obj, context_getPackageResourePath);
    const char *mPackageResourePath = env->GetStringUTFChars((jstring) v32, 0);

    setenv("APKPATH", mPackageResourePath, 1);
    //LOGI("APK Path is %s",mPackageResourePath);

    jobject v36 = env->CallObjectMethod(ctx, context_getPackageName);
    mPackageName = env->GetStringUTFChars((jstring) v36, 0);
    LOGI("mPackageName:%s", mPackageName);

    // public ClassLoader getClassLoader() 
    jobject classLoader = env->CallObjectMethod(ctx, context_getClassLoader);
    LOGI("classLoader:%p", classLoader);

    char szPath[260] = {0};
    //   sprintf(szPath,"%s/dump.dex",mAbsolutePath_str);
    sprintf(szPath, "/data/data/%s/files/dump.dex", mPackageName);
    LOGI("szPath:%s", szPath);

    myExtractFile(env, application_obj, szPath);

    //important
    if (isDalvik) {

        myLoadDex_dvm(env, szPath);
    } else {

        myLoadDex_art(env, szPath);
    }

    //important
    replace_classloader_cookie(env, classLoader);

    LOGI("enter new application");


    jstring newapp = env->NewStringUTF("android.app.Application");
    //
    jobject findclass_classz = env->CallObjectMethod(classLoader, classLoader_findClass, newapp);
    if (!findclass_classz) {
        LOGI("can't findClass realAppClass");
        return;
    }

    jmethodID initMethod = env->GetMethodID((jclass) findclass_classz, "<init>", "()V");
    onCreateObj = env->NewObject((jclass) findclass_classz, initMethod);
    env->CallVoidMethod(onCreateObj, Application_attach, ctx);

    if (onCreateObj) {
        onCreateObj = env->NewGlobalRef(onCreateObj);
    }
    LOGI("enter realAppClass");

}

void native_onCreate(JNIEnv *env, jobject obj) {

    jobject theCurrentActivityThread = env->CallStaticObjectMethod(ActivityThread,
                                                                   currentActivityThread);

    //final ArrayMap<String, WeakReference<LoadedApk>> mPackages= new ArrayMap<String, WeakReference<LoadedApk>>();
    jobject arraymap_mPackages = env->GetObjectField(theCurrentActivityThread, mPackages);
    jstring thePackagename = env->NewStringUTF(mPackageName);
    LOGI("mPackageName %s", mPackageName);

    // 调用arrayMap_get函数 获取WeakReference<LoadedApk>类
    jobject v9 = env->CallObjectMethod(arraymap_mPackages, ArrayMap_get, thePackagename);
    //获取LoadedApk类
    jobject v15 = env->CallObjectMethod(v9, WeakReference_get);

    env->SetObjectField(v15, LoadedApk_mApplication, onCreateObj);
    env->SetObjectField(theCurrentActivityThread, mInitialApplication, onCreateObj);

    // AppBindData mBoundApplication;
    jobject v16 = env->GetObjectField(theCurrentActivityThread, mBoundApplication);

    //AppBindData_info=env->GetFieldID( AppBindData, "info", "Landroid/app/LoadedApk;");
    jobject v17 = env->GetObjectField(v16, AppBindData_info);
    env->SetObjectField(v17, LoadedApk_mApplication, onCreateObj);

    /*     mAllApplications = env->GetFieldID(
                                  ActivityThread,
                                  "mAllApplications",
                                   "Ljava/util/ArrayList;");*/
    jobject v11 = env->GetObjectField(theCurrentActivityThread, mAllApplications);
    int count = env->CallIntMethod(v11, arraylist_size);
    LOGI("array_size:%d", count);
    int index = 0;
    while (index < count) {
        jobject v12 = env->CallObjectMethod(v11, arraylist_get, index);
        LOGI("compare: i=%d item=%p", index, v12);
        if ((env->IsSameObject)(obj, v12) == 1) {
            LOGI("replace: find same replace");
            env->CallObjectMethod(v11, arraylist_set, index, onCreateObj);
        }
        // _JNIEnv::DeleteLocalRef( v12);
        ++index;
    }
    //IDA反编译这里出错，多了一个参数
    env->CallVoidMethod(onCreateObj, Application_onCreate);
    env->DeleteGlobalRef(onCreateObj);

}


static JNINativeMethod method_table[] = {

        {
                "attachBaseContext",
                "(Landroid/content/Context;)V",
                (void *) native_attachContextBaseContext
        },
        {
                "onCreate",
                "()V",
                (void *) native_onCreate
        }


};


static int registerNativeMethods(JNIEnv *env, const char *className, JNINativeMethod *gMethods,
                                 int numMethods) {
    jclass clazz;
    clazz = env->FindClass(className);
    if (clazz == 0) {
        return JNI_FALSE;
    }

    LOGI("gMethods  %s,%s,%p\n ", gMethods[0].name, gMethods[0].signature, gMethods[0].fnPtr);

    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

int register_ndk_load(JNIEnv *env) {

    return registerNativeMethods(env, JNIREG_CLASS,
                                 method_table, 2);
}

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {

    JNIEnv *env = 0;
    jint result = -1;
    // LOGI("JNI_OnLoad is called");
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) {
        return result;
    }

    int status = register_ndk_load(env);
    if (!status) {
        LOGI("register call failed");
    }

    return JNI_VERSION_1_4;
}

