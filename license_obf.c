// license_original.c

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

int secret_check(const char *user, const char *key);

int main(void) {
    char user[32];
    char key[64];

    printf("User name: ");
    if (!fgets(user, sizeof(user), stdin)) return 1;
    user[strcspn(user, "\n")] = '\0';

    printf("License key: ");
    if (!fgets(key, sizeof(key), stdin)) return 1;
    key[strcspn(key, "\n")] = '\0';

    if (secret_check(user, key)) {
        printf("Access granted!\n");
    } else {
        printf("Access denied.\n");
    }
    return 0;
}

/* ===== SECRET REGION (난독화 대상) =====
 * 기능:
 *   user == "admin"
 *   AND
 *   key  == "SECURE-OBFUSCATION-2025"
 * 일 때만 1 반환, 나머지는 0 반환
 */

 // VM-based 난독화
 static int vm_execute(const uint8_t *code, uintptr_t *memory) {
    int16_t reg[16] = {0};
    uint8_t pc = 0;
    uint8_t zflag = 0;

    while (1) {
        switch (code[pc]) {
            case 0x01: { // LOAD
                uint8_t reg_index = code[++pc];
                uint8_t mem_address = code[++pc] - 0xA0;
                reg[reg_index] = memory[mem_address];
            }
            break;

            case 0x02: { // STORE
                uint8_t mem_address = code[++pc] - 0xA0;
                uint8_t reg_index = code[++pc];
                memory[mem_address] = reg[reg_index];
            }
            break;

            case 0x03: { //ADD
                uint8_t reg_index1 = code[++pc];
                uint8_t reg_index2 = code[++pc];
                reg[reg_index1] += reg[reg_index2];
            }
            break;

            case 0x04: { // SUB
                uint8_t reg_index1 = code[++pc];
                uint8_t reg_index2 = code[++pc];
                reg[reg_index1] -= reg[reg_index2];
            }
            break;

            case 0x05: { // CMPSTR
                uint8_t reg_index1 = code[++pc];
                uint8_t reg_index2 = code[++pc];
                const char *str1 = (const char *)reg[reg_index1];
                const char *str2 = (const char *)reg[reg_index2];
                if (strcmp(str1, str2) == 0) {
                    zflag = 1;
                } else {
                    zflag = 0;
                }
            }
            break;

            case 0x06: { // target
                uint8_t target_address = code[++pc];
                if (zflag) {
                    pc = target_address;
                }
            }
            break;

            case 0xFF: { // RETURN
                uint8_t reg_index = code[++pc];
                return (int)reg[reg_index];
            }

            default: {
                return 0;
            }
        }
        pc++;
    }
    return 0;
 }

// xor 복호화 함수
// 문자열의 각 바이트를 xor 키(k)로 다시 xor 연산하여 복호화
static void xor_decode(char *s, size_t n, uint8_t k) {
    for (size_t i = 0; i < n; i++) {
        s[i] ^= k;
    }
}

// secret_check
int secret_check(const char *user, const char *key) {
    srand(time(0));
    int a = (rand() % 50) + 10;
    int b = (rand() % 50) + 10;
    int x = 77;

    const uint8_t K = 0x5A; // xor 키

    // data aggregation + xor 인코딩
    static const uint8_t aggr[31] = {
        ('a' ^ 0x5A), ('S' ^ 0x5A), ('E' ^ 0x5A), ('C' ^ 0x5A),
        ('d' ^ 0x5A), ('U' ^ 0x5A), ('R' ^ 0x5A), ('E' ^ 0x5A),
        ('m' ^ 0x5A), ('-' ^ 0x5A), ('O' ^ 0x5A), ('B' ^ 0x5A),
        ('i' ^ 0x5A), ('F' ^ 0x5A), ('U' ^ 0x5A), ('S' ^ 0x5A),
        ('n' ^ 0x5A), ('C' ^ 0x5A), ('A' ^ 0x5A), ('T' ^ 0x5A),
        (0 ^ 0x5A), ('I' ^ 0x5A), ('O' ^ 0x5A), ('N' ^ 0x5A),
        (0 ^ 0x5A), ('-' ^ 0x5A), ('2' ^ 0x5A), ('0' ^ 0x5A),
        (0 ^ 0x5A), ('2' ^ 0x5A), ('5' ^ 0x5A)
    };

    char e_u[9];
    char e_k[24];

    // Control-flow Flattening
    sw: switch (x) {
        case 0: // 검증 실패
            return 0;

        case 999: // Dead Code
            printf("[ERROR] Dead code executed.\n");
            x = 1;
            goto sw;


        case 100: // 검증 성공
            return 1;

        case 77: // 루프로 재조립
            int ui = 0, ki = 0;
            for (int i = 0; i < 31; i++) {
                int take_u = ((i % 4) == 0 && ui < 8);
                if (take_u) {
                    e_u[ui++] = (char)aggr[i];
                } else {
                    e_k[ki++]  = (char)aggr[i];
                }
            }
            x = 256;
            goto sw;

        case 1: // Dead Code 종료
            return -1;

        case 256: // xor로 복호화
            xor_decode(e_u , 5, K);
            xor_decode(e_k, 23, K);
            e_u[5] = '\0';
            e_k[23] = '\0';

            // Opaque Predicate
            if ((a * b) > 0) {
                // VM-based 난독화
                uintptr_t memory[256] = {0};
                memory[0xA0 - 0xA0] = (uintptr_t)user;
                memory[0xA1 - 0xA0] = (uintptr_t)key;
                memory[0xA2 - 0xA0] = (uintptr_t)e_u;
                memory[0xA3 - 0xA0] = (uintptr_t)e_k;

                uint8_t bytecode[] = {
                    0x01, 0x06, 0xB3, // default fail

                    // user_check:
                    0x01, 0x00, 0xA0, // LOAD user -> r0
                    0x01, 0x01, 0xA2, // LOAD e_u -> r1
                    0x05, 0x00, 0x01, // CMP r0, r1
                    0x06, 0x12,       // JZ 0x12 (user_ok로 점프)
                    0xFF, 0x06, // RETURN r6 (0)

                    // user_ok:
                    0x01, 0x03, 0xA1, // LOAD key -> r3
                    0x01, 0x04, 0xA3, // LOAD e_k -> r4
                    0x05, 0x03, 0x04, // CMP r3, r4
                    0x06, 0x1E,       // JZ 0x1E (key_ok로 점프)
                    0xFF, 0x06, // RETURN r6 (0)

                    // key_ok:
                    0x01, 0x06, 0xA4, // LOAD 1 -> r6
                    0xFF, 0x06  // RETURN r6 (1)
                };

                int ok = vm_execute(bytecode, memory);
                if (ok) {
                    x = 100;
                }
                else {
                    x = 0;
                }
            }
            else {
                x = 999; // Dead code
            }
            goto sw;

        default:
            return 0;

    }
}

/* ===== SECRET REGION END ===== */
