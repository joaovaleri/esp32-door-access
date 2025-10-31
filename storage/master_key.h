#pragma once
#include <stdbool.h>
#include "core/app_ctx.h"  // uid_t

#ifdef __cplusplus
extern "C" {
#endif

void master_key_init_from_menuconfig(void);           // lê CONFIG_MASTER_KEY_HASH (hex)
bool master_key_is_uid(const uid_t *uid);             // compara CRC32(uid) com hash

#ifdef __cplusplus
}
#endif
