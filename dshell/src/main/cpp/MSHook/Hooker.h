#ifndef HOOKER_H_
#define HOOKER_H_

#include <sys/mman.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <functional>

#include "Debug.h"
#include "Log.h"
#include "PosixMemory.h"
#include "CydiaSubstrate.h"

namespace Cydia{

	_extern void elfHookFunction(const char *soname, const char *symbol, void *replace_func, void **old_func);
	_extern void MSHookFunction(void *symbol, void *replace, void **result);
	_extern void elfEnumFunction(const char *soname, std::function<bool(const char *name)> name_callback);
}
#endif /* HOOKER_H_ */