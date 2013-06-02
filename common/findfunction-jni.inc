/// @file findfunction-jni.inc

static struct JNINativeInterface JNIEnv_functions;
static struct JNIInvokeInterface JavaVM_functions;


static
void
find_and_call_function_runtime(void *addr)
{
    uint32_t k;
    void **table = (void **)&JNIEnv_functions;
    uint32_t table_size = sizeof(struct JNINativeInterface)/sizeof(void *);

    for (k = 0; k < table_size; k ++) {
        if (table[k] == addr) {
            __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "find_and_call_function_runtime: "
                "function %p found in table JNIEnv_functions at %d, but not called", addr, k);
            return;
        }
    }

    table = (void **)&JavaVM_functions;
    table_size = sizeof(struct JNIInvokeInterface)/sizeof(void *);

    for (k = 0; k < table_size; k ++) {
        if (table[k] == addr) {
            __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "find_and_call_function_runtime: "
                "function %p found in table JavaVM_functions at %d, but not called", addr, k);
            return;
        }
    }

    __android_log_print(ANDROID_LOG_INFO, "libfranken", "find_and_call_function_runtime: "
        "function %p not found", addr);
}
