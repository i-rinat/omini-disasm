/// @file findfunction-jni.inc

static struct JNINativeInterface JNIEnv_functions;
static struct JNIInvokeInterface JavaVM_functions;


static
int
find_and_call_function_runtime(void *addr)
{
    if (addr == JNIEnv_functions.GetJavaVM) {   // 219
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling GetJavaVM(%p, %p)", aa(r0), aa(r1));
        reg.r0_signed = JNIEnv_functions.GetJavaVM((JNIEnv*)aa(r0), (JavaVM**)aa(r1));
        return 1;
    } else if (addr == JNIEnv_functions.GetVersion) {   // 4
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling GetVersion(%p)", aa(r0));
        reg.r0_signed = JNIEnv_functions.GetVersion((JNIEnv *)aa(r0));
        return 1;
    } else if (addr == JNIEnv_functions.ThrowNew) { // 14
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling ThrowNew(%p, %p, %s)", aa(r0), aa(r1), aa(r2));
        reg.r0_signed = JNIEnv_functions.ThrowNew((JNIEnv *)aa(r0), (jclass)aa(r1), (const char *)aa(r2));
        return 1;
    } else if (addr == JNIEnv_functions.FindClass) { // 6
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling FindClass(%p, %s)", aa(r0), aa(r1));
        r0 = (uint32_t)JNIEnv_functions.FindClass((JNIEnv*)aa(r0), (const char*)aa(r1));
        return 1;
    } else if (addr == JNIEnv_functions.GetFieldID) { // 94
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling GetFieldID(%p, %p, %s, %s)",
                                                                    aa(r0), aa(r1), aa(r2), aa(r3));
        r0 = (uint32_t)JNIEnv_functions.GetFieldID((JNIEnv*)aa(r0), (jclass)aa(r1),
                                                    (const char*)aa(r2), (const char*)aa(r3));
        return 1;
    } else if (addr == JNIEnv_functions.DeleteLocalRef) {   // 23
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling DeleteLocalRef(%p, %p)", aa(r0), aa(r1));
        (void)JNIEnv_functions.DeleteLocalRef((JNIEnv*)aa(r0), (jobject)aa(r1));
        return 1;
    } else if (addr == JNIEnv_functions.NewGlobalRef) { // 21
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling NewGlobalRef(%p, %p)", aa(r0), aa(r1));
        r0 = (uint32_t)JNIEnv_functions.NewGlobalRef((JNIEnv*)aa(r0), (jobject)aa(r1));
        return 1;
    } else if (addr == JNIEnv_functions.GetStaticFieldID) { // 144
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling GetStaticFieldID(%p, %p, %s, %s)",
                                                            aa(r0), aa(r1), aa(r2), aa(r3));
        r0 = (uint32_t)JNIEnv_functions.GetStaticFieldID((JNIEnv*)aa(r0), (jclass)aa(r1),
                                            (const char*)aa(r2), (const char*)aa(r3));
        return 1;
    } else if (addr == JNIEnv_functions.GetMethodID) {  // 33
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling GetMethodID(%p, %p, %s, %s)",
                                                            aa(r0), aa(r1), aa(r2), aa(r3));
        r0 = (uint32_t)JNIEnv_functions.GetMethodID((JNIEnv*)aa(r0), (jclass)aa(r1),
                                                    (const char*)aa(r2), (const char*)aa(r3));
        return 1;
    } else if (addr == JNIEnv_functions.GetStaticMethodID) {    // 113
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling GetStaticMethodID(%p, %p, %s, %s)",
                                                            aa(r0), aa(r1), aa(r2), aa(r3));
        r0 = (uint32_t)JNIEnv_functions.GetStaticMethodID((JNIEnv*)aa(r0), (jclass)aa(r1),
                                                    (const char*)aa(r2), (const char*)aa(r3));
        return 1;
    } else if (addr == JNIEnv_functions.GetStaticLongField) {   // 151
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling GetStaticLongField");
        reg.x_int64_t = JNIEnv_functions.GetStaticLongField((JNIEnv*)aa(r0), (jclass)aa(r1), (jfieldID)aa(r2));
        return 1;
    } else if (addr == JNIEnv_functions.ExceptionClear) {   // 17
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling ExceptionClear");
        (void)JNIEnv_functions.ExceptionClear((JNIEnv*)aa(r0));
        return 1;
    } else if (addr == JNIEnv_functions.ExceptionCheck) {   // 228
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling ExceptionCheck");
        r0 = (uint32_t)JNIEnv_functions.ExceptionCheck((JNIEnv*)aa(r0));
        return 1;
    } else if (addr == JNIEnv_functions.GetLongField) {   // 101
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling GetLongField");
        reg.x_int64_t = JNIEnv_functions.GetLongField((JNIEnv*)aa(r0), (jobject)aa(r1), (jfieldID)aa(r2));
        return 1;
    } else if (addr == JNIEnv_functions.SetLongField) {   // 110
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling SetLongField");
        jlong value = load(r13) + (((uint64_t)load(r13 + 4)) << 32);
        (void)JNIEnv_functions.SetLongField((JNIEnv*)aa(r0), (jobject)aa(r1), (jfieldID)aa(r2), value);
        return 1;
    } else if (addr == JNIEnv_functions.GetPrimitiveArrayCritical) {    // 222
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling GetPrimitiveArrayCritical");
        r0 = (uint32_t)JNIEnv_functions.GetPrimitiveArrayCritical((JNIEnv*)aa(r0), (jarray)aa(r1),
                                                                    (jboolean*)aa(r2));
        return 1;
    } else if (addr == JNIEnv_functions.ReleasePrimitiveArrayCritical) {    // 223
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling ReleasePrimitiveArrayCritical");
        (void)JNIEnv_functions.ReleasePrimitiveArrayCritical((JNIEnv*)aa(r0), (jarray)aa(r1),
                                                                    (void*)aa(r2), (jint)r3);
        return 1;
    }


    // ===================

    if (addr == JavaVM_functions.GetEnv) {  //  6
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling JavaVM_functions.GetEnv");
        reg.r0_signed = JavaVM_functions.GetEnv((JavaVM*)aa(r0), (void**)aa(r1), (jint)r2);
        return 1;
    }


    uint32_t k;
    void **table = (void **)&JNIEnv_functions;
    uint32_t table_size = sizeof(struct JNINativeInterface)/sizeof(void *);

    for (k = 0; k < table_size; k ++) {
        if (table[k] == addr) {
            __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "find_and_call_function_runtime: "
                "function %p found in table JNIEnv_functions at %d, but not called", addr, k);
            return 1;
        }
    }

    table = (void **)&JavaVM_functions;
    table_size = sizeof(struct JNIInvokeInterface)/sizeof(void *);

    for (k = 0; k < table_size; k ++) {
        if (table[k] == addr) {
            __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "find_and_call_function_runtime: "
                "function %p found in table JavaVM_functions at %d, but not called", addr, k);
            return 1;
        }
    }

    return 0;
}
