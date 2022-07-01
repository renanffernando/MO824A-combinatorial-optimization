#ifndef GRASP_H
#define GRASP_H
#include "Neighborhood.hpp"

enum MethodGrasp {Classic, Bias, Pop};

namespace GRASP{
  Solution* Grasp(Instance* instance, MethodGrasp methodGrasp, MethodLS methodls, double alpha, int timeLimit, int target);
};

#endif
