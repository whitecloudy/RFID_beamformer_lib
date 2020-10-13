cmake_minimum_required(VERSION 2.6)
project(AD5371_ctrl)

list(APPEND AD5371_CTRL_SOURCES
  Phase_Attenuator_controller.cpp
  Phase_Attenuator_controller.hpp
  SPI_communicator.cpp
  SPI_communicator.hpp
  Vout_controller.cpp
  Vout_controller.hpp
  )

list(APPEND AD5371_CTRL_LINKS
  pthread
  )


add_library(AD5371_CTRL_LIB STATIC ${AD5371_CTRL_SOURCES})
target_link_libraries(AD5371_CTRL_LIB ${AD5371_CTRL_LINKS})
