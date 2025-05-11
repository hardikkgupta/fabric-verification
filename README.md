# Fabric Verification and Bare-Metal Firmware

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![CMake](https://github.com/hardikkgupta/fabric-verification/actions/workflows/cmake.yml/badge.svg)](https://github.com/hardikkgupta/fabric-verification/actions/workflows/cmake.yml)
[![Python Tests](https://github.com/hardikkgupta/fabric-verification/actions/workflows/python-tests.yml/badge.svg)](https://github.com/hardikkgupta/fabric-verification/actions/workflows/python-tests.yml)

This project implements a transaction-level model of a high-radix interconnect fabric along with bare-metal firmware for hardware verification and testing. The project demonstrates advanced hardware verification techniques, fault injection, and real-time operating system integration.

## Features

- High-radix interconnect fabric transaction-level model in C++
- Python-based testbench for fault injection and stress testing
- Bare-metal firmware in C and Assembly
- Custom RTOS integration for real-time operations
- Comprehensive test suite with fault injection scenarios
- CI/CD pipeline integration

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

## Prerequisites

- C++17 or later
- Python 3.8+
- ARM GCC toolchain
- CMake 3.15+
- Git
- SystemC library

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

## Key Components

### Transaction-Level Model (TLM)
- High-radix router implementation (up to 64 ports)
- Link-level error injection and detection
- Packet routing and switching
- Performance monitoring and statistics

### Bare-Metal Firmware
- Hardware register access
- Link initialization and management
- Error detection and recovery
- Self-test routines
- Real-time operating system integration

### Testbench
- Fault injection framework
- Stress testing capabilities
- Traffic pattern generation
- Performance analysis
- Reliability measurement

## Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- SystemC community for the TLM modeling framework
- ARM architecture documentation
- Real-time operating system design patterns