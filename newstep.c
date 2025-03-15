#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>

void old_1_2(mpz_t v) {
  size_t limbs = mpz_size(v);

  mpz_t root; mpz_init(root);
  {
    mp_limb_t *root_mpp = mpz_limbs_write(root, (limbs + 1) / 2);

    mpn_sqrtrem(root_mpp, NULL, mpz_limbs_read(v), limbs);

    mpz_limbs_finish(root, (limbs + 1) / 2);
  }

  mpz_clear(v);
  *v = *root;
}

void new_1_2(
  const mp_limb_t *v, const mp_size_t limbs,
  mp_limb_t **op, mp_size_t *ol
) {
  mp_size_t res = (limbs + 1) >> 1;

  mp_limb_t *root = malloc(sizeof(*root) * res);
  mpn_sqrtrem(root, NULL, v, limbs);

  while (root[res - 1] == 0)
    --res;

  *op = root; *ol = res;
}

int main(void) {
  mpz_t n; mpz_init(n);

  mpz_ui_pow_ui(n, 2, 300); mpz_add_ui(n, n, 2);
  gmp_printf("%Zd\n", n);

  mpz_ui_pow_ui(n, 2, 300); mpz_add_ui(n, n, 2);
  old_1_2(n);
  gmp_printf("%Zd\n", n);

  mpz_ui_pow_ui(n, 2, 300); mpz_add_ui(n, n, 2);
  mp_limb_t *rp; mp_size_t lp;
  new_1_2(
    mpz_limbs_read(n), mpz_size(n),
    &rp, &lp
  );
  mpz_t r; r->_mp_d = rp; r->_mp_alloc = r->_mp_size = lp;
  gmp_printf("%Zd\n", r);

  mpz_clear(n); mpz_clear(r);
}
