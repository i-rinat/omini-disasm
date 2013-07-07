#include <stdio.h>
#include <unistd.h>
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
static uint32_t        trace_msg_id = 0;
static char            trace_msg[1024];

#define TRACING_WRITE_TO_FILE
//#define TRACING_PROXY_DURATION

#define TRACING_FILE_NAME   "/sdcard/mylog.txt"


#ifdef TRACING_WRITE_TO_FILE
static FILE *log_fp;

__attribute__((constructor))
static
void
constructor_log(void)
{
    log_fp = fopen(TRACING_FILE_NAME, "w");
    if (!log_fp) {
        __android_log_print(ANDROID_LOG_ERROR, "libfranken", "can't open logfile %s, errno = %d",
                                                                TRACING_FILE_NAME, errno);
    }
}

#define LOG_I(...) \
{ \
    pthread_mutex_lock(&log_mutex); \
    sprintf(trace_msg, __VA_ARGS__); \
    if (log_fp) fprintf(log_fp, "[%7d]  [info] %s\n",  trace_msg_id++, trace_msg); \
    if (log_fp) fflush(log_fp); \
    pthread_mutex_unlock(&log_mutex); \
}

//#undef LOG_I
//#define LOG_I(...)

#define LOG_E(...) \
{ \
    pthread_mutex_lock(&log_mutex); \
    sprintf(trace_msg, __VA_ARGS__); \
    if (log_fp) fprintf(log_fp, "[%7d] [error] %s\n",  trace_msg_id++, trace_msg); \
    if (log_fp) fflush(log_fp); \
    pthread_mutex_unlock(&log_mutex); \
}

#else   // TRACING_WRITE_TO_FILE

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

#endif  // TRACING_WRITE_TO_FILE
