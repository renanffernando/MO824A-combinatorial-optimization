#include "GA.hpp"

int main()
{
  cin.tie(0)->sync_with_stdio(0);

  Instance *instance = Instance::readInstance();
  const int timeLimit = 60000 * 30;
  const int maxGenerations = 10000;
  vi popSizes = {100};

  for (int popSize : popSizes)
  {
    GA ga(instance, popSize, timeLimit, maxGenerations);
    Solution *sol = ga.run();

    assert(sol->value == Solution::computeValue(sol));
    assert(sol->getWeight() <= instance->W);

    cout << setprecision(3) << fixed;
    cout << "PopSize: : " << popSize << " - Cost: " << sol->getCost();
    cout << " - Weight: " << sol->getWeight() << " - Time: " << sol->elapsedTime / 1000.0;
    cout << " - generations: " << sol->generations;
    cout << endl;
    delete sol;
  }

  delete instance;
}
