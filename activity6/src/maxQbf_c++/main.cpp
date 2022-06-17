#include "GA.hpp"

int main(){
  cin.tie(0)->sync_with_stdio(0);

  Instance *instance = Instance::readInstance();
  const int timeLimit = 60000 * 30;
  const int maxGenerations = 10000;
  vi popSizes = {100, 500};
  vd mutationRates = {0.01, 0.05};
  vector<GAType> gaTypes = {Random, Classic, Diversity, Stochastic};

  for(GAType gaType : gaTypes)
    for(ld mutationRate : mutationRates)
      for (int popSize : popSizes){
        GA ga(instance, gaType, mutationRate, popSize, timeLimit, maxGenerations);
        Solution *sol = ga.run();

        assert(sol->value == Solution::computeValue(sol));
        assert(sol->getWeight() <= instance->W);
        switch (gaType){
          case Random:
            cout << "Random     - ";
            break;
          case Stochastic:
            cout << "Stochastic - ";
            break;
          case Classic:
            cout << "Classic    - ";
            break;
          case Diversity:
            cout << "Diversity  - ";
            break;
          default:
            assert(false);
        }

        cout << setprecision(3) << fixed;
        cout << "Mutation Rate: " << mutationRate << " - PopSize: : " << popSize;
        cout << " - InitialCost: " << sol->bestInitial << " - Cost: " << sol->getCost();
        cout << " - Weight: " << sol->getWeight() << " - Time: " << sol->elapsedTime / 1000.0;
        cout << " - generations: " << sol->generations;
        cout << endl;
        delete sol;
      }

  delete instance;
}
