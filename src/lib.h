
//STL
#include <iostream>
#include <cstdint>
#include <numeric>
#include <iomanip>

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
  #ifdef __CUDACC__
    __host__ __device__
  #endif //__CUDACC__
  T operator()(uint64_t i) const {
    return m_func(m_lowerBound+((T)i+0.5)*m_step);
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
      m_chunkSize = (m_nbSteps+/*m_world.size()*/1-1ul)/1/*m_world.size()*/;
      m_gridRes = (m_upperBound-m_lowerBound)/m_nbSteps;
      m_firstIndex = /*m_world.rank()*/1*m_chunkSize;
      m_lastIndex = std::min(m_firstIndex+m_chunkSize,m_nbSteps);
    }

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
    T lIntVal, gIntVal, sum = 0.0;
   
    // Define the midpoint functor
    Integrator<T,F> op(m_gridRes,m_lowerBound,f);

    lIntVal = sum*m_gridRes;

    // Reduce over all ranks the value of the integral
    //boost::mpi::reduce(m_world, lIntVal, gIntVal, std::plus<T>(), 0);

    return gIntVal;
  }

protected:
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

  // First index of the counting iterator in the current rank
  uint64_t m_firstIndex;

  // Last index of the counting iterator in the current rank
  uint64_t m_lastIndex; 
 
  /// MPI related communication handler
  //boost::mpi::communicator m_world;
};
