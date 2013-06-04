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
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "        GetJavaVM returned %d", r0);
        return 1;
    } else if (addr == JNIEnv_functions.GetVersion) {   // 4
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling GetVersion(%p)", aa(r0));
        reg.r0_signed = JNIEnv_functions.GetVersion((JNIEnv *)aa(r0));
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "        GetVersion returned 0x%x", r0);
        return 1;
    } else if (addr == JNIEnv_functions.ThrowNew) { // 14
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling ThrowNew(%p, %p, %s)", aa(r0), aa(r1), aa(r2));
        reg.r0_signed = JNIEnv_functions.ThrowNew((JNIEnv *)aa(r0), (jclass)aa(r1), (const char *)aa(r2));
        return 1;
    } else if (addr == JNIEnv_functions.FindClass) { // 6
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling FindClass(%p, %p \"%s\")", aa(r0), aa(r1), aa(r1));
        r0 = (uint32_t)JNIEnv_functions.FindClass((JNIEnv*)aa(r0), (const char*)aa(r1));
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "        FindClass retuned %p", r0);
        return 1;
    } else if (addr == JNIEnv_functions.GetFieldID) { // 94
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling GetFieldID(%p, %p, %p \"%s\", %p \"%s\")",
                                                aa(r0), aa(r1), aa(r2), aa(r2), aa(r3), aa(r3));
        r0 = (uint32_t)JNIEnv_functions.GetFieldID((JNIEnv*)aa(r0), (jclass)aa(r1),
                                                    (const char*)aa(r2), (const char*)aa(r3));
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "        GetFieldID returned %p", r0);
        return 1;
    } else if (addr == JNIEnv_functions.DeleteLocalRef) {   // 23
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling DeleteLocalRef(%p, %p)", aa(r0), aa(r1));
        (void)JNIEnv_functions.DeleteLocalRef((JNIEnv*)aa(r0), (jobject)aa(r1));
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "        DeleteLocalRef returned void");
        return 1;
    } else if (addr == JNIEnv_functions.NewGlobalRef) { // 21
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling NewGlobalRef(%p, %p)", aa(r0), aa(r1));
        r0 = (uint32_t)JNIEnv_functions.NewGlobalRef((JNIEnv*)aa(r0), (jobject)aa(r1));
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "        NewGlobalRef returned %p", r0);
        return 1;
    } else if (addr == JNIEnv_functions.GetStaticFieldID) { // 144
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling GetStaticFieldID(%p, %p, %p \"%s\", %p \"%s\")",
                                                    aa(r0), aa(r1), aa(r2), aa(r2), aa(r3), aa(r3));
        r0 = (uint32_t)JNIEnv_functions.GetStaticFieldID((JNIEnv*)aa(r0), (jclass)aa(r1),
                                            (const char*)aa(r2), (const char*)aa(r3));
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "        GetStaticFieldID returned %p", r0);
        return 1;
    } else if (addr == JNIEnv_functions.GetMethodID) {  // 33
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling GetMethodID(%p, %p, %p \"%s\", %p \"%s\")",
                                                            aa(r0), aa(r1), aa(r2), aa(r2), aa(r3), aa(r3));
        r0 = (uint32_t)JNIEnv_functions.GetMethodID((JNIEnv*)aa(r0), (jclass)aa(r1),
                                                    (const char*)aa(r2), (const char*)aa(r3));
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "        GetMethodID returned %p", r0);
        return 1;
    } else if (addr == JNIEnv_functions.GetStaticMethodID) {    // 113
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling GetStaticMethodID(%p, %p, %p \"%s\", %p \"%s\")",
                                                    aa(r0), aa(r1), aa(r2), aa(r2), aa(r3), aa(r3));
        r0 = (uint32_t)JNIEnv_functions.GetStaticMethodID((JNIEnv*)aa(r0), (jclass)aa(r1),
                                                    (const char*)aa(r2), (const char*)aa(r3));
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "        GetStaticMethodID returned %p", r0);
        return 1;
    } else if (addr == JNIEnv_functions.GetStaticLongField) {   // 151
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling GetStaticLongField(%p, %p, %p)", aa(r0), aa(r1), aa(r2));
        reg.x_int64_t = JNIEnv_functions.GetStaticLongField((JNIEnv*)aa(r0), (jclass)aa(r1), (jfieldID)aa(r2));
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "        GetStaticLongField returned %ld", reg.x_int64_t);
        return 1;
    } else if (addr == JNIEnv_functions.ExceptionClear) {   // 17
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling ExceptionClear");
        (void)JNIEnv_functions.ExceptionClear((JNIEnv*)aa(r0));
        return 1;
    } else if (addr == JNIEnv_functions.ExceptionCheck) {   // 228
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling ExceptionCheck(%p)", aa(r0));
        r0 = (uint32_t)JNIEnv_functions.ExceptionCheck((JNIEnv*)aa(r0));
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "        ExceptionCheck returned %d", r0);
        return 1;
    } else if (addr == JNIEnv_functions.GetLongField) {   // 101
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling GetLongField(%p, %p, %p)", r0, r1, r2);
        reg.x_int64_t = JNIEnv_functions.GetLongField((JNIEnv*)aa(r0), (jobject)aa(r1), (jfieldID)aa(r2));
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "        GetLongField returned 0x%08x%08x", r1, r0);
        return 1;
    } else if (addr == JNIEnv_functions.SetLongField) {   // 110
        jlong value = load(r13) + (((uint64_t)load(r13 + 4)) << 32);
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling SetLongField(%p, %p, %p, 0x%08x%08x)",
                                                                aa(r0), aa(r1), aa(r2), (uint32_t)(value >> 32), (uint32_t)(value & 0xffffffff));
        (void)JNIEnv_functions.SetLongField((JNIEnv*)aa(r0), (jobject)aa(r1), (jfieldID)aa(r2), value);
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "        SetLongField returned void");
        return 1;
    } else if (addr == JNIEnv_functions.GetPrimitiveArrayCritical) {    // 222
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling GetPrimitiveArrayCritical(%p, %p, %p)", aa(r0), aa(r1), aa(r2));
        r0 = (uint32_t)JNIEnv_functions.GetPrimitiveArrayCritical((JNIEnv*)aa(r0), (jarray)aa(r1),
                                                                    (jboolean*)aa(r2));
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "        GetPrimitiveArrayCritical returned %p", r0);
        return 1;
    } else if (addr == JNIEnv_functions.ReleasePrimitiveArrayCritical) {    // 223
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling ReleasePrimitiveArrayCritical(%p, %p, %p, %d)", aa(r0), aa(r1), aa(r2), r3);
        (void)JNIEnv_functions.ReleasePrimitiveArrayCritical((JNIEnv*)aa(r0), (jarray)aa(r1),
                                                                    (void*)aa(r2), (jint)r3);
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "        ReleasePrimitiveArrayCritical returned void");
        return 1;
    } else if (addr == JNIEnv_functions.GetObjectClass) {       // 31
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling GetObjectClass(%p, %p)", aa(r0), aa(r1));
        r0 = (uint32_t)JNIEnv_functions.GetObjectClass((JNIEnv*)aa(r0), (jobject)aa(r1));
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "        GetObjectClass returned %p", r0);
        return 1;
    } else if (addr == JNIEnv_functions.GetObjectField) {   // 95
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling GetObjectField(%p, %p, %p)", aa(r0), aa(r1), aa(r2));
        r0 = (uint32_t)JNIEnv_functions.GetObjectField((JNIEnv*)aa(r0), (jobject)aa(r1), (jfieldID)aa(r2));
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "        GetObjectField returned %p", r0);
        return 1;
    } else if (addr == JNIEnv_functions.GetIntArrayElements) {  // 187
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling GetIntArrayElements(%p, %p, %p)", aa(r0), aa(r1), aa(r2));
        r0 = (uint32_t)JNIEnv_functions.GetIntArrayElements((JNIEnv*)aa(r0), (jintArray)aa(r1), (jboolean*)aa(r2));
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "        GetIntArrayElements returned %p", r0);
        return 1;
    } else if (addr == JNIEnv_functions.ReleaseIntArrayElements) {  // 195
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling ReleaseIntArrayElements(%p, %p, %p, %d)", aa(r0), aa(r1), aa(r2), r3);
        (void)JNIEnv_functions.ReleaseIntArrayElements((JNIEnv*)aa(r0), (jintArray)aa(r1), (jint*)aa(r2), (jint)r3);
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "        ReleaseIntArrayElements retrned void");
        return 1;
    } else if (addr == JNIEnv_functions.GetByteArrayRegion) {   // 200
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling GetByteArrayRegion");
        (void)JNIEnv_functions.GetByteArrayRegion((JNIEnv*)aa(r0), (jbyteArray)aa(r1), (jsize)r2,
                                                    (jsize)r3, (jbyte*)aa(load(r13)));
        return 1;
    } else if (addr == JNIEnv_functions.SetByteArrayRegion) {   // 208
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling SetByteArrayRegion");
        (void)JNIEnv_functions.SetByteArrayRegion((JNIEnv*)aa(r0), (jbyteArray)aa(r1), (jsize)r2,
                                                    (jsize)r3, (const jbyte*)aa(load(r13)));
        return 1;
    } else if (addr == JNIEnv_functions.CallVoidMethod) {   // 61
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling CallVoidMethod(%p, %p, %p, ... )",
            aa(r0), aa(r1), aa(r2));
        // TODO: what if jlong comes unaligned?
        // TODO: use runtime tables to recall method signature
        (void)JNIEnv_functions.CallVoidMethod((JNIEnv*)aa(r0), (jobject)aa(r1), (jmethodID)aa(r2),
            aa(r3), aa(load(r13)), aa(load(r13 + 4)), aa(load(r13 + 4*2)), aa(load(r13 + 4*3)),
            aa(load(r13 + 4*4)), aa(load(r13 + 4*5)), aa(load(r13 + 4*6)), aa(load(r13 + 4*7)),
            aa(load(r13 + 4*8)), aa(load(r13 + 4*9)), aa(load(r13 + 4*10)), aa(load(r13 + 4*11)),
            aa(load(r13 + 4*12)), aa(load(r13 + 4*13)), aa(load(r13 + 4*14)), aa(load(r13 + 4*15)));
        return 1;
    }


    // ===================

    if (addr == JavaVM_functions.GetEnv) {  //  6
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "calling JavaVM_functions.GetEnv(%p, %p, 0x%x)", aa(r0), aa(r1), r2);
        reg.r0_signed = JavaVM_functions.GetEnv((JavaVM*)aa(r0), (void**)aa(r1), (jint)r2);
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "        JavaVM_functions.GetEnv returned %d", r0);
        return 1;
    } else if (addr == JavaVM_functions.AttachCurrentThread) {   //  4
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken",
            "calling JavaVM_functions.AttachCurrentThread(%p, %p, %p)", aa(r0), aa(r1), aa(r2));
        reg.r0_signed = JavaVM_functions.AttachCurrentThread((JavaVM*)aa(r0), (JNIEnv**)aa(r1), (void*)aa(r2));
        __android_log_print(ANDROID_LOG_DEBUG, "libfranken", "        JavaVM_functions.AttachCurrentThread returned %d", r0);
        return 1;
    }


    // ===================


    uint32_t k;
    void **table = (void **)&JNIEnv_functions;
    uint32_t table_size = sizeof(struct JNINativeInterface)/sizeof(void *);

    for (k = 0; k < table_size; k ++) {
        if (table[k] == addr) {
            __android_log_print(ANDROID_LOG_ERROR, "libfranken", "find_and_call_function_runtime: "
                "function %p found in table JNIEnv_functions at %d, but not called", addr, k);
            return 1;
        }
    }

    table = (void **)&JavaVM_functions;
    table_size = sizeof(struct JNIInvokeInterface)/sizeof(void *);

    for (k = 0; k < table_size; k ++) {
        if (table[k] == addr) {
            __android_log_print(ANDROID_LOG_ERROR, "libfranken", "find_and_call_function_runtime: "
                "function %p found in table JavaVM_functions at %d, but not called", addr, k);
            return 1;
        }
    }

    return 0;
}
