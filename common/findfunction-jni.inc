/// @file findfunction-jni.inc

static struct JNINativeInterface JNIEnv_functions;
static struct JNIInvokeInterface JavaVM_functions;


static
void
find_and_call_function_runtime(void *addr)
{
    if (addr == JNIEnv_functions.GetJavaVM) {   // 219
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling GetJavaVM (%x, %x)", r0, r1);
        reg.r0_signed = JNIEnv_functions.GetJavaVM((JNIEnv*)adjust_address(r0), (JavaVM**)adjust_address(r1));
        return;
    } else if (addr == JNIEnv_functions.GetVersion) {   // 4
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling GetVersion (%x)", r0);
        reg.r0_signed = JNIEnv_functions.GetVersion((JNIEnv *)adjust_address(r0));
        return;
    } else if (addr == JNIEnv_functions.ThrowNew) { // 14
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling ThrowNew");
        reg.r0_signed = JNIEnv_functions.ThrowNew((JNIEnv *)adjust_address(r0), (jclass)adjust_address(r1), (const char *)adjust_address(r2));
        return;
    }



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
