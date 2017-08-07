#include "DSMemDexArt.h"
#include "elfReade.h"
#include "Object.h"
#include <sys/mman.h>
#include <stdlib.h>
#include <errno.h>
#include <bits/unique_ptr.h>
#include <vector>
#include <utility>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

int DSMemDexArt::sdk_int = 0;//6.0

char *DSMemDexArt::the_dex_byte;
int DSMemDexArt::the_dex_size;
const char *DSMemDexArt::mPackageName;
jstring DSMemDexArt::thePackagename;


int dvm_Cookie = 0;//4.4
jlong art_Cookie = 0;//5.0-5.1
jobject art_MarCookie = 0;//6.0


const void *
(*org_artDexFileOpenMemory)(const uint8_t *base, size_t size, const std::string &location,
                            uint32_t location_checksum, void */* MemMap* */ mem_map,
                            std::string *error_msg) = NULL;


const void *
(*org_artDexFileOpenMemory_6_0_32)(void *, const uint8_t *base,
                                   size_t size,
                                   const std::string &location,
                                   uint32_t location_checksum,
                                   void *mem_map,
                                   const void *oat_dex_file,
                                   std::string *error_msg) = NULL;

const void *
(*org_artDexFileOpenMemory_6_0_64)(const uint8_t *base,
                                   size_t size,
                                   const std::string &location,
                                   uint32_t location_checksum,
                                   void *mem_map,
                                   const void *oat_dex_file,
                                   std::string *error_msg) = NULL;


int DSMemDexArt::sdkVersion() {

    static bool done = false;
    static int sdkVersion = 0;
    if (!done) {
        done = true;
        char propValue[PROP_VALUE_MAX] = {0};
        __system_property_get("ro.build.version.sdk", propValue);
        sdkVersion = atoi(propValue);
    }

    return sdkVersion;
}


char *DSMemDexArt::execute(char *str) {
    FILE *fstream = NULL;
    char buff[1024];
    memset(buff, 0, sizeof(buff));
    if (NULL == (fstream = popen(str, "r"))) {
        fprintf(stderr, "execute command failed: %s", strerror(errno));
        return NULL;
    }
    if (NULL != fgets(buff, sizeof(buff), fstream)) {
        printf("%s", buff);
    } else {
        pclose(fstream);
        return NULL;
    }
    pclose(fstream);
    return buff;
}

const jbyte *DSMemDexArt::readDexFileFromApk(JNIEnv *env, jobject cxt, int &size) {
    size = 0;
    jclass context_class = env->GetObjectClass(cxt);
    jclass ApplicationInfo_class = env->FindClass("android/content/pm/ApplicationInfo");
    jmethodID ApplicationInfo_id = env->GetMethodID(context_class, "getApplicationInfo",
                                                    "()Landroid/content/pm/ApplicationInfo;");

    jobject ApplicationInfo = env->CallObjectMethod(cxt, ApplicationInfo_id);

    jfieldID sourceDir_id = env->GetFieldID(ApplicationInfo_class, "sourceDir",
                                            "Ljava/lang/String;");

    jstring sourceDir = (jstring) env->GetObjectField(ApplicationInfo, sourceDir_id);

    jclass ByteArrayOutputStream_class = env->FindClass("java/io/ByteArrayOutputStream");
    jmethodID ByteArrayOutputStream_id = env->GetMethodID(ByteArrayOutputStream_class, "<init>",
                                                          "()V");
    jobject ByteArrayOutputStream = env->NewObject(ByteArrayOutputStream_class,
                                                   ByteArrayOutputStream_id);

    jclass ZipInputStream_class = env->FindClass("java/util/zip/ZipInputStream");
    jmethodID ZipInputStream_id = env->GetMethodID(ZipInputStream_class, "<init>",
                                                   "(Ljava/io/InputStream;)V");

    jclass BufferedInputStream_class = env->FindClass("java/io/BufferedInputStream");
    jmethodID BufferedInputStream_id = env->GetMethodID(BufferedInputStream_class, "<init>",
                                                        "(Ljava/io/InputStream;)V");

    jclass FileInputStream_class = env->FindClass("java/io/FileInputStream");
    jmethodID FileInputStream_id = env->GetMethodID(FileInputStream_class, "<init>",
                                                    "(Ljava/lang/String;)V");
    jobject FileInputStream = env->NewObject(FileInputStream_class, FileInputStream_id, sourceDir);

    jobject BufferedInputStream = env->NewObject(BufferedInputStream_class, BufferedInputStream_id,
                                                 FileInputStream);

    jobject ZipInputStream = env->NewObject(ZipInputStream_class, ZipInputStream_id,
                                            BufferedInputStream);


    jmethodID ZipEntry_id = env->GetMethodID(ZipInputStream_class, "getNextEntry",
                                             "()Ljava/util/zip/ZipEntry;");

    jclass ZipEntry_class = env->FindClass("java/util/zip/ZipEntry");
    jmethodID getName = env->GetMethodID(ZipEntry_class, "getName", "()Ljava/lang/String;");

    jmethodID read_id = env->GetMethodID(ZipInputStream_class, "read", "([B)I");
    jmethodID write_id = env->GetMethodID(ByteArrayOutputStream_class, "write", "([BII)V");

    jobject ZipEntry;
    jstring name;
    const char *name_;
    jmethodID closeEntry_id = env->GetMethodID(ZipInputStream_class, "closeEntry", "()V");
    while (true) {

        ZipEntry = env->CallObjectMethod(ZipInputStream, ZipEntry_id);

        if (ZipEntry == NULL) {
            break;
        }


        name = (jstring) env->CallObjectMethod(ZipEntry, getName);

        name_ = env->GetStringUTFChars(name, NULL);

        printf("%s", name_);
        if (strcmp(name_, "classes.dex") == 0) {
            jbyteArray arrayOfByte = env->NewByteArray(1024);
            while (true) {

                int i = env->CallIntMethod(ZipInputStream, read_id, arrayOfByte);

                if (i == -1)
                    break;
                size += i;
                env->CallVoidMethod(ByteArrayOutputStream, write_id, arrayOfByte, 0, i);
            }
        }

        env->CallVoidMethod(ZipInputStream, closeEntry_id);
    }

    jmethodID close_id = env->GetMethodID(ZipInputStream_class, "close", "()V");
    env->CallVoidMethod(ZipInputStream, close_id);

    jmethodID toByteArray_id = env->GetMethodID(ByteArrayOutputStream_class, "toByteArray", "()[B");
    jbyteArray toByteArray = (jbyteArray) env->CallObjectMethod(ByteArrayOutputStream,
                                                                toByteArray_id);

    jbyte *jbyte1 = env->GetByteArrayElements(toByteArray, NULL);

    return jbyte1;

}


const void *DSMemDexArt::LoadByte(JNIEnv *env, jobject pJobject) {
    std::string location = "";
    std::string err_msg;
    char *OpenMemoryname;
    void *handle1 = dlopen("libart.so", RTLD_NOW);

    /*  jclass DexFile_class = env->FindClass("dalvik/system/DexFile");
      jmethodID loadDex_id = env->GetStaticMethodID(DexFile_class, "loadDex",
                                                    "(Ljava/lang/String;Ljava/lang/String;I)Ldalvik/system/DexFile;");
      jobject DexFile = env->CallStaticObjectMethod(DexFile_class, loadDex_id,
                                                    env->NewStringUTF(OpenMemoryname),
                                                    env->NewStringUTF(OpenMemoryname), 0);

                 int fd = open("/data/data/com.example.nativedex/files/classes.dex", O_RDONLY);
                 size_t fs = lseek(fd, 0, SEEK_END);
                 lseek(fd, 0, SEEK_SET);

                 void *pMap = mmap(0, fs, PROT_READ, MAP_SHARED, fd, 0);
  */
    ElfReade *read;
    if (is64()) {
        read = new ElfReade("/system/lib64/libart.so");
    } else {
        read = new ElfReade("/system/lib/libart.so");
    }

    OpenMemoryname = read->printElfSymbol();
    sdk_int = DSMemDexArt::sdkVersion();

    void *OpenMemoryname_sys = dlsym(handle1, OpenMemoryname);


    const void *p = NULL;
    const DexHeader *dex_header = reinterpret_cast<const DexHeader *>(the_dex_byte);


    if (sdk_int >= 23) {

        if (is64()) {
            int *buffer2 = (int *) malloc(256);
            org_artDexFileOpenMemory_6_0_64 = (const void *(*)(const uint8_t *, size_t,
                                                               const string &,
                                                               uint32_t, void *, const void *,
                                                               string *)) OpenMemoryname_sys;

            p = org_artDexFileOpenMemory_6_0_64((const uint8_t *) the_dex_byte, the_dex_size,
                                                location,
                                                dex_header->checksum, NULL, NULL, &err_msg);





            // DexFile *xxx=(DexFile*)buffer2;
            // std::vector<std::unique_ptr<const DexFile>> dex_files;
            //
            // dex_files.push_back(std::move(*buffer2));
            art_MarCookie = ConvertNativeToJavaArray(env, buffer2);
            /* jboolean is_long_data_copied;
             jlong *long_data = env->GetLongArrayElements((jlongArray)art_MarCookie, &is_long_data_copied);
             long_data = (jlong *) buffer2;*/
            // int i=10;

        } else {
            int *buffer2 = (int *) malloc(256);

            org_artDexFileOpenMemory_6_0_32 = (const void *(*)(void *, const uint8_t *, size_t,
                                                               const string &, uint32_t, void *,
                                                               const void *,
                                                               string *)) OpenMemoryname_sys;


            p = org_artDexFileOpenMemory_6_0_32(buffer2,
                                                (const uint8_t *) the_dex_byte,
                                                the_dex_size,
                                                location,
                                                dex_header->checksum,
                                                NULL, NULL,
                                                &err_msg);

            //  DexFile *xxx=(DexFile*)buffer2;
            // std::vector<std::unique_ptr<const DexFile>> dex_files;
            //
            // dex_files.push_back(std::move(*buffer2));
            art_MarCookie = ConvertNativeToJavaArray(env, buffer2);
            /* jboolean is_long_data_copied;
             jlong *long_data = env->GetLongArrayElements((jlongArray)art_MarCookie, &is_long_data_copied);
             long_data = (jlong *) buffer2;*/
            //  int i=10;
        }


        printf("%s", "hani");

    } else {
        org_artDexFileOpenMemory = (const void *(*)(const uint8_t *, size_t, const string &,
                                                    uint32_t, void *, string *)) OpenMemoryname_sys;


        std::unique_ptr<std::vector<const void *>> dex_files(new std::vector<const void *>());

        p = org_artDexFileOpenMemory((const uint8_t *) the_dex_byte, the_dex_size, location,
                                     dex_header->checksum,
                                     NULL, &err_msg);

        dex_files.get()->push_back(p);
        art_Cookie = static_cast<jlong>(reinterpret_cast<uintptr_t>(dex_files.release()));
    }


    if (!p) {
        LOGD("DSMemDex::LoadByte Failed %s", err_msg.c_str());
    } else {
        LOGD("DSMemDex::LoadByte : %x", p);
    }
    replace_classloader_cookie(env, pJobject);

    return p;
}

bool DSMemDexArt::is64() {

    char propValue[PROP_VALUE_MAX] = {0};
    __system_property_get("ro.product.cpu.abilist64", propValue);

    if (strcmp("", propValue) == 0) {
        return false;
    }


    return true;
}

void DSMemDexArt::replace_resouce(JNIEnv *env, jobject pJobject) {

    jclass ActivityThread_class = env->FindClass("android/app/ActivityThread");
    jclass LoadedApk_class = env->FindClass("android/app/LoadedApk");

    jmethodID currentActivityThread_id = env->GetStaticMethodID(ActivityThread_class,
                                                                "currentActivityThread",
                                                                "()Landroid/app/ActivityThread;");


    jobject theCurrentActivityThread = env->CallStaticObjectMethod(ActivityThread_class,
                                                                   currentActivityThread_id);

    jfieldID mPackages_id = env->GetFieldID(ActivityThread_class, "mPackages",
                                            "Landroid/util/ArrayMap;");


    jobject map = env->GetObjectField(theCurrentActivityThread, mPackages_id);


    jclass Context_class = env->FindClass("android/content/Context");
    jmethodID context_getPackageName_id = env->GetMethodID(Context_class, "getPackageName",
                                                           "()Ljava/lang/String;");
    jobject v36 = env->CallObjectMethod(pJobject, context_getPackageName_id);
    const char *mPackageName = env->GetStringUTFChars((jstring) v36, 0);
    jstring thePackagename = env->NewStringUTF(mPackageName);


    jclass WeakReference_class = env->FindClass("java/lang/ref/WeakReference");
    jmethodID WeakReference_get_id = env->GetMethodID(WeakReference_class, "get",
                                                      "()Ljava/lang/Object;");
    jclass ArrayMap_class = env->FindClass("android/util/ArrayMap");
    jmethodID ArrayMap_get = env->GetMethodID(ArrayMap_class, "get",
                                              "(Ljava/lang/Object;)Ljava/lang/Object;");

// 调用arrayMap_get函数 获取WeakReference<LoadedApk>类
    jobject o = env->CallObjectMethod(map, ArrayMap_get, thePackagename);
    //获取LoadedApk类

    jobject LoadedApk = env->CallObjectMethod(o, WeakReference_get_id);


    jfieldID Resources_id = env->GetFieldID(LoadedApk_class, "mResources",
                                            "Landroid/content/res/Resources;");


    jfieldID mResDir_id = env->GetFieldID(LoadedApk_class, "mResDir", "Ljava/lang/String;");
    env->GetObjectField(LoadedApk, Resources_id);
    env->GetObjectField(LoadedApk,mResDir_id);


    jmethodID v20 = env->GetMethodID(Context_class, "getAssets",
                                     "()Landroid/content/res/AssetManager;");
    jobject AssetManager = env->CallObjectMethod(pJobject, v20);
    jclass AssetManager_class = env->GetObjectClass(AssetManager);
    jmethodID addAssetPath_id = env->GetMethodID(AssetManager_class, "addAssetPath",
                                                 "(Ljava/lang/String;)I");


    char szPathxx[260] = {0};
    sprintf(szPathxx, "/data/data/%s/files/res.zip", mPackageName);

    env->CallIntMethod(AssetManager, addAssetPath_id,
                       env->NewStringUTF(szPathxx));

    jclass Resources_class = env->FindClass("android/content/res/Resources");
    jmethodID id = env->GetMethodID(Resources_class, "<init>",
                                    "(Landroid/content/res/AssetManager;Landroid/util/DisplayMetrics;Landroid/content/res/Configuration;)V");


    jmethodID getResources_id = env->GetMethodID(Context_class, "getResources",
                                                 "()Landroid/content/res/Resources;");
    jobject Resources_new = env->CallObjectMethod(pJobject, getResources_id);

    jmethodID Configuration_id = env->GetMethodID(Resources_class, "getConfiguration",
                                                  "()Landroid/content/res/Configuration;");
    jmethodID DisplayMetrics_id = env->GetMethodID(Resources_class, "getDisplayMetrics",
                                                   "()Landroid/util/DisplayMetrics;");

    jobject DisplayMetrics = env->CallObjectMethod(Resources_new, DisplayMetrics_id);
    jobject Configuration = env->CallObjectMethod(Resources_new, Configuration_id);

    jobject jobject1 = env->NewObject(Resources_class, id, AssetManager, DisplayMetrics,
                                      Configuration);

    env->SetObjectField(LoadedApk, Resources_id, jobject1);
    env->SetObjectField(LoadedApk,mResDir_id,env->NewStringUTF(szPathxx));
    printf("%s","ss");
}


void DSMemDexArt::replace_classloader_cookie(JNIEnv *env, jobject pJobject) {

    jclass DexFile_class = env->FindClass("dalvik/system/DexFile");
    jmethodID loadDex_id = env->GetStaticMethodID(DexFile_class, "loadDex",
                                                  "(Ljava/lang/String;Ljava/lang/String;I)Ldalvik/system/DexFile;");

    char szPathxx[260] = {0};
    sprintf(szPathxx, "/data/data/%s/files/classesjia.dex", mPackageName);


    jobject DexFile = env->CallStaticObjectMethod(DexFile_class, loadDex_id,
                                                  env->NewStringUTF(szPathxx), NULL, 0);


    if (sdk_int >= 23) {
        jfieldID mCookie_id = env->GetFieldID(DexFile_class, "mCookie", "Ljava/lang/Object;");
        env->SetObjectField(DexFile, mCookie_id, art_MarCookie);
        // jobject jobject1 = env->GetObjectField(DexFile, mCookie_id);
    } else {
        jfieldID mCookie_id = env->GetFieldID(DexFile_class, "mCookie", "J");
        //jlong mCookie = env->GetLongField(DexFile, mCookie_id);
        env->SetLongField(DexFile, mCookie_id, art_Cookie);
    }


    jclass Context_class = env->FindClass("android/content/Context");
    jmethodID getClassLoader_id = env->GetMethodID(Context_class, "getClassLoader",
                                                   "()Ljava/lang/ClassLoader;");
    jobject ClassLoader = env->CallObjectMethod(pJobject, getClassLoader_id);

    jclass BaseDexClassLoader_class = env->FindClass("dalvik/system/BaseDexClassLoader");
    jfieldID DexPathList_id = env->GetFieldID(BaseDexClassLoader_class, "pathList",
                                              "Ldalvik/system/DexPathList;");
    jobject pathList = env->GetObjectField(ClassLoader, DexPathList_id);

    jclass DexPathList_class = env->FindClass("dalvik/system/DexPathList");
    jfieldID base_ClassLoader_dexElements_id = env->GetFieldID(DexPathList_class, "dexElements",
                                                               "[Ldalvik/system/DexPathList$Element;");
    jobject base_ClassLoader_dexElements = env->GetObjectField(pathList,
                                                               base_ClassLoader_dexElements_id);

    int count = env->GetArrayLength((jarray) base_ClassLoader_dexElements);
    jclass Element_class = env->FindClass("dalvik/system/DexPathList$Element");
    jfieldID dexFile_id = env->GetFieldID(Element_class, "dexFile", "Ldalvik/system/DexFile;");
    for (int i = 0; i < count; ++i) {
        jobject Element = env->GetObjectArrayElement((jobjectArray) base_ClassLoader_dexElements,
                                                     i);

        jobject DexFile_get = env->GetObjectField(Element, dexFile_id);// 获取DexFile类
        env->SetObjectField(Element, dexFile_id, DexFile);
    }

}

jlongArray DSMemDexArt::ConvertNativeToJavaArray(JNIEnv *env, void *buff) {

    jlongArray long_array = env->NewLongArray(static_cast<jsize>(1));
    if (env->ExceptionCheck() == JNI_TRUE) {
        return nullptr;
    }

    jboolean is_long_data_copied = true;
    jlong *long_data = env->GetLongArrayElements(long_array, &is_long_data_copied);
    if (env->ExceptionCheck() == JNI_TRUE) {
        return nullptr;
    }

    long_data = (jlong *) buff;

    env->ReleaseLongArrayElements(long_array, long_data, 0);
    if (env->ExceptionCheck() == JNI_TRUE) {
        return nullptr;
    }

    return long_array;
}


int32_t DSMemDexArt::swapInt32(int32_t value) {
    return ((value & 0x000000FF) << 24) |
           ((value & 0x0000FF00) << 8) |
           ((value & 0x00FF0000) >> 8) |
           ((value & 0xFF000000) >> 24);
}

void DSMemDexArt::copyFile(JNIEnv *env, jobject obj) {
    jclass Context_class = env->FindClass("android/content/Context");
    jmethodID context_getPackageName_id = env->GetMethodID(Context_class, "getPackageName",
                                                           "()Ljava/lang/String;");
    jobject v36 = env->CallObjectMethod(obj, context_getPackageName_id);
    mPackageName = env->GetStringUTFChars((jstring) v36, 0);
    thePackagename = env->NewStringUTF(mPackageName);

    char szPathxx[260] = {0};
    sprintf(szPathxx, "/data/data/%s/files", mPackageName);
    mkdir(szPathxx, 0755);
    int len;

    if (strcmp(mPackageName, "com.example.nativedex") == 0) {
        copyDexToByte(env, obj, "res.zip", len);
        LOGD("%s","第一个copyDexToByte执行完");
        the_dex_byte = copyDexToByte(env, obj, "classes.dex", the_dex_size);
        LOGD("%s","第2个copyDexToByte执行完");
    } else {
        //char *dex_byte_all = copyDexToByte(env, obj, "ForceApkObj_ok.dex", len);

        char *dex_byte_all= (char *) DSMemDexArt::readDexFileFromApk(env, obj, len);
        LOGD("%s","第3个copyDexToByte执行完");
        char *dexSize_s = getSizeByoff(dex_byte_all, 4, len - 4);
        int dexSize = bytesToInt(dexSize_s);
        the_dex_size = dexSize;
        char *dexByte = getSizeByoff(dex_byte_all, dexSize, len - 4 - dexSize);
        the_dex_byte = decrpt(dexByte, dexSize);

        char *assets_size = getSizeByoff(dex_byte_all, 4, len - 4 - dexSize - 4);
        int assets_dexSize = bytesToInt(assets_size);
        char *assets_byte = getSizeByoff(dex_byte_all, assets_dexSize,
                                         len - 4 - dexSize - 4 - assets_dexSize);
        assets_byte = decrpt(assets_byte, assets_dexSize);

        char szPathxx1[260] = {0};
        sprintf(szPathxx1, "/data/data/%s/files/%s", mPackageName, "res.zip");
        FILE *fp = fopen(szPathxx1, "w");

        fwrite(assets_byte, assets_dexSize, 1, fp);
        fclose(fp);
        printf("%s", assets_byte);

    }
    replace_resouce(env, obj);
    copyDexToByte(env, obj, "classesjia.dex", len);
    LOGD("%s","第4个copyDexToByte执行完");
}


/*
char *DSMemDexArt::copyDexToByte_java(JNIEnv *env,jstring dexName, jobject context, int &dexlen) {
    AssetManager assetManager = context.getAssets();
    try {
        InputStream open = assetManager.open(dexName);
        ByteArrayOutputStream fileOutputStream = new ByteArrayOutputStream();

        byte b[] = new byte[1024];
        int len = 0;
        while ((len = open.read(b)) != -1) {
            fileOutputStream.write(b, 0, len);
        }
        return fileOutputStream.toByteArray();

    } catch (IOException e) {
        e.printStackTrace();
        return null;
    }
}
*/


char *DSMemDexArt::copyDexToByte(JNIEnv *env, jobject application_obj, char *szPath, int &dexlen) {
    AAssetManager *mgr;
    dexlen = 0;
    if (access(szPath, R_OK)) {
        jclass v19 = env->GetObjectClass(application_obj);
        jmethodID v20 = env->GetMethodID(v19, "getAssets", "()Landroid/content/res/AssetManager;");
        jobject v22 = env->CallObjectMethod(application_obj, v20);


        mgr = AAssetManager_fromJava(env, v22);
        if (mgr == NULL) {
            LOGD(" %s", "AAssetManager==NULL");
            return NULL;
        }
        AAsset *asset = AAssetManager_open(mgr, szPath, AASSET_MODE_STREAMING);
        FILE *file;
        char *buffer;
        int numBytesRead;
        if (asset) {
            char szPathxx[260] = {0};
            //   sprintf(szPath,"%s/dump.dex",mAbsolutePath_str);
            sprintf(szPathxx, "/data/data/%s/files/%s", mPackageName, szPath);
            file = fopen(szPathxx, "w");
            int bufferSize = AAsset_getLength(asset);
            //LOGD("buffersize is %d", bufferSize);
            dexlen = bufferSize;
            buffer = (char *) malloc(bufferSize);
            char *buffer1 = buffer;
            while (1) {
                numBytesRead = AAsset_read(asset, buffer, 1024);
                if (numBytesRead <= 0)
                    break;

                fwrite(buffer, numBytesRead, 1, file);
                buffer = (buffer + numBytesRead);
            }
            //  free(buffer);
            fclose(file);
            AAsset_close(asset);
            return buffer1;
        } else {
            LOGD("Error AAssetManager_open");
            return NULL;
        }

    } else {
        LOGD("%s existed", szPath);
        return NULL;
    }


}

char *DSMemDexArt::getSizeByoff(char *DexFileBytes, int lenth, int DexFileBytesIndex) {

    char *bytes = (char *) malloc(lenth);
    for (int i = 0; i < lenth; i++) {
        bytes[i] = DexFileBytes[DexFileBytesIndex + i];
    }
    return bytes;

}

int DSMemDexArt::bytesToInt(char *bytes) {
    int addr = 0;

    addr = bytes[0] & 0xFF;
    addr = (addr << 8) | (bytes[1] & 0xff);
    addr = (addr << 8) | (bytes[2] & 0xff);
    addr = (addr << 8) | (bytes[3] & 0xff);


    return addr;
}

char *DSMemDexArt::decrpt(char *srcdata, int len) {
    for (int i = 0; i < len; i++) {
        srcdata[i] = (char) (0xFF ^ srcdata[i]);
    }
    return srcdata;
}