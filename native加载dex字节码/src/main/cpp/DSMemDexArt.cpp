#include "DSMemDexArt.h"
#include "elfReade.h"
#include <sys/mman.h>
#include <stdlib.h>
#include <errno.h>

const void *
(*org_artDexFileOpenMemory)(const uint8_t *base, size_t size, const std::string &location,
                            uint32_t location_checksum, void */* MemMap* */ mem_map,
                            std::string *error_msg) = NULL;


void *
(*org_artDexFileOpenMemory_6_0_32)(void *,const uint8_t *base,
                                   size_t size,
                                   const std::string &location,
                                   uint32_t location_checksum,
                                   void *mem_map,
                                   const void *oat_dex_file,
                                   std::string *error_msg) = NULL;

void*
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






const void *DSMemDexArt::LoadByte(JNIEnv *env, const char *base, size_t size) {
    std::string location = "";
    std::string err_msg;
    char *OpenMemoryname;
    void * handle1 = dlopen("libart.so", RTLD_LAZY);

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

    ElfReade *read = new ElfReade("/system/lib/libart.so");
    OpenMemoryname = read->printElfSymbol();
    OpenMemoryname="_ZN3art7DexFile10OpenMemoryEPKhmRKNSt3__112basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEEjPNS_6MemMapEPKNS_10OatDexFileEPS9_";
    //32位程序  _ZN3art7DexFile10OpenMemoryEPKhjRKNSt3__112basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEEjPNS_6MemMapEPKNS_10OatDexFileEPS9_
    //64位程序   _ZN3art7DexFile10OpenMemoryEPKhmRKNSt3__112basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEEjPNS_6MemMapEPKNS_10OatDexFileEPS9_
    int sdkVer = DSMemDexArt::sdkVersion();

    void *OpenMemoryname_sys = dlsym(handle1, OpenMemoryname);


    const void *p = NULL;
    const DexHeader *dex_header = reinterpret_cast<const DexHeader *>(base);


    if (sdkVer >= 23) {

        if(is64()){
            int *buffer2 = (int *) malloc(256);
            org_artDexFileOpenMemory_6_0_64 = (void *(*)( const uint8_t *, size_t, const string &,
                                                          uint32_t, void *, const void *,
                                                          string *)) OpenMemoryname_sys;
          p=  org_artDexFileOpenMemory_6_0_64( (const uint8_t *) base, size, location,  dex_header->checksum, NULL, NULL, &err_msg);


        }else{
            int *buffer2 = (int *) malloc(256);
            org_artDexFileOpenMemory_6_0_32 = (void *(*)(void *, const uint8_t *, size_t, const string &, uint32_t, void *, const void *, string *)) OpenMemoryname_sys;

            p= org_artDexFileOpenMemory_6_0_32(buffer2, (const uint8_t *) base, size, location,  dex_header->checksum, NULL, NULL, &err_msg);
        }




        printf("%s", "hani");

    } else {
        org_artDexFileOpenMemory = (const void *(*)(const uint8_t *, size_t, const string &,
                                                    uint32_t, void *, string *)) OpenMemoryname_sys;

        p = org_artDexFileOpenMemory((const uint8_t *) base, size, location,
                                     dex_header->checksum, NULL, &err_msg);
    }


    if (!p) {
        LOGD("DSMemDex::LoadByte Failed %s", err_msg.c_str());
    } else {
        LOGD("DSMemDex::LoadByte : %x", p);
    }

    return p;
}

bool DSMemDexArt::is64() {

    char propValue[PROP_VALUE_MAX] = {0};
    __system_property_get("ro.product.cpu.abilist64", propValue);

    if(strcmp("",propValue)==0){
        return false;
    }


    return true;
}


