#include "storage/master_key.h"
#include "core/config.h"
#include "esp_crc.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static uint32_t s_master_hash_crc32 = 0;

static uint32_t parse_hex32(const char *hex) {
    // aceita "0x..." ou só "ABCDEF12"
    if (!hex) return 0;
    while (isspace((unsigned char)*hex)) hex++;
    if (hex[0]=='0' && (hex[1]=='x' || hex[1]=='X')) hex += 2;
    return (uint32_t)strtoul(hex, NULL, 16);
}

void master_key_init_from_menuconfig(void) {
    s_master_hash_crc32 = parse_hex32(MASTER_KEY_HASH);
}

bool master_key_is_uid(const uid_t *uid) {
    if (!uid || uid->length == 0) return false;
    uint32_t h = esp_crc32_le(0, uid->value, uid->length);
    return (h == s_master_hash_crc32);
}
