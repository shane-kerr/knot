#include <assert.h>
#include <gnutls/abstract.h>
#include <gnutls/gnutls.h>
#include <gnutls/x509.h>

#include "binary.h"
#include "error.h"
#include "key.h"
#include "keyid.h"
#include "keyid/internal.h"
#include "keystore/pem.h"
#include "shared.h"

/* -- internal API --------------------------------------------------------- */

/*!
 * Create GnuTLS private key from unencrypted PEM data.
 */
int pem_to_privkey(const dnssec_binary_t *data, gnutls_privkey_t *key, char **id_ptr)
{
	assert(data);
	assert(key);
	assert(id_ptr);

	gnutls_datum_t pem;
	binary_to_datum(data, &pem);

	// create X.509 private key

	gnutls_x509_privkey_t key_x509 = NULL;
	int result = gnutls_x509_privkey_init(&key_x509);
	if (result != GNUTLS_E_SUCCESS) {
		return DNSSEC_ENOMEM;
	}

	int format = GNUTLS_X509_FMT_PEM;
	result = gnutls_x509_privkey_import_pkcs8(key_x509, &pem, format, NULL, 0);
	if (result != GNUTLS_E_SUCCESS) {
		gnutls_x509_privkey_deinit(key_x509);
		return DNSSEC_PKCS8_IMPORT_ERROR;
	}

	// convert to abstract private key

	gnutls_privkey_t key_abs = NULL;
	result = gnutls_privkey_init(&key_abs);
	if (result != GNUTLS_E_SUCCESS) {
		gnutls_x509_privkey_deinit(key_x509);
		return DNSSEC_ENOMEM;
	}

	int flags = GNUTLS_PRIVKEY_IMPORT_AUTO_RELEASE;
	result = gnutls_privkey_import_x509(key_abs, key_x509, flags);
	if (result != GNUTLS_E_SUCCESS) {
		gnutls_x509_privkey_deinit(key_x509);
		gnutls_privkey_deinit(key_abs);
		return DNSSEC_ENOMEM;
	}

	// extract keytag

	char *id = gnutls_x509_privkey_hex_key_id(key_x509);
	if (id == NULL) {
		gnutls_privkey_deinit(key_abs);
		return DNSSEC_ENOMEM;
	}

	*key = key_abs;
	*id_ptr = id;

	return DNSSEC_EOK;
}

static int export_pem(gnutls_x509_privkey_t key, uint8_t *data, size_t *size)
{
	gnutls_x509_crt_fmt_t format = GNUTLS_X509_FMT_PEM;
	return gnutls_x509_privkey_export_pkcs8(key, format, NULL, 0, data, size);
}

int pem_generate(gnutls_pk_algorithm_t algorithm, unsigned bits,
		 dnssec_binary_t *pem, char **id_ptr)
{
	assert(pem);
	assert(id_ptr);

	// generate key

	_cleanup_x509_privkey_ gnutls_x509_privkey_t key = NULL;
	int r = gnutls_x509_privkey_init(&key);
	if (r != GNUTLS_E_SUCCESS) {
		return DNSSEC_ENOMEM;
	}

	r = gnutls_x509_privkey_generate(key, algorithm, bits, 0);
	if (r != GNUTLS_E_SUCCESS) {
		return DNSSEC_KEY_GENERATE_ERROR;
	}

	// convert to PEM and export the ID

	size_t pem_size = 0;
	r = export_pem(key, NULL, &pem_size);
	if (r != GNUTLS_E_SHORT_MEMORY_BUFFER || pem_size == 0) {
		return DNSSEC_KEY_EXPORT_ERROR;
	}

	dnssec_binary_t new_pem = { 0 };
	r = dnssec_binary_alloc(&new_pem, pem_size);
	if (r != DNSSEC_EOK) {
		return r;
	}

	r = export_pem(key, new_pem.data, &new_pem.size);
	if (r != GNUTLS_E_SUCCESS) {
		dnssec_binary_free(&new_pem);
		return DNSSEC_KEY_EXPORT_ERROR;
	}

	// export key ID

	char *id = gnutls_x509_privkey_hex_key_id(key);
	if (!id) {
		dnssec_binary_free(&new_pem);
		return DNSSEC_ENOMEM;
	}

	*pem = new_pem;
	*id_ptr = id;

	return DNSSEC_EOK;
}
