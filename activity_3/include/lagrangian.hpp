#include "util.hpp"

vvd applyLambdaToCost(const vd& lambda, const vvd& cost);
void updateLambda(const vvd& x1, const vvd& x2, const vvd& z, vd& lambda1, vd& lambda2, ld lb, ld ub);
