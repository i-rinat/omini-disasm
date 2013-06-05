union reg_t {
	struct {
		uint32_t r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14;
	};
	struct {
		int32_t r0_signed, r1_signed;
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
};

struct APSR_t {
	int C;
	int V;
	int N;
	int Z;
};


union reg_t *reg_ptr;
#define reg     (*reg_ptr)

struct APSR_t *APSR_ptr;
#define APSR    (*APSR_ptr)

#define r0  reg_ptr->r0
#define r1  reg_ptr->r1
#define r2  reg_ptr->r2
#define r3  reg_ptr->r3
#define r4  reg_ptr->r4
#define r5  reg_ptr->r5
#define r6  reg_ptr->r6
#define r7  reg_ptr->r7
#define r8  reg_ptr->r8
#define r9  reg_ptr->r9
#define r10  reg_ptr->r10
#define r11  reg_ptr->r11
#define r12  reg_ptr->r12
#define r13  reg_ptr->r13
#define r14  reg_ptr->r14

pthread_key_t register_key_1;
pthread_key_t register_key_2;

static
void
initialize_registers(void)
{
    __android_log_print(ANDROID_LOG_INFO, "libfranken", "initialize_registers");
    reg_ptr = pthread_getspecific(register_key_1);
    if (NULL == reg_ptr) {
        reg_ptr = calloc(1, sizeof(union reg_t));
        r13 = d_stack_start;
    }

    APSR_ptr = pthread_getspecific(register_key_2);
    if (NULL == APSR_ptr) {
        APSR_ptr = calloc(1, sizeof(struct APSR_t));
    }
}

__attribute__((constructor))
static
void
constructor_registers(void)
{
    __android_log_print(ANDROID_LOG_INFO, "libfranken", "constructor_registers");
    pthread_key_create(&register_key_1, NULL);
    pthread_key_create(&register_key_2, NULL);
}
