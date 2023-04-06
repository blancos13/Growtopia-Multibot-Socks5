
#include "ItemInfoManager.h"

#include <algorithm>
#include <fstream>
#include <iostream>

ItemInfoManager* itemDefs = new ItemInfoManager();

// ItemInfoManager

void ItemInfoManager::LoadFromFile(std::string path) {
	
	std::ifstream file(path, std::ios::binary | std::ios::ate);
	size = (int)file.tellg();
	
	if (size != -1) {
		char* data = new char[size];
		file.seekg(0, std::ios::beg);
		if (file.read(data, size))
			Instance = LoadFromMem(data);
		file.close();
		delete[] data;
	}

#ifdef _CONSOLE
	if (!Instance)
		std::cout << "failed load itemDefs" << std::endl;
#endif
}

bool ItemInfoManager::LoadFromMem(char* data) {
	std::string secret = "PBG892FXX982ABC*";
	
	memcpy(&version, data, 2);
	data += 2;
	
	memcpy(&itemcount, data, 4);
	data += 4;
	
	for (int i = 0; i < itemcount; i++) {
		ItemInfo itm;
		
		memcpy(&itm.id, data, 4);
		data += 4;
		
		data ++;
		data ++;
		itm.type = *(uint8_t*)data;
		data ++;
		data ++;
		
		short len;
		
		len = *(short*)data;
		data += 2;
		
		for (short l = 0; l < len; l++) itm.name += (data[l])^(secret[(l + itm.id)%secret.length()]);
		data += len;
		
		len = *(short*)data;
		data += 2;
		data += len;
		
		data += 13;
		
		itm.collisionType = *(int8_t*)data;
		data ++;
		
		itm.breakHits = *(uint8_t*)data;
		data ++;
		
		data += 4;
		
		itm.clothingType = *(int8_t*)data;
		data ++;
		
		memcpy(&itm.rarity, data, 2);
		data += 2;
		
		data ++;
		
		len = *(short*)data;
		data += 2;
		data += len;
		
		data += 8;
		
		len = *(short*)data;
		data += 2;
		data += len;
		
		len = *(short*)data;
		data += 2;
		data += len;
		
		len = *(short*)data;
		data += 2;
		data += len;
		
		len = *(short*)data;
		data += 2;
		data += len;
		
		data += 16;
		
		memcpy(&itm.growTime, data, 4);
		data += 4;
		
		data += 4;
		
		len = *(short*)data;
		data += 2;
		data += len;
		
		len = *(short*)data;
		data += 2;
		data += len;
		
		len = *(short*)data;
		data += 2;
		data += len;
		
		data += 80;
		if (version >= 11) {
			len = *(short*)data;
			data += 2;
			data += len;
		}
		
		if (version >= 12) data += 13;
		if (version >= 13) data += 4;
		if (version >= 14) data += 4;
		if (itm.id != i) {
#ifdef _CONSOLE
			std::cout << "unordered item at " << i << std::endl;
#endif
			return false;
		}
		items.push_back(itm);
	}

#ifdef _CONSOLE
	std::cout << "iteminfo loaded. byte: " << size << ", version: " << version << ", count: " << itemcount << std::endl;
#endif
	return true;
}



ItemInfo* ItemInfoManager::Get(int id) {
	if (id > -1 && id < itemcount)
		return &items[id];
	return NULL;
}

bool ItemInfoManager::IncludeExtra(int id) {
	ItemInfo* info = Get(id);
	if (!info)
		return false;
	return
	info->id == 3760 || // Data Bedrock
	info->type == 2 || // Door
	info->type == 3 || // Lock
	info->type == 10 || // Sign
	info->type == 13 || // Main Door
	info->type == 19 || // Seed
	info->type == 26 || // Portal
	info->type == 33 || // Mailbox
	info->type == 34 || // Bulletin Board
	info->type == 36 || // Dice Block
	info->type == 38 || // Science Station
	info->type == 40 || // Achievement Block
	info->type == 43 || // Sungate
	info->type == 46 ||
	info->type == 47 ||
	info->type == 49 ||
	info->type == 50 ||
	info->type == 51 || // Bunny Egg
	info->type == 52 ||
	info->type == 53 ||
	info->type == 54 || // Xenonite
	info->type == 55 || // Phone Booth
	info->type == 56 || // Crystal
	info->type == 57 || // Crime In Progress
	info->type == 59 || // Spotlight
	info->type == 61 ||
	info->type == 62 || // Vending Machine
	info->type == 63 || // Fish Tank Port
	info->type == 66 || // Forge
	info->type == 67 || // Giving Tree
	info->type == 73 || // Sewing Machine
	info->type == 74 ||
	info->type == 76 || // Painting Easel
	info->type == 78 || // Pet Trainer (WHY?!)
	info->type == 80 || // Lock-Bot (Why?!)
	info->type == 81 || // Weather Machine - Background
	info->type == 83 || // Display Shelf
	info->type == 84 ||
	info->type == 85 || // Challenge Timer
	info->type == 86 || // Challenge Start/End Flags
	info->type == 87 || // Fish Wall Mount
	info->type == 88 || // Portrait
	info->type == 89 || // Weather Machine - Stuff
	info->type == 91 || // Fossil Prep Station
	info->type == 92 || // DNA Processorr
	info->type == 93 || // Howler
	info->type == 97 || // Storage Box Xtreme / Untrade-a-box
	info->type == 98 || // Oven
	info->type == 100 || // Geiger Charger
	info->type == 101 ||
	info->type == 111 || // Magplant
	info->type == 113 || // CyBot
	info->type == 115 || // Lucky Token
	info->type == 116 || // GrowScan 9000 ???
	info->type == 126 || // Storm Cloud
	info->type == 127 || // Temp. Platform
	info->type == 130;
}

bool ItemInfoManager::IsBackground(int id) {
	ItemInfo* info = Get(id);
	if (!info)
		return false;
	return info->type == 22 || info->type == 28 || info->type == 18;
}



void ItemInfoManager::GetItemsByPartialName(std::vector<ItemInfo*>& list, std::string partial) {
	list.clear();
	
	if (partial.empty())
		return;
	
	std::transform(partial.begin(), partial.end(), partial.begin(), ::tolower);
	for (auto& item : items) {
		std::string src = item.name;
		std::transform(src.begin(), src.end(), src.begin(), ::tolower);
		if (src.find(partial) != std::string::npos)
			list.push_back(&item);
	}
	
}