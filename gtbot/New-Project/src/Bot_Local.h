#pragma once

#include "proton/vector.hpp"
#include "Struct.h"

#include <map>
#include <string>
#include <vector>



struct LocalData {
	
	
	// world
	std::string name = "";
	int netid = 0;
	int userid = 0;
	vector2_t pos;
	std::string country = "";
	
	
	int level = 0;
	int gems = 0;
	
	
	std::vector<InventoryItem> items;
	std::vector<WorldObject> objects;
	std::vector<Tile> tiles;
	std::vector<Player> players;

	int PlayerTotal();

	std::string worldname = "";
	int width = 0;
	int height = 0;
	int tilecount = 0;
	
	int objectcount = 0;
	int last_oid = 0;
	
	Tile* GetTile(int x, int y);
	InventoryItem* GetItem(int id);


	void ExitWorld();
	void Reset();
	void InitOnSpawn(std::string spawn);
	void PlayerSpawn(std::string spawn);
	void PlayerRemove(std::string remove);

	void PlayerMove(TankPacketStruct* packet);

	void Serialize9(uint8_t* ptr);
	void Serialize4(uint8_t* ptr);
	
	void Update3(TankPacketStruct* packet);
	void Update13(TankPacketStruct* packet);
	void Update14(TankPacketStruct* packet);
	
	// growscan smh
	std::map<int, int> scanned_object;
	std::map<int, int> scanned_tile;

	void RescanObject();
	void RescanTile();
	
};

