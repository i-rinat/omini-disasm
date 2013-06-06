
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

#define LOG_I(str, ...) \
{ \
    pthread_mutex_lock(&log_mutex); \
    __android_log_print(ANDROID_LOG_INFO, "libfranken", str, __VA_ARGS__); \
    pthread_mutex_unlock(&log_mutex); \
}

#define LOG_E(str, ...) \
{ \
    pthread_mutex_lock(&log_mutex); \
    __android_log_print(ANDROID_LOG_ERROR, "libfranken", str, __VA_ARGS__); \
    pthread_mutex_unlock(&log_mutex); \
}

#define LOG_D(str, ...) \
{ \
    pthread_mutex_lock(&log_mutex); \
    __android_log_print(ANDROID_LOG_DEBUG, "libfranken", str, __VA_ARGS__); \
    pthread_mutex_unlock(&log_mutex); \
}
