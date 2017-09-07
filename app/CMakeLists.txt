cmake_minimum_required (VERSION 3.5)

#####################################
#  Adding various code directories  #
#####################################

add_subdirectory(app1)
add_subdirectory(app3)

if (CUDA_FOUND AND USE_CUDA)
  add_subdirectory(app2)
endif ()
