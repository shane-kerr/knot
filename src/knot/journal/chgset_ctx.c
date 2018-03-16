



#include "chgset_ctx.h"

#include "knot/journal/journal.h"

chgset_ctx_t *chgset_ctx_create(size_t chunk_count)
{
	chgset_ctx_t *ch = calloc(1, sizeof(*ch));
	if (ch != NULL) {
		ch->chunk_count = chunk_count;
		ch->src_chunks = calloc(chunk_count, sizeof(*ch->src_chunks));
		ch->chunk_sizes = calloc(chunk_count, sizeof(*ch->chunk_sizes));
		if (ch->src_chunks == NULL || ch->chunk_sizes == NULL) {
			chgset_ctx_free(ch);
			ch = NULL;
		}
	}
	return ch;
}

void chgset_ctx_free(chgset_ctx_t *ch)
{
	free(ch->src_chunks);
	free(ch->chunk_sizes);
	free(ch);
}

void chgset_ctx_list_close(chgset_ctx_list_t *l)
{
	chgset_ctx_t *ch = NULL, *nxt = NULL;
	WALK_LIST_DELSAFE(ch, nxt, l->l) {
		chgset_ctx_free(ch);
	}
	journal_txn_commit(l->txn);
	free(l->txn);
	memset(l, 0, sizeof(*l));
}
