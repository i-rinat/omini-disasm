/// @file findfunction-jni.h

static struct JNINativeInterface JNIEnv_functions;
static struct JNIInvokeInterface JavaVM_functions;

#define jni_method_table_max_elements   200
static uint32_t jni_method_table_element_count = 0;
static struct {
    jmethodID   methodID;
    const char *signature;
} jni_method_table[jni_method_table_max_elements];

static
void
memorize_method_signature(jmethodID methodID, const char *provided_signature)
{
    if (jni_method_table_element_count >= jni_method_table_max_elements) {
        LOG_E("memorize_method_signature: too many methods");
        return;
    }

    char *signature = strdup(provided_signature);

    // simplify signature. Convert L's to P (pointer), Z to I, F and D to D, J to J
    // It seems (android) JNI requires floats to be expanded to doubles

    const char *src_ptr = provided_signature + 1; // skip '('
    char *dst_ptr = signature;


    while (1) {
        switch (*src_ptr) {
        case 'J':
            *dst_ptr++ = 'J';
            src_ptr ++;
            break;
        case 'F':
            // fall through
        case 'D':
            *dst_ptr++ = 'D';
            src_ptr ++;
            break;
        case 'L':
            *dst_ptr++ = 'P';
            while (*src_ptr != ';') src_ptr ++;
            src_ptr ++;
            break;
        case '[':
            *dst_ptr++ = 'P';
            src_ptr ++;
            if ('L' == *src_ptr)
                while (*src_ptr != ';') src_ptr ++;
            src_ptr ++;
            break;
        case ')':
            *dst_ptr = 0;
            goto done;
            break;
        default:
            *dst_ptr++ = 'I';
            src_ptr ++;
            break;
        }
    }
done:

    LOG_I("registered methodID %p with signature %s simplified to %s", vv(methodID), provided_signature, signature);

    jni_method_table[jni_method_table_element_count].signature = signature;
    jni_method_table[jni_method_table_element_count].methodID = methodID;
    jni_method_table_element_count ++;
}

static
const char *
get_signature_for_method(jmethodID methodID)
{
    uint32_t k;
    for (k = 0; k < jni_method_table_element_count; k ++)
        if (methodID == jni_method_table[k].methodID)
            return jni_method_table[k].signature;
    return NULL;
}

static
void
doCallStaticVoidMethod(state_t *state)
{
    JNIEnv *jnienv = (JNIEnv*)r0;
    jobject obj = (jobject)r1;
    jmethodID   methodID = (jmethodID)r2;
    const char *signature = get_signature_for_method(methodID);

    if (NULL == signature) {
        LOG_E("doCallStaticVoidMethod: no method %p in jni_method_table", vv(methodID));
        return;
    }

    if (!strcmp(signature, "")) {   // no arguments
        LOG_I("calling JNIEnv_functions.CallStaticVoidMethod(%p, %p, %p, [])", vv(jnienv), vv(obj),
            vv(methodID));
        (void)JNIEnv_functions.CallStaticVoidMethod(jnienv, obj, methodID);
        LOG_I("        JNIEnv_functions.CallStaticVoidMethod returned void");
    } else if (!strcmp(signature, "I")) {
        uint32_t param1 = r3;
        LOG_I("calling JNIEnv_functions.CallStaticVoidMethod(%p, %p, %p, [%d])", vv(jnienv), vv(obj),
            vv(methodID), param1);
        (void)JNIEnv_functions.CallStaticVoidMethod(jnienv, obj, methodID, param1);
        LOG_I("        JNIEnv_functions.CallStaticVoidMethod returned void");
    } else if (!strcmp(signature, "unknown")) {
    } else if (!strcmp(signature, "unknown")) {
    } else if (!strcmp(signature, "unknown")) {
    } else if (!strcmp(signature, "unknown")) {
    } else if (!strcmp(signature, "unknown")) {
    } else {
        LOG_E("doCallStaticVoidMethod: unknown signature %s", signature);
        return;
    }
}

static
void
doCallStaticObjectMethod(state_t *state)
{
    JNIEnv *jnienv = (JNIEnv*)r0;
    jobject obj = (jobject)r1;
    jmethodID   methodID = (jmethodID)r2;
    const char *signature = get_signature_for_method(methodID);

    if (NULL == signature) {
        LOG_E("doCallStaticObjectMethod: no method %p in jni_method_table", vv(methodID));
        return;
    }

    if (!strcmp(signature, "PIIIIDPI")) {
        void    *param1 = (void*)r3;
        uint32_t param2 = load(r13);
        uint32_t param3 = load(r13 + 4);
        uint32_t param4 = load(r13 + 8);
        uint32_t param5 = load(r13 + 12);
        double   param6 = load_double(r13 + 16);
        void    *param7 = (void*)load(r13 + 24);
        uint32_t param8 = load(r13 + 28);
        LOG_I("calling JNIEnv_functions.CallStaticObjectMethod(%p, %p, %p, [%p, %d, %d, %d, %d, %f, %p, %d])",
                vv(jnienv), vv(obj), vv(methodID), param1, param2, param3, param4, param5,
                param6, param7, param8);
        r0 = (uint32_t)JNIEnv_functions.CallStaticObjectMethod(jnienv, obj, methodID, param1,
                param2, param3, param4, param5, param6, param7, param8);
        LOG_I("        JNIEnv_functions.CallStaticObjectMethod returned %p", vv(r0));
    } else if (!strcmp(signature, "I")) {
        uint32_t    param1 = r3;
        LOG_I("calling JNIEnv_functions.CallStaticObjectMethod(%p, %p, %p, [%d])",
                vv(jnienv), vv(obj), vv(methodID), param1);
        r0 = (uint32_t)JNIEnv_functions.CallStaticObjectMethod(jnienv, obj, methodID, param1);
        LOG_I("        JNIEnv_functions.CallStaticObjectMethod returned %p", vv(r0));
    } else if (!strcmp(signature, "unknown")) {
    } else if (!strcmp(signature, "unknown")) {
    } else if (!strcmp(signature, "unknown")) {
    } else if (!strcmp(signature, "unknown")) {
    } else if (!strcmp(signature, "unknown")) {
    } else {
        LOG_E("doCallStaticObjectMethod: unknown signature %s", signature);
        return;
    }
}

static
void
doCallVoidMethod(state_t *state)
{
    JNIEnv *jnienv = (JNIEnv*)r0;
    jobject obj = (jobject)r1;
    jmethodID   methodID = (jmethodID)r2;
    const char *signature = get_signature_for_method(methodID);

    if (NULL == signature) {
        LOG_E("doCallVoidMethod: no method %p in jni_method_table", vv(methodID));
        return;
    }

    if (!strcmp(signature, "")) {   // no arguments
        LOG_I("calling JNIEnv_functions.CallVoidMethod(%p, %p, %p, [])", vv(jnienv), vv(obj), vv(methodID));
        (void)JNIEnv_functions.CallVoidMethod(jnienv, obj, methodID);
        LOG_I("        JNIEnv_functions.CallVoidMethod returned void");
    } else if (!strcmp(signature, "I")) {
        uint32_t param1 = r3;
        LOG_I("calling JNIEnv_functions.CallVoidMethod(%p, %p, %p, [%d])", vv(jnienv), vv(obj),
                    vv(methodID), param1);
        (void)JNIEnv_functions.CallVoidMethod(jnienv, obj, methodID, param1);
        LOG_I("        JNIEnv_functions.CallVoidMethod returned void");
    } else if (!strcmp(signature, "P")) {
        void *param1 = (void*)r3;
        LOG_I("calling JNIEnv_functions.CallVoidMethod(%p, %p, %p, [%p])", vv(jnienv), vv(obj),
                    vv(methodID), param1);
        (void)JNIEnv_functions.CallVoidMethod(jnienv, obj, methodID, param1);
        LOG_I("        JNIEnv_functions.CallVoidMethod returned void");
    } else if (!strcmp(signature, "II")) {
        uint32_t param1 = r3;
        uint32_t param2 = load(r13);
        LOG_I("calling JNIEnv_functions.CallVoidMethod(%p, %p, %p, [%d, %d])", vv(jnienv), vv(obj),
                    vv(methodID), param1, param2);
        (void)JNIEnv_functions.CallVoidMethod(jnienv, obj, methodID, param1, param2);
        LOG_I("        JNIEnv_functions.CallVoidMethod returned void");
    } else if (!strcmp(signature, "IIII")) {
        uint32_t param1 = r3;
        uint32_t param2 = load(r13);
        uint32_t param3 = load(r13 + 4);
        uint32_t param4 = load(r13 + 8);
        LOG_I("calling JNIEnv_functions.CallVoidMethod(%p, %p, %p, [%d, %d, %d, %d])", vv(jnienv), vv(obj),
                    vv(methodID), param1, param2, param3, param4);
        (void)JNIEnv_functions.CallVoidMethod(jnienv, obj, methodID, param1, param2, param3, param4);
        LOG_I("        JNIEnv_functions.CallVoidMethod returned void");
    } else if (!strcmp(signature, "unknown")) {
    } else if (!strcmp(signature, "unknown")) {
    } else {
        LOG_E("doCallVoidMethod: unknown signature %s", signature);
        return;
    }
}

static
void
doCallIntMethod(state_t *state)
{
    JNIEnv *jnienv = (JNIEnv*)r0;
    jobject obj = (jobject)r1;
    jmethodID   methodID = (jmethodID)r2;
    const char *signature = get_signature_for_method(methodID);

    if (NULL == signature) {
        LOG_E("doCallIntMethod: no method %p in jni_method_table", vv(methodID));
        return;
    }

    if (!strcmp(signature, "")) {
        LOG_I("calling JNIEnv_functions.CallIntMethod(%p, %p, %p, [])", vv(jnienv), vv(obj), vv(methodID));
        r0_signed =    JNIEnv_functions.CallIntMethod(jnienv, obj, methodID);
        LOG_I("        JNIEnv_functions.CallIntMethod returned %d", r0);
    } else if (!strcmp(signature, "PIID")) {
        void       *param1 = (void*)r3;
        uint32_t    param2 = load(r13);
        uint32_t    param3 = load(r13 + 4);
        double      param4 = load_double(r13 + 8);
        LOG_I("calling JNIEnv_functions.CallIntMethod(%p, %p, %p, [%p, %d, %d, %f])", vv(jnienv),
            vv(obj), vv(methodID), param1, param2, param3, param4);
        r0_signed =    JNIEnv_functions.CallIntMethod(jnienv, obj, methodID, param1, param2, param3, param4);
        LOG_I("        JNIEnv_functions.CallIntMethod returned %d", r0);
    } else if (!strcmp(signature, "unknown")) {
    } else if (!strcmp(signature, "unknown")) {
    } else if (!strcmp(signature, "unknown")) {
    } else if (!strcmp(signature, "unknown")) {
    } else if (!strcmp(signature, "unknown")) {
    } else {
        LOG_E("doCallIntMethod: unknown signature %s", signature);
        return;
    }
}

static
void
doCallObjectMethod(state_t *state)
{
    JNIEnv *jnienv = (JNIEnv*)r0;
    jobject obj = (jobject)r1;
    jmethodID   methodID = (jmethodID)r2;
    const char *signature = get_signature_for_method(methodID);

    if (NULL == signature) {
        LOG_E("doCallObjectMethod: no method %p in jni_method_table", vv(methodID));
        return;
    }

    if (!strcmp(signature, "")) {
        LOG_I("calling JNIEnv_functions.CallObjectMethod(%p, %p, %p, [])", vv(jnienv), vv(obj), vv(methodID));
        r0 = (uint32_t)JNIEnv_functions.CallObjectMethod(jnienv, obj, methodID);
        LOG_I("        JNIEnv_functions.CallObjectMethod returned %p", vv(r0));
    } else if (!strcmp(signature, "I")) {
        uint32_t param1 = r3;
        LOG_I("calling JNIEnv_functions.CallObjectMethod(%p, %p, %p, [%d])", vv(jnienv), vv(obj),
            vv(methodID), param1);
        r0 = (uint32_t)JNIEnv_functions.CallObjectMethod(jnienv, obj, methodID, param1);
        LOG_I("        JNIEnv_functions.CallObjectMethod returned %p", vv(r0));
    } else if (!strcmp(signature, "IPI")) {
        uint32_t    param1 = r3;
        void       *param2 = (void*)load(r13);
        uint32_t    param3 = load(r13 + 4);
        LOG_I("calling JNIEnv_functions.CallObjectMethod(%p, %p, %p, [%d, %p, %d])", vv(jnienv), vv(obj),
            vv(methodID), param1, param2, param3);
        r0 = (uint32_t)JNIEnv_functions.CallObjectMethod(jnienv, obj, methodID, param1, param2, param3);
        LOG_I("        JNIEnv_functions.CallObjectMethod returned %p", vv(r0));
    } else if (!strcmp(signature, "II")) {
        uint32_t    param1 = r3;
        uint32_t    param2 = load(r13);
        LOG_I("calling JNIEnv_functions.CallObjectMethod(%p, %p, %p, [%d, %d])", vv(jnienv), vv(obj),
            vv(methodID), param1, param2);
        r0 = (uint32_t)JNIEnv_functions.CallObjectMethod(jnienv, obj, methodID, param1, param2);
        LOG_I("        JNIEnv_functions.CallObjectMethod returned %p", vv(r0));
    } else if (!strcmp(signature, "unknown")) {
    } else if (!strcmp(signature, "unknown")) {
    } else if (!strcmp(signature, "unknown")) {
    } else if (!strcmp(signature, "unknown")) {
    } else {
        LOG_E("doCallObjectMethod: unknown signature %s", signature);
        return;
    }
}

static
void
doCallBooleanMethod(state_t *state)
{
    JNIEnv *jnienv = (JNIEnv*)r0;
    jobject obj = (jobject)r1;
    jmethodID   methodID = (jmethodID)r2;
    const char *signature = get_signature_for_method(methodID);

    if (NULL == signature) {
        LOG_E("doCallBooleanMethod: no method %p in jni_method_table", vv(methodID));
        return;
    }

    if (!strcmp(signature, "")) {
        LOG_I("calling JNIEnv_functions.CallBooleanMethod(%p, %p, %p, [])", vv(jnienv), vv(obj), vv(methodID));
        r0 = (uint32_t)JNIEnv_functions.CallBooleanMethod(jnienv, obj, methodID);
        LOG_I("        JNIEnv_functions.CallBooleanMethod returned %d", r0);
    } else if (!strcmp(signature, "unknown")) {
    } else if (!strcmp(signature, "unknown")) {
    } else if (!strcmp(signature, "unknown")) {
    } else if (!strcmp(signature, "unknown")) {
    } else if (!strcmp(signature, "unknown")) {
    } else if (!strcmp(signature, "unknown")) {
    } else {
        LOG_E("doCallBooleanMethod: unknown signature %s", signature);
        return;
    }
}

static
int
find_and_call_function_runtime(state_t *state, void *addr)
{
    if (addr == JNIEnv_functions.GetJavaVM) {   // 219
        LOG_I("calling GetJavaVM(%p, %p)", vv(r0), vv(r1));
        r0_signed = JNIEnv_functions.GetJavaVM((JNIEnv*)r0, (JavaVM**)r1);
        LOG_I("        GetJavaVM returned %d", r0);
        return 1;
    } else if (addr == JNIEnv_functions.GetVersion) {   // 4
        LOG_I("calling GetVersion(%p)", vv(r0));
        r0_signed = JNIEnv_functions.GetVersion((JNIEnv *)r0);
        LOG_I("        GetVersion returned 0x%x", r0);
        return 1;
    } else if (addr == JNIEnv_functions.ThrowNew) { // 14
        LOG_I("calling ThrowNew(%p, %p, %p \"%s\")", vv(r0), vv(r1), vv(r2), r2);
        r0_signed = JNIEnv_functions.ThrowNew((JNIEnv *)r0, (jclass)r1, (const char *)r2);
        return 1;
    } else if (addr == JNIEnv_functions.FindClass) { // 6
        LOG_I("calling FindClass(%p, %p \"%s\")", vv(r0), vv(r1), r1);
        r0 = (uint32_t)JNIEnv_functions.FindClass((JNIEnv*)r0, (const char*)r1);
        LOG_I("        FindClass retuned %p", vv(r0));
        return 1;
    } else if (addr == JNIEnv_functions.GetFieldID) { // 94
        LOG_I("calling GetFieldID(%p, %p, %p \"%s\", %p \"%s\")", vv(r0), vv(r1), vv(r2), r2, vv(r3), r3);
        r0 = (uint32_t)JNIEnv_functions.GetFieldID((JNIEnv*)r0, (jclass)r1,
                                                    (const char*)r2, (const char*)r3);
        LOG_I("        GetFieldID returned %p", vv(r0));
        return 1;
    } else if (addr == JNIEnv_functions.DeleteLocalRef) {   // 23
        LOG_I("calling DeleteLocalRef(%p, %p)", vv(r0), vv(r1));
        (void)JNIEnv_functions.DeleteLocalRef((JNIEnv*)r0, (jobject)r1);
        LOG_I("        DeleteLocalRef returned void");
        return 1;
    } else if (addr == JNIEnv_functions.NewGlobalRef) { // 21
        LOG_I("calling NewGlobalRef(%p, %p)", vv(r0), vv(r1));
        r0 = (uint32_t)JNIEnv_functions.NewGlobalRef((JNIEnv*)r0, (jobject)r1);
        LOG_I("        NewGlobalRef returned %p", vv(r0));
        return 1;
    } else if (addr == JNIEnv_functions.DeleteGlobalRef) { // 22
        LOG_I("calling JNIEnv_functions.DeleteGlobalRef(%p, %p)", vv(r0), vv(r1));
        (void)JNIEnv_functions.DeleteGlobalRef((JNIEnv*)r0, (jobject)r1);
        LOG_I("        JNIEnv_functions.DeleteGlobalRef returned void");
        return 1;
    } else if (addr == JNIEnv_functions.GetStaticFieldID) { // 144
        LOG_I("calling GetStaticFieldID(%p, %p, %p \"%s\", %p \"%s\")", vv(r0), vv(r1), vv(r2), r2, vv(r3), r3);
        r0 = (uint32_t)JNIEnv_functions.GetStaticFieldID((JNIEnv*)r0, (jclass)r1,
                                            (const char*)r2, (const char*)r3);
        LOG_I("        GetStaticFieldID returned %p", vv(r0));
        return 1;
    } else if (addr == JNIEnv_functions.GetMethodID) {  // 33
        LOG_I("calling GetMethodID(%p, %p, %p \"%s\", %p \"%s\")", vv(r0), vv(r1), vv(r2), r2, vv(r3), r3);
        const char *signature = (const char *)r3;
        r0 = (uint32_t)JNIEnv_functions.GetMethodID((JNIEnv*)r0, (jclass)r1,
                                                    (const char*)r2, (const char*)r3);
        LOG_I("        GetMethodID returned %p", vv(r0));
        memorize_method_signature((jmethodID)r0, signature);
        return 1;
    } else if (addr == JNIEnv_functions.GetStaticMethodID) {    // 113
        LOG_I("calling GetStaticMethodID(%p, %p, %p \"%s\", %p \"%s\")", vv(r0), vv(r1), vv(r2), r2, vv(r3), r3);
        const char *signature = (const char *)r3;
        r0 = (uint32_t)JNIEnv_functions.GetStaticMethodID((JNIEnv*)r0, (jclass)r1,
                                                    (const char*)r2, (const char*)r3);
        LOG_I("        GetStaticMethodID returned %p", vv(r0));
        memorize_method_signature((jmethodID)r0, signature);
        return 1;
    } else if (addr == JNIEnv_functions.GetStaticLongField) {   // 151
        LOG_I("calling GetStaticLongField(%p, %p, %p)", vv(r0), vv(r1), vv(r2));
        x_int64_t = JNIEnv_functions.GetStaticLongField((JNIEnv*)r0, (jclass)r1, (jfieldID)r2);
        LOG_I("        GetStaticLongField returned %lld", x_int64_t);
        return 1;
    } else if (addr == JNIEnv_functions.ExceptionClear) {   // 17
        LOG_I("calling ExceptionClear");
        (void)JNIEnv_functions.ExceptionClear((JNIEnv*)r0);
        return 1;
    } else if (addr == JNIEnv_functions.ExceptionCheck) {   // 228
        LOG_I("calling ExceptionCheck(%p)", vv(r0));
        r0 = (uint32_t)JNIEnv_functions.ExceptionCheck((JNIEnv*)r0);
        LOG_I("        ExceptionCheck returned %d", r0);
        return 1;
    } else if (addr == JNIEnv_functions.GetBooleanField) {   // 96
        LOG_I("calling JNIEnv_functions.GetBooleanField(%p, %p, %p)", vv(r0), vv(r1), vv(r2));
        r0 = (uint32_t)JNIEnv_functions.GetBooleanField((JNIEnv*)r0, (jobject)r1, (jfieldID)r2);
        LOG_I("        JNIEnv_functions.GetBooleanField returned %p", vv(r0));
        return 1;
    } else if (addr == JNIEnv_functions.GetIntField) {   // 100
        LOG_I("calling GetIntField(%p, %p, %p)", vv(r0), vv(r1), vv(r2));
        r0_signed = JNIEnv_functions.GetIntField((JNIEnv*)r0, (jobject)r1, (jfieldID)r2);
        LOG_I("        GetIntField returned 0x%08x", r0);
        return 1;
    } else if (addr == JNIEnv_functions.GetLongField) {   // 101
        LOG_I("calling GetLongField(%p, %p, %p)", vv(r0), vv(r1), vv(r2));
        x_int64_t = JNIEnv_functions.GetLongField((JNIEnv*)r0, (jobject)r1, (jfieldID)r2);
        LOG_I("        GetLongField returned 0x%08x%08x", r1, r0);
        return 1;
    } else if (addr == JNIEnv_functions.SetObjectField) {   // 104
        LOG_I("calling JNIEnv_functions.SetObjectField(%p, %p, %p, %p)", vv(r0), vv(r1), vv(r2), vv(r3));
        (void)JNIEnv_functions.SetObjectField((JNIEnv*)r0, (jobject)r1, (jfieldID)r2, (jobject)r3);
        LOG_I("        JNIEnv_functions.SetObjectField returned void");
        return 1;
    } else if (addr == JNIEnv_functions.SetIntField) {   // 109
        LOG_I("calling JNIEnv_functions.SetIntField(%p, %p, %p, %p)", vv(r0), vv(r1), vv(r2), vv(r3));
        (void)JNIEnv_functions.SetIntField((JNIEnv*)r0, (jobject)r1, (jfieldID)r2, (jint)r3);
        LOG_I("        JNIEnv_functions.SetIntField returned void");
        return 1;
    } else if (addr == JNIEnv_functions.SetLongField) {   // 110
        jlong value = load(r13) + (((uint64_t)load(r13 + 4)) << 32);
        LOG_I("calling SetLongField(%p, %p, %p, 0x%08x%08x)", vv(r0), vv(r1), vv(r2),
                            (uint32_t)(value >> 32), (uint32_t)(value & 0xffffffff));
        (void)JNIEnv_functions.SetLongField((JNIEnv*)r0, (jobject)r1, (jfieldID)r2, value);
        LOG_I("        SetLongField returned void");
        return 1;
    } else if (addr == JNIEnv_functions.GetPrimitiveArrayCritical) {    // 222
        LOG_I("calling GetPrimitiveArrayCritical(%p, %p, %p)", vv(r0), vv(r1), vv(r2));
        r0 = (uint32_t)JNIEnv_functions.GetPrimitiveArrayCritical((JNIEnv*)r0, (jarray)r1,
                                                                    (jboolean*)r2);
        LOG_I("        GetPrimitiveArrayCritical returned %p", vv(r0));
        return 1;
    } else if (addr == JNIEnv_functions.ReleasePrimitiveArrayCritical) {    // 223
        LOG_I("calling ReleasePrimitiveArrayCritical(%p, %p, %p, %d)", vv(r0), vv(r1), vv(r2), r3);
        (void)JNIEnv_functions.ReleasePrimitiveArrayCritical((JNIEnv*)r0, (jarray)r1,
                                                                    (void*)r2, (jint)r3);
        LOG_I("        ReleasePrimitiveArrayCritical returned void");
        return 1;
    } else if (addr == JNIEnv_functions.GetObjectClass) {       // 31
        LOG_I("calling GetObjectClass(%p, %p)", vv(r0), vv(r1));
        r0 = (uint32_t)JNIEnv_functions.GetObjectClass((JNIEnv*)r0, (jobject)r1);
        LOG_I("        GetObjectClass returned %p", vv(r0));
        return 1;
    } else if (addr == JNIEnv_functions.GetObjectField) {   // 95
        LOG_I("calling GetObjectField(%p, %p, %p)", vv(r0), vv(r1), vv(r2));
        r0 = (uint32_t)JNIEnv_functions.GetObjectField((JNIEnv*)r0, (jobject)r1, (jfieldID)r2);
        LOG_I("        GetObjectField returned %p", vv(r0));
        return 1;
    } else if (addr == JNIEnv_functions.GetIntArrayElements) {  // 187
        LOG_I("calling GetIntArrayElements(%p, %p, %p)", vv(r0), vv(r1), vv(r2));
        r0 = (uint32_t)JNIEnv_functions.GetIntArrayElements((JNIEnv*)r0, (jintArray)r1, (jboolean*)r2);
        LOG_I("        GetIntArrayElements returned %p", vv(r0));
        return 1;
    } else if (addr == JNIEnv_functions.ReleaseIntArrayElements) {  // 195
        LOG_I("calling ReleaseIntArrayElements(%p, %p, %p, %d)", vv(r0), vv(r1), vv(r2), r3);
        (void)JNIEnv_functions.ReleaseIntArrayElements((JNIEnv*)r0, (jintArray)r1, (jint*)r2, (jint)r3);
        LOG_I("        ReleaseIntArrayElements retrned void");
        return 1;
    } else if (addr == JNIEnv_functions.GetByteArrayRegion) {   // 200
        uint32_t param5 = load(r13);
        LOG_I("calling JNIEnv_functions.GetByteArrayRegion(%p, %p, %d, %d, %p)", vv(r0), vv(r1), r2, r3, vv(param5));
        (void)JNIEnv_functions.GetByteArrayRegion((JNIEnv*)r0, (jbyteArray)r1, (jsize)r2,
                                                    (jsize)r3, (jbyte*)param5);
        LOG_I("        JNIEnv_functions.GetByteArrayRegion returned void");
        return 1;
    } else if (addr == JNIEnv_functions.GetIntArrayRegion) {   // 203
        uint32_t param5 = load(r13);
        LOG_I("calling JNIEnv_functions.GetIntArrayRegion(%p, %p, %d, %d, %p)", vv(r0), vv(r1), r2, r3, vv(param5));
        (void)JNIEnv_functions.GetIntArrayRegion((JNIEnv*)r0, (jintArray)r1, (jsize)r2,
                                                    (jsize)r3, (jint*)param5);
        LOG_I("        JNIEnv_functions.GetIntArrayRegion returned void");
        return 1;
    } else if (addr == JNIEnv_functions.SetByteArrayRegion) {   // 208
        uint32_t param5 = load(r13);
        LOG_I("calling SetByteArrayRegion(%p, %p, %d, %d, %p)", vv(r0), vv(r1), r2, r3, vv(param5));
        (void)JNIEnv_functions.SetByteArrayRegion((JNIEnv*)r0, (jbyteArray)r1, (jsize)r2,
                                                    (jsize)r3, (const jbyte*)param5);
        return 1;
    } else if (addr == JNIEnv_functions.CallVoidMethod) {   // 61
        doCallVoidMethod(state);
        return 1;
    } else if (addr == JNIEnv_functions.CallIntMethod) {   // 49
        doCallIntMethod(state);
        return 1;
    } else if (addr == JNIEnv_functions.CallObjectMethod) {   // 34
        doCallObjectMethod(state);
        return 1;
    } else if (addr == JNIEnv_functions.CallBooleanMethod) {   // 37
        doCallBooleanMethod(state);
        return 1;
    } else if (addr == JNIEnv_functions.CallStaticVoidMethod) {   // 141
        doCallStaticVoidMethod(state);
        return 1;
    } else if (addr == JNIEnv_functions.CallStaticObjectMethod) {   // 114
        doCallStaticObjectMethod(state);
        return 1;
    } else if (addr == JNIEnv_functions.NewStringUTF) { //167
        LOG_I("calling JNIEnv_functions.NewStringUTF(%p, %p \"%s\")", vv(r0), vv(r1), r1);
        r0 = (uint32_t)JNIEnv_functions.NewStringUTF((JNIEnv*)r0, (const char*)r1);
        LOG_I("        JNIEnv_functions.NewStringUTF returned %p", vv(r0));
        return 1;
    } else if (addr == JNIEnv_functions.GetStringLength) {  // 164
        LOG_I("calling JNIEnv_functions.GetStringLength(%p, %p)", vv(r0), vv(r1));
        r0 = (uint32_t)JNIEnv_functions.GetStringLength((JNIEnv*)r0, (jstring)r1);
        LOG_I("        JNIEnv_functions.GetStringLength returned %d", r0);
        return 1;
    } else if (addr == JNIEnv_functions.GetArrayLength) {  // 171
        LOG_I("calling JNIEnv_functions.GetArrayLength(%p, %p)", vv(r0), vv(r1));
        r0 = (uint32_t)JNIEnv_functions.GetArrayLength((JNIEnv*)r0, (jarray)r1);
        LOG_I("        JNIEnv_functions.GetArrayLength returned %d", r0);
        return 1;
    } else if (addr == JNIEnv_functions.GetStringCritical) {  // 224
        LOG_I("calling JNIEnv_functions.GetStringCritical(%p, %p, %p)", vv(r0), vv(r1), vv(r2));
        r0 = (uint32_t)JNIEnv_functions.GetStringCritical((JNIEnv*)r0, (jstring)r1, (jboolean*)r2);
        LOG_I("        JNIEnv_functions.GetStringCritical returned %p", vv(r0));
        return 1;
    } else if (addr == JNIEnv_functions.ReleaseStringCritical) {  // 225
        LOG_I("calling JNIEnv_functions.ReleaseStringCritical(%p, %p, %p)", vv(r0), vv(r1), vv(r2));
        (void)JNIEnv_functions.ReleaseStringCritical((JNIEnv*)r0, (jstring)r1, (const jchar*)r2);
        LOG_I("        JNIEnv_functions.ReleaseStringCritical returned void");
        return 1;
    } else if (addr == JNIEnv_functions.NewByteArray) {  // 176
        LOG_I("calling JNIEnv_functions.NewByteArray(%p, %d)", vv(r0), r1);
        r0 = (uint32_t)JNIEnv_functions.NewByteArray((JNIEnv*)r0, (jsize)r1);
        LOG_I("        JNIEnv_functions.NewByteArray returned %p", vv(r0));
        return 1;
    } else if (addr == JNIEnv_functions.NewCharArray) {  // 177
        LOG_I("calling JNIEnv_functions.NewCharArray(%p, %d)", vv(r0), r1);
        r0 = (uint32_t)JNIEnv_functions.NewCharArray((JNIEnv*)r0, (jsize)r1);
        LOG_I("        JNIEnv_functions.NewCharArray returned %p", vv(r0));
        return 1;
    } else if (addr == JNIEnv_functions.NewIntArray) {  // 179
        LOG_I("calling JNIEnv_functions.NewIntArray(%p, %d)", vv(r0), r1);
        r0 = (uint32_t)JNIEnv_functions.NewIntArray((JNIEnv*)r0, (jsize)r1);
        LOG_I("        JNIEnv_functions.NewIntArray returned %p", vv(r0));
        return 1;
    } else if (addr == JNIEnv_functions.GetStaticObjectField) { //  145
        LOG_I("calling JNIEnv_functions.GetStaticObjectField(%p, %p, %p)", vv(r0), vv(r1), vv(r2));
        r0 = (uint32_t)JNIEnv_functions.GetStaticObjectField((JNIEnv*)r0, (jclass)r1, (jfieldID)r2);
        LOG_I("        JNIEnv_functions.GetStaticObjectField returned %p", vv(r0));
        return 1;
    } else if (addr == JNIEnv_functions.GetStaticIntField) { //  150
        LOG_I("calling JNIEnv_functions.GetStaticIntField(%p, %p, %p)", vv(r0), vv(r1), vv(r2));
        r0_signed =    JNIEnv_functions.GetStaticIntField((JNIEnv*)r0, (jclass)r1, (jfieldID)r2);
        LOG_I("        JNIEnv_functions.GetStaticIntField returned %p", vv(r0));
        return 1;
    } else if (addr == JNIEnv_functions.IsSameObject) { //  24
        LOG_I("calling JNIEnv_functions.IsSameObject(%p, %p, %p)", vv(r0), vv(r1), vv(r2));
        r0 = (uint32_t)JNIEnv_functions.IsSameObject((JNIEnv*)r0, (jobject)r1, (jobject)r2);
        LOG_I("        JNIEnv_functions.IsSameObject returned %d", r0);
        return 1;
    } else if (addr == JNIEnv_functions.SetIntArrayRegion) {    //  211
        const uint32_t param5 = load(r13);
        LOG_I("calling JNIEnv_functions.SetIntArrayRegion(%p, %p, %d, %d, %p)", vv(r0), vv(r1), r2, r3, vv(param5));
        (void)JNIEnv_functions.SetIntArrayRegion((JNIEnv*)r0, (jintArray)r1, (jsize)r2, (jsize)r3, (const jint*)param5);
        LOG_I("calling JNIEnv_functions.SetIntArrayRegion returned void");
        return 1;
    } else if (addr == JNIEnv_functions.NewLocalRef) {  //  25
        LOG_I("calling JNIEnv_functions.NewLocalRef(%p, %p)", vv(r0), vv(r1));
        r0 = (uint32_t)JNIEnv_functions.NewLocalRef((JNIEnv*)r0, (jobject)r1);
        LOG_I("        JNIEnv_functions.NewLocalRef returned %p", vv(r0));
        return 1;
    } else if (addr == JNIEnv_functions.NewDirectByteBuffer) {  //  229
        int64_t jlongvalue = r2 | ((int64_t)r3 << 32);
        LOG_I("calling JNIEnv_functions.NewDirectByteBuffer(%p, %p, 0x%08x%08x)", vv(r0), vv(r1), r3, r2);
        r0 = (uint32_t)JNIEnv_functions.NewDirectByteBuffer((JNIEnv*)r0, (void*)r1, (jlong)jlongvalue);
        LOG_I("        JNIEnv_functions.NewDirectByteBuffer returned %p", vv(r0));
        return 1;
    } else if (addr == JNIEnv_functions.GetStringUTFLength) {   //  168
        LOG_I("calling JNIEnv_functions.GetStringUTFLength(%p, %p)", vv(r0), vv(r1));
        r0_signed =    JNIEnv_functions.GetStringUTFLength((JNIEnv*)r0, (jstring)r1);
        LOG_I("        JNIEnv_functions.GetStringUTFLength returned %d)", r0);
        return 1;
    } else if (addr == JNIEnv_functions.GetStringUTFChars) {   //  169
        LOG_I("calling JNIEnv_functions.GetStringUTFChars(%p, %p, %p)", vv(r0), vv(r1), vv(r2));
        r0 = (uint32_t)JNIEnv_functions.GetStringUTFChars((JNIEnv*)r0, (jstring)r1, (jboolean*)r2);
        LOG_I("        JNIEnv_functions.GetStringUTFChars returned %p)", vv(r0));
        return 1;
    } else if (addr == JNIEnv_functions.ReleaseStringUTFChars) {   //  170
        LOG_I("calling JNIEnv_functions.ReleaseStringUTFChars(%p, %p, %p)", vv(r0), vv(r1), vv(r2));
        (void)JNIEnv_functions.ReleaseStringUTFChars((JNIEnv*)r0, (jstring)r1, (const char*)r2);
        LOG_I("        JNIEnv_functions.ReleaseStringUTFChars returned void)");
        return 1;
    } else if (addr == JNIEnv_functions.GetStringUTFRegion) {   // 221
        uint32_t param5 = load(r13);
        LOG_I("calling JNIEnv_functions.GetStringUTFRegion(%p, %p, %d, %d, %p)", vv(r0), vv(r1), r2, r3, vv(param5));
        (void)JNIEnv_functions.GetStringUTFRegion((JNIEnv*)r0, (jstring)r1, (jsize)r2, (jsize)r3, (char*)param5);
        LOG_I("        JNIEnv_functions.GetStringUTFRegion returned void");
        return 1;
    } else if (addr == JNIEnv_functions.IsInstanceOf) { // 32
        LOG_I("calling JNIEnv_functions.IsInstanceOf(%p, %p, %p)", vv(r0), vv(r1), vv(r2));
        r0 = (uint32_t)JNIEnv_functions.IsInstanceOf((JNIEnv*)r0, (jobject)r1, (jclass)r2);
        LOG_I("        JNIEnv_functions.IsInstanceOf returned %d", r0);
        return 1;
    } else if (addr == JNIEnv_functions.ExceptionOccurred) {    // 15
        LOG_I("calling JNIEnv_functions.ExceptionOccurred(%p)", vv(r0));
        r0 = (uint32_t)JNIEnv_functions.ExceptionOccurred((JNIEnv*)r0);
        LOG_I("        JNIEnv_functions.ExceptionOccurred returned %p", vv(r0));
        return 1;
    } else if (addr == JNIEnv_functions.NewString) {    // 163
        LOG_I("calling JNIEnv_functions.NewString(%p, %p \"%s\", %d)", vv(r0), vv(r1), (const char*)r1, r2);
        r0 = (uint32_t)JNIEnv_functions.NewString((JNIEnv*)r0, (const jchar*)r1, (jsize)r2);
        return 1;
    }


    // ===================

    if (addr == JavaVM_functions.GetEnv) {  //  6
        LOG_I("calling JavaVM_functions.GetEnv(%p, %p, 0x%x)", vv(r0), vv(r1), r2);
        r0_signed = JavaVM_functions.GetEnv((JavaVM*)r0, (void**)r1, (jint)r2);
        LOG_I("        JavaVM_functions.GetEnv returned %d", r0);
        return 1;
    } else if (addr == JavaVM_functions.AttachCurrentThread) {   //  4
        LOG_I("calling JavaVM_functions.AttachCurrentThread(%p, %p, %p)", vv(r0), vv(r1), vv(r2));
        if (0 != r2) {
            JavaVMAttachArgs *attach_args = (void *)(r2);
            attach_args->name = (char *)(uint32_t)(attach_args->name);
        }
        r0_signed = JavaVM_functions.AttachCurrentThread((JavaVM*)r0, (JNIEnv**)r1, (void*)r2);
        LOG_I("        JavaVM_functions.AttachCurrentThread returned %d", r0);
        return 1;
    } else if (addr == JavaVM_functions.DetachCurrentThread) {  // 5
        LOG_I("calling JavaVM_functions.DetachCurrentThread(%p)", vv(r0));
        r0_signed = JavaVM_functions.DetachCurrentThread((JavaVM*)r0);
        LOG_I("        JavaVM_functions.DetachCurrentThread returned %d", r0);
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
