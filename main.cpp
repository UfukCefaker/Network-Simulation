#include <iostream>
#include "Network.h"

using namespace std;

int main(int argc, char *argv[]) {

    // Instantiate HUBBMNET
    Network* HUBBMNET = new Network();

    // Read from input files
    vector<Client> clients = HUBBMNET->read_clients(argv[1]);
    HUBBMNET->read_routing_tables(clients, argv[2]);
    vector<string> commands = HUBBMNET->read_commands(argv[3]);

    // Get additional parameters from the cmd arguments
    int message_limit = stoi(argv[4]);
    string sender_port = argv[5];
    string receiver_port = argv[6];

    /*
    for(Client client:clients){
        std::cout<<client.client_id<<std::endl;
        std::cout<<client.client_ip<<std::endl;
        std::cout<<client.client_mac<<std::endl;
        for (const auto& entry : client.routing_table) {
            std::cout << "ReceiverID: " << entry.first << " NextHopID: " << entry.second << std::endl;
        }
    }
    */

    // Run the commands
    HUBBMNET->process_commands(clients, commands, message_limit, sender_port, receiver_port);

    // Delete HUBBMNET
    delete HUBBMNET;

    return 0;
}


