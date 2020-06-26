#include <stdint.h> /* uint32_t, uint64_t */
#include <stdio.h>  /* size_t */

#define sha256_blksize 64

struct sha256_ctx {
	uint32_t state[8];
	uint64_t size;
	uint32_t offset;
	uint8_t buf[sha256_blksize];
};

typedef struct sha256_ctx sha256_ctx;

void sha256_init(sha256_ctx *ctx);
void sha256_update(sha256_ctx *ctx, const void *data, size_t len);
void sha256_final(unsigned char *digest, sha256_ctx *ctx);
