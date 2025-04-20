#ifndef NETWORK_H
#define NETWORK_H

#include <vector>
#include <iostream>
#include "Packet.h"
#include "Client.h"

using namespace std;

class Network {
public:
    Network();
    ~Network();

    // Executes commands given as a vector of strings while utilizing the remaining arguments.
    void process_commands(vector<Client> &clients, vector<string> &commands, int message_limit, const string &sender_port,
                     const string &receiver_port);

    // Initialize the network from the input files.
    vector<Client> read_clients(string const &filename);
    void read_routing_tables(vector<Client> & clients, string const &filename);
    vector<string> read_commands(const string &filename);
    Client creating_Client(const string line);
    int returned_the_position_of_that_client(vector<Client> clients,string client_name);
    stack<Packet*> creating_stacks_of_client_message(vector<Client> clients,int client_sender_line,string message,
                                                     int client_receiver_line,const string &sender_port,
                                                     const string &receiver_port,int frame_count);

    int checking_how_many_queues_have_exist_out(vector<Client> clients,int client_index);
    int checking_how_many_queues_have_exist_in(vector<Client> clients,int client_index);
    void printing_all_frames(vector<Client> clients,int sender_index);
    void printing_and_checking_related_frame(vector<Client> clients,int sender_index,bool checker_out_or_in,int checking_frame);
    void sending_message_to_every_client(vector<Client> &clients);
    void receiving_message_to_every_client(vector<Client> &clients);
    int finding_index_of_specified_mac(vector<Client> clients,string client_mac);
    void printing_logs_of_that_client(vector<Client> &clients,int printed_client_index);
    bool is_there_a_way_of_receiver_id(vector<Client> clients,Client sender_client,string receiver_id);
    std::string converting_mac_to_id(vector<Client> clients,std::string mac_adress);
    std::string converting_id_to_mac(vector<Client> clients,std::string id);
};

#endif  // NETWORK_H
