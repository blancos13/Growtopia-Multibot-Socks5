#include "pch.h"
#include <iostream>
#include "enet/include/enet.h"

using namespace std;

string host = "213.179.209.168";
uint32_t port = 17199;

ENetHost* client;
ENetPeer* peer;

void sendLog(string s)
{
    cout << s << endl;
}

void eventLoop()
{
    ENetEvent event;
    while (enet_host_service(client, &event, 1) > 0)
    {
        peer = event.peer;
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
            sendLog("connect!");
            break;
        case ENET_EVENT_TYPE_RECEIVE:
            if (event.packet)
            {
                sendLog("receive!");
                enet_packet_destroy(event.packet);
                enet_host_flush(client);
            }
            break;
        case ENET_EVENT_TYPE_DISCONNECT:
            sendLog("disconnect!");
            break;
        default:
            break;
        }
    }
}

int main()
{
    if (enet_initialize() != 0)
    {
        fprintf(stderr, "An error occurred while initializing ENet.\n");
        return EXIT_FAILURE;
    }

    // Setup client
    client = enet_host_create(0, 10, 0, 0, 0);
    if (client == NULL)
    {
        sendLog("An error occurred while trying to create an ENet client host.");
    }

    client->checksum = enet_crc32;
    client->usingSocks5 = false;
    client->usingNewPacket = true;
    enet_host_compress_with_range_coder(client);

    ENetAddress address;
    enet_address_set_host(&address, host.c_str());
    address.port = port;

    // Connect
    peer = enet_host_connect(client, &address, NULL, 10, 0);
    if (peer == NULL)
    {
        sendLog("No available peers for initiating an ENet connection.");
    }
    sendLog("connecting...");
    enet_host_flush(client);
    while (true)
    {
        eventLoop();
    }
}