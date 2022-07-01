#include "GA.hpp"

int main(){
  cin.tie(0)->sync_with_stdio(0);

  Instance *instance = Instance::readInstance();
  const int timeLimit = 60 * 1e6;
  vi popSizes = {100};
  vd mutationRates = {0.01};
  vector<GAType> gaTypes = {Classic}; //, Diversity};
  map<int, int> targets;
  targets.insert({20, 110});
  targets.insert({40, 308});
  targets.insert({60, 446});
  targets.insert({80, 750});
  targets.insert({100, 1200});
  targets.insert({200, 3900});
  targets.insert({400, 10600});
  
  for(GAType gaType : gaTypes)
    for(ld mutationRate : mutationRates)
      for (int popSize : popSizes){
        assert(targets.count(instance->n));
        GA ga(instance, gaType, mutationRate, popSize, timeLimit, targets[instance->n]);
        Solution *sol = ga.run();

        assert(sol->value == Solution::computeValue(sol));
        assert(sol->getWeight() <= instance->W);

        cout << setprecision(6) << fixed;
        cout << "Cost: " << sol->getCost() << " - Time: " << sol->elapsedTime / 1e6;
        cout << endl;
        delete sol;
      }

  delete instance;
}
