#include"HyperGraph.h"

HyperGraph::HyperGraph(){
    Nodes = new vector<node>;
    Nets = new vector<net>;
    Conn_list = new vector<connectivity_list>;
    Adj_list = new vector<adjacency_list>;
    freeze = false;
    connectivity_list tem;
    tem.id = -1;
    tem.next = 0;
    Conn_list->push_back(tem);
    adjacency_list tem1;
    tem1.id = 0;
    tem1.common_net_count = 0;
    tem1.next = 0;
    Adj_list->push_back(tem1);
}

HyperGraph::HyperGraph(uint32_t tot_node, uint32_t tot_net){
    Nodes = new vector<node>;
    Nets = new vector<net>;
    Conn_list = new vector<connectivity_list>;
    Adj_list = new vector<adjacency_list>;
    freeze = false;
    connectivity_list tem;
    tem.id = -1;
    tem.next = 0;
    Conn_list->push_back(tem);
    adjacency_list tem1;
    tem1.id = 0;
    tem1.common_net_count = 0;
    tem1.next = 0;
    Adj_list->push_back(tem1);
    Nodes->reserve(tot_node);
    Nets->reserve(tot_net);
}

HyperGraph::~HyperGraph(){
    delete Nodes;
    delete Nets;
    delete Conn_list;
    delete Adj_list;
}

uint32_t HyperGraph::add_node(string name){
    if(freeze){
        throw HyperGraph_Exception("Hypergraph already freezed. Can't add any node");
    }
    uint32_t ret = Nodes->size();
    map_it = name_to_id.find(name);
    if(map_it != name_to_id.end()){
        throw HyperGraph_Exception(("Same name instance is already in HyperGraph. Given node name is : "+name).c_str());
    }
    name_to_id.insert(pair<string, uint32_t>(name, ret));
    node tem;
    tem.id = ret;
    tem.degree = 0;
    tem.connected_net_ptr = 0;
    tem.adjacent_node_ptr = 0;
    Nodes->push_back(tem);
    return ret;
}

uint32_t HyperGraph::add_net(string name){
    if(freeze){
        throw HyperGraph_Exception("Hypergraph already freezed. Can't add any net");
    }
    uint32_t ret = Nets->size();
    map_it = name_to_id.find(name);
    if(map_it != name_to_id.end()){
        throw HyperGraph_Exception(("Same name instance is already in HyperGraph. Given net name is : "+name).c_str());
    }
    name_to_id.insert(pair<string, uint32_t>(name, ret));
    net tem;
    tem.id = ret;
    tem.degree = 0;
    tem.connected_node_ptr = 0;
    Nets->push_back(tem);
    return ret;
}

void HyperGraph::connect_net(string net_name, string node_name){
    if(freeze){
        throw HyperGraph_Exception("Hypergraph already freezed. Can't connect any net");
    }

    map_it = name_to_id.find(net_name);
    if(map_it == name_to_id.end()){
        throw HyperGraph_Exception(("net name not found. net name = "+net_name).c_str());
    }
    uint32_t net_id = map_it->second;

    map_it = name_to_id.find(node_name);
    if(map_it == name_to_id.end()){
        throw HyperGraph_Exception(("node name not found. node name = "+node_name).c_str());
    }
    uint32_t node_id = map_it->second;
    connect_net(net_id, node_id);
    
}

void HyperGraph::connect_net(uint32_t net_id, string node_name){
    if(freeze){
        throw HyperGraph_Exception("Hypergraph already freezed. Can't connect any net");
    }
    map_it = name_to_id.find(node_name);
    if(map_it == name_to_id.end()){
        throw HyperGraph_Exception(("node name not found. node name = "+node_name).c_str());
    }
    uint32_t node_id = map_it->second;
    connect_net(net_id, node_id);
}

void HyperGraph::connect_net(uint32_t net_id, uint32_t node_id){
    if(freeze){
        throw HyperGraph_Exception("Hypergraph already freezed. Can't connect any net");
    }
    Nets->at(net_id).connected_node_ptr = insert_connection(Nets->at(net_id).connected_node_ptr, node_id);
}

uint64_t HyperGraph::insert_connection(uint64_t anchor_index, uint32_t connection_id){
    uint64_t tem_ptr = anchor_index;
    while (tem_ptr != 0){
        if(Conn_list->at(tem_ptr).id == connection_id){
            return anchor_index;
        }
        tem_ptr = Conn_list->at(tem_ptr).next;
    }
    uint64_t ret = Conn_list->size();
    connectivity_list tem;
    tem.id = connection_id;
    tem.next = anchor_index;
    Conn_list->push_back(tem);
    return ret;
}

const vector<uint32_t>& HyperGraph::get_connected_nodes(uint64_t anchor_index){
    static vector<uint32_t> connected_nodes;
    connected_nodes.clear();
    uint64_t tem = anchor_index;
    while (tem != 0)
    {
        //cout<<"Pushing id = "<<to_string(Conn_list->at(tem).id)<<endl;
        connected_nodes.push_back(Conn_list->at(tem).id);
        tem = Conn_list->at(tem).next;
    }
    return connected_nodes;
}
const vector<uint32_t>& HyperGraph::get_connected_nodes_of(uint32_t net_id){
    return get_connected_nodes(Nets->at(net_id).connected_node_ptr);
}

const vector<uint32_t>& HyperGraph::get_connected_nets_of(uint32_t node_id){
    return get_connected_nodes(Nodes->at(node_id).connected_net_ptr);
}

const vector<adjacent_node>& HyperGraph::get_adjacent_nodes_of(uint32_t node_id){
    return get_adjacent_nodes(Nodes->at(node_id).adjacent_node_ptr);
}

const vector<adjacent_node>& HyperGraph::get_adjacent_nodes(uint64_t anchor_index){
    static vector<adjacent_node> Adj_nodes;
    Adj_nodes.clear();
    uint64_t tem = anchor_index;
    while (tem != 0)
    {
        adjacent_node tem_adj;
        tem_adj.id = Adj_list->at(tem).id;
        tem_adj.common_net_count = Adj_list->at(tem).common_net_count;
        Adj_nodes.push_back(tem_adj);
        tem = Adj_list->at(tem).next;
    }
    return Adj_nodes;
}

void HyperGraph::build_node_adjacency(uint32_t node_u, uint32_t node_v){
    uint64_t tem_ptr = Nodes->at(node_u).adjacent_node_ptr;

    while (tem_ptr != 0)
    {
        if(Adj_list->at(tem_ptr).id == node_v){
            Adj_list->at(tem_ptr).common_net_count += 1;
            return;
        }
        tem_ptr = Adj_list->at(tem_ptr).next;
    }

    tem_ptr = Nodes->at(node_u).adjacent_node_ptr;
    adjacency_list tem;
    tem.id = node_v;
    tem.common_net_count = 1;
    tem.next = tem_ptr;

    tem_ptr = Adj_list->size();
    Adj_list->push_back(tem);
    Nodes->at(node_u).adjacent_node_ptr = tem_ptr;
}

void HyperGraph::build_node_adjacency(uint32_t node_u, uint32_t node_v, uint32_t common_net_count){
    uint64_t tem_ptr = Nodes->at(node_u).adjacent_node_ptr;

    tem_ptr = Nodes->at(node_u).adjacent_node_ptr;
    adjacency_list tem;
    tem.id = node_v;
    tem.common_net_count = common_net_count;
    tem.next = tem_ptr;

    tem_ptr = Adj_list->size();
    Adj_list->push_back(tem);
    Nodes->at(node_u).adjacent_node_ptr = tem_ptr;
}


uint32_t set_intersection_size (const vector<uint32_t>& List1, const vector<uint32_t>& List2)
{
    uint32_t result = 0;

    for(uint32_t i = 0, j = 0; i < List1.size() && j < List2.size(); ){
        if(List1[i] < List2[j]){
            j += 1;
        }
        else if(List1[i] > List2[j]){
            i += 1;
        }
        else{
            result += 1;
            i += 1;
            j += 1;
        }
    }
    return result;
}

void HyperGraph::build_connectivity(){
    freeze = true;
    for(uint32_t i = 0; i < Nets->size(); i++)
    {
        const vector<uint32_t> connected_nodes = get_connected_nodes(Nets->at(i).connected_node_ptr);
        Nets->at(i).degree = connected_nodes.size();
        //cout<<"Connected node size = "<<to_string(connected_nodes.size())<<endl;
        if(connected_nodes.size() > 1){
            for(uint32_t j = 0; j < connected_nodes.size(); j++)
            {
                /*for(uint32_t k = j+1; k < connected_nodes.size(); k++)
                {
                    if(connected_nodes[j] != connected_nodes[k]){
                        build_node_adjacency(connected_nodes[j], connected_nodes[k]);
                        build_node_adjacency(connected_nodes[k], connected_nodes[j]);
                    } 
                }*/

                // Inserts the net id in node's connection list
                Nodes->at(connected_nodes[j]).connected_net_ptr = insert_connection(Nodes->at(connected_nodes[j]).connected_net_ptr, Nets->at(i).id);
                Nodes->at(connected_nodes[j]).degree += 1;
                //cout<<"\tDone for node = "<<to_string(connected_nodes[j])<<endl;
            }
        }
        else if(connected_nodes.size() == 1){
            Nodes->at(connected_nodes[0]).connected_net_ptr = insert_connection(Nodes->at(connected_nodes[0]).connected_net_ptr, Nets->at(i).id);
            Nodes->at(connected_nodes[0]).degree += 1;
        }
        //cout<<"Connectivity built for net = "<<to_string(i)<<endl;
    }

    cout<<"Net pointer insertion done"<<endl;

    /*for(uint32_t i = 0; i < Nodes->size(); i++){
        const vector<uint32_t> connect_nets = get_connected_nets_of(i);
        vector<uint32_t> connect_nets1(connect_nets);

        for(uint32_t j = i + 1; j < Nodes->size(); j ++){
            const vector<uint32_t> connect_nets = get_connected_nets_of(j);
            vector<uint32_t> connect_nets2(connect_nets);

            uint32_t common_net = set_intersection_size(connect_nets1, connect_nets2);
            
            if(common_net > 0){
                build_node_adjacency(i, j, common_net);
                build_node_adjacency(j, i, common_net);
            }
        }
        cout<<"Connectivity buit for node = "<<to_string(i)<<endl;
    }*/

    for(uint32_t i = 0; i < Nodes->size(); i++){
        const vector<uint32_t> connect_nets = get_connected_nets_of(i);
        vector<uint32_t> connect_nets1(connect_nets);
        
        set<uint32_t> Connected_Set;
        Connected_Set.insert(i);

        for(uint32_t j = 0; j < connect_nets1.size(); j++){
            const vector<uint32_t> connected_nodes = get_connected_nodes_of(connect_nets1[j]);
            
            for(uint32_t k = 0; k < connected_nodes.size(); k++){
                if(Connected_Set.find(connected_nodes[k]) == Connected_Set.end()){
                    const vector<uint32_t> connect_nets = get_connected_nets_of(connected_nodes[k]);
                    vector<uint32_t> connect_nets2(connect_nets);

                    uint32_t common_net = set_intersection_size(connect_nets1, connect_nets2);
                    if(common_net > 0){
                        build_node_adjacency(i, connected_nodes[k], common_net);
                    }
                    Connected_Set.insert(connected_nodes[k]);
                }
            }
        }
        //cout<<"New Connectivity buit for node = "<<to_string(i)<<endl;
    }


}


