//
// Created by Administrator on 2016/9/5.
//

#include <string.h>
#include <sys/system_properties.h>
#include <stdlib.h>
#include "DSUtil.h"

int DShell::lookupJniMethod(JNINativeMethod *table, const char *name, const char *sig, void (**fnPtrout)(u4 const *, union JValue *)) {
    int i = 0;
    while (table[i].name != NULL)
    {
        LOGI("lookup %d %s" ,i,table[i].name);
        if ((strcmp(name, table[i].name) == 0)
            && (strcmp(sig, table[i].signature) == 0))
        {
            *fnPtrout = (void (*)(const u4 *, JValue *)) table[i].fnPtr;
            return 1;
        }
        i++;
    }
    return 0;
}

int DShell::sdkVersion()
{
    static bool done = false;
    static int sdkVersion = 0;
    if (!done)
    {
        done = true;
        char propValue[PROP_VALUE_MAX] = {0};
        __system_property_get("ro.build.version.sdk", propValue);
        sdkVersion = atoi(propValue);
    }

    return sdkVersion;
}

bool DShell::isArtMode()
{
    static bool done = false;
    static bool isArt = false;
    if (!done)
    {
        done = true;
        char propValue[PROP_VALUE_MAX] = {0};
        __system_property_get("persist.sys.dalvik.vm.lib.2", propValue);
        if (propValue[0] == '\x0')
            __system_property_get("persist.sys.dalvik.vm.lib", propValue);
        isArt = ( !strcmp(propValue, SO_ART) );
    }

    return isArt;
}
