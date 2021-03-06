cmake_minimum_required(VERSION 3.5)

#################
#  Generic Part #
#################

project(lib)
set(OUTPUT_NAME lib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
###############
#  Cuda Part  #
###############
if (CUDA_FOUND AND USE_CUDA)
  set(sources_cuda
    Cuda/lib.cu
  )
  set(headers_cuda
    Cuda/lib.cu.h
  )
endif ()
################
#  /Cuda Part  #
################i

###############
#  Src files  #
###############
set(sources
  lib.cpp
  # cuda source files
  ${sources_cuda}
)

set(headers
  lib.h
  # cuda header files
  ${headers_cuda}
)

################
#  Executable  #
################

if (CUDA_FOUND)
  cuda_add_library(${OUTPUT_NAME} SHARED ${sources} ${headers})
  #CUDA_ADD_CUBLAS_TO_TARGET(${OUTPUT_NAME})
else ()
  add_library(${OUTPUT_NAME} SHARED ${sources} ${headers})
endif ()

target_link_libraries(${OUTPUT_NAME}
  ${cuda_deps}
  ${mpi_deps}
  ${boost_deps})

set_property(TARGET ${OUTPUT_NAME}
  PROPERTY OUTPUT_NAME ${output_name})

install(TARGETS ${OUTPUT_NAME}
  LIBRARY DESTINATION ${RUNTIME_DEST}
)
