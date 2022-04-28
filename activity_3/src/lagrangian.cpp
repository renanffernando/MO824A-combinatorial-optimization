#include "lagrangian.hpp"

vvd applyLambdaToCost(const vd& lambda, const vvd& cost) {
  int n = SZ(cost);
  assert(SZ(lambda) == n * n);
  vvd ret = cost;
  FOR(i, n)
    FOR(j, n)
      ret[i][j] = cost[i][j] - lambda[i * n + j];
  return ret;
}

void updateLambda(const vvd& x1, const vvd& x2, const vvd& z, vd& lambda1, vd& lambda2, ld lb, ld ub){
  int n = SZ(x1);
  vvd grad1(n, vd(n)), grad2(n, vd(n));
  FOR(i, n)
    FOR(j, n){
      grad1[i][j] = z[i][j] - x1[i][j];
      grad2[i][j] = z[i][j] - x2[i][j];
    }

  ld norm = 0;
  FOR(i, n)
    FOR(j, n)
      norm += grad1[i][j] * grad1[i][j] + grad2[i][j] * grad2[i][j];

  ld alpha = (sqrt(5) - 1) / 2 * (ub - lb) / norm;

  FOR(i, n)
    FOR(j, n){
      lambda1[i * n + j] = max(0.0, lambda1[i * n + j] + alpha * grad1[i][j]);
      lambda2[i * n + j] = max(0.0, lambda2[i * n + j] + alpha * grad2[i][j]);
    }
}
