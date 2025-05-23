#ifndef PACKET_H
#define PACKET_H

#include <string>
#include <iostream>

using namespace std;

class Packet {
public:
    Packet(int layerID);
    virtual ~Packet();

    int layer_ID;
    int frame_number;
    int hop_number;

    friend ostream &operator<<(ostream &os, const Packet &packet);
    virtual void print() {};
};

#endif // PACKET_H
