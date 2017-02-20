
//STL
#include <iostream>
#include <cstdint>
#include <numeric>
#include <iomanip>

//Boost
#include <boost/iterator/transform_iterator.hpp>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/mpi/collectives.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/mpi/environment.hpp>
#include <boost/mpi/timer.hpp>

//Local
#ifdef USE_CUDA
  #include "Cuda/lib.cu.h"
#endif //USE_CUDA

/** \struct Integrator
 * \brief operator to be mapped over a range before composing with
 * the range accumulator
 *
 * \author Thibault Notargiacomo
 */
template<typename T, typename F>
struct Integrator{
  Integrator(T step, T lowerBound, F func): m_step(step),
    m_lowerBound(lowerBound), m_func(func) {};
  T operator()(uint64_t i) const {
    return m_func(m_lowerBound+(i+0.5)*m_step);
  }
  const T m_step;
  const T m_lowerBound;
  const F m_func;
};

/** \class NumericalMidPointIntegrator1D
 * \brief Performs numerical integration on a 1D domain
 *
 * \author Thibault Notargiacomo
 */
template<typename T>
class NumericalMidPointIntegrator1D {
public:
  /// Constructor that force domain bounds definition
  NumericalMidPointIntegrator1D(T lowerBound, T upperBound, uint64_t nbSteps):
    m_lowerBound(lowerBound), m_upperBound(upperBound),
    m_nbSteps(nbSteps) {
      m_chunkSize = std::max(m_nbSteps/m_world.size(),1ul);
      m_gridRes = (m_upperBound-m_lowerBound)/m_nbSteps;
    }
;

  /// Destructor defaulted on purpose
  virtual ~NumericalMidPointIntegrator1D()=default;

  /**
   * Function that will integrate a 1D scalar function over a 1D domain whose
   * bounds are defined by 2 fields in the object
   *
   * \param[in] f the 1D scalar function to be integrated
   *
   * \return The value of the numerical integral
   */
  template<typename F>
  T Integrate(F f) {
    T lIntVal, gIntVal, res, sum = 0.0;
   
    // Monitor runtime
    boost::mpi::timer timer;

    // Define local bounds
    uint64_t firstIndex = m_world.rank()*m_chunkSize;
    uint64_t lastIndex = std::min(firstIndex+m_chunkSize,m_nbSteps); 
    Integrator<T,F> op(m_gridRes,m_lowerBound+firstIndex*m_gridRes,f);
 
    #ifdef USE_CUDA

    #else //USE_CUDA
    sum = std::accumulate(
      boost::make_transform_iterator(
        boost::make_counting_iterator<uint64_t>(firstIndex), op),
      boost::make_transform_iterator(
        boost::make_counting_iterator<uint64_t>(lastIndex), op),
      0.0, std::plus<T>());
    #endif //USE_CUDA
    lIntVal = sum*m_gridRes;

    // Reduce over all ranks the value of the integral
    boost::mpi::all_reduce(m_world, lIntVal, gIntVal, std::plus<T>());

    // Print out pi value and time elapsed since beginning
    if (m_world.rank() == 0) {
      std::cout << "Pi is approximately "<< gIntVal << std::endl;
      std::cout << "Elapsed time " << timer.elapsed() << std::endl;
    }

    return gIntVal;
  }

private:
  /// Lower bound for numerical integration
  const T m_lowerBound;

  /// Upper bound for numerical integration
  const T m_upperBound;

  /// 1D grid resolution
  T m_gridRes;

  /// Number of node used to defined the discrete grid
  const uint64_t m_nbSteps;
 
  // Number of nodes per chunk that will be distributed across ranks
  uint64_t m_chunkSize; 
 
  /// MPI related communication handler
  const boost::mpi::communicator m_world;

  /// MPI related environment handler
  const boost::mpi::environment m_env;
};


