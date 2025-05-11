cmake_minimum_required(VERSION 3.15)
project(fabric_verification VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find required packages
find_package(SystemC REQUIRED)
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# Add simulation library
add_library(fabric_tlm
    sim/tlm/fabric_tlm.cpp
)

target_include_directories(fabric_tlm
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/sim/tlm
)

target_link_libraries(fabric_tlm
    PUBLIC
        SystemC::SystemC
)

# Add firmware library
add_library(firmware
    firmware/src/firmware.c
    firmware/src/firmware_asm.S
    firmware/src/rtos.c
)

target_include_directories(firmware
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/firmware/include
)

# Add testbench executable
add_executable(fault_injector
    sim/testbench/fault_injector.py
)

# Install Python dependencies
find_program(PIP3 pip3)
if(PIP3)
    execute_process(
        COMMAND ${PIP3} install -r requirements.txt
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()

# Add tests
enable_testing()
add_test(NAME fault_injection
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/sim/testbench/fault_injector.py
)

# Install targets
install(TARGETS fabric_tlm firmware
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install headers
install(DIRECTORY firmware/include/
    DESTINATION include/firmware
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY sim/tlm/
    DESTINATION include/fabric_tlm
    FILES_MATCHING PATTERN "*.hpp"
)

# Add documentation target
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
endif() 