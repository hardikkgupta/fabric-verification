# Fabric Verification and Bare-Metal Firmware

This project implements a transaction-level model of a high-radix interconnect fabric along with bare-metal firmware for hardware verification and testing. The project demonstrates advanced hardware verification techniques, fault injection, and real-time operating system integration.

## Project Structure

```
.
├── firmware/           # Bare-metal firmware and RTOS integration
│   ├── src/           # C and Assembly source files
│   ├── include/       # Header files
│   └── rtos/          # RTOS integration code
├── sim/               # Simulation environment
│   ├── tlm/          # Transaction-level model (C++)
│   ├── testbench/    # Python testbench
│   └── tests/        # Test scenarios
├── tools/            # Utility scripts and tools
└── docs/             # Documentation
```

## Features

- High-radix interconnect fabric transaction-level model in C++
- Python-based testbench for fault injection and stress testing
- Bare-metal firmware in C and Assembly
- Custom RTOS integration for real-time operations
- Comprehensive test suite with fault injection scenarios
- CI/CD pipeline integration

## Prerequisites

- C++17 or later
- Python 3.8+
- ARM GCC toolchain
- CMake 3.15+
- Git

## Building the Project

### Simulation Environment

```bash
cd sim
mkdir build && cd build
cmake ..
make
```

### Firmware

```bash
cd firmware
make
```

## Running Tests

```bash
cd sim
python3 run_tests.py
```

## License

This project is licensed under the MIT License - see the LICENSE file for details.