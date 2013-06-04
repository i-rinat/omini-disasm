#include "target-specific.h"
#include <rhash/rhash.h>
#include <string.h>
#include "pc-stack.h"
#include "output.h"
#include "section.h"
#include <assert.h>

char so_md5_hash[33];

void
compute_hash_of_so(const char *so_name)
{
    unsigned char md5_hash[16];
    so_md5_hash[0] = 0;
    rhash_library_init();
    rhash_file(RHASH_MD5, so_name, md5_hash);
    for (int k = 0; k < 16; k ++)
        sprintf(so_md5_hash + 2*k, "%02x", md5_hash[k]);
}

static
const char *
signature_return_type(const char *signature)
{
    const char *ptr = strstr(signature, ")");
    assert(ptr);
    switch (*(ptr + 1)) {
    case 'Z': // return "jboolean"; break;
    case 'B': // return "jbyte"; break;
    case 'C': // return "jchar"; break;
    case 'S': // return "jshort"; break;
    case 'F': // return "jfloat"; break;
    case 'L': // return "jobject"; break;
    case '[': // return "jobject"; break;
    case 'I': return "uint32_t"; break;
    case 'D': // return "jdouble"; break;
    case 'J': return "uint64_t"; break;
    case 'V': return "void"; break;
    default:
        assert(0 && "unknown type");
    }
}

static
int
signature_get_param_count(const char *signature)
{
    const char *ptr = strstr(signature, "(");
    assert(ptr);
    ptr ++;
    uint32_t param_count = 0;
    while (1) {
        switch (*ptr) {
        case 'Z':
        case 'B':
        case 'C':
        case 'S':
        case 'F':
        case 'I':
        case 'D':
        case 'J':
        case 'V':
            param_count ++; ptr ++; break;
        case ')':
            return param_count; break;
        case 'L':
            param_count ++;
            while (*ptr != ';')
                ptr ++;
            ptr ++;
            break;
        case '[':
            param_count ++;
            ptr += 2;
            break;
        default:
            assert(0);
        }
    }
    assert(0);
    return 0;
}

static
const char *
signature_get_param(const char *signature, int n)
{
    const char *ptr = strstr(signature, "(");
    assert(ptr);
    ptr ++;
    uint32_t param_count = 0;
    while (1) {
        switch (*ptr) {
        case 'D':
        case 'J':
            if (0 == n)
                return "uint64_t";
        case 'Z':
        case 'B':
        case 'C':
        case 'S':
        case 'F':
        case 'I':
        case 'V':
            if (0 == n)
                return "uint32_t";
            param_count ++; ptr ++; break;
        case ')':
            assert(0); break;
        case 'L':
            if (0 == n)
                return "uint32_t";
            param_count ++;
            while (*ptr != ';')
                ptr ++;
            ptr ++;
            break;
        case '[':
            if (0 == n)
                return "uint32_t";
            param_count ++;
            ptr += 2;
            break;
        default:
            assert(0);
        }
        n = n - 1;
    }
    assert(0);
    return 0;
}

static
void
generate_pthread_proxy(uint32_t addr)
{
    emit_code("// start_routine for pthread_create");
    emit_code("static void *proxy_%04x(void *param) {", addr);
    emit_code("    r13 = d_stack_start;");
    emit_code("    r0 = (uint32_t)param;");
    emit_code("    func_%04x();", addr);
    emit_code("    return (void *)r0;");
    emit_code("}");
}

static
void
apply_quirks_for_c3630424f7c9514b203301154218db40(void)
{
    func_list_mark_as_non_returning(0x7840);    // directly calls longjmp
    func_list_mark_as_non_returning(0x4da4);    // longjmp


    uint32_t additional_functions[] = {
        0x43448, 0x401f4, 0x1fc1c, 0x29d6c, 0x31fe8,
        0x27b6c, 0x21e9c, 0x33b54, 0x278a0, 0x27b0c,
        0x29d54, 0x5888, 0x5814, 0xc5fc,
        0x38450 // pthread
    };

    generate_pthread_proxy(0x38450);

    uint32_t k;
    for (k = 0; k < sizeof(additional_functions)/sizeof(additional_functions[0]); k ++) {
        func_list_add(additional_functions[k]);
    }

    func_list_add(0xffff0fc0);
    func_list_mark_done(0xffff0fc0);

    uint32_t addr_table = 0x5CBC4;
    char *string_table = get_charptr_at(0x57888);

    char *ptr = string_table;
    uint32_t addr_table_offset = 0;
    while (1) {
        if (!*ptr)
            break;
        // class_name = ptr;
        // printf("class: %s\n", class_name);
        ptr += strlen(ptr) + 1;
        while (1) {
            if (!*ptr)
                break;
            // method_name = ptr;

            ptr += strlen(ptr) + 1;
            if (!*ptr)
                break;
            const char *signature = ptr;

            const uint32_t func_addr = get_word_at(addr_table + addr_table_offset);
            // printf("    %s->%s, signature: %s\n", class_name, method_name, signature);
            char buf[2048];
            char *buf_ptr = &buf[0];

            // printf("%s ", signature);
            buf_ptr += sprintf(buf_ptr, "static %s proxy_%04x(uint32_t env, uint32_t obj", signature_return_type(signature),
                func_addr);
            for (int k = 0; k < signature_get_param_count(signature); k ++) {
                buf_ptr += sprintf(buf_ptr, ", %s param%d", signature_get_param(signature, k), k);
            }
            buf_ptr += sprintf(buf_ptr, ") {");

            emit_code("%s", buf);
            emit_code("    __android_log_print(ANDROID_LOG_INFO, \"libfranken\", \"called proxy_%04x\");", func_addr);
            emit_code("    r13 = d_stack_start;");
            emit_code("    uint32_t saved_fp = r13;");
            emit_code("    r0 = env;");
            emit_code("    r1 = obj;");

            buf_ptr = &buf[0];
            uint32_t param_offset = 2;
            for (int k = 0; k < signature_get_param_count(signature); k ++) {
                if (!strcmp(signature_get_param(signature, k), "uint64_t")) {
                    if (param_offset & 1)
                        param_offset ++;
                    if (param_offset < 4) {
                        emit_code("    r%u = param%u;", param_offset, k);
                        emit_code("    r%u = param%u >> 32;", param_offset + 1, k);
                    } else {
                        emit_code("    store(r13 + %u, param%u);", 4*(param_offset - 4), k);
                        emit_code("    store(r13 + %u, param%u >> 32);", 4*(param_offset - 4) + 4, k);
                    }

                    param_offset += 2;
                } else {
                    if (param_offset < 4)
                        emit_code("    r%u = param%u;", param_offset, k);
                    else
                        emit_code("    store(r13 + %u, param%u);", 4*(param_offset - 4), k);

                    param_offset += 1;
                }
            }
            emit_code("    func_%04x();", func_addr);
            emit_code("    if (r13 != saved_fp) {");
            emit_code("      __android_log_print(ANDROID_LOG_ERROR, \"libfranken\", "
                                        "\"failed saved_fp == fp for proxy_%04x\");", func_addr);
            emit_code("    }");
            emit_code("    __android_log_print(ANDROID_LOG_INFO, \"libfranken\", \"       proxy_%04x quits\");", func_addr);

            if (!strcmp(signature_return_type(signature), "void")) {
                emit_code("    return;");
            } else if (!strcmp(signature_return_type(signature), "uint32_t")) {
                emit_code("    return r0;");
            } else if (!strcmp(signature_return_type(signature), "uint64_t")) {
                emit_code("    return reg.x_uint64_t;");
            } else {
                assert(0);
            }

            emit_code("}");

            addr_table_offset += 4;
            ptr += strlen(ptr) + 1;
        }
        ptr ++;
    }

    // produce JNI_OnLoad
    emit_code("jint JNI_OnLoad(JavaVM* aVm, void* aReserved) {");
    // TODO:  gJavaVM = aVm;
    emit_code("    JNIEnv* env;");
    emit_code("    JavaVM_functions = **aVm;");
    emit_code("    if ((*aVm)->GetEnv(aVm, (void **)&env, JNI_VERSION_1_4) != JNI_OK) {");
    emit_code("      __android_log_print(ANDROID_LOG_ERROR, \"libfranken\", \"Failed to get the environment\");");
    emit_code("      return -1;");
    emit_code("    }");
    emit_code("    JNIEnv_functions = **env;");

    emit_code("    jclass aClass;\n");
    emit_code("    JNINativeMethod methodTable;");

    addr_table_offset = 0;
    ptr = string_table;
    while (1) {
        if (!*ptr)
            break;
        const char *class_name = ptr;
        emit_code("    aClass = (*env)->FindClass(env, \"%s\");", class_name);
        ptr += strlen(ptr) + 1;
        while (1) {
            if (!*ptr)
                break;
            const char *method_name = ptr;

            ptr += strlen(ptr) + 1;
            if (!*ptr)
                break;
            const char *signature = ptr;

            const uint32_t func_addr = get_word_at(addr_table + addr_table_offset);

            emit_code("\n");
            emit_code("    methodTable.name = \"%s\";", method_name);
            emit_code("    methodTable.signature = \"%s\";", signature);
            emit_code("    methodTable.fnPtr = (void *)&proxy_%04x;", func_addr);
            emit_code("    (*env)->RegisterNatives(env, aClass, &methodTable, 1);");

            func_list_add(func_addr);

            addr_table_offset += 4;
            ptr += strlen(ptr) + 1;
        }
        ptr ++;
    }

    emit_code("    return JNI_VERSION_1_4;");
    emit_code("}");

}

void
apply_target_specific_quirks(void)
{
    if (!strcmp(so_md5_hash, "c3630424f7c9514b203301154218db40")) {
        printf("applying quirks for Opera Mini's libom.so\n");
        apply_quirks_for_c3630424f7c9514b203301154218db40();
    } else if (!strcmp(so_md5_hash, "<place-hash-here>")) {
    } else {
        printf("unknown target, no quirks\n");
    }
}

static
int
target_specific_symbols_c3630424f7c9514b203301154218db40(const char *symname, uint32_t func_addr)
{
    (void)func_addr;
    if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_accessStringData")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_agetStatic")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_allocatedIntSize")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_allocateNativeArrayData")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_allocateObject")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_allocateWeakReference_1n")) {
        emit_code("void Java_com_opera_bream_vm_BreamVM_allocateWeakReference_1n(JNIEnv * env, jobject  obj) {");
        emit_code("__android_log_print(ANDROID_LOG_INFO, \"frankenstein\", "
            "\"missing method Java_com_opera_bream_vm_BreamVM_allocateWeakReference_1n\");");
        emit_code("}");
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_apeek")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_apop")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_apush")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_aputField")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_aputStatic")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_checkCallWhileSuspended")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_checkSuspend")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_claimVmForThisThread")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_connectDebugger")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_finalize_1n")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_finishInitialization")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_gc_1n")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_gcCounter")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_gcCounter2")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_gcCounter2")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_getField")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_getField")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_getMemoryUsage")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_getNativeArrayData")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_getNativeArrayData")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_getNativeArraySize")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_getNativeByteArrayDataN")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_getNativeByteArraySize")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_getNativeIntArrayData")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_getNativeIntArraySize")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_getNativeType")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_getNewReference")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_getObjectMemory")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_getStringSize")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_igetStatic")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_igetStatic")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_invokeEntry")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_invokeIntEntry")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_invokeIntEntry")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_ipeek")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_ipop")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_ipush")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_iputField")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_iputStatic")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_loadLong")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_loadObject")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_loadProgram_1n")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_loadString__I")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_loadString__III")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_markObject")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_markObjects")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_nativeArrayDataAddN")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_nativeVmCreate")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_nonNull")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_prettyPrint")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_putField")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_reset")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_setNativeArrayData")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_setNativeByteArrayData")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_setNativeIntArray")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_setNativeIntArrayData")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_startInitialization")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_updateReferences")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_wrapLong")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_wrapNativeArrayData")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_wrapNativeByteArrayDataN")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_wrapNativeIntArrayDataN")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_wrapObject")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_wrapUTF8String")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_wrapUTF8String2")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_IOUtils_decodeUTF8___3BI")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_IOUtils_decodeUTF8___3BI_3CII")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_IOUtils_getUTF8CharLength")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidApplication_nativeOnMarkActiveDocument")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidApplication_nativePaint")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidApplication_nativeSetZoomRange")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGraphics_00024SWBufferContainer_lockBuffer")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGraphics_00024SWBufferContainer_unlockBuffer")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGraphics_00024SWDirectBufferContainer_lockBuffer")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGraphics_00024SWDirectBufferContainer_unlockBuffer")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGraphics_clipRect")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGraphics_createContext")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGraphics_createStaticContext")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGraphics_destroyTilesController")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGraphics_drawCharsN")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGraphics_drawCharsScaledN")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGraphics_drawImage")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGraphics_drawImageRegionTiled")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGraphics_drawImageTiled")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGraphics_drawImageTiled")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGraphics_drawRegion")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGraphics_drawString")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGraphics_fillRect")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGraphics_fillRectAlpha")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGraphics_finalize")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGraphics_forceRender")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGraphics_getClipHeight")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGraphics_getClipWidth")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGraphics_getClipX")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGraphics_getClipY")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGraphics_glCreateContext")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGraphics_glCreateContext")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGraphics_glSurfaceChanged")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGraphics_glSurfaceCreated")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGraphics_hasDirectDraw")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGraphics_initTilesController")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGraphics_nativeBeginFrame")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGraphics_nativeEndFrame")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGraphics_nativeStaticInit")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGraphics_reset")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGraphics_reset")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGraphics_setClip")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGraphics_setRenderViewportOnly")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGraphics_syncTextures")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGuiResource_free")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidGuiResource_initGuiRes")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidImage_createImmutableImage")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidImage_createImmutableSubImage")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidImage_createMutableImage")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidImage_finalize")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidImage_getHeight")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidImage_getUsage")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidImage_getWidth")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidImage_nativeStaticInit")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidImage_updateBuffer")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidNativeImage_finalize")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidWebPDecoder_nativeDecode")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_AndroidWebPDecoder_nativeGetWidthHeight")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_Browser_destroyNative")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_Browser_nativeOnCreate")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_android_CrashHandler_installSignalHandlers")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_Application_onAddLogicalDocument")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_Application_onDeleteLogicalDocument")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_Application_onLogicalDocumentsCleared")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_Document_nativeInvalidate")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_Document_nativeOnDataRead")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_Document_nativePostInvalidate")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_Document_nativeSetCacheFoldData")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_Document_nativeSetVisibleArea")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_LogicalDocument_groupModified")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_LogicalDocument_lockCritical")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_LogicalDocument_nativeReportFormValuesStatus")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_LogicalDocument_syncDocument")) {
    } else if (!strcmp(symname, "Java_com_opera_mini_LogicalDocument_unlockCritical")) {
    } else if (!strcmp(symname, "javaFillRectAlpha")) {
    } else if (!strcmp(symname, "JNI_OnLoad")) {
    } else if (!strcmp(symname, "passed_paint_time_limit")) {
    } else {
        return 0;
    }

    return 1;
}


int
target_specific_symbols(const char *symname, uint32_t func_addr)
{
    if (!strcmp(so_md5_hash, "c3630424f7c9514b203301154218db40")) {
        return target_specific_symbols_c3630424f7c9514b203301154218db40(symname, func_addr);
    }

    return 0;
}
