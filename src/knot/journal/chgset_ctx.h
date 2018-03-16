#pragma once


#include "contrib/ucw/lists.h"
#include "contrib/wire_ctx.h"

typedef enum {
	CHGSET_CTX_NOITER = 0,
	CHGSET_CTX_SOA_FROM,
	CHGSET_CTX_REM,
	CHGSET_CTX_SOA_TO,
	CHGSET_CTX_ADD,
} chgset_ctx_phase_t;

struct journal_txn; // journal.c

typedef struct {
	node_t n;

	uint8_t **src_chunks;
	size_t *chunk_sizes;
	size_t chunk_count;
	size_t curr_chunk;
	wire_ctx_t wire;
	chgset_ctx_phase_t phase;

	uint32_t serial_from;
	uint32_t serial_to;
} chgset_ctx_t;

typedef struct {
	list_t l;
	struct journal_txn *txn;
} chgset_ctx_list_t;

chgset_ctx_t *chgset_ctx_create(size_t chunk_count);

void chgset_ctx_free(chgset_ctx_t *ch);

void chgset_ctx_list_close(chgset_ctx_list_t *l);





