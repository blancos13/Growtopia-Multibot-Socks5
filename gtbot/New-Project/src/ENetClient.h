#pragma once

#include "../vendor/enet/include/enet.h"
#include <iostream>

#include "proton/variant.hpp"
#include "misc/Logger.h"
#include "misc/Pathfinder.h"
#include "Bot_Local.h"
#include "Lua_Functions.h"
#include "misc/Functions.h"

struct PersonalSetting {
	// separated
	bool filter_innapropriate = false; // Filter inappropriate language
	bool show_chat_friend = false; // Show chat only from friends
	
	// using enumeration
	bool disable_public_broadcast = false; // Disable public broadcast messages
	bool disable_adding_friends = false; // Disable adding friends
	bool disable_guild_invites = false; // Disable guild invites
	bool disable_guild_flag = false; // Disable guild flag
	bool disable_player_text = false; // Disable player text on signs and bulletin boards
	bool disable_billboard = false; // Disable Billboard
	bool use_classic_store = false; // Use classic store categories
	bool disable_in_app = false; // Disable In App Purchase options
	bool disable_tapjoy = false; // Disable Tapjoy earn free gems option
	
	int GetCbits();
};

struct ClientData {
	std::string name = "";
	std::string pass = "";
	
	std::string status = "";
	bool captcha = false;
	
	std::string ip = "";
	int port = 0;
	
	int lmode = 0;
	
	PersonalSetting personal;
	
	std::string meta = "";
	std::string rid = "";
	std::string mac = "";
	
	int user = 0;
	int token = 0;
	
	std::string uuid = "";
	std::string doorid = "";
	
	ClientData();
	
	bool Flush();
	
	void Set(variantlist_t& var);
	void Reset();
	
	std::string Create();
};


class ENetClient {
private:
	ENetPeer* peer = NULL;
	ENetHost* host = NULL;
	Pathfinder* pathfinder = NULL;
	bool finding_path = false;
	bool callback = false;
public:
	int dt_amount = 0;
	bool l_t = false;
	bool hook_var = false;
	lua_State* l = NULL;
	std::vector<lua_State*> t;

	ClientData data;
	LocalData local;
	Logger logger;
	CheatData cheat;

	ENetClient(std::string alpha = "", std::string beta = "");

	bool InWorld();

	bool FindPath(int x, int y);

	void Move(int radx, int rady);
	void Hit(int radx, int rady, int id);
	void Door();
	void Collect(int radius, bool force = false);

	int GetState();
	int GetPing();

	void SendPacket(int type, std::string const& text);
	void SendPacketRaw(int type, uint8_t* ptr, int flags = 1);

	void Init();
	void Destroy();

	void Connect(bool flush = false);
	void Disconnect(bool reset = false);

	void Poll();

	void OnDisconnect();
	void OnReceive(ENetPacket* packet);

	void OnTextPacket(int type, std::string text);
	void OnTankPacket(int type, uint8_t* ptr, int size);

};

extern std::vector<ENetClient*> bots;

