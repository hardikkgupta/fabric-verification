#!/usr/bin/env python3

import random
import numpy as np
from dataclasses import dataclass
from typing import List, Tuple, Dict
import time
import logging

@dataclass
class TestConfig:
    num_routers: int
    num_packets: int
    error_rate: float
    stress_level: float  # 0.0 to 1.0
    test_duration: int   # seconds

class FaultInjector:
    def __init__(self, config: TestConfig):
        self.config = config
        self.logger = self._setup_logger()
        self.stats = {
            'total_packets': 0,
            'error_packets': 0,
            'latency': [],
            'throughput': []
        }
        
    def _setup_logger(self) -> logging.Logger:
        logger = logging.getLogger('FaultInjector')
        logger.setLevel(logging.INFO)
        handler = logging.StreamHandler()
        formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
        handler.setFormatter(formatter)
        logger.addHandler(handler)
        return logger
    
    def generate_traffic_pattern(self) -> List[Tuple[int, int]]:
        """Generate a traffic pattern based on stress level."""
        pattern = []
        for _ in range(self.config.num_packets):
            src = random.randint(0, self.config.num_routers - 1)
            dst = random.randint(0, self.config.num_routers - 1)
            while dst == src:  # Ensure source and destination are different
                dst = random.randint(0, self.config.num_routers - 1)
            pattern.append((src, dst))
        return pattern
    
    def inject_faults(self, packet: bytes) -> bytes:
        """Inject random bit errors into the packet."""
        if random.random() < self.config.error_rate:
            # Convert to bytearray for mutation
            packet_array = bytearray(packet)
            # Flip random bits
            num_errors = random.randint(1, 3)  # Inject 1-3 bit errors
            for _ in range(num_errors):
                byte_idx = random.randint(0, len(packet_array) - 1)
                bit_idx = random.randint(0, 7)
                packet_array[byte_idx] ^= (1 << bit_idx)
            self.stats['error_packets'] += 1
            return bytes(packet_array)
        return packet
    
    def stress_test(self):
        """Run stress test with fault injection."""
        self.logger.info("Starting stress test...")
        start_time = time.time()
        
        while time.time() - start_time < self.config.test_duration:
            # Generate traffic pattern
            pattern = self.generate_traffic_pattern()
            
            # Simulate packet transmission
            for src, dst in pattern:
                # Generate random packet data
                packet_data = bytes(random.getrandbits(8) for _ in range(64))
                
                # Inject faults
                corrupted_packet = self.inject_faults(packet_data)
                
                # Simulate transmission delay
                delay = random.uniform(0.1, 1.0) * (1.0 + self.config.stress_level)
                time.sleep(delay)
                
                # Update statistics
                self.stats['total_packets'] += 1
                self.stats['latency'].append(delay)
                
                # Calculate throughput (packets per second)
                elapsed = time.time() - start_time
                if elapsed > 0:
                    self.stats['throughput'].append(self.stats['total_packets'] / elapsed)
        
        self._print_statistics()
    
    def _print_statistics(self):
        """Print test statistics."""
        reliability = (1.0 - self.stats['error_packets'] / self.stats['total_packets']) * 100
        avg_latency = np.mean(self.stats['latency'])
        avg_throughput = np.mean(self.stats['throughput'])
        
        self.logger.info("\nTest Statistics:")
        self.logger.info(f"Total Packets: {self.stats['total_packets']}")
        self.logger.info(f"Error Packets: {self.stats['error_packets']}")
        self.logger.info(f"Reliability: {reliability:.2f}%")
        self.logger.info(f"Average Latency: {avg_latency:.3f} seconds")
        self.logger.info(f"Average Throughput: {avg_throughput:.2f} packets/second")

def main():
    # Test configuration
    config = TestConfig(
        num_routers=64,
        num_packets=1000,
        error_rate=0.001,  # 0.1% error rate
        stress_level=0.5,  # Moderate stress
        test_duration=60   # 1 minute test
    )
    
    # Create and run fault injector
    injector = FaultInjector(config)
    injector.stress_test()

if __name__ == "__main__":
    main() 