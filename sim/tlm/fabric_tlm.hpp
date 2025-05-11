#pragma once

#include <systemc>
#include <tlm>
#include <vector>
#include <queue>
#include <memory>
#include <random>

namespace fabric {

// Forward declarations
class Router;
class Link;
class Packet;

// Constants
constexpr int MAX_RADIX = 64;
constexpr int PACKET_SIZE = 64;  // bytes
constexpr int LINK_WIDTH = 16;   // bits

// Packet structure
struct Packet {
    uint64_t src_id;
    uint64_t dst_id;
    uint64_t timestamp;
    std::vector<uint8_t> payload;
    bool is_control;
    
    Packet(uint64_t src, uint64_t dst, bool control = false)
        : src_id(src), dst_id(dst), timestamp(0), is_control(control) {
        payload.resize(PACKET_SIZE);
    }
};

// Link class representing a physical connection between routers
class Link : public sc_core::sc_module {
public:
    sc_core::sc_in<bool> clk;
    sc_core::sc_in<bool> rst_n;
    
    // TLM sockets
    tlm::tlm_initiator_socket<> init_socket;
    tlm::tlm_target_socket<> target_socket;
    
    // Link state
    bool is_active;
    double error_rate;
    uint64_t error_count;
    uint64_t packet_count;
    
    SC_HAS_PROCESS(Link);
    Link(sc_core::sc_module_name name, double err_rate = 0.0);
    
    // Link methods
    void reset();
    bool inject_error();
    void update_statistics(bool error);
    
private:
    std::mt19937 rng;
    std::uniform_real_distribution<double> error_dist;
};

// Router class implementing the high-radix switch
class Router : public sc_core::sc_module {
public:
    sc_core::sc_in<bool> clk;
    sc_core::sc_in<bool> rst_n;
    
    // Router configuration
    int radix;
    std::vector<std::unique_ptr<Link>> links;
    
    // Router state
    std::vector<std::queue<Packet>> input_queues;
    std::vector<std::queue<Packet>> output_queues;
    
    SC_HAS_PROCESS(Router);
    Router(sc_core::sc_module_name name, int radix = MAX_RADIX);
    
    // Router methods
    void reset();
    void route_packet(Packet& packet);
    void process_queues();
    
private:
    void routing_logic();
    void switch_fabric();
};

// Top-level fabric model
class Fabric : public sc_core::sc_module {
public:
    sc_core::sc_in<bool> clk;
    sc_core::sc_in<bool> rst_n;
    
    // Fabric configuration
    int num_routers;
    std::vector<std::unique_ptr<Router>> routers;
    
    SC_HAS_PROCESS(Fabric);
    Fabric(sc_core::sc_module_name name, int num_routers);
    
    // Fabric methods
    void reset();
    void inject_packet(uint64_t src, uint64_t dst, const std::vector<uint8_t>& data);
    void get_statistics();
    
private:
    void initialize_network();
};

} // namespace fabric 