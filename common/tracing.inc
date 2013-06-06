#include <stdio.h>
#include <unistd.h>
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
uint32_t        trace_msg_id = 0;
char            trace_msg[1024];

#define LOG_I(...) \
{ \
    pthread_mutex_lock(&log_mutex); \
    usleep(1); \
    sprintf(trace_msg, __VA_ARGS__); \
    __android_log_print(ANDROID_LOG_INFO, "libfranken", "[%d] %s", trace_msg_id++, trace_msg); \
    pthread_mutex_unlock(&log_mutex); \
}

#define LOG_E(...) \
{ \
    pthread_mutex_lock(&log_mutex); \
    usleep(1); \
    sprintf(trace_msg, __VA_ARGS__); \
    __android_log_print(ANDROID_LOG_ERROR, "libfranken", "[%d] %s", trace_msg_id++, trace_msg); \
    pthread_mutex_unlock(&log_mutex); \
}
