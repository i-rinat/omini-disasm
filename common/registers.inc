union {
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
} reg;

uint32_t tmp;
struct {
	int C;
	int V;
	int N;
	int Z;
} APSR;

#define r0	reg.r0
#define r1	reg.r1
#define r2	reg.r2
#define r3	reg.r3
#define r4	reg.r4
#define r5	reg.r5
#define r6	reg.r6
#define r7	reg.r7
#define r8	reg.r8
#define r9	reg.r9
#define r10	reg.r10
#define r11	reg.r11
#define r12	reg.r12
#define r13	reg.r13
#define r14	reg.r14
