#include "fabric_tlm.hpp"
#include <iostream>
#include <algorithm>

namespace fabric {

// Link implementation
Link::Link(sc_core::sc_module_name name, double err_rate)
    : sc_module(name)
    , error_rate(err_rate)
    , is_active(false)
    , error_count(0)
    , packet_count(0)
    , rng(std::random_device{}())
    , error_dist(0.0, 1.0)
{
    SC_METHOD(reset);
    sensitive << rst_n.neg();
    
    init_socket.register_b_transport(this, &Link::b_transport);
}

void Link::reset() {
    is_active = false;
    error_count = 0;
    packet_count = 0;
}

bool Link::inject_error() {
    return error_dist(rng) < error_rate;
}

void Link::update_statistics(bool error) {
    packet_count++;
    if (error) error_count++;
}

// Router implementation
Router::Router(sc_core::sc_module_name name, int radix)
    : sc_module(name)
    , radix(radix)
{
    SC_METHOD(reset);
    sensitive << rst_n.neg();
    
    SC_METHOD(routing_logic);
    sensitive << clk.pos();
    
    SC_METHOD(switch_fabric);
    sensitive << clk.pos();
    
    // Initialize queues
    input_queues.resize(radix);
    output_queues.resize(radix);
    
    // Create links
    for (int i = 0; i < radix; i++) {
        links.push_back(std::make_unique<Link>(("link_" + std::to_string(i)).c_str()));
    }
}

void Router::reset() {
    for (auto& queue : input_queues) {
        while (!queue.empty()) queue.pop();
    }
    for (auto& queue : output_queues) {
        while (!queue.empty()) queue.pop();
    }
}

void Router::route_packet(Packet& packet) {
    // Simple dimension-order routing
    uint64_t current = packet.src_id;
    uint64_t target = packet.dst_id;
    
    // Calculate next hop
    int next_port = (target > current) ? 1 : 0;
    output_queues[next_port].push(packet);
}

void Router::routing_logic() {
    // Process input queues
    for (int i = 0; i < radix; i++) {
        if (!input_queues[i].empty()) {
            Packet packet = input_queues[i].front();
            input_queues[i].pop();
            route_packet(packet);
        }
    }
}

void Router::switch_fabric() {
    // Process output queues
    for (int i = 0; i < radix; i++) {
        if (!output_queues[i].empty()) {
            Packet packet = output_queues[i].front();
            output_queues[i].pop();
            
            // Send packet through link
            if (links[i]->is_active) {
                tlm::tlm_generic_payload trans;
                trans.set_data_ptr(packet.payload.data());
                trans.set_data_length(packet.payload.size());
                
                sc_core::sc_time delay = sc_core::SC_ZERO_TIME;
                links[i]->init_socket->b_transport(trans, delay);
            }
        }
    }
}

// Fabric implementation
Fabric::Fabric(sc_core::sc_module_name name, int num_routers)
    : sc_module(name)
    , num_routers(num_routers)
{
    SC_METHOD(reset);
    sensitive << rst_n.neg();
    
    initialize_network();
}

void Fabric::reset() {
    for (auto& router : routers) {
        router->reset();
    }
}

void Fabric::inject_packet(uint64_t src, uint64_t dst, const std::vector<uint8_t>& data) {
    if (src >= num_routers || dst >= num_routers) {
        std::cerr << "Invalid source or destination router ID" << std::endl;
        return;
    }
    
    Packet packet(src, dst);
    std::copy(data.begin(), data.end(), packet.payload.begin());
    
    // Inject into source router
    routers[src]->input_queues[0].push(packet);
}

void Fabric::get_statistics() {
    uint64_t total_packets = 0;
    uint64_t total_errors = 0;
    
    for (auto& router : routers) {
        for (auto& link : router->links) {
            total_packets += link->packet_count;
            total_errors += link->error_count;
        }
    }
    
    double reliability = (total_packets > 0) ? 
        (1.0 - static_cast<double>(total_errors) / total_packets) * 100.0 : 0.0;
    
    std::cout << "Fabric Statistics:" << std::endl;
    std::cout << "Total Packets: " << total_packets << std::endl;
    std::cout << "Total Errors: " << total_errors << std::endl;
    std::cout << "Reliability: " << reliability << "%" << std::endl;
}

void Fabric::initialize_network() {
    // Create routers
    for (int i = 0; i < num_routers; i++) {
        routers.push_back(std::make_unique<Router>(("router_" + std::to_string(i)).c_str()));
    }
    
    // Connect routers in a mesh topology
    for (int i = 0; i < num_routers; i++) {
        for (int j = 0; j < num_routers; j++) {
            if (i != j) {
                // Connect router i to router j
                routers[i]->links[j]->init_socket.bind(routers[j]->links[i]->target_socket);
            }
        }
    }
}

} // namespace fabric 