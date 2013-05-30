#include <stdint.h>
#include <stdio.h>

union {
    struct {    uint8_t x, y;  };
    struct {    int8_t xs, ys; };
} q;

struct {
    int C;
    int V;
    int N;
    int Z;
} APSR, reference_APSR;

void do_cmp()
{
    const int32_t signed_sum = (int32_t)q.xs + (int32_t)(int8_t)(0xff ^ q.ys) + 1;
    const uint32_t unsigned_sum = (uint32_t)q.x + (0xff ^ q.y) + 1;

    const int8_t signed_sum8 = (int8_t)(0xff & unsigned_sum);
    const uint8_t unsigned_sum8 = (uint8_t)unsigned_sum;

    reference_APSR.V = signed_sum != signed_sum8;
    reference_APSR.C = unsigned_sum != unsigned_sum8;
    reference_APSR.Z = (0 == unsigned_sum8);
    reference_APSR.N = (signed_sum8 < 0);

    uint16_t tmp = q.x + ~q.y + 1;
    APSR.C = (tmp < q.x) || !q.y;
    APSR.V = !((uint32_t)(q.x ^ (~q.y + 1)) & 0x80) && ((tmp ^ q.x) & 0x80);
    if (q.y == 0x80) APSR.V = !(q.x & 0x80);

    APSR.N = !!(tmp & 0x80);
    APSR.Z = (0 == tmp);
}

void do_sbb(uint32_t old_C)
{
    const int32_t signed_sum = (int32_t)q.xs + (int32_t)(int8_t)(0xff ^ q.ys) + old_C;
    const uint32_t unsigned_sum = (uint32_t)q.x + (0xff ^ q.y) + old_C;

    const int8_t signed_sum8 = (int32_t)(int8_t)(0xff & unsigned_sum);
    const uint8_t unsigned_sum8 = (uint8_t)unsigned_sum;

    reference_APSR.V = signed_sum != signed_sum8;
    reference_APSR.C = unsigned_sum != unsigned_sum8;
    reference_APSR.Z = (0 == unsigned_sum8);
    reference_APSR.N = (signed_sum8 < 0);

    uint8_t tmp = q.x + ~q.y + old_C;
    APSR.C = (tmp < q.x);
    APSR.V = !((q.x ^ (~q.y + old_C)) & 0x80) && ((tmp ^ q.x) & 0x80);
    if (old_C) {
        if (q.y == 0x80) APSR.V = !(q.x & 0x80);
        APSR.C = APSR.C || !q.y;
    }

    APSR.N = !!(tmp & 0x80);
    APSR.Z = (0 == tmp);
}

void do_adc(uint32_t old_C)
{
    const int32_t signed_sum = (int32_t)q.xs + (int32_t)q.ys + old_C;
    const uint32_t unsigned_sum = (uint32_t)q.x + q.y + old_C;

    const int8_t signed_sum8 = (int32_t)(int8_t)(0xff & unsigned_sum);
    const uint8_t unsigned_sum8 = (uint8_t)unsigned_sum;

    reference_APSR.V = signed_sum != signed_sum8;
    reference_APSR.C = unsigned_sum != unsigned_sum8;
    reference_APSR.Z = (0 == unsigned_sum8);
    reference_APSR.N = (signed_sum8 < 0);

    uint8_t tmp = q.x + q.y + old_C;
    APSR.C = (tmp < q.x);
    APSR.V = !((q.x ^ (q.y + old_C)) & 0x80) && ((tmp ^ q.x) & 0x80);
    if (old_C) {
        if (q.y == 0x7f) APSR.V = !(q.x & 0x80);
        APSR.C = APSR.C || (q.y == 0xff);
    }

    APSR.N = !!(tmp & 0x80);
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
    for (int k1 = 0; k1 < 256; k1 ++) {
        for (int k2 = 0; k2 < 256; k2 ++) {
            q.x = k1;
            q.y = k2;

            do_sbb(1);      check_flags();
            do_sbb(0);      check_flags();
            do_adc(0);      check_flags();
            do_adc(1);      check_flags();

            do_cmp();       check_flags();  check_collations();
        }
    }

    printf("done\n");
}
