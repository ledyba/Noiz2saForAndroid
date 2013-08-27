#include <android/log.h>
#ifdef stderr
#undef stderr
#endif
#define stderr ANDROID_LOG_ERROR
#ifdef stdout
#undef stdout
#endif
#define stdout ANDROID_LOG_DEBUG
#define fprintf(__LEVEL__, ...) __android_log_print((__LEVEL__), ("Noiz2sa"), __VA_ARGS__);
