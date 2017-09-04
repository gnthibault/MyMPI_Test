//STL
#include <cstdlib> 

//Local
#include <Cuda/lib.cu.h>

int main(int argc, char* argv[]) {
  using T = double;
  
  //Define the function to be integrated between 0 and 1 to get pi
  uint64_t nbSteps = 100000000;
  auto f = [] __host__ __device__ (T x)->T{ return 4./(1.+x*x); };
  T lowBound=0, upBound=1;

  // Init mpi and monitor runtime
  /*boost::mpi::environment env;
  boost::mpi::communicator world;
  boost::mpi::timer timer;

  // Actually run computations
  NumericalMidPointIntegrator1DCuda<T> n(lowBound,upBound,nbSteps);
  auto approx = n.Integrate(f);
  
  // Print out pi value and time elapsed since beginning
  if (world.rank() == 0) {
    std::cout << "Pi is approximately "<< std::setprecision(10)
      << approx << std::endl;
    std::cout << "Elapsed time " << std::setprecision(6)
      << timer.elapsed() << "s" << std::endl;
  }*/

  return EXIT_SUCCESS;
}
