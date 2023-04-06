

#include "Bot_Local.h"
#include "Utils.h"
#include "ItemInfoManager.h"
#include "proton/rtparam.hpp"

// LocalData
using namespace std;
Tile* LocalData::GetTile(int x, int y) {
	int index = x + width * y;
	if (index > -1 && index < tiles.size() && x > -1 && x < width && y > -1 && y < height)
		return &tiles[index];
	return NULL;
}


InventoryItem* LocalData::GetItem(int id) {
	for (int i = 0; i < items.size(); i++) {
		if (items.at(i).id == id) {
			return &items[i];
		}
	}
	return NULL;
}


void LocalData::ExitWorld() {
	name = "";
	netid = 0;
	userid = 0;
	pos = vector2_t(0, 0);
	country = "";
	
	objects.clear();
	tiles.clear();
	players.clear();
	
	worldname = "EXIT";
	width = 0;
	height = 0;
	tilecount = 0;
	
	objectcount = 0;
	last_oid = 0;
	
	scanned_object.clear();
	scanned_tile.clear();
}

void LocalData::Reset() {
	ExitWorld();
	items.clear();
	level = 0;
	gems = 0;
}

void LocalData::InitOnSpawn(std::string spawn) {
	for (auto row : Utils::Split(spawn, "\n")) {
		auto nrow = Utils::Split(row, "|");
		if (nrow.size() < 2)
			continue;
		if (nrow[0] == "netID")
			netid = std::stoi(nrow[1]);
		else if (nrow[0] == "userID")
			userid = std::stoi(nrow[1]);
		else if (nrow[0] == "posXY")
			pos = vector2_t((float)std::stoi(nrow[1]), (float)std::stoi(nrow[2]));
		else if (nrow[0] == "name")
			name = nrow[1];
		else if (nrow[0] == "country")
			country = nrow[1];
	}
}
rtvar rtpar;
void LocalData::PlayerSpawn(std::string spawn) {
	rtpar = rtvar::parse(spawn);
	Player player;
	player.name = rtpar.get("name");
	player.netid = rtpar.get_int("netID");
	player.userid = rtpar.get_int("userID");
	auto pos = rtpar.find("posXY");
	if (pos && pos->m_values.size() >= 2) {
		auto x = atoi(pos->m_values[0].c_str());
		auto y = atoi(pos->m_values[1].c_str());
		player.pos.m_x = x;
		player.pos.m_y = y;
	}
	players.push_back(player);
}

rtvar rtpur;
void LocalData::PlayerRemove(std::string remove) {
	rtpur = rtvar::parse(remove);
	Player player;
	int netidd = rtpar.get_int("netID");

	for (int i = 0; i < players.size(); i++) {
		if (players.at(i).netid == netidd) {
			players.erase(players.begin() + i);
		}
	}
}
int LocalData::PlayerTotal() {
	return players.size();
}

void LocalData::Serialize9(uint8_t* ptr) {
	items.clear();
	ptr += 56;
	ptr += 5;
	uint8_t size = *(ptr);
	
	for (uint8_t i = 0; i < size; i++) {
		InventoryItem item;
		ptr += 2;
		item.id = *(short*)(ptr);
		ptr += 2;
		item.amount = *(ptr);
		
		items.push_back(item);
	}
}

void LocalData::Serialize4(uint8_t* ptr) {
	objects.clear();
	tiles.clear();
	players.clear();
	
	uint8_t* extended = ptr + *(uint32_t*)(ptr + 52) + 40;
	int estimate = *(int*)(extended);
	
	for (int i = 0; i
		< estimate + 1; i++) {
		if (*(extended - i*16 - 2) == 0) {
			objectcount = *(int*)(extended - i*16 - 4);
			last_oid = *(int*)(extended - i*16);
#ifdef _CONSOLE
			std::cout << "object count: " << objectcount << ", oid: " << last_oid << std::endl;
#endif
			break;
		}
		
		WorldObject obj;
		obj.id = *(short*)(extended - i*16 - 12);
		memcpy(&obj.pos.m_x, extended - i*16 - 10, 4);
		memcpy(&obj.pos.m_y, extended - i*16 - 6, 4);
		obj.amount = *(extended - i*16 - 2);
		obj.flags = *(extended - i*16 - 1);
		obj.oid = *(int*)(extended - i*16);
		objects.push_back(obj);
	}
	
	extended = ptr + 62;
	
	short len = *(short*)(extended);
	extended += 2;
	
	worldname = "";
	for (int i = 0; i < len; i++)
		worldname += *(char*)(extended + i);
	
	extended += len;
	
	width = *(int*)(extended);
	extended += 4;
	
	height = *(int*)(extended);
	extended += 4;
	
	tilecount = *(int*)(extended);
	extended += 4;

#ifdef _CONSOLE
	std::cout << "name: " << worldname << ", width: " << width << ", height: " << height << ", tile count: " << tilecount << std::endl;
#endif
	if (!itemDefs->Instance)
		return;
	
	for (int index = 0; index < tilecount; index ++) {
		if (extended - ptr < 50) {
#ifdef _CONSOLE
			std::cout << "failed serialize" << std::endl;
#endif
			tiles.clear();
			return;
		}
		Tile tile;
		memcpy(&tile.fg, extended, 2);
		extended += 2;
		memcpy(&tile.bg, extended, 2);
		memcpy(&tile.data, extended + 4, 2);
		memcpy(&tile.flags_1, extended + 6, 1);
		memcpy(&tile.flags_2, extended + 7, 1);
		extended += 2;
		
		tiles.push_back(tile);
		
		short flags = *(short*)(extended);
		if (flags != 0)
			extended += 2;
		
		extended += 4;
		
		if (itemDefs->IncludeExtra(tile.fg)) {
			uint8_t extratype = extended[0];
			extended ++;
			switch (extratype) {
				case 0x1: { // door
					short len = *(short*)extended;
					extended += 2;
					extended += len;
					extended += 1; // locked
				} break;
				case 0x2: { // sign
					short len = *(short*)extended;
					extended += 2;
					extended += len;
					extended += 4;
				} break;
				case 0x3: { // lock
					extended ++;
					extended += 4; // owner uid
					int admincount = *(int*)extended;
					extended += 4;
					extended += 4 * admincount; // list of admin uid
					extended += 8;
				} break;
				case 0x4: { // seed
					 int growth = *(int*)(extended);
					 uint8_t fruit = *(extended + 4);
					extended += 5;
				} break;
				case 0x8: {
					extended ++;
				} break;
				case 0x9: {
					extended += 4;
				} break;
				case 0xA: {
					extended += 5;
				} break;
				case 0xB: {
					extended += 4;
					short len = *(short*)extended;
					extended += 2;
					extended += len;
				} break;
				case 0xE: {
					short len = *(short*)extended;
					extended += 2;
					extended += len;
					extended ++;
					extended += 4;
					extended += 18;
					/*
					extended += 6;
					short shoes = *(short*)extended;
					extended += 8;
					short hair = *(short*)extended;
					extended += 4;
					if (shoes == 10044)
						extended += 8;
					if (hair == 5712)
						extended += 8;
					*/
				} break;
				case 0xF: {
					extended ++;
				} break;
				case 0x10: {
					extended ++;
				} break;
				case 0x12: {
					extended += 5;
				} break;
				case 0x13: {
					extended += 18;
				} break;
				case 0x14: {
					short len = *(short*)extended;
					extended += 2;
					extended += len;
				} break;
				case 0x15: {
					short len = *(short*)extended;
					extended += 2;
					extended += len;
					extended += 5;
				} break;
				case 0x17: {
					extended += 4;
				} break;
				case 0x18: {
					extended += 8;
				} break;
				case 0x19: {
					extended ++;
					int smth = *(int*)extended;
					extended += 4;
					extended += 4 * smth;
				} break;
				case 0x1B: {
					extended += 4;
				} break;
				case 0x1C: {
					extended += 6;
				} break;
				case 0x20: {
					extended += 4;
				} break;
				case 0x21: {
					if (tile.fg == 3394) {
						short len = *(short*)extended;
						extended += 2;
						extended += len;
					}
				} break;
				case 0x23: {
					extended += 4;
					short len = *(short*)extended;
					extended += 2;
					extended += len;
				} break;
				case 0x25: {
					short len = *(short*)extended;
					extended += 2;
					extended += len;
					extended += 32;
				} break;
				case 0x27: {
					extended += 4;
				} break;
				case 0x28: {
					extended += 4;
				} break;
				case 0x2A: {
					extended += 21;
				} break;
				case 0x2B: {
					extended += 16;
				} break;
				case 0x2C: {
					extended ++;
					extended += 4;
					int admincount = *(int*)extended;
					extended += 4;
					extended += 4 * admincount;
				} break;
				case 0x2F: {
					short len = *(short*)extended;
					extended += 2;
					extended += len;
					extended += 5;
				} break;
				case 0x30: {
					short len = *(short*)extended;
					extended += 2;
					extended += len;
					extended += 26;
				} break;
				case 0x31: {
					extended += 9;
				} break;
				case 0x32: {
					extended += 4;
				} break;
				case 0x34: {
					// Trickster
				} break;
				case 0x36: {
					short len = *(short*)extended;
					extended += 2;
					extended += len;
				} break;
				case 0x37: {
					extended += 20;
				} break;
				case 0x38: {
					short len = *(short*)extended;
					extended += 2;
					extended += len;
					extended += 4;
				} break;
				case 0x39: {
					extended += 4;
				} break;
				case 0x3A: {
					
				} break;
				case 0x3E: {
					extended += 14;
				} break;
				case 0x3F: {
					int r = *(int*)extended;
					extended += 4;
					extended += 15 * r;
					extended += 8;
				} break;
				case 0x41: {
					extended += 17;
				} break;
				case 0x42: {
					extended ++;
				} break;
				case 0x49: {
					extended += 4;
				} break;
				case 0x4A: {
					// Safe Vault
				} break;
				default: {
#ifdef _CONSOLE
					std::cout << "unhandled type " << (int)extratype << " index: " << index << ", item id: " << tile.fg << std::endl;
					std::cout << "preview 40 first byte: ";
					for (int i = 0; i < 40; i++)
						std::cout << "+" << i << ":" << (int)*(uint8_t*)(extended + i) << "|";
					std::cout << std::endl;
#endif
				} break;
			}
		}
	}
}



void LocalData::Update3(TankPacketStruct* packet) {
	if (packet->netid == netid && packet->value != 18) {
		for (int i = 0; i < items.size(); i++) {
			if (items[i].id == packet->value) {
				items[i].amount --;
				if (items[i].amount < 1 || items[i].amount > 200)
					items.erase(items.begin() + i);
				break;
			}
		}
	}
	
	Tile* tile = GetTile(packet->tilex, packet->tiley);
	if (tile) {
		if (packet->value == 18) {
			if (tile->fg != 0)
				tile->fg = 0;
			else
				tile->bg = 0;
		} else {
			if (itemDefs->IsBackground(packet->value))
				tile->bg = packet->value;
			else
				tile->fg = packet->value;
		}
	}
}

void LocalData::Update13(TankPacketStruct* packet) {
	for (int i = 0; i < items.size(); i++) {
		if (items[i].id == packet->value) {
			int temp = items[i].amount - packet->padding1;
			if (temp < 1)
				items.erase(items.begin() + i);
			else
				items[i].amount = temp;
			break;
		}
	}
}

void LocalData::PlayerMove(TankPacketStruct* packet) {
	for (int i = 0; i < players.size(); i++) {
		if (players[i].netid == packet->netid) {
			players[i].pos.m_x = packet->x;
			players[i].pos.m_y = packet->y;
			break;
		}

	}
}

void LocalData::Update14(TankPacketStruct* packet) {
	if (packet->netid == -1) {
		WorldObject obj;
		obj.id = packet->value;
		obj.pos.m_x = packet->x;
		obj.pos.m_y = packet->y;
		obj.amount = (uint8_t)packet->padding4;
		obj.flags = packet->dropped;
		obj.oid = ++ last_oid;
		objects.push_back(obj);
	} else if (packet->netid == -3) {
		for (auto& obj : objects) {
			if (obj.id == packet->value && obj.pos.m_x == packet->x && obj.pos.m_y == packet->y) {
				obj.amount = (uint8_t)packet->padding4;
				break;
			}
		}
	} else if (packet->netid > 0) {
		for (int i = 0; i < objects.size(); i++) {
			if (objects[i].oid == packet->value) {
				if (packet->netid == netid) {
					if (objects[i].id == 112) {
						gems += objects[i].amount;
					} else {
						bool added = false;
						for (auto& item : items) {
							if (item.id == objects[i].id) {
								int temp = item.amount + objects[i].amount;
								if (temp > 200)
									item.amount = 200;
								else
									item.amount = temp;
								added = true;
								break;
							}
						}
						if (!added) {
							InventoryItem item;
							item.id = objects[i].id;
							item.amount = objects[i].amount;
							items.push_back(item);
						}
					}
				}
				objects.erase(objects.begin() + i);
				break;
			}
		}
	} else {
#ifdef _CONSOLE
		std::cout << "object update unhandled netid: " << packet->netid << std::endl;
#endif
	}
}



void LocalData::RescanObject() {
	scanned_object.clear();
	for (auto& obj : objects)
		scanned_object[obj.id] += obj.amount;
}

void LocalData::RescanTile() {
	scanned_tile.clear();
	for (auto& tile : tiles) {
		if (tile.fg != 0)
			scanned_tile[tile.fg] ++;
		if (tile.bg != 0)
			scanned_tile[tile.bg] ++;
	}
}