#include <string>


#ifndef STRUCT_H_
#define STRUCT_H_



struct TankPacketStruct {
    uint8_t type = 0;
    uint8_t dropped = 0;
	uint8_t padding1 = 0;
	uint8_t padding2 = 0;
    int netid = 0;
    int snetid = 0;
    int state = 0;
    float padding4 = 0;
    int value = 0;
    float x = 0;
	float y = 0;
    float xspeed = 0;
	float yspeed = 0;
    int padding5 = 0;
    int tilex = 0;
	int tiley = 0;
    uint32_t extDataSize = 0;
};

struct InventoryItem {
	short id = 0;
	uint8_t amount = 0;
};

struct WorldObject {
	short id = 0;
	vector2_t pos;
	uint8_t amount = 0;
	uint8_t flags = 0;
	int oid = 0;
};

struct Tile {
	uint16_t fg = 0;
	uint16_t bg = 0;
	bool readyharvest;
	uint16_t data;
	uint8_t flags_1;
	uint8_t flags_2;
};

struct Player {
	std::string name;
	int userid;
	int netid;
	vector2_t pos;
};



#endif