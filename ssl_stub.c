/* set ts=4 sw=4 enc=utf-8: -*- Mode: c; tab-width: 4; c-basic-offset:4; coding: utf-8 -*- */
/*
 * ssl_stub.c
 * 14 June 2018, Chul-Woong Yang (cwyang@gmail.com)
 */
#include <openssl/ssl.h>
#include "ssl_stub.h"

/*
  OPENSSL_VERSION_NUMBER  0x1 00 01 15 fL  // 1.0.1u
  OPENSSL_VERSION_NUMBER  0x1 00 02 0f fL  // 1.0.2o
  OPENSSL_VERSION_NUMBER  0x1 01 00 08 fL  // 1.1.0h
  OPENSSL_VERSION_NUMBER  0x1 01 01 00 7L  // 1.1.1-pre7
*/

#if OPENSSL_VERSION_NUMBER >= 0x1010001fL /* >= 1.1.0.a */
void init_library(void) 
{
    OPENSSL_init_ssl(0, NULL);
}

int check_rsa (const SSL_CIPHER *cipher)
{
    int kx_nid = SSL_CIPHER_get_kx_nid(cipher);
    int auth_nid = SSL_CIPHER_get_auth_nid(cipher);

    if (kx_nid != NID_kx_rsa) {
        return -1;
    }
    if (auth_nid != NID_auth_rsa) {
        return -2;
    }
    return 0;
}
/* allround ssl method */
const SSL_METHOD *SSL_method(uint8_t major, uint8_t minor)
{
    return TLS_method();
}
#else /* < 1.1.0.a */
void init_library(void) 
{
    SSL_library_init();
}

int check_rsa (const SSL_CIPHER *cipher)
{
    unsigned long alg_mkey, alg_auth;

    alg_mkey = cipher->algorithm_mkey;
    alg_auth = cipher->algorithm_auth;

# define SSL_kRSA                0x00000001L
# define SSL_aRSA                0x00000001L
    
    if (alg_mkey != SSL_kRSA) {
        return -1;
    }
    if (alg_auth != SSL_aRSA) {
        return -2;
    }
    return 0;
}
/* allround ssl method */
const SSL_METHOD *SSL_method(uint8_t major, uint8_t minor)
{
    switch(minor) {
    case 0: /* SSL 3.0 */
        return SSLv3_method();
    case 1: /* TLS 1.0 */
        return TLSv1_method();
    case 2: /* TLS 1.1 */
        return TLSv1_1_method();
    case 3: /* TLS 1.2 or greater */
        return TLSv1_2_method();
    default:
        return SSLv23_method();
    }
}

RSA *EVP_PKEY_get0_RSA(EVP_PKEY *pkey) 
{
    RSA *rsa = EVP_PKEY_get1_RSA(pkey);

    if (rsa == NULL)
        return NULL;
    RSA_free(rsa);
    return rsa;
}

#endif


