typedef struct {
    union reg_t {
        struct {
            uint32_t r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14;
        };
        struct {
            int32_t r0_signed, r1_signed, r2_signed, r3_signed, r4_signed, r5_signed, r6_signed, \
                    r7_signed, r8_signed, r9_signed, r10_signed, r11_signed, r12_signed, \
                    r13_signed, r14_signed;
        };
        struct {
            double x_double, y_double;
        };
        struct {
            uint64_t x_uint64_t, y_uint64_t;
        };
        struct {
            int64_t x_int64_t, y_int64_t;
        };
        struct {
            float x_float, y_float, z_float, w_float;
        };
    } reg;

    struct APSR_t {
        int C;
        int V;
        int N;
        int Z;
    } APSR;

    unsigned char *d_stack;
} state_t;


#define r0      state->reg.r0
#define r1      state->reg.r1
#define r2      state->reg.r2
#define r3      state->reg.r3
#define r4      state->reg.r4
#define r5      state->reg.r5
#define r6      state->reg.r6
#define r7      state->reg.r7
#define r8      state->reg.r8
#define r9      state->reg.r9
#define r10     state->reg.r10
#define r11     state->reg.r11
#define r12     state->reg.r12
#define r13     state->reg.r13
#define r14     state->reg.r14
#define APSR    state->APSR
#define r0_signed      state->reg.r0_signed
#define r1_signed      state->reg.r1_signed
#define r2_signed      state->reg.r2_signed
#define r3_signed      state->reg.r3_signed
#define r4_signed      state->reg.r4_signed
#define r5_signed      state->reg.r5_signed
#define r6_signed      state->reg.r6_signed
#define r7_signed      state->reg.r7_signed
#define r8_signed      state->reg.r8_signed
#define r9_signed      state->reg.r9_signed
#define r10_signed     state->reg.r10_signed
#define r11_signed     state->reg.r11_signed
#define r12_signed     state->reg.r12_signed
#define r13_signed     state->reg.r13_signed
#define r14_signed     state->reg.r14_signed


#define x_double    state->reg.x_double
#define y_double    state->reg.y_double
#define x_int64_t   state->reg.x_int64_t
#define y_int64_t   state->reg.y_int64_t
#define x_uint64_t  state->reg.x_uint64_t
#define y_uint64_t  state->reg.y_uint64_t
#define x_float     state->reg.x_float
#define y_float     state->reg.y_float
#define x_double    state->reg.x_double
#define y_double    state->reg.y_double


pthread_key_t register_key;

static
state_t *
initialize_state(void)
{
    LOG_I("initialize_registers");
    state_t *state = pthread_getspecific(register_key);
    if (NULL == state) {
        state = calloc(1, sizeof(state_t));

        const uint32_t stack_length = 1024*1024;// 2000 * 4;
        state->d_stack = calloc(1, stack_length);
        r13 = (uint32_t)(state->d_stack + stack_length - 4 * 32);

        pthread_setspecific(register_key, state);
    }

    return state;
}

__attribute__((constructor))
static
void
constructor_registers(void)
{
    LOG_I("constructor_registers");
    pthread_key_create(&register_key, NULL);
}
