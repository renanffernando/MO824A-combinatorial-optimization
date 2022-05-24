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


void updateLambda(const vi& tour0, const vi& tour1, const vi& z, vd& lambda1, vd& lambda2, ld lb, ld ub, ld learnRate){
    int n = SZ(tour1);
    vd grad1(z.cbegin(), z.cend()), grad2(z.cbegin(), z.cend());

    FOR(i, n){
        int u0 = tour0[i], v0 = tour0[(i + 1) % n];
        int u1 = tour1[i], v1 = tour1[(i + 1) % n];
        grad1[u0 * n + v0] -= 1;
        grad2[u1 * n + v1] -= 1;
        grad1[v0 * n + u0] -= 1;
        grad2[v1 * n + u1] -= 1;
    }

    ld norm = 0;
    FOR(i, SZ(grad1))
        norm += grad1[i]* grad1[i] + grad2[i] * grad2[i];

    ld alpha = learnRate * (ub - lb) / norm;

    FOR(i, SZ(lambda1)){
        lambda1[i] = max(0.0, lambda1[i] + alpha * grad1[i]);
        lambda2[i] = max(0.0, lambda2[i] + alpha * grad2[i]);
    }

    FOR(i, n)
        FOR(j, n){
            assert(abs(lambda1[i * n + j] - lambda1[j * n + i]) < 1e-6);
            assert(abs(lambda2[i * n + j] - lambda2[j * n + i]) < 1e-6);
        }
}
