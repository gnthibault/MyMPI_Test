//STL
#include <cstdlib> 
#include <cassert> 

//Local
#include <lib.h>

int main(int argc, char* argv[]) {
  using T = double;
  
  //Define the function to be integrated between 0 and 1 to get pi
  uint64_t nbSteps = 100000000;
  auto f = [](T x){ return 4./(1.+x*x);};
  T lowBound=0, upBound=1;

  // Init mpi and monitor runtime
  /*boost::mpi::environment env;
  boost::mpi::communicator world;
  boost::mpi::timer timer;*/

  // Actually run computations
  NumericalMidPointIntegrator1D<T> n(lowBound,upBound,nbSteps);
  auto approx = n.Integrate(f);
  
  // Print out pi value and time elapsed since beginning
  /*if (world.rank() == 0) {
    assert(std::abs(approx-M_PI)<1.0e-12);
  }*/

  return EXIT_SUCCESS;
}
