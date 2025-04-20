#include "Network.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

Network::Network() {

}

void Network::process_commands(vector<Client> &clients, vector<string> &commands, int message_limit,
                               const string &sender_port, const string &receiver_port) {
    // TODO: Execute the commands given as a vector of strings while utilizing the remaining arguments.
    /* Don't use any static variables, assume this method will be called over and over during testing.
     Don't forget to update the necessary member variables after processing each command. For example,
     after the MESSAGE command, the outgoing queue of the sender must have the expected frames ready to send. */
    for (string command_line:commands) {
        std::istringstream iss(command_line);
        std::string token;
        int counter=0;
        vector<std::string> commands_by_one_by;
        while (iss >> token) {
            commands_by_one_by.push_back(token);
        }
        std::cout<<"----------------------\nCommand: "<<command_line<<"\n----------------------\n";


        if (commands_by_one_by[0] == "MESSAGE"){
            int sender_client = returned_the_position_of_that_client(clients,commands_by_one_by[1]);
            int receiver_client = returned_the_position_of_that_client(clients,commands_by_one_by[2]);

            std::vector<std::string> tokens;
            std::stringstream ss(command_line);
            std::string token;

            while (std::getline(ss, token,'#')) {
                tokens.push_back(token);
            }

            vector<std::string> message_chunks;
            std::string chunk="";
            int counter_for_message=0;
            for (char character:tokens[1]) {
                chunk+= character;
                counter_for_message++;
                if (counter_for_message == message_limit){
                    message_chunks.push_back(chunk);
                    chunk = "";
                    counter_for_message =0;
                }
            }
            if (counter_for_message != 0){
                message_chunks.push_back(chunk);
            }
            std::string final_message ="";

            for (string chunk:message_chunks) {

                stack<Packet*> client_stack = creating_stacks_of_client_message(clients,sender_client,
                                                                                chunk,receiver_client,
                                                                                sender_port,receiver_port,
                                                                                message_chunks.size());
                clients[sender_client].outgoing_queue.push(client_stack);
                final_message+=chunk;

            }

            std::cout<<"Message to be sent: \""<<tokens[1]<<"\""<<std::endl<<std::endl;
            printing_all_frames(clients,sender_client);


            //Log* log_enters = new Log("2023-11-22 20:30:03",final_message,message_chunks.size(),0,commands_by_one_by[1],commands_by_one_by[2],
            //                       true,ActivityType::MESSAGE_SENT);

            Log log_creates("2023-11-22 20:30:03",tokens[1],message_chunks.size(),0,commands_by_one_by[1],commands_by_one_by[2],
                            true,ActivityType::MESSAGE_SENT);

            clients[sender_client].log_entries.push_back(log_creates);


        }
        else if (commands_by_one_by[0] == "SHOW_FRAME_INFO"){
            int checker_index = returned_the_position_of_that_client(clients,commands_by_one_by[1]);
            bool checking_out_or_in = false;
            if(commands_by_one_by[2] == "out"){
                checking_out_or_in = true;
            }
            int checking_frame = std::stoi(commands_by_one_by[3]);

            printing_and_checking_related_frame(clients,checker_index,checking_out_or_in,checking_frame);

            //std::cout<<"Current Frame "<<commands_by_one_by[3]<<" on the outgoing queue of client "<<commands_by_one_by[1]<<std::endl;
            //std::cout<<"Carried Message: \" "<<clients[checker_index].outgoing_queue

        } else if(commands_by_one_by[0] == "SHOW_Q_INFO"){
            int checker_index = returned_the_position_of_that_client(clients,commands_by_one_by[1]);
            if (commands_by_one_by[2] == "out"){
                int frame_number = checking_how_many_queues_have_exist_out(clients,checker_index);
                std::cout<<"Client "<<commands_by_one_by[1]<<" Outgoing Queue Status"<<std::endl;
                std::cout<<"Current total number of frames: "<<frame_number<<std::endl;
                //Client C Outgoing Queue Status
            }else{
                int frame_number = checking_how_many_queues_have_exist_in(clients,checker_index);
                std::cout<<"Client "<<commands_by_one_by[1]<<" Incoming Queue Status"<<std::endl;
                std::cout<<"Current total number of frames: "<<frame_number<<std::endl;
            }
        } else if(commands_by_one_by[0] == "SEND"){
            sending_message_to_every_client(clients);
        } else if(commands_by_one_by[0] == "RECEIVE"){
            receiving_message_to_every_client(clients);
        } else if(commands_by_one_by[0] == "PRINT_LOG"){
            int index_of_pring_log_client = returned_the_position_of_that_client(clients,commands_by_one_by[1]);
            printing_logs_of_that_client(clients,index_of_pring_log_client);
        } else{
            std::cout<<"Invalid command.\n";
        }


    }

}
void Network::printing_logs_of_that_client(vector<Client> &clients,int printed_client_index){
    Client client = clients[printed_client_index];
    int log_counter=1;
    if(client.log_entries.size() != 0){
        std::cout<<"Client "<<client.client_id<<" Logs:\n--------------\n";
    }
    for(Log log:client.log_entries){
        std::cout<<"Log Entry #"<<log_counter<<std::endl;
        if (log.activity_type == ActivityType::MESSAGE_SENT){
            std::cout<<"Activity: "<<"Message Sent"<<std::endl;
        }else if(log.activity_type == ActivityType::MESSAGE_DROPPED){
            std::cout<<"Activity: "<<"Message Dropped"<<std::endl;
        }else if(log.activity_type == ActivityType::MESSAGE_FORWARDED){
            std::cout<<"Activity: "<<"Message Forwarded"<<std::endl;
        }else{
            std::cout<<"Activity: "<<"Message Received"<<std::endl;
        }
        std::cout<<"Timestamp: "<<log.timestamp<<std::endl;
        std::cout<<"Number of frames: "<<log.number_of_frames<<std::endl;
        std::cout<<"Number of hops: "<<log.number_of_hops<<std::endl;
        std::cout<<"Sender ID: "<<log.sender_id<<std::endl;
        std::cout<<"Receiver ID: "<<log.receiver_id<<std::endl;
        if (log.success_status){
            std::cout<<"Success: Yes"<<std::endl;
        }else{
            std::cout<<"Success: No"<<std::endl;
        }
        std::cout<<"Message: \""<<log.message_content<<"\""<<std::endl;
        std::cout<<"--------------"<<std::endl;

        log_counter++;
    }
}
void Network::sending_message_to_every_client(vector<Client> &clients){
    int counter_for_clients=0;
    for(Client& client:clients){
        if (client.outgoing_queue.size() !=0){

            // alt değiştir.
            printing_all_frames(clients,counter_for_clients);
            queue<stack<Packet*>> temp_queue_of_clients;

            while (!client.outgoing_queue.empty()){
                PhysicalLayerPacket* temp_physical;
                NetworkLayerPacket* temp_network;
                TransportLayerPacket* temp_transport;
                ApplicationLayerPacket* temp_app;






                // Burası işleri karıştırıyor.
                client.outgoing_queue.front().top()->hop_number++;
                //




                stack<Packet*> packetStack = client.outgoing_queue.front();
                stack<Packet*> packetStack_temp;
                Packet* packet = packetStack.top();
                temp_physical = dynamic_cast<PhysicalLayerPacket*>(packet);

                int index_of_receiver =finding_index_of_specified_mac(clients,temp_physical->receiver_MAC_address);
                clients[index_of_receiver].incoming_queue.push(packetStack);

                int counter_for_poping=0;
                packetStack.pop();
                while(!packetStack.empty()){
                    Packet* packet1 = packetStack.top();
                    if(counter_for_poping==0){
                        temp_network = dynamic_cast<NetworkLayerPacket*>(packet1);
                    }else if(counter_for_poping==1){
                        temp_transport = dynamic_cast<TransportLayerPacket*>(packet1);
                    }else{
                        temp_app = dynamic_cast<ApplicationLayerPacket*>(packet1);
                    }
                    counter_for_poping++;
                    packetStack.pop();
                }

                packetStack_temp.push(temp_app);
                packetStack_temp.push(temp_transport);
                packetStack_temp.push(temp_network);
                packetStack_temp.push(temp_physical);
                temp_queue_of_clients.push(packetStack_temp);

                client.outgoing_queue.pop();
            }
        }
        counter_for_clients++;
    }
}
void Network::receiving_message_to_every_client(vector<Client> &clients){
    int counter_for_clients=0;
    for(Client& client:clients){
        if (client.incoming_queue.size() !=0){
            queue<stack<Packet*>> temp_queue_of_clients;
            Packet* temp_packet_for_check = client.incoming_queue.front().top();
            int frame_check_counter = temp_packet_for_check->frame_number;
            int frame_reset_counter =1;
            std::string received_message = "";
            while(!client.incoming_queue.empty()){
                PhysicalLayerPacket* temp_physical;
                NetworkLayerPacket* temp_network;
                TransportLayerPacket* temp_transport;
                ApplicationLayerPacket* temp_app;

                stack<Packet*> packetStack = client.incoming_queue.front();
                stack<Packet*> packetStack_temp;
                Packet* packet = packetStack.top();
                temp_physical = dynamic_cast<PhysicalLayerPacket*>(packet);


                int counter_for_poping=0;
                packetStack.pop();
                while(!packetStack.empty()){
                    Packet* packet1 = packetStack.top();
                    if(counter_for_poping==0){
                        temp_network = dynamic_cast<NetworkLayerPacket*>(packet1);
                    }else if(counter_for_poping==1){
                        temp_transport = dynamic_cast<TransportLayerPacket*>(packet1);
                    }else{
                        temp_app = dynamic_cast<ApplicationLayerPacket*>(packet1);
                    }
                    counter_for_poping++;
                    packetStack.pop();
                }

                packetStack_temp.push(temp_app);
                packetStack_temp.push(temp_transport);
                packetStack_temp.push(temp_network);
                packetStack_temp.push(temp_physical);

                if (temp_app->receiver_ID == client.client_id){
                    std::cout<<"Client "<<client.client_id<<" receiving frame #"<<frame_reset_counter<<" from client "<<
                             converting_mac_to_id(clients,temp_physical->sender_MAC_address)<<", originating from client "<<temp_app->sender_ID<<
                             std::endl;

                    std::cout<<"Sender MAC address: "<<temp_physical->sender_MAC_address<<", Receiver MAC address: "<<
                             temp_physical->receiver_MAC_address<<std::endl;
                    std::cout<<"Sender IP address: "<<temp_network->sender_IP_address<<", Receiver IP address: "<<
                             temp_network->receiver_IP_address<<std::endl;
                    std::cout<<"Sender port number: "<<temp_transport->sender_port_number<<", Receiver port number: "<<
                             temp_transport->receiver_port_number<<std::endl;
                    std::cout<<"Sender ID: "<<temp_app->sender_ID<<", Receiver ID: "<<temp_app->receiver_ID<<std::endl;
                    std::cout<<"Message chunk carried: \""<<temp_app->message_data<<"\""<<std::endl;
                    std::cout<<"Number of hops so far: "<<temp_physical->hop_number<<std::endl;
                    std::cout<<"--------"<<std::endl;

                    received_message += temp_app->message_data;
                }else if(!is_there_a_way_of_receiver_id(clients,client,client.routing_table[temp_app->receiver_ID])){
                    std::cout<<"Client "<<client.client_id<<" receiving frame #"<<frame_reset_counter<<" from client "<<
                             converting_mac_to_id(clients,temp_physical->sender_MAC_address)<<", but intended for client "<<
                             temp_app->receiver_ID<<". Forwarding..."<<std::endl;
                    std::cout<<"Error: Unreachable destination. Packets are dropped after "<<temp_physical->hop_number<<
                             " hops!"<<std::endl;
                    received_message += temp_app->message_data;

                }else{
                    if (frame_reset_counter==1){
                        std::cout<<"Client "<<client.client_id<<" receiving a message from client "<<
                                 converting_mac_to_id(clients,temp_physical->sender_MAC_address)<<", but intended for client "<<
                                 temp_app->receiver_ID<<". Forwarding..."<<std::endl;
                    }
                    int sender_index =finding_index_of_specified_mac(clients,temp_physical->receiver_MAC_address);
                    std::string receiver_id = clients[sender_index].routing_table[temp_app->receiver_ID];

                    std::cout<<"Frame #"<<frame_reset_counter<<" MAC address change: New sender MAC "<<
                             temp_physical->receiver_MAC_address<<", new receiver MAC "<<converting_id_to_mac(clients,receiver_id)<<std::endl;
                    packetStack_temp.pop();
                    temp_physical->sender_MAC_address= temp_physical->receiver_MAC_address;
                    temp_physical->receiver_MAC_address = converting_id_to_mac(clients,receiver_id);
                    packetStack_temp.push(temp_physical);
                    client.outgoing_queue.push(packetStack_temp);
                    received_message += temp_app->message_data;
                }

                frame_reset_counter++;

                if (frame_reset_counter == temp_app->frame_number + 1 && temp_app->receiver_ID == client.client_id){
                    frame_reset_counter =1;
                    std::cout<<"Client "<<client.client_id<<" received the message \""<<received_message<<"\" from client "<<
                             temp_app->sender_ID<<std::endl;

                    Log log_creates("2023-11-22 20:30:03",received_message,temp_physical->frame_number,temp_physical->hop_number+1,
                                    temp_app->sender_ID,temp_app->receiver_ID,
                                    true,ActivityType::MESSAGE_RECEIVED);
                    client.log_entries.push_back(log_creates);

                    received_message = "";
                    std::cout<<"--------"<<std::endl;
                }else if (frame_reset_counter == temp_app->frame_number + 1 && !is_there_a_way_of_receiver_id(clients,client,client.routing_table[temp_app->receiver_ID])){
                    std::cout<<"--------"<<std::endl;
                    frame_reset_counter =1;

                    Log log_creates("2023-11-22 20:30:03",received_message,temp_physical->frame_number,temp_physical->hop_number+1,
                                    temp_app->sender_ID,temp_app->receiver_ID,
                                    false,ActivityType::MESSAGE_DROPPED);
                    client.log_entries.push_back(log_creates);
                    received_message = "";
                }

                else if (frame_reset_counter == temp_app->frame_number + 1){
                    std::cout<<"--------"<<std::endl;
                    frame_reset_counter =1;

                    Log log_creates("2023-11-22 20:30:03",received_message,temp_physical->frame_number,temp_physical->hop_number+1,
                                    temp_app->sender_ID,temp_app->receiver_ID,
                                    true,ActivityType::MESSAGE_FORWARDED);
                    client.log_entries.push_back(log_creates);
                    received_message = "";
                }

                client.incoming_queue.pop();
                temp_queue_of_clients.push(packetStack_temp);
            }
        }
        counter_for_clients++;
    }
}
std::string Network::converting_mac_to_id(vector<Client> clients,std::string mac_adress){
    for(Client client:clients){
        if (client.client_mac==mac_adress){
            return client.client_id;
        }
    }
    return "";
}
std::string Network::converting_id_to_mac(vector<Client> clients,std::string id){
    for(Client client:clients){
        if (id == client.client_id){
            return client.client_mac;
        }
    }
    return "";
}
int Network::finding_index_of_specified_mac(vector<Client> clients,string client_mac){
    int counter =0;
    for(Client client:clients){
        if (client_mac == client.client_mac){
            return counter;
        }
        counter++;
    }
    return counter;
}
bool Network::is_there_a_way_of_receiver_id(vector<Client> clients,Client sender_client,string receiver_id){
    bool checker = false;
    for(Client client:clients){
        if (client.client_id == receiver_id){
            checker = true;
        }
    }
    return checker;
}
int Network::returned_the_position_of_that_client(vector<Client> clients,string client_name){
    int counter =0;
    int returned_value = -1;
    for (Client client:clients) {
        if (client.client_id == client_name){
            returned_value = counter;
        }
        counter++;
    }
    return returned_value;
}
int Network::checking_how_many_queues_have_exist_out(vector<Client> clients,int client_index){
    queue<stack<Packet*>> temp = clients[client_index].outgoing_queue;
    int counter = 0;
    while (!temp.empty()){
        counter++;
        temp.pop();
    }
    return counter;
}
int Network::checking_how_many_queues_have_exist_in(vector<Client> clients,int client_index){
    queue<stack<Packet*>> temp = clients[client_index].incoming_queue;
    int counter = 0;
    while (!temp.empty()){
        counter++;
        temp.pop();
    }
    return counter;
}
stack<Packet*> Network::creating_stacks_of_client_message(vector<Client> clients,int client_sender_line,string message,
                                                          int client_receiver_line,const string &sender_port,
                                                          const string &receiver_port,int frame_count){
    std::stack<Packet*> returned_stack;

    string receiver_id = clients[client_receiver_line].client_id;
    string receiver_id_for_mac = clients[client_sender_line].routing_table[receiver_id];
    int index_of_receiver_next = returned_the_position_of_that_client(clients,receiver_id_for_mac);
    string mac_for_next = clients[index_of_receiver_next].client_mac;

    ApplicationLayerPacket* applicationLayerPacket = new ApplicationLayerPacket(0,clients[client_sender_line].client_id,clients[client_receiver_line].client_id,message);
    TransportLayerPacket* transportLayerPacket = new TransportLayerPacket(1,sender_port,receiver_port);
    NetworkLayerPacket* networkLayerPacket = new NetworkLayerPacket(2,clients[client_sender_line].client_ip,clients[client_receiver_line].client_ip);
    PhysicalLayerPacket* physicalLayerPacket = new PhysicalLayerPacket(3,clients[client_sender_line].client_mac,mac_for_next);

    applicationLayerPacket->frame_number = frame_count;
    transportLayerPacket->frame_number = frame_count;
    networkLayerPacket->frame_number = frame_count;
    physicalLayerPacket->frame_number = frame_count;

    applicationLayerPacket->hop_number =0;
    transportLayerPacket->hop_number =0;
    networkLayerPacket->hop_number =0;
    physicalLayerPacket->hop_number =0;

    returned_stack.push(applicationLayerPacket);
    returned_stack.push(transportLayerPacket);
    returned_stack.push(networkLayerPacket);
    returned_stack.push(physicalLayerPacket);

    return returned_stack;
}
void Network::printing_all_frames(vector<Client> clients,int sender_index){
    queue<stack<Packet*>> temp = clients[sender_index].outgoing_queue;
    int counter = 1;
    while (!temp.empty()){
        stack<Packet*> temp_stack = temp.front();
        ApplicationLayerPacket* temp_app;
        NetworkLayerPacket* temp_network;
        PhysicalLayerPacket* temp_physical;
        TransportLayerPacket* temp_transport;
        int counting=0;
        std::cout<<"Frame: #"<<counter<<std::endl;
        while (!temp_stack.empty()){
            Packet* packet = temp_stack.top();
            if (counting ==0){
                temp_physical = dynamic_cast<PhysicalLayerPacket*>(packet);
                std::cout<<"Sender MAC address: "<<temp_physical->sender_MAC_address<<", Receiver MAC address: "<<temp_physical->receiver_MAC_address<<std::endl;
            }else if(counting==1){
                temp_network = dynamic_cast<NetworkLayerPacket*>(packet);
                std::cout<<"Sender IP address: "<<temp_network->sender_IP_address<<", Receiver IP address: "<<temp_network->receiver_IP_address<<std::endl;
            }else if(counting==2){
                temp_transport = dynamic_cast<TransportLayerPacket*>(packet);
                std::cout<<"Sender port number: "<<temp_transport->sender_port_number<<", Receiver port number: "<<temp_transport->receiver_port_number<<std::endl;
            } else{
                temp_app = dynamic_cast<ApplicationLayerPacket*>(packet);
                std::cout<<"Sender ID: "<<temp_app->sender_ID<<", Receiver ID: "<<temp_app->receiver_ID<<std::endl;
                std::cout<<"Message chunk carried: \""<<temp_app->message_data<<"\""<<std::endl;
                std::cout<<"Number of hops so far: "<<"0"<<std::endl;
            }
            counting++;
            temp_stack.pop();
        }
        std::cout<<"--------"<<std::endl;
        counter++;
        temp.pop();
    }
}
void Network::printing_and_checking_related_frame(vector<Client> clients,int sender_index,bool checker_out_or_in,int checking_frame){
    queue<stack<Packet*>> temp;
    if (checker_out_or_in){
        temp = clients[sender_index].outgoing_queue;
    } else{
        temp = clients[sender_index].incoming_queue;
    }
    int counter = 1;
    bool checker = false;
    while (!temp.empty()){
        if (counter == checking_frame){
            checker = true;
            stack<Packet*> temp_stack = temp.front();
            ApplicationLayerPacket* temp_app;
            NetworkLayerPacket* temp_network;
            PhysicalLayerPacket* temp_physical;
            TransportLayerPacket* temp_transport;
            int counting=0;
            if (checker_out_or_in){
                std::cout<<"Current Frame #"<<counter<<" on the outgoing queue of client "<<clients[sender_index].client_id<<std::endl;
            } else{
                std::cout<<"Current Frame #"<<counter<<" on the incoming queue of client "<<clients[sender_index].client_id<<std::endl;
            }

            std::string st1,st2,st3,st4;


            //hop number tanımla onu direkt 0 dedik.


            while (!temp_stack.empty()){
                Packet* packet = temp_stack.top();
                if (counting ==0){
                    temp_physical = dynamic_cast<PhysicalLayerPacket*>(packet);
                    st4 = "Layer 3 info: Sender MAC address: "+temp_physical->sender_MAC_address+", Receiver MAC address: "+
                          temp_physical->receiver_MAC_address+"\nNumber of hops so far: 0";
                    //std::cout<<"Sender MAC address: "<<temp_physical->sender_MAC_address<<", Receiver MAC address: "<<temp_physical->receiver_MAC_address<<std::endl;
                }else if(counting==1){
                    temp_network = dynamic_cast<NetworkLayerPacket*>(packet);
                    st3 = "Layer 2 info: Sender IP address: "+temp_network->sender_IP_address+", Receiver IP address: "+
                          temp_network->receiver_IP_address+"\n";
                    //std::cout<<"Sender IP address: "<<temp_network->sender_IP_address<<", Receiver IP address: "<<temp_network->receiver_IP_address<<std::endl;
                }else if(counting==2){
                    temp_transport = dynamic_cast<TransportLayerPacket*>(packet);
                    st2 = "Layer 1 info: Sender port number: "+temp_transport->sender_port_number+", Receiver port number: "+
                          temp_transport->receiver_port_number+"\n";
                    //std::cout<<"Sender port number: "<<temp_transport->sender_port_number<<", Receiver port number: "<<temp_transport->receiver_port_number<<std::endl;
                } else{
                    temp_app = dynamic_cast<ApplicationLayerPacket*>(packet);
                    st1 = "Carried Message: \""+temp_app->message_data+"\"" + "\nLayer 0 info: Sender ID: "+
                          temp_app->sender_ID+", Receiver ID: "+temp_app->receiver_ID+"\n";
                    //std::cout<<"Sender ID: "<<temp_app->sender_ID<<", Receiver ID: "<<temp_app->receiver_ID<<std::endl;
                    //std::cout<<"Message chunk carried: \""<<temp_app->message_data<<"\""<<std::endl;
                    //std::cout<<"Number of hops so far: "<<"0"<<std::endl;
                }
                counting++;
                temp_stack.pop();
            }
            std::cout<<st1<<st2<<st3<<st4<<std::endl;
        }
        temp.pop();
        counter++;
    }
    if(!checker){
        std::cout<<"No such frame."<<std::endl;
    }
}
vector<Client> Network::read_clients(const string &filename) {
    vector<Client> clients;
    // TODO: Read clients from the given input file and return a vector of Client instances.
    std::ifstream input_file(filename);
    std::string line;
    vector<std::string> all_lines;
    while (std::getline(input_file, line)) {
        all_lines.push_back(line);
    }
    input_file.close();
    int port_numbers = std::stoi(all_lines[0]);

    for (int i = 1; i <= port_numbers; ++i) {
        Client client = creating_Client(all_lines[i]);
        clients.push_back(client);
    }
    return clients;
}
Client Network::creating_Client(const std::string line) {
    std::string client_id;
    std::string client_ip;
    std::string client_mac;

    std::istringstream iss(line);
    std::string token;
    int counter=0;
    while (iss >> token) {
        if (counter ==0){
            client_id = token;
        }else if (counter==1){
            client_ip = token;
        } else{
            client_mac = token;
        }
        counter++;
    }
    Client client(client_id,client_ip,client_mac);

    return client;
}
void Network::read_routing_tables(vector<Client> &clients, const string &filename) {
    // TODO: Read the routing tables from the given input file and populate the clients' routing_table member variable.
    std::ifstream input_file(filename);
    std::string line;
    vector<std::string> lines_by_client;
    int counter=0;
    while (std::getline(input_file, line)) {
        if (line =="-"){
            for(string line_for_temp:lines_by_client){
                std::istringstream iss(line_for_temp);
                std::string token;
                std::string receiverID;
                std::string nexthopID;
                int counter_temp =0;
                while (iss >> token) {
                    if (counter_temp ==0){
                        receiverID = token;
                    } else{
                        nexthopID = token;
                    }
                    counter_temp++;
                }
                clients[counter].routing_table[receiverID] = nexthopID;
            }
            counter++;
            lines_by_client.clear();
        } else{
            lines_by_client.push_back(line);
        }
    }
    for(string line_for_temp:lines_by_client){
        std::istringstream iss(line_for_temp);
        std::string token;
        std::string receiverID;
        std::string nexthopID;
        int counter_temp =0;
        while (iss >> token) {
            if (counter_temp ==0){
                receiverID = token;
            } else{
                nexthopID = token;
            }
            counter_temp++;
        }
        clients[counter].routing_table[receiverID] = nexthopID;
    }
    input_file.close();
}

// Returns a list of token lists for each command
vector<string> Network::read_commands(const string &filename) {
    vector<string> commands;
    // TODO: Read commands from the given input file and return them as a vector of strings.
    std::ifstream input_file(filename);
    std::string line;
    vector<std::string> all_lines;
    while (std::getline(input_file, line)) {
        all_lines.push_back(line);
    }
    input_file.close();
    int command_lines_counter = std::stoi(all_lines[0]);

    for (int i = 0; i < command_lines_counter; ++i) {
        commands.push_back(all_lines[i+1]);
    }

    return commands;
}

Network::~Network() {
    // TODO: Free any dynamically allocated memory if necessary.

}

