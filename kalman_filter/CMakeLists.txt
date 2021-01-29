cmake_minimum_required(VERSION 2.6)
project(KALMAN_FILTER)

list(APPEND KALMAN_FILTER_SOURCES
  Kalman_filter.cpp
  Kalman_filter.hpp  
  )

list(APPEND KALMAN_FILTER_LINKS
  armadillo
  )

add_library(KALMAN_FILTER_LIB STATIC ${KALMAN_FILTER_SOURCES})
target_link_libraries(KALMAN_FILTER_LIB ${KALMAN_FILTER_LINKS})
