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
    } else if (addr == JNIEnv_functions.FindClass) { // 6
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling FindClass");
        r0 = (uint32_t)JNIEnv_functions.FindClass((JNIEnv*)aa(r0), (const char*)aa(r1));
        return;
    } else if (addr == JNIEnv_functions.GetFieldID) { // 94
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling GetFieldID");
        r0 = (uint32_t)JNIEnv_functions.GetFieldID((JNIEnv*)aa(r0), (jclass)aa(r1),
                                                    (const char*)aa(r2), (const char*)aa(r3));
        return;
    } else if (addr == JNIEnv_functions.DeleteLocalRef) {   // 23
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling DeleteLocalRef");
        (void)JNIEnv_functions.DeleteLocalRef((JNIEnv*)aa(r0), (jobject)aa(r1));
        return;
    } else if (addr == JNIEnv_functions.NewGlobalRef) { // 21
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling NewGlobalRef");
        r0 = (uint32_t)JNIEnv_functions.NewGlobalRef((JNIEnv*)aa(r0), (jobject)aa(r1));
        return;
    } else if (addr == JNIEnv_functions.GetStaticFieldID) { // 144
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling GetStaticFieldID");
        r0 = (uint32_t)JNIEnv_functions.GetStaticFieldID((JNIEnv*)aa(r0), (jclass)aa(r1),
                                            (const char*)aa(r2), (const char*)aa(r3));
        return;
    } else if (addr == JNIEnv_functions.GetMethodID) {  // 33
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling GetMethodID");
        r0 = (uint32_t)JNIEnv_functions.GetMethodID((JNIEnv*)aa(r0), (jclass)aa(r1),
                                                    (const char*)aa(r2), (const char*)aa(r3));
        return;
    } else if (addr == JNIEnv_functions.GetStaticMethodID) {    // 113
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling GetStaticMethodID");
        r0 = (uint32_t)JNIEnv_functions.GetStaticMethodID((JNIEnv*)aa(r0), (jclass)aa(r1),
                                                    (const char*)aa(r2), (const char*)aa(r3));
        return;
    } else if (addr == JNIEnv_functions.GetStaticLongField) {   // 151
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling GetStaticLongField");
        reg.x_int64_t = JNIEnv_functions.GetStaticLongField((JNIEnv*)aa(r0), (jclass)aa(r1), (jfieldID)aa(r2));
        return;
    } else if (addr == JNIEnv_functions.ExceptionClear) {   // 17
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling ExceptionClear");
        (void)JNIEnv_functions.ExceptionClear((JNIEnv*)aa(r0));
        return;
    } else if (addr == JNIEnv_functions.ExceptionCheck) {   // 228
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling ExceptionCheck");
        r0 = (uint32_t)JNIEnv_functions.ExceptionCheck((JNIEnv*)aa(r0));
        return;
    }



    // ===================

    if (addr == JavaVM_functions.GetEnv) {  //  6
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling JavaVM_functions.GetEnv");
        reg.r0_signed = JavaVM_functions.GetEnv((JavaVM*)aa(r0), (void**)aa(r1), (jint)r2);
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
