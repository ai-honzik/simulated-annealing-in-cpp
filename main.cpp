#include "cooling.hpp"
#include <memory>
#include "sa.hpp"
#include "problem.hpp"
#include <iostream>

using namespace COP;
using namespace std;

int main( void ){

  auto SA = SimulatedAnnealing(
              std::make_unique<GeometricCooling>( 70, 0.01, 0.995 ),
              std::make_unique<Knapsack>(
                std::vector<std::size_t>{ 36, 1, 43, 113, 202, 10, 149, 209, 28, 65 },
                std::vector<std::size_t>{ 574, 253, 636, 1266, 2068, 334, 1588, 2126, 495, 831 },
                522
              ),
              true
            );

  cout << SA.solve().size() << endl;
  cout << "Best cost : " << SA.getBestCost() << endl;
  
}
