//
// Created by Administrator on 2016/9/8.
//

#ifndef TESTSHELL_DSMEMDEXART_H
#define TESTSHELL_DSMEMDEXART_H

#include <jni.h>

namespace DShell
{
class DSMemDexArt {
public:
    static void HookOpenDexFileNative(JNIEnv* env);
};
}


#endif //TESTSHELL_DSMEMDEXART_H
