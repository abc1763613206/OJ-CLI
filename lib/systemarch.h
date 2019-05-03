// 简单的操作系统判定 

#define OS_PLATFORM_UTIL_VERSION 1.0.0.180723
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__) || defined(__WINDOWS_)
#  define OS_WIN
#elif defined(_UNIX) || defined(linux) || defined(__linux__)
#  define OS_LINUX
#else
#  define OS_UNKNOWN
#endif
