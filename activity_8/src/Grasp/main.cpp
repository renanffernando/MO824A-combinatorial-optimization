#include "GRASP.hpp"

int main(){
  cin.tie(0)->sync_with_stdio(0);

  Instance* instance = Instance::readInstance();
  vector<double> alphas = {0.05};
  const int timeLimit = 60 * 1e6;
  vector<MethodLS> methods = {BestImprovement};
  vector<MethodGrasp> methodsGrasp = {Classic};// , Pop};
  map<int, int> targets;
  targets.insert({20, 110});
  targets.insert({40, 308});
  targets.insert({60, 446});
  targets.insert({80, 750});
  targets.insert({100, 1200});
  targets.insert({200, 3900});
  targets.insert({400, 10600});

  for(auto alpha : alphas)
    for(auto methodls : methods)
      for(auto methodGrasp: methodsGrasp){
        assert(targets.count(instance->n));
        Solution* sol = GRASP::Grasp(instance, methodGrasp, methodls, alpha, timeLimit, targets[instance->n]);
        assert(sol->cost == Solution::computeCost(sol));
        assert(sol->weight <= instance->W);

        cout << setprecision(6) << fixed;
        
        cout <<  "Cost: " << sol->cost << " - Time: " << sol->elapsedTime/1e6 << endl;
        delete sol;
      }

  delete instance;
}
