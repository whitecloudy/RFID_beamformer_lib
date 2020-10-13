cmake_minimum_required(VERSION 2.6)
project(SIC)

list(APPEND SIC_SOURCES
  SIC_controller.cpp
  SIC_controller.hpp  
  )

add_library(SIC_LIB STATIC ${SIC_SOURCES})
target_link_libraries(SIC_LIB ${SIC_LINKS})
