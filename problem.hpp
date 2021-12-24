#ifndef __problem_hpp__
#define __problem_hpp__

#include <memory>
#include <vector>
#include <random>
#include <algorithm>

namespace COP {

class State {
  public:
    virtual ~State() = default;
};

class KnapsackState : public State {

  public:
    KnapsackState( std::vector<bool> && state )
    : state( std::move( state ) ) {}
    std::vector<bool> getState() const { return state; }

  private:
    std::vector<bool> state;     
};

class Problem {

  public:
    Problem( std::size_t randomState )
    : randomState( randomState ),
      randomGenerator( randomState ) {}
    virtual ~Problem() = default;
    virtual std::shared_ptr<State> getInitialState() = 0;
    virtual std::shared_ptr<State> getRandomInitialState() = 0;
    virtual std::shared_ptr<State> getRandomNeighbourOfState(
      std::shared_ptr<State> state
    ) = 0;
    virtual std::pair<double,bool> getStateCost( std::shared_ptr<State> ) const = 0;
    virtual std::size_t getInstanceSize() const = 0;

  protected:
    std::size_t randomState;
    std::mt19937_64 randomGenerator;

};

class Knapsack : public Problem {

  public:
    Knapsack(
      const std::vector<std::size_t> & weights,
      const std::vector<std::size_t> & costs,
      std::size_t maxWeight,
      std::size_t randomState
    ) : Problem( randomState ),
        weights( weights ),
        costs( costs ),
        maxWeight( maxWeight ) {}
    Knapsack(
      const std::vector<std::size_t> & weights,
      const std::vector<std::size_t> & costs,
      std::size_t maxWeight
    ) : Problem( std::random_device()() ),
        weights( weights ),
        costs( costs ),
        maxWeight( maxWeight ) {}
    Knapsack(
      std::vector<std::size_t> && weights,
      std::vector<std::size_t> && costs,
      std::size_t maxWeight,
      std::size_t randomState
    ) : Problem( randomState ),
        weights( std::move( weights ) ),
        costs( std::move( costs ) ),
        maxWeight( maxWeight ) {}
    Knapsack(
      std::vector<std::size_t> && weights,
      std::vector<std::size_t> && costs,
      std::size_t maxWeight
    ) : Problem( std::random_device()() ),
        weights( std::move( weights ) ),
        costs( std::move( costs ) ),
        maxWeight( maxWeight ) {}

    virtual std::shared_ptr<State> getInitialState() {
      return std::make_shared<KnapsackState>( std::vector<bool>( weights.size(), false ) );
    }
    virtual std::shared_ptr<State> getRandomInitialState() {
      std::vector<bool> state( weights.size() );
      std::uniform_int_distribution<> distribution( 0, 1 );
      auto genLambda = [&distribution, this]() {
        return distribution( randomGenerator );
      };
      
      std::generate( state.begin(), state.end(), genLambda );
      return std::make_shared<KnapsackState>( std::move( state ) );
    }
    virtual std::shared_ptr<State> getRandomNeighbourOfState(
      std::shared_ptr<State> state
    ) {
      auto index = getRandomIndex();
      auto neighbour = std::dynamic_pointer_cast<KnapsackState>( state ) -> getState();

      // either remove or add item
      neighbour[index] = neighbour[index] ? false : true;
      return std::make_shared<KnapsackState>( std::move( neighbour ) );
    }
    virtual std::pair<double,bool> getStateCost( std::shared_ptr<State> state ) const {

      double cost = 0;
      double weight = 0;
      auto array = std::dynamic_pointer_cast<KnapsackState>( state ) -> getState();

      for( std::size_t ii = 0; ii < array.size(); ++ii ) {
        cost += costs[ii] * array[ii];
        weight += weights[ii] * array[ii];
      }

      // penalize
      cost -= std::max( 1000 * ( weight - (double)maxWeight ), 0. );

      if( weight > maxWeight )
        return { cost, false };

      return { cost, true };
    }
    virtual std::size_t getInstanceSize() const {
      return weights.size();
    }

  private:
    std::vector<std::size_t> weights;
    std::vector<std::size_t> costs;
    std::size_t maxWeight;

    std::size_t getRandomIndex() {
      std::uniform_int_distribution<std::size_t> distribution( 0, weights.size() );
      return distribution( randomGenerator );
    }
};

}

#endif /* __problem_hpp__ */
