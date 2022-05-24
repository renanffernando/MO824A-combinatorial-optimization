#include "GRASP.hpp"

int main(){
  cin.tie(0)->sync_with_stdio(0);

  Instance* instance = Instance::readInstance();
  vector<double> alphas = {0.05, 0.2};
  const int timeLimit = 60000 * 30;
  const int maxIterations = 500;
  vector<MethodLS> methods = {FirstImprovement, BestImprovement};
  vector<MethodGrasp> methodsGrasp = {Classic, Bias, Pop};

  for(auto alpha : alphas)
    for(auto methodls : methods)
      for(auto methodGrasp: methodsGrasp){
        Solution* sol = GRASP::Grasp(instance, methodGrasp, methodls, alpha, timeLimit, maxIterations);
        assert(sol->cost == Solution::computeCost(sol));
        assert(sol->weight <= instance->W);

        cout << setprecision(3) << fixed;
        switch (methodls){
          case BestImprovement:
            cout << "Best  Improvement - ";
            break;
          case FirstImprovement:
            cout << "First Improvement - ";
            break;
          default:
            throw runtime_error("Invalid Local Search Method");
        }
        switch (methodGrasp){
          case Classic:
            cout << "Classic - ";
            break;
          case Bias:
            cout << "Bias    - ";
            break;
          case Pop:
            cout << "Pop     - ";
            break;
          default:
            throw runtime_error("Invalid Grasp Variant");
        }
        cout <<  "alpha: " << alpha << " - Cost: " << sol->cost << " - Weight: " << sol->weight  << " - Time: " << sol->elapsedTime/1000.0 << "s - numIterationsGrasp: " << sol->iterations << " - numIterationsLS: " << Neighborhood::lsIterations << endl;
        delete sol;
      }

  delete instance;
}
