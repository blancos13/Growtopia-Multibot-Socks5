#pragma once

#include <iostream>
#include <string>
#include <vector>

struct ItemInfo {
	int id = 0;
	uint8_t type = 0; // used for serialize check
	std::string name = "";
	int8_t collisionType = 0;
	uint8_t breakHits = 0; // 6 * actual break hit
	int8_t clothingType = 0;
	short rarity = 0;
	int growTime = 0; // as seconds
};

class ItemInfoManager {
	int size = 0;
public:
	std::vector<ItemInfo> items;
	short version = 0;
	int itemcount = 0;
	bool Instance = true;
	
	void LoadFromFile(std::string path = "C:\\items");
	bool LoadFromMem(char* address);
	
	ItemInfo* Get(int id);
	bool IncludeExtra(int id);
	bool IsBackground(int id);
	
	void GetItemsByPartialName(std::vector<ItemInfo*>& list, std::string partial);
};

extern ItemInfoManager* itemDefs;