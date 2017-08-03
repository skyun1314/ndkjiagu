#include "DSMemDexArt.h"
#include "elfReade.h"
#include "Object.h"
#include <sys/mman.h>
#include <stdlib.h>
#include <errno.h>
#include <bits/unique_ptr.h>
#include <vector>
#include <utility>

int DSMemDexArt::sdk_int = 0;//6.0

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


const void *DSMemDexArt::LoadByte(JNIEnv *env, const char *base, jsize size, jobject pJobject) {
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
    int sdkVer = DSMemDexArt::sdkVersion();
    sdk_int = DSMemDexArt::sdkVersion();

    void *OpenMemoryname_sys = dlsym(handle1, OpenMemoryname);


    const void *p = NULL;
    const DexHeader *dex_header = reinterpret_cast<const DexHeader *>(base);


    if (sdkVer >= 23) {

        if (is64()) {
            int *buffer2 = (int *) malloc(256);
            org_artDexFileOpenMemory_6_0_64 = (const void *(*)(const uint8_t *, size_t,
                                                               const string &,
                                                               uint32_t, void *, const void *,
                                                               string *)) OpenMemoryname_sys;

            p = org_artDexFileOpenMemory_6_0_64((const uint8_t *) base, size, location,
                                                dex_header->checksum, NULL, NULL, &err_msg);


        } else {
            int *buffer2 = (int *) malloc(256);

            org_artDexFileOpenMemory_6_0_32 = (const void *(*)(void *, const uint8_t *, size_t,
                                                               const string &, uint32_t, void *,
                                                               const void *,
                                                               string *)) OpenMemoryname_sys;


            p = org_artDexFileOpenMemory_6_0_32(buffer2, (const uint8_t *) base, size, location,
                                                dex_header->checksum, NULL, NULL, &err_msg);


        }


        printf("%s", "hani");

    } else {
        org_artDexFileOpenMemory = (const void *(*)(const uint8_t *, size_t, const string &,
                                                    uint32_t, void *, string *)) OpenMemoryname_sys;


        std::unique_ptr<std::vector<const void *>> dex_files(new std::vector<const void *>());

        p = org_artDexFileOpenMemory((const uint8_t *) base, size, location, dex_header->checksum,
                                     NULL, &err_msg);

        dex_files.get()->push_back(p);
        art_Cookie = static_cast<jlong>(reinterpret_cast<uintptr_t>(dex_files.release()));
    }


    if (!p) {
        LOGD("DSMemDex::LoadByte Failed %s", err_msg.c_str());
    } else {
        LOGD("DSMemDex::LoadByte : %x", p);
    }
    replace_classloader_cookie(env, art_Cookie, pJobject);
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

void DSMemDexArt::replace_classloader_cookie(JNIEnv *env, jlong cookie, jobject pJobject) {

    jclass DexFile_class = env->FindClass("dalvik/system/DexFile");
    jmethodID loadDex_id = env->GetStaticMethodID(DexFile_class, "loadDex",
                                                  "(Ljava/lang/String;Ljava/lang/String;I)Ldalvik/system/DexFile;");
    jobject DexFile = env->CallStaticObjectMethod(DexFile_class, loadDex_id, env->NewStringUTF(
            "/data/data/com.example.nativedex/files/classes.dex"), NULL, 0);

    jfieldID mCookie_id = env->GetFieldID(DexFile_class, "mCookie", "J");
    jobject mCookie = env->GetObjectField(DexFile, mCookie_id);
    jlong xx = env->GetLongField(DexFile, mCookie_id);
    env->SetLongField(DexFile, mCookie_id, cookie);
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

