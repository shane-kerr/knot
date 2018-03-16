/*  Copyright (C) 2018 CZ.NIC, z.s.p.o. <knot-dns@labs.nic.cz>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "knot/include/module.h"
#include "libknot/libknot.h"

const yp_item_t geoip_conf[] = {
};

typedef struct {
} geoip_ctx_t;

static knotd_state_t geoip_process(knotd_state_t state, knot_pkt_t *pkt,
                                   knotd_qdata_t *qdata, knotd_mod_t *mod)
{
	assert(pkt && qdata && mod);
	return state;
}

int geoip_load(knotd_mod_t *mod)
{
	// Create module context.
	geoip_ctx_t *ctx = calloc(1, sizeof(geoip_ctx_t));
	if (ctx == NULL) {
		return KNOT_ENOMEM;
	}

	return knotd_mod_hook(mod, KNOTD_STAGE_BEGIN, geoip_process);
}

void geoip_unload(knotd_mod_t *mod)
{
	assert(mod);
}

KNOTD_MOD_API(geoip, KNOTD_MOD_FLAG_SCOPE_ZONE | KNOTD_MOD_FLAG_OPT_CONF,
              geoip_load, geoip_unload, geoip_conf, NULL);
