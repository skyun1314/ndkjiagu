#ifndef HOOK_UTIL_H_
#define HOOK_UTIL_H_

#include <functional>

extern int find_name(pid_t pid, const char *name, const  char *libn, unsigned long *addr);
extern int find_libbase(pid_t pid, char *libn, unsigned long *addr);


// add by liam to search function
extern int enum_func(pid_t pid, const  char *libn, std::function<bool(const char *name)> name_callback);

#endif
