cmake_minimum_required(VERSION 2.6)
project(Reader_comm)

list(APPEND READER_COMM_SOURCES
  IPC_controller.cpp
  IPC_controller.hpp  
  )

add_library(READER_COMM_LIB STATIC ${READER_COMM_SOURCES})
target_link_libraries(READER_COMM_LIB ${READER_COMM_LINKS})
