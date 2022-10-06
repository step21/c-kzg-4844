#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "c_kzg_4844.h"

int verify_kzg_proof_wrap(const uint8_t c[48], const uint8_t x[32], const uint8_t y[32], const uint8_t p[48], KZGSettings *s) {
  KZGCommitment commitment;
  BLSFieldElement px, py;
  KZGProof proof;
  bool out;

  if (bytes_to_g1(&commitment, c) != C_KZG_OK) return -1;
  bytes_to_bls_field(&px, x);
  bytes_to_bls_field(&py, y);
  if (bytes_to_g1(&proof, p) != C_KZG_OK) return -1;

  if (verify_kzg_proof(&out, &commitment, &px, &py, &proof, s) != C_KZG_OK)
    return -2;
  return out ? 1 : 0;
}

KZGSettings* load_trusted_setup_wrap(const char* file) {
  KZGSettings* out = (KZGSettings*)malloc(sizeof(KZGSettings));

  if (out == NULL) return NULL;

  FILE* f = fopen(file, "r");

  if (f == NULL) return NULL;

  if (load_trusted_setup(out, f) != C_KZG_OK) return NULL;

  return out;
}

int evaluate_polynomial_wrap(uint8_t out[32], const uint8_t pvals[], size_t n, const uint8_t point[32], const KZGSettings *s) {
  PolynomialEvalForm p;

  if (alloc_polynomial(&p, n) != C_KZG_OK)
    return -1;

  for (size_t i = 0; i < n; i++)
    bytes_to_bls_field(&p.values[i], &pvals[i * 32]);

  BLSFieldElement z;
  bytes_to_bls_field(&z, point);

  BLSFieldElement r;

  if (evaluate_polynomial_in_evaluation_form(&r, &p, &z, s) != C_KZG_OK) {
    free_polynomial(&p);
    return -1;
  }

  bytes_from_bls_field(out, &r);

  return 0;
}

void free_trusted_setup_wrap(KZGSettings* s) {
  free_trusted_setup(s);
  free(s);
}

BLSFieldElement* bytes_to_bls_field_wrap(const uint8_t bytes[]) {
  BLSFieldElement* out = (BLSFieldElement*)malloc(sizeof(BLSFieldElement));
  bytes_to_bls_field(out, bytes);
  return out;
}

uint64_t* uint64s_from_bls_field(BLSFieldElement *fr) {
  uint64_t *r = (uint64_t*)calloc(4, sizeof(uint64_t));
  uint64s_from_BLSFieldElement(r, fr);
  return r;
}
