cmake_minimum_required(VERSION 3.5)

##################
#  Generic Part  #
##################

set(OUTPUT_NAME  app3)

###############
#  Src files  #
###############

set(sources
  app3.cpp
)

set(headers
)

####################
#  Lib Dependancy  #
####################

include_directories("${PRJ_ROOT_DIR}/src")

################
#  Executable  #
################

add_executable(${OUTPUT_NAME} ${sources})

target_link_libraries(${OUTPUT_NAME}
  lib
  ${mpi_deps}
  ${CMAKE_THREAD_LIBS_INIT})

set_property(TARGET ${OUTPUT_NAME}
  PROPERTY OUTPUT_NAME ${OUTPUT_NAME})

install(TARGETS ${OUTPUT_NAME}
  RUNTIME DESTINATION ${RUNTIME_DEST})
