#include <stdint.h>
#include <stdio.h>

union {
    struct {    uint32_t x, y;  };
    struct {    int32_t xs, ys; };
} q;

struct {
    int C;
    int V;
    int N;
    int Z;
} APSR, reference_APSR;

void do_cmp()
{
    const int64_t signed_sum64 = (int64_t)(int32_t)q.xs + (int64_t)(int32_t)(~q.ys) + 1;
    const uint64_t unsigned_sum64 = (uint64_t)q.x + (uint64_t)(~q.y) + 1;

    const int32_t signed_sum32 = (int32_t)(uint32_t)unsigned_sum64;
    const uint32_t unsigned_sum32 = (uint32_t)unsigned_sum64;

    reference_APSR.V = signed_sum64 != signed_sum32;
    reference_APSR.C = unsigned_sum64 != unsigned_sum32;
    reference_APSR.Z = (0 == unsigned_sum32);
    reference_APSR.N = (signed_sum32 < 0);

    uint32_t tmp = q.x + ~q.y + 1;
    APSR.C = (tmp < q.x) || !q.y;
    APSR.V = !((q.x ^ (~q.y + 1)) & 0x80000000) && ((tmp ^ q.x) & 0x80000000);
    if (q.y == 0x80000000) APSR.V = !(q.x & 0x80000000);

    APSR.N = !!(tmp & 0x80000000);
    APSR.Z = (0 == tmp);
}

void do_sbb(uint32_t old_C)
{
    const int64_t signed_sum64 = (int64_t)(int32_t)q.xs + (int64_t)(int32_t)(~q.ys) + old_C;
    const uint64_t unsigned_sum64 = (uint64_t)q.x + (uint64_t)(~q.y) + old_C;

    const int32_t signed_sum32 = (int32_t)(uint32_t)unsigned_sum64;
    const uint32_t unsigned_sum32 = (uint32_t)unsigned_sum64;

    reference_APSR.V = signed_sum64 != signed_sum32;
    reference_APSR.C = unsigned_sum64 != unsigned_sum32;
    reference_APSR.Z = (0 == unsigned_sum32);
    reference_APSR.N = (signed_sum32 < 0);

    uint32_t tmp = q.x + ~q.y + old_C;
    APSR.C = (tmp < q.x);
    APSR.V = !((q.x ^ (~q.y + old_C)) & 0x80000000) && ((tmp ^ q.x) & 0x80000000);
    if (old_C) {
        if (q.y == 0x80000000) APSR.V = !(q.x & 0x80000000);
        APSR.C = APSR.C || !q.y;
    }

    APSR.N = !!(tmp & 0x80000000);
    APSR.Z = (0 == tmp);
}

void do_adc(uint32_t old_C)
{
    const int64_t signed_sum64 = (int64_t)(int32_t)q.xs + (int64_t)(int32_t)q.ys + old_C;
    const uint64_t unsigned_sum64 = (uint64_t)q.x + (uint64_t)q.y + old_C;

    const int32_t signed_sum32 = (int32_t)(uint32_t)unsigned_sum64;
    const uint32_t unsigned_sum32 = (uint32_t)unsigned_sum64;

    reference_APSR.V = signed_sum64 != signed_sum32;
    reference_APSR.C = unsigned_sum64 != unsigned_sum32;
    reference_APSR.Z = (0 == unsigned_sum32);
    reference_APSR.N = (signed_sum32 < 0);

    uint32_t tmp = q.x + q.y + old_C;
    APSR.C = (tmp < q.x);
    APSR.V = !((q.x ^ (q.y + old_C)) & 0x80000000) && ((tmp ^ q.x) & 0x80000000);
    if (old_C) {
        if (q.y == 0x7fffffff) APSR.V = !(q.x & 0x80000000);
        APSR.C = APSR.C || (q.y == 0xffffffff);
    }

    APSR.N = !!(tmp & 0x80000000);
    APSR.Z = (0 == tmp);
}


void check_flags()
{
    if (reference_APSR.Z != APSR.Z) {
        printf("Z mismatch @ %u, %u\n", q.x, q.y);
    }
    if (reference_APSR.N != APSR.N) {
        printf("N mismatch @ %u, %u\n", q.x, q.y);
    }
    if (reference_APSR.C != APSR.C) {
        printf("C mismatch @ %u, %u. Now %d, should be %d\n", q.x, q.y,
            APSR.C, reference_APSR.C);
    }
    if (reference_APSR.V != APSR.V) {
        printf("V mismatch @ %u, %u. Now %d, should be %d\n", q.x, q.y,
            APSR.V, reference_APSR.V);
    }
}

void check_collations()
{
    // eq
    if (APSR.Z && !(q.xs == q.ys))      printf("fail eq @ %d, %d\n", q.xs, q.ys);

    // ne
    if (!APSR.Z && !(q.xs != q.ys))     printf("fail ne @ %d, %d\n", q.xs, q.ys);

    // hs
    if (APSR.C && !(q.x >= q.y))        printf("fail hs @ %u, %u\n", q.x, q.y);

    // lo
    if (!APSR.C && !(q.x < q.y))        printf("fail lo @ %u, %u\n", q.x, q.y);

    // mi?   // pl?    // vs?    // vc?

    // hi
    if ((APSR.C && !APSR.Z) && !(q.x > q.y))    printf("fail hi @ %u, %u\n", q.x, q.y);

    // ls
    if ((!APSR.C || APSR.Z) && !(q.x <= q.y))   printf("fail ls @ %u, %u\n", q.x, q.y);

    // ge
    if ((APSR.N == APSR.V) && !(q.xs >= q.ys))  printf("fail ge @ %d, %d\n", q.xs, q.ys);

    // lt
    if ((APSR.N != APSR.V) && !(q.xs < q.ys))   printf("fail lt @ %d, %d\n", q.xs, q.ys);

    // gt
    if ((!APSR.Z && (APSR.N == APSR.V)) && !(q.xs > q.ys)) printf("fail gt @ %d, %d\n", q.xs, q.ys);

    // le
    if ((APSR.Z || (APSR.N != APSR.V)) && !(q.xs <= q.ys))  printf("fail le @ %d, %d\n", q.xs, q.ys);
}

int main(void)
{
    uint32_t dt[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf,
        0x7ffffff0, 0x7ffffff1, 0x7ffffff2, 0x7ffffff3, 0x7ffffff4, 0x7ffffff5, 0x7ffffff6, 0x7ffffff7,
        0x7ffffff8, 0x7ffffff9, 0x7ffffffa, 0x7ffffffb, 0x7ffffffc, 0x7ffffffd, 0x7ffffffe, 0x7fffffff,
        0x80000000, 0x80000001, 0x80000002, 0x80000003, 0x80000004, 0x80000005, 0x80000006, 0x80000007,
        0x80000008, 0x80000009, 0x8000000a, 0x8000000b, 0x8000000c, 0x8000000d, 0x8000000e, 0x8000000f,
        0xfffffff0, 0xfffffff1, 0xfffffff2, 0xfffffff3, 0xfffffff4, 0xfffffff5, 0xfffffff6, 0xfffffff7,
        0xfffffff8, 0xfffffff9, 0xfffffffa, 0xfffffffb, 0xfffffffc, 0xfffffffd,
        0xfffffffe, 0xffffffff
    };
    for (int k1 = 0; k1 < sizeof(dt)/sizeof(dt[0]); k1 ++) {
        for (int k2 = 0; k2 < sizeof(dt)/sizeof(dt[0]); k2 ++) {
            q.x = dt[k1];
            q.y = dt[k2];

            do_sbb(1);      check_flags();
            do_sbb(0);      check_flags();
            do_adc(0);      check_flags();
            do_adc(1);      check_flags();

            do_cmp();       check_flags();  check_collations();
        }
    }

    printf("done\n");
}
