// license_original.c (과제 배포용)

#include <stdio.h>
#include <string.h>

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

int secret_check(const char *user, const char *key) {
    const char *expected_user = "admin";
    const char *expected_key  = "SECURE-OBFUSCATION-2025";

    if (strcmp(user, expected_user) != 0)
        return 0;
    if (strcmp(key, expected_key) != 0)
        return 0;
    return 1;
}

/* ===== SECRET REGION END ===== */

