//
// Created by Administrator on 2016/9/5.
//

#include "DSMemDex.h"
#include "DSUtil.h"
#include "DSMemDexImpl/DSMemDexArt21.h"
#include "DSMemDexImpl/DSMemDexArt.h"

using namespace DShell;

static void DSMemDex::HookOpenDexFileNative(JNIEnv* env)
{
    // TODO : 区分系统版本
    if (isArtMode())
    {
        DSMemDexArt::HookOpenDexFileNative(env);
    }
}