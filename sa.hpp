#ifndef __sa_hpp__
#define __sa_hpp__

#include <memory>
#include <utility>
#include <cmath>
#include <random>
#include <limits>
#include "cooling.hpp"
#include "problem.hpp"

namespace COP {

class SimulatedAnnealing {

  public:
    SimulatedAnnealing() = delete;
    SimulatedAnnealing(
      std::unique_ptr<Cooling> cooling,
      std::unique_ptr<Problem> problem,
      std::size_t randomState,
      bool setAutomatically = false,
      std::size_t innerLoops = 50,
      std::size_t numberOfRandomIterations = 33
    ) : cooling( std::move( cooling ) ),
        problem( std::move( problem ) ),
        numberOfRandomIterations( numberOfRandomIterations ),
        innerLoops( innerLoops ),
        randomState( randomState ),
        randomGenerator( randomState ),
        bestCost( -1. ),
        bestState( nullptr ) {
      if( setAutomatically ) {
        bestStartingState = calculateInitialTemperature();
        innerLoops = getProblem().getInstanceSize() * 2;
      }
      else
        bestStartingState = getProblem().getRandomInitialState();
    } 
    SimulatedAnnealing(
      std::unique_ptr<Cooling> cooling,
      std::unique_ptr<Problem> problem,
      bool setAutomatically = false,
      std::size_t innerLoops = 50,
      std::size_t numberOfRandomIterations = 33
    ) : SimulatedAnnealing(
          std::move( cooling ),
          std::move( problem ),
          std::random_device()(),
          setAutomatically,
          innerLoops,
          numberOfRandomIterations
        ) {} 

    std::vector<double> solve();
    auto getBestCost() const {
      return bestCost;
    }
    auto getBestState() const {
      return bestState;
    }

  private:
    std::unique_ptr<Cooling> cooling;
    std::unique_ptr<Problem> problem;
    std::size_t numberOfRandomIterations;
    std::size_t innerLoops;
    std::size_t randomState;
    std::mt19937_64 randomGenerator;
    std::shared_ptr<State> bestStartingState;
    // found in solve() method
    double bestCost;
    std::shared_ptr<State> bestState;

    Cooling& getCooling() { return *cooling; }
    Cooling& getCooling() const { return *cooling; }
    Problem& getProblem() { return *problem; }
    Problem& getProblem() const { return *problem; }
    double getRandomNumber() {
      std::uniform_real_distribution<> distribution( 0., 1. );
      return distribution( randomGenerator );
    }
    double acceptProbability( double deltaCost, double temp ) const {
      return std::exp( - deltaCost / temp );
    }
    double acceptProbability( double deltaCost ) const {
      return acceptProbability( deltaCost, getCooling().getTemperature() );
    }
    double acceptCost( double deltaCost ) {
      auto prob = acceptProbability( deltaCost );
      auto rand = getRandomNumber();
      return prob > rand;
      return acceptProbability( deltaCost ) > getRandomNumber();
    }
    std::shared_ptr<State> calculateInitialTemperature();

};

/**
  * Try to find a global maximum using Simulated Annealing.
  */
std::vector<double> SimulatedAnnealing::solve() {

  auto state = bestStartingState;
  bestState = state;
  bestCost = getProblem().getStateCost( bestState ).first;
  std::vector<double> history;

  // main loop
  while( getCooling().isActive() ) {

    // inner loop
    for( std::size_t ii = 0; ii < innerLoops; ++ii ) {
      auto neighbour = getProblem().getRandomNeighbourOfState( state );
      auto stateCost = getProblem().getStateCost( state ).first;
      auto neighbourCost = getProblem().getStateCost( neighbour ).first;
    
      auto deltaCost = stateCost - neighbourCost;
      if( deltaCost < 0 || acceptCost( deltaCost ) )
        state = neighbour;

      // Remember best state
      if( neighbourCost > bestCost ) {
        bestCost = neighbourCost;
        bestState = neighbour;        
      }
    }
    getCooling().cool();

    history.emplace_back( getProblem().getStateCost( state ).first );
  }

  return history;
}

/**
  * Reversed simulated annealing
  * Tries to find the best starting temperature.
  * This is done by quickly increasing the starting temperature 0.
  * Currently done in quadratic style, however this can take quite some
  * time if it is hard to find some "good" state (that is, the state is
  * not invalid)
  */
std::shared_ptr<State> SimulatedAnnealing::calculateInitialTemperature() {

  auto state = getProblem().getInitialState();
  auto bestStartingState = state;
  double goalProbability = 0.5;
  double bestProbability = 0;

  double bestTemp = 0.;

  // don't stop until better temperature is found
  while( bestTemp < 1 ){
    for( std::size_t jj = 0; jj < numberOfRandomIterations; ++jj ) {

      auto curTemp = jj * jj + 1;
      for( std::size_t ii = 0; ii < innerLoops; ++ii ) {
        auto neighbour = getProblem().getRandomNeighbourOfState( state );
        auto stateCost = getProblem().getStateCost( state ).first;
        auto neighbourCost = getProblem().getStateCost( neighbour ).first;

        auto deltaCost = stateCost - neighbourCost;
        auto probability = acceptProbability( deltaCost, curTemp );
     
        if( std::pow( goalProbability - probability, 2 ) <
            std::pow( goalProbability - bestProbability, 2 ) ) {
          bestProbability = probability;
          bestTemp = curTemp;
          state = neighbour;
        }
      }

      bestStartingState = state;
      // state = getProblem().getRandomInitialState();
      state = getProblem().getRandomNeighbourOfState( state );
    }
  }

  getCooling().setStartTemperature( bestTemp );
  getCooling().setMinTemperature( bestTemp / 10 );

  return bestStartingState; 
}

}

#endif /* __sa_hpp__ */
