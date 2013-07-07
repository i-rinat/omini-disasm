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
    emit_code("    state_t *state = initialize_state();");
    emit_code("    r0 = (uint32_t)param;");
    emit_code("    func_%04x(state);", addr);
    emit_code("    return (void *)r0;");
    emit_code("}");
}

static
uint32_t
get_max_param_offset(const char *signature)
{
    uint32_t param_offset = 2;
    for (int k = 0; k < signature_get_param_count(signature); k ++) {
        if (!strcmp(signature_get_param(signature, k), "uint64_t")) {
            if (param_offset & 1)
                param_offset ++;
            param_offset += 2;
        } else {
            param_offset += 1;
        }
    }

    return param_offset;
}

static
void
apply_quirks_for_c3630424f7c9514b203301154218db40(void)
{
    func_list_mark_as_non_returning(0x7840);    // directly calls longjmp
    func_list_mark_as_non_returning(0x4da4);    // longjmp


    uint32_t additional_functions[] = {
        0x05008, 0x05018, 0x053cc, 0x053d0, 0x057b4, 0x057b8, 0x057e4, 0x05814, 0x05844, 0x05888,
        0x0647c, 0x064e0, 0x06570, 0x066a0, 0x067bc, 0x068f8, 0x06a2c, 0x06ae8, 0x06c24, 0x06c94,
        0x06cf0, 0x06d54, 0x07150, 0x07180, 0x071e0, 0x0720c, 0x07298, 0x072c8, 0x072f8, 0x08224,
        0x0a83c, 0x0a868, 0x0a8b8, 0x0a90c, 0x0a970, 0x0bf48, 0x0c5fc, 0x0ea10, 0x0ece0, 0x1f220,
        0x1f224, 0x1f22c, 0x1f234, 0x1f23c, 0x1f254, 0x1f280, 0x1f298, 0x1f2e8, 0x1f33c, 0x1f538,
        0x1f53c, 0x1f540, 0x1f544, 0x1f548, 0x1fa58, 0x1fa9c, 0x1fac0, 0x1fad8, 0x1fb04, 0x1fb44,
        0x1fb98, 0x1fc14, 0x1fc1c, 0x1fc54, 0x1fca4, 0x20230, 0x202ec, 0x20704, 0x2071c, 0x20770,
        0x20780, 0x2155c, 0x217bc, 0x217ec, 0x21818, 0x21970, 0x21ca0, 0x21e9c, 0x22544, 0x2255c,
        0x22574, 0x2258c, 0x225a4, 0x225bc, 0x225d4, 0x225ec, 0x226d8, 0x22774, 0x22790, 0x227ac,
        0x227e0, 0x2280c, 0x22918, 0x22a38, 0x22b50, 0x22cb8, 0x23134, 0x231c8, 0x27878, 0x27880,
        0x27888, 0x278a0, 0x278a8, 0x278c4, 0x278cc, 0x278d4, 0x278f0, 0x278f8, 0x27964, 0x279d8,
        0x279f4, 0x27a38, 0x27a7c, 0x27ac4, 0x27b0c, 0x27b6c, 0x27bd0, 0x27c4c, 0x27cdc, 0x27d58,
        0x27dfc, 0x27e78, 0x27f8c, 0x27fd0, 0x2801c, 0x280e8, 0x28238, 0x282fc, 0x283b0, 0x2845c,
        0x284e0, 0x28554, 0x28608, 0x28630, 0x286c4, 0x2874c, 0x287a4, 0x287e8, 0x29968, 0x29970,
        0x29ab0, 0x29b68, 0x29d54, 0x29d6c, 0x2a180, 0x2a21c, 0x2c2d8, 0x2c3e8, 0x2c458, 0x2c528,
        0x2cc68, 0x31fe8, 0x3262c, 0x33750, 0x33960, 0x33b54, 0x3423c, 0x343e8, 0x34810, 0x34d30,
        0x34d48, 0x34d60, 0x34d88, 0x34da0, 0x34db8, 0x34dbc, 0x34dc0, 0x34dd8, 0x34e10, 0x34e58,
        0x34e9c, 0x34ebc, 0x34ef8, 0x34f4c, 0x34f6c, 0x34fd0, 0x34ff0, 0x35060, 0x35080, 0x350f0,
        0x35110, 0x3515c, 0x351b8, 0x35338, 0x35340, 0x35348, 0x35350, 0x353ec, 0x35488, 0x35524,
        0x35898, 0x35a5c, 0x35b4c, 0x35c1c, 0x35cc4, 0x35e58, 0x36080, 0x36268, 0x36464, 0x3668c,
        0x36890, 0x36b10, 0x36d8c, 0x37510, 0x37838, 0x37b8c, 0x37e94, 0x38650, 0x39ea0, 0x3a0c8,
        0x3a8b0, 0x3a8c0, 0x3a910, 0x3a980, 0x3adb8, 0x3aed0, 0x3af4c, 0x3af84, 0x3b008, 0x3b060,
        0x3b0d4, 0x3b0dc, 0x3b0e4, 0x3b118, 0x3b2b8, 0x3b3a8, 0x3b588, 0x3b5a8, 0x3b5c8, 0x3b6fc,
        0x3c554, 0x3c620, 0x3d0d0, 0x3d0ec, 0x3d100, 0x3d154, 0x3d314, 0x3d3ac, 0x3d8ec, 0x3e744,
        0x3e970, 0x3f4b0, 0x3f984, 0x3fa7c, 0x4011c, 0x40124, 0x40134, 0x401b8, 0x401f4, 0x40238,
        0x4025c, 0x403a0, 0x404bc, 0x40570, 0x40d9c, 0x41ec8, 0x42288, 0x4236c, 0x42560, 0x43448,
        0x437d8, 0x441ac, 0x45634, 0x4567c, 0x456c8, 0x45710, 0x45cc4, 0x45cf0, 0x45d1c, 0x45ee8,
        0x46128, 0x461d8, 0x46244, 0x46258, 0x46274, 0x4630c, 0x463fc, 0x46564, 0x47138, 0x471a4,
        0x472e4, 0x4a654, 0x4af2c, 0x4eeb8, 0x4eed8, 0x4eef8, 0x4ef14, 0x4ef24, 0x4ef2c, 0x4ef34,
        0x4ef3c, 0x4ef50, 0x4ef68, 0x4ef80, 0x4ef98, 0x4effc, 0x4f0d0,
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
            emit_code("    LOG_I(\"called proxy_%04x\");", func_addr);
            emit_code("#ifdef TRACING_PROXY_DURATION");
            emit_code("    clock_t ts, te;");
            emit_code("    ts = clock();");
            emit_code("#endif // TRACING_PROXY_DURATION");
            emit_code("    state_t *state = initialize_state();");
            emit_code("#ifdef TRACING_CHECK_PROXY_R13");
            emit_code("    uint32_t saved_fp = r13;");
            emit_code("#endif");
            emit_code("    r0 = env;");
            emit_code("    r1 = obj;");

            // If function parameters don't fit into registers, they passed in stack.
            // We need to reserve appropriate amount.
            const uint32_t max_param_offset = get_max_param_offset(signature);
            const int32_t stack_gap = 4 * (max_param_offset - 4);

            if (stack_gap > 0)
                emit_code("    r13 -= %d;", stack_gap);

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
            emit_code("    func_%04x(state);", func_addr);

            if (stack_gap > 0)
                emit_code("    r13 += %d;", stack_gap);

            emit_code("#ifdef TRACING_CHECK_PROXY_R13");
            emit_code("    if (r13 != saved_fp) {");
            emit_code("      LOG_E(\"failed saved_fp == fp for proxy_%04x\");", func_addr);
            emit_code("    }");
            emit_code("#endif");
            emit_code("#ifdef TRACING_PROXY_DURATION");
            emit_code("    te = clock();");
            emit_code("    LOG_I(\"       proxy_%04x quits (%%ld clocks)\", te - ts);", func_addr);
            emit_code("#else");
            emit_code("    LOG_I(\"       proxy_%04x quits\");", func_addr);
            emit_code("#endif // TRACING_PROXY_DURATION");

            if (!strcmp(signature_return_type(signature), "void")) {
                emit_code("    return;");
            } else if (!strcmp(signature_return_type(signature), "uint32_t")) {
                emit_code("    return r0;");
            } else if (!strcmp(signature_return_type(signature), "uint64_t")) {
                emit_code("    return x_uint64_t;");
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
    emit_code("      LOG_E(\"Failed to get the environment\");");
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
    if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_accessStringData")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_agetStatic")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_allocatedIntSize")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_allocateNativeArrayData")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_allocateObject")) {
    } else if (!strcmp(symname, "Java_com_opera_bream_vm_BreamVM_allocateWeakReference_1n")) {
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

    func_list_add(func_addr);
    return 1;
}

static
int
target_specific_symbols_1716e1d0d45f2912d761f2f46587ea7d(const char *symname, uint32_t func_addr)
{
    // libsanangeles.so
    if (!strcmp(symname, "importGLInit")) {
    } else if (!strcmp(symname, "importGLDeinit")) {
    } else if (!strcmp(symname, "appInit")) {
    } else if (!strcmp(symname, "appDeinit")) {
    } else if (!strcmp(symname, "appRender")) {
    } else if (!strcmp(symname, "gAppAlive")) {
    } else if (!strcmp(symname, "Java_com_example_SanAngeles_DemoRenderer_nativeInit")) {
        emit_code("JNIEXPORT void JNICALL Java_com_example_SanAngeles_DemoRenderer_nativeInit(JNIEnv * env) {");
        emit_code("    state_t *state = initialize_state();");
        emit_code("    r0 = (uint32_t)env;");
        emit_code("    func_%04x(state);", func_addr);
        emit_code("}");
    } else if (!strcmp(symname, "Java_com_example_SanAngeles_DemoRenderer_nativeResize")) {
        emit_code("JNIEXPORT void JNICALL Java_com_example_SanAngeles_DemoRenderer_nativeResize "
                                        "(JNIEnv*  env, jobject  thiz, jint w, jint h ) {");
        emit_code("    state_t *state = initialize_state();");
        emit_code("    r0 = (uint32_t)env;");
        emit_code("    r1 = (uint32_t)thiz;");
        emit_code("    r2 = (uint32_t)w;");
        emit_code("    r3 = (uint32_t)h;");
        emit_code("    func_%04x(state);", func_addr);
        emit_code("}");
    } else if (!strcmp(symname, "Java_com_example_SanAngeles_DemoRenderer_nativeDone")) {
        emit_code("JNIEXPORT void JNICALL Java_com_example_SanAngeles_DemoRenderer_nativeDone(JNIEnv * env) {");
        emit_code("    state_t *state = initialize_state();");
        emit_code("    r0 = (uint32_t)env;");
        emit_code("    func_%04x(state);", func_addr);
        emit_code("}");
    } else if (!strcmp(symname, "Java_com_example_SanAngeles_DemoGLSurfaceView_nativeTogglePauseResume")) {
        emit_code("JNIEXPORT void JNICALL Java_com_example_SanAngeles_DemoGLSurfaceView_nativeTogglePauseResume(JNIEnv * env) {");
        emit_code("    state_t *state = initialize_state();");
        emit_code("    r0 = (uint32_t)env;");
        emit_code("    func_%04x(state);", func_addr);
        emit_code("}");
    } else if (!strcmp(symname, "Java_com_example_SanAngeles_DemoGLSurfaceView_nativePause")) {
        emit_code("JNIEXPORT void JNICALL Java_com_example_SanAngeles_DemoGLSurfaceView_nativePause(JNIEnv * env) {");
        emit_code("    state_t *state = initialize_state();");
        emit_code("    r0 = (uint32_t)env;");
        emit_code("    func_%04x(state);", func_addr);
        emit_code("}");
    } else if (!strcmp(symname, "Java_com_example_SanAngeles_DemoGLSurfaceView_nativeResume")) {
        emit_code("JNIEXPORT void JNICALL Java_com_example_SanAngeles_DemoGLSurfaceView_nativeResume(JNIEnv * env) {");
        emit_code("    state_t *state = initialize_state();");
        emit_code("    r0 = (uint32_t)env;");
        emit_code("    func_%04x(state);", func_addr);
        emit_code("}");
    } else if (!strcmp(symname, "Java_com_example_SanAngeles_DemoRenderer_nativeRender")) {
        emit_code("JNIEXPORT void JNICALL Java_com_example_SanAngeles_DemoRenderer_nativeRender(JNIEnv * env) {");
        emit_code("    state_t *state = initialize_state();");
        emit_code("    r0 = (uint32_t)env;");
        emit_code("    func_%04x(state);", func_addr);
        emit_code("}");
    } else if (!strcmp(symname, "")) {
    } else if (!strcmp(symname, "")) {
    } else if (!strcmp(symname, "")) {
    } else if (!strcmp(symname, "")) {
    } else if (!strcmp(symname, "")) {
    } else {
        return 0;
    }

    func_list_add(func_addr);
    return 1;
}

int
target_specific_symbols(const char *symname, uint32_t func_addr)
{
    if (!strcmp(so_md5_hash, "c3630424f7c9514b203301154218db40")) {
        return target_specific_symbols_c3630424f7c9514b203301154218db40(symname, func_addr);
    } else if (!strcmp(so_md5_hash, "1716e1d0d45f2912d761f2f46587ea7d")) {
        // libsanangeles.so
        return target_specific_symbols_1716e1d0d45f2912d761f2f46587ea7d(symname, func_addr);
    }

    return 0;
}
