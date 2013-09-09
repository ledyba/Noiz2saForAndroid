#include <android/log.h>
#ifdef stderr
#undef stderr
#endif
#define stderr ANDROID_LOG_ERROR
#ifdef stdout
#undef stdout
#endif
#define stdout ANDROID_LOG_DEBUG
#ifndef ANDROID
#error "ANDROID MUST BE DEFINED"
// supress warning
#define __android_log_print(...) do{}while(0);
#endif
#define fprintf(__LEVEL__, ...) __android_log_print((__LEVEL__), ("Noiz2sa"), __VA_ARGS__);

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, ("Noiz2sa"), __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, ("Noiz2sa"), __VA_ARGS__)
