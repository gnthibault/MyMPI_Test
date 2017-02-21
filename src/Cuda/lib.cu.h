//Thrust
#include <thrust/system_error.h>
#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <thrust/functional.h>
#include <thrust/fill.h>
#include <thrust/transform.h>
#include <thrust/inner_product.h>
#include <thrust/random.h>
#include <thrust/iterator/counting_iterator.h>

// Local
#include "../lib.h"

/** \class NumericalMidPointIntegrator1DCuda
 * \brief Performs numerical integration on a 1D domain using cuda
 *
 * \author Thibault Notargiacomo
 */
template<typename T>
class NumericalMidPointIntegrator1DCuda /*: public
  NumericalMidPointIntegrator1D<T>*/ {
public:
  /// Constructor that force domain bounds definition
  NumericalMidPointIntegrator1DCuda(T lowerBound, T upperBound,
      uint64_t nbSteps) /*: NumericalMidPointIntegrator1D<T>(lowerBound,
        upperBound, nbSteps)*/ {}

  /// Destructor defaulted on purpose
  virtual ~NumericalMidPointIntegrator1DCuda()=default;

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
  /* 
    // Define the midpoint functor
    Integrator<T,F> op(this->m_gridRes,this->m_lowerBound,f);

    sum = thrust::reduce(
      thrust::make_transform_iterator(
        thrust::make_counting_iterator<uint64_t>(this->m_firstIndex), op),
      thrust::make_transform_iterator(
        thrust::make_counting_iterator<uint64_t>(this->m_lastIndex), op),
      0.0, thrust::plus<T>() );

    lIntVal = sum*this->m_gridRes;

    // Reduce over all ranks the value of the integral
    boost::mpi::reduce(this->m_world, lIntVal, gIntVal, std::plus<T>(), 0);
*/
    return gIntVal;
  }
};
