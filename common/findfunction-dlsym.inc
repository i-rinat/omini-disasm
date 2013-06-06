// findfunction-dlsym.inc

#define DLSYM_TABLE_SIZE  200
pthread_mutex_t dlsym_mutex = PTHREAD_MUTEX_INITIALIZER;

void* dlsym_table_ptr[DLSYM_TABLE_SIZE];
uint32_t dlsym_table_addr[DLSYM_TABLE_SIZE];

uint32_t dlsym_table_element_count = 0;


static
void
proxy_AndroidBitmap_lockPixels(state_t *state)
{
    LOG_I("calling proxy_AndroidBitmap_lockPixels");
    r0_signed = AndroidBitmap_lockPixels((JNIEnv*)r0, (jobject)r1, (void**)r2);
}

static
void
proxy_AndroidBitmap_unlockPixels(state_t *state)
{
    LOG_I("calling proxy_AndroidBitmap_unlockPixels");
    r0_signed = AndroidBitmap_unlockPixels((JNIEnv*)r0, (jobject)r1);
}

static
void
proxy_AndroidBitmap_getInfo(state_t *state)
{
    LOG_I("calling proxy_AndroidBitmap_getInfo");
    r0_signed = AndroidBitmap_getInfo((JNIEnv*)r0, (jobject)r1, (AndroidBitmapInfo*)r2);
}

static
void
add_dlsym_table_entry(uint32_t addr, const char *name)
{
    pthread_mutex_lock(&dlsym_mutex);
    if (!strcmp(name, "AndroidBitmap_lockPixels")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_AndroidBitmap_lockPixels;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "AndroidBitmap_getInfo")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_AndroidBitmap_getInfo;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "AndroidBitmap_unlockPixels")) {
        dlsym_table_ptr[dlsym_table_element_count] = &proxy_AndroidBitmap_unlockPixels;
        dlsym_table_addr[dlsym_table_element_count] = addr;
        dlsym_table_element_count ++;
    } else if (!strcmp(name, "")) {
    } else if (!strcmp(name, "")) {
    } else if (!strcmp(name, "")) {
    } else if (!strcmp(name, "")) {
    } else if (!strcmp(name, "")) {
    } else if (!strcmp(name, "")) {
    } else if (!strcmp(name, "")) {
    } else if (!strcmp(name, "")) {
    } else {
        LOG_E("add_dlsym_table_entry: failed to add %s", name);
    }

    pthread_mutex_unlock(&dlsym_mutex);
}

static
int
find_and_call_dlsym(state_t *state, uint32_t addr)
{
    int k;
    for (k = 0; k < dlsym_table_element_count; k ++) {
        if (addr == dlsym_table_addr[k]) {
            LOG_I("find_and_call_function: func_%04x", addr);
            ((void (*)(state_t *))(dlsym_table_ptr[k]))(state);
            return 1;
        }
    }

    return 0;
}
