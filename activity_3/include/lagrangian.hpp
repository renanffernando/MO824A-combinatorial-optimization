#include "util.hpp"

vvd applyLambdaToCost(const vd& lambda, const vvd& cost);
void updateLambda(const vi& tour1, const vi& tour2, const vi& z, vd& lambda1, vd& lambda2, ld lb, ld ub, ld learnRate);
