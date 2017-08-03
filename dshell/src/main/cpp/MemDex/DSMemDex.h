//
// Created by Administrator on 2016/9/5.
//

#ifndef TESTSHELL_DSMEMDEX_H
#define TESTSHELL_DSMEMDEX_H


#include <stddef.h>
#include <jni.h>

namespace DShell {

class DSMemDex {

public:
    static void HookOpenDexFileNative(JNIEnv* env);
};

}


#endif //TESTSHELL_DSMEMDEX_H
