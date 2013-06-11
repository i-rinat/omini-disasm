/// @file findfunction-jni.inc

static struct JNINativeInterface JNIEnv_functions;
static struct JNIInvokeInterface JavaVM_functions;


static
int
find_and_call_function_runtime(state_t *state, void *addr)
{
    if (addr == JNIEnv_functions.GetJavaVM) {   // 219
        LOG_I("calling GetJavaVM(%p, %p)", r0, r1);
        r0_signed = JNIEnv_functions.GetJavaVM((JNIEnv*)aa(r0), (JavaVM**)aa(r1));
        LOG_I("        GetJavaVM returned %d", r0);
        return 1;
    } else if (addr == JNIEnv_functions.GetVersion) {   // 4
        LOG_I("calling GetVersion(%p)", r0);
        r0_signed = JNIEnv_functions.GetVersion((JNIEnv *)aa(r0));
        LOG_I("        GetVersion returned 0x%x", r0);
        return 1;
    } else if (addr == JNIEnv_functions.ThrowNew) { // 14
        LOG_I("calling ThrowNew(%p, %p, %p \"%s\")", r0, r1, r2, aa(r2));
        r0_signed = JNIEnv_functions.ThrowNew((JNIEnv *)aa(r0), (jclass)aa(r1), (const char *)aa(r2));
        return 1;
    } else if (addr == JNIEnv_functions.FindClass) { // 6
        LOG_I("calling FindClass(%p, %p \"%s\")", r0, r1, aa(r1));
        r0 = (uint32_t)JNIEnv_functions.FindClass((JNIEnv*)aa(r0), (const char*)aa(r1));
        LOG_I("        FindClass retuned %p", r0);
        return 1;
    } else if (addr == JNIEnv_functions.GetFieldID) { // 94
        LOG_I("calling GetFieldID(%p, %p, %p \"%s\", %p \"%s\")", r0, r1, r2, aa(r2), r3, aa(r3));
        r0 = (uint32_t)JNIEnv_functions.GetFieldID((JNIEnv*)aa(r0), (jclass)aa(r1),
                                                    (const char*)aa(r2), (const char*)aa(r3));
        LOG_I("        GetFieldID returned %p", r0);
        return 1;
    } else if (addr == JNIEnv_functions.DeleteLocalRef) {   // 23
        LOG_I("calling DeleteLocalRef(%p, %p)", r0, r1);
        (void)JNIEnv_functions.DeleteLocalRef((JNIEnv*)aa(r0), (jobject)aa(r1));
        LOG_I("        DeleteLocalRef returned void");
        return 1;
    } else if (addr == JNIEnv_functions.NewGlobalRef) { // 21
        LOG_I("calling NewGlobalRef(%p, %p)", r0, r1);
        r0 = (uint32_t)JNIEnv_functions.NewGlobalRef((JNIEnv*)aa(r0), (jobject)aa(r1));
        LOG_I("        NewGlobalRef returned %p", r0);
        return 1;
    } else if (addr == JNIEnv_functions.GetStaticFieldID) { // 144
        LOG_I("calling GetStaticFieldID(%p, %p, %p \"%s\", %p \"%s\")", r0, r1, r2, aa(r2), r3, aa(r3));
        r0 = (uint32_t)JNIEnv_functions.GetStaticFieldID((JNIEnv*)aa(r0), (jclass)aa(r1),
                                            (const char*)aa(r2), (const char*)aa(r3));
        LOG_I("        GetStaticFieldID returned %p", r0);
        return 1;
    } else if (addr == JNIEnv_functions.GetMethodID) {  // 33
        LOG_I("calling GetMethodID(%p, %p, %p \"%s\", %p \"%s\")", r0, r1, r2, aa(r2), r3, aa(r3));
        r0 = (uint32_t)JNIEnv_functions.GetMethodID((JNIEnv*)aa(r0), (jclass)aa(r1),
                                                    (const char*)aa(r2), (const char*)aa(r3));
        LOG_I("        GetMethodID returned %p", r0);
        return 1;
    } else if (addr == JNIEnv_functions.GetStaticMethodID) {    // 113
        LOG_I("calling GetStaticMethodID(%p, %p, %p \"%s\", %p \"%s\")", r0, r1, r2, aa(r2), r3, aa(r3));
        r0 = (uint32_t)JNIEnv_functions.GetStaticMethodID((JNIEnv*)aa(r0), (jclass)aa(r1),
                                                    (const char*)aa(r2), (const char*)aa(r3));
        LOG_I("        GetStaticMethodID returned %p", r0);
        return 1;
    } else if (addr == JNIEnv_functions.GetStaticLongField) {   // 151
        LOG_I("calling GetStaticLongField(%p, %p, %p)", r0, r1, r2);
        x_int64_t = JNIEnv_functions.GetStaticLongField((JNIEnv*)aa(r0), (jclass)aa(r1), (jfieldID)aa(r2));
        LOG_I("        GetStaticLongField returned %ld", x_int64_t);
        return 1;
    } else if (addr == JNIEnv_functions.ExceptionClear) {   // 17
        LOG_I("calling ExceptionClear");
        (void)JNIEnv_functions.ExceptionClear((JNIEnv*)aa(r0));
        return 1;
    } else if (addr == JNIEnv_functions.ExceptionCheck) {   // 228
        LOG_I("calling ExceptionCheck(%p)", r0);
        r0 = (uint32_t)JNIEnv_functions.ExceptionCheck((JNIEnv*)aa(r0));
        LOG_I("        ExceptionCheck returned %d", r0);
        return 1;
    } else if (addr == JNIEnv_functions.GetLongField) {   // 101
        LOG_I("calling GetLongField(%p, %p, %p)", r0, r1, r2);
        x_int64_t = JNIEnv_functions.GetLongField((JNIEnv*)aa(r0), (jobject)aa(r1), (jfieldID)aa(r2));
        LOG_I("        GetLongField returned 0x%08x%08x", r1, r0);
        return 1;
    } else if (addr == JNIEnv_functions.SetLongField) {   // 110
        jlong value = load(r13) + (((uint64_t)load(r13 + 4)) << 32);
        LOG_I("calling SetLongField(%p, %p, %p, 0x%08x%08x)", r0, r1, r2,
                            (uint32_t)(value >> 32), (uint32_t)(value & 0xffffffff));
        (void)JNIEnv_functions.SetLongField((JNIEnv*)aa(r0), (jobject)aa(r1), (jfieldID)aa(r2), value);
        LOG_I("        SetLongField returned void");
        return 1;
    } else if (addr == JNIEnv_functions.GetPrimitiveArrayCritical) {    // 222
        LOG_I("calling GetPrimitiveArrayCritical(%p, %p, %p)", r0, r1, r2);
        r0 = (uint32_t)JNIEnv_functions.GetPrimitiveArrayCritical((JNIEnv*)aa(r0), (jarray)aa(r1),
                                                                    (jboolean*)aa(r2));
        LOG_I("        GetPrimitiveArrayCritical returned %p", r0);
        return 1;
    } else if (addr == JNIEnv_functions.ReleasePrimitiveArrayCritical) {    // 223
        LOG_I("calling ReleasePrimitiveArrayCritical(%p, %p, %p, %d)", r0, r1, r2, r3);
        (void)JNIEnv_functions.ReleasePrimitiveArrayCritical((JNIEnv*)aa(r0), (jarray)aa(r1),
                                                                    (void*)aa(r2), (jint)r3);
        LOG_I("        ReleasePrimitiveArrayCritical returned void");
        return 1;
    } else if (addr == JNIEnv_functions.GetObjectClass) {       // 31
        LOG_I("calling GetObjectClass(%p, %p)", r0, r1);
        r0 = (uint32_t)JNIEnv_functions.GetObjectClass((JNIEnv*)aa(r0), (jobject)aa(r1));
        LOG_I("        GetObjectClass returned %p", r0);
        return 1;
    } else if (addr == JNIEnv_functions.GetObjectField) {   // 95
        LOG_I("calling GetObjectField(%p, %p, %p)", r0, r1, r2);
        r0 = (uint32_t)JNIEnv_functions.GetObjectField((JNIEnv*)aa(r0), (jobject)aa(r1), (jfieldID)aa(r2));
        LOG_I("        GetObjectField returned %p", r0);
        return 1;
    } else if (addr == JNIEnv_functions.GetIntArrayElements) {  // 187
        LOG_I("calling GetIntArrayElements(%p, %p, %p)", r0, r1, r2);
        r0 = (uint32_t)JNIEnv_functions.GetIntArrayElements((JNIEnv*)aa(r0), (jintArray)aa(r1), (jboolean*)aa(r2));
        LOG_I("        GetIntArrayElements returned %p", r0);
        return 1;
    } else if (addr == JNIEnv_functions.ReleaseIntArrayElements) {  // 195
        LOG_I("calling ReleaseIntArrayElements(%p, %p, %p, %d)", r0, r1, r2, r3);
        (void)JNIEnv_functions.ReleaseIntArrayElements((JNIEnv*)aa(r0), (jintArray)aa(r1), (jint*)aa(r2), (jint)r3);
        LOG_I("        ReleaseIntArrayElements retrned void");
        return 1;
    } else if (addr == JNIEnv_functions.GetByteArrayRegion) {   // 200
        uint32_t param5 = load(r13);
        LOG_I("calling GetByteArrayRegion(%p, %p, %d, %d, %p)", r0, r1, r2, r3, param5);
        (void)JNIEnv_functions.GetByteArrayRegion((JNIEnv*)aa(r0), (jbyteArray)aa(r1), (jsize)r2,
                                                    (jsize)r3, (jbyte*)aa(param5));
        return 1;
    } else if (addr == JNIEnv_functions.SetByteArrayRegion) {   // 208
        uint32_t param5 = load(r13);
        LOG_I("calling SetByteArrayRegion(%p, %p, %d, %d, %p)", r0, r1, r2, r3, param5);
        (void)JNIEnv_functions.SetByteArrayRegion((JNIEnv*)aa(r0), (jbyteArray)aa(r1), (jsize)r2,
                                                    (jsize)r3, (const jbyte*)aa(param5));
        return 1;
    } else if (addr == JNIEnv_functions.CallVoidMethod) {   // 61
        LOG_I("calling CallVoidMethod(%p, %p, %p, [%p, %p, ... ])", r0, r1, r2, r3, load(r13));
        // TODO: what if jlong comes unaligned?
        // TODO: use runtime tables to recall method signature
        (void)JNIEnv_functions.CallVoidMethod((JNIEnv*)aa(r0), (jobject)aa(r1), (jmethodID)aa(r2),
            aa(r3), aa(load(r13)), aa(load(r13 + 4)), aa(load(r13 + 4*2)), aa(load(r13 + 4*3)),
            aa(load(r13 + 4*4)), aa(load(r13 + 4*5)), aa(load(r13 + 4*6)), aa(load(r13 + 4*7)),
            aa(load(r13 + 4*8)), aa(load(r13 + 4*9)), aa(load(r13 + 4*10)), aa(load(r13 + 4*11)),
            aa(load(r13 + 4*12)), aa(load(r13 + 4*13)), aa(load(r13 + 4*14)), aa(load(r13 + 4*15)));
        return 1;
    } else if (addr == JNIEnv_functions.CallStaticVoidMethod) {   // 141
        LOG_I("calling CallStaticVoidMethod(%p, %p, %p, [%p, %p, ... ])", r0, r1, r2, r3, load(r13));
        // TODO: what if jlong comes unaligned?
        // TODO: use runtime tables to recall method signature
        (void)JNIEnv_functions.CallStaticVoidMethod((JNIEnv*)aa(r0), (jobject)aa(r1), (jmethodID)aa(r2),
            aa(r3), aa(load(r13)), aa(load(r13 + 4)), aa(load(r13 + 4*2)), aa(load(r13 + 4*3)),
            aa(load(r13 + 4*4)), aa(load(r13 + 4*5)), aa(load(r13 + 4*6)), aa(load(r13 + 4*7)),
            aa(load(r13 + 4*8)), aa(load(r13 + 4*9)), aa(load(r13 + 4*10)), aa(load(r13 + 4*11)),
            aa(load(r13 + 4*12)), aa(load(r13 + 4*13)), aa(load(r13 + 4*14)), aa(load(r13 + 4*15)));
        return 1;
    }


    // ===================

    if (addr == JavaVM_functions.GetEnv) {  //  6
        LOG_I("calling JavaVM_functions.GetEnv(%p, %p, %p)", r0, r1, r2);
        r0_signed = JavaVM_functions.GetEnv((JavaVM*)aa(r0), (void**)aa(r1), (jint)r2);
        LOG_I("        JavaVM_functions.GetEnv returned %d", r0);
        return 1;
    } else if (addr == JavaVM_functions.AttachCurrentThread) {   //  4
        LOG_I("calling JavaVM_functions.AttachCurrentThread(%p, %p, %p)", r0, r1, r2);
        if (0 != r2) {
            JavaVMAttachArgs *attach_args = (void *)(aa(r2));
            attach_args->name = aa((uint32_t)(attach_args->name));
        }
        r0_signed = JavaVM_functions.AttachCurrentThread((JavaVM*)aa(r0), (JNIEnv**)aa(r1), (void*)aa(r2));
        LOG_I("        JavaVM_functions.AttachCurrentThread returned %d", r0);
        return 1;
    } else if (addr == JavaVM_functions.DetachCurrentThread) {  // 5
        LOG_I("calling JavaVM_functionsDetachCurrentThread(%p)", r0);
        r0_signed = JavaVM_functions.DetachCurrentThread((JavaVM*)aa(r0));
        LOG_I("        JavaVM_functionsDetachCurrentThread returned %d", r0);
        return 1;
    }


    // ===================


    uint32_t k;
    void **table = (void **)&JNIEnv_functions;
    uint32_t table_size = sizeof(struct JNINativeInterface)/sizeof(void *);

    for (k = 0; k < table_size; k ++) {
        if (table[k] == addr) {
            LOG_E("find_and_call_function_runtime: "
                "function %p found in table JNIEnv_functions at %d, but not called", addr, k);
            return 1;
        }
    }

    table = (void **)&JavaVM_functions;
    table_size = sizeof(struct JNIInvokeInterface)/sizeof(void *);

    for (k = 0; k < table_size; k ++) {
        if (table[k] == addr) {
            LOG_E("find_and_call_function_runtime: "
                "function %p found in table JavaVM_functions at %d, but not called", addr, k);
            return 1;
        }
    }

    return 0;
}
