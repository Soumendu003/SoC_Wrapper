#ifndef HYPERGRAPH_H
#define HYPERGRAPH_H

#include<vector>
#include<map>
#include<string>
#include<iostream>
#include"HyperGraph_Exception.h"

using namespace std;

struct node{
    uint32_t id;
    uint32_t degree;                                // no of Nets it is connected with
    uint64_t adjacent_node_ptr;
    uint64_t connected_net_ptr;
};

struct net{
    uint32_t id;
    uint32_t degree;                               // no of Nodes it is connected with                   
    uint64_t connected_node_ptr;
};

struct adjacency_list{
    uint32_t id;
    uint32_t common_net_count;                    // Total number of common nets
    uint64_t next;
};

struct adjacent_node{
    uint32_t id;
    uint32_t common_net_count;
};

struct connectivity_list{
    uint32_t id;
    uint64_t next;                             // next = 0 means , invalid next element
};

class HyperGraph{
    private:
        vector<node> *Nodes = 0;
        vector<net> *Nets = 0;
        vector<connectivity_list> *Conn_list = 0;
        vector<adjacency_list> *Adj_list = 0;
        map<string,uint32_t> name_to_id;
        map<string,uint32_t>::iterator map_it;
        bool freeze;
    public:
        explicit HyperGraph();
        ~HyperGraph();
        HyperGraph(uint32_t tot_node, uint32_t tot_net);
        uint32_t add_node(string name);
        uint32_t add_net(string name);
        void build_connectivity();
        void connect_net(string net_name, string node_name);
        void connect_net(uint32_t net_id, string node_name);
        void connect_net(uint32_t net_id, uint32_t node_id);
        const vector<uint32_t> &get_connected_nodes_of(uint32_t net_id);
        const vector<uint32_t> &get_connected_nets_of(uint32_t node_id);
        const vector<adjacent_node> &get_adjacent_nodes_of(uint32_t node_id);
        uint32_t get_degree_of_net(uint32_t net_id){
            return Nets->at(net_id).degree;
        }
        uint32_t get_Nets_Size(){
            return Nets->size();
        }
        uint32_t get_Nodes_Size(){
            return Nodes->size();
        }
    private:
        uint64_t insert_connection(uint64_t anchor_index, uint32_t connection_id);
        const vector<uint32_t> &get_connected_nodes(uint64_t anchor_index);  
        const vector<adjacent_node> &get_adjacent_nodes(uint64_t anchor_index);
        void build_node_adjacency(uint32_t node_u, uint32_t node_v);
        void build_node_adjacency(uint32_t node_u, uint32_t node_v, uint32_t common_net_count);
};

#endif