

#include "Lua_Functions.h"
#include "ItemInfoManager.h"
#include "ENetClient.h"

#include <chrono>
#include <thread>

ENetClient* L_GC(lua_State* l) {
	if (lua_getglobal(l, "_CLIENT") == LUA_TNUMBER)
		return (ENetClient*)luaL_checkinteger(l, -1);
	return NULL;
}

// HELPER: PUSH

void lua_pushInventoryItem(lua_State* l, InventoryItem* item) {
	if (!item) {
		lua_pushnil(l);
		return;
	}
	
	lua_newtable(l);
	
	lua_pushliteral(l, "id");
	lua_pushinteger(l, item->id);
	lua_settable(l, -3);
	
	lua_pushliteral(l, "amount");
	lua_pushinteger(l, item->amount);
	lua_settable(l, -3);
}

void lua_pushItemInfo(lua_State* l, ItemInfo* info) {
	if (!info) {
		lua_pushnil(l);
		return;
	}
	
	lua_newtable(l);
	
	lua_pushliteral(l, "id");
	lua_pushinteger(l, info->id);
	lua_settable(l, -3);
	
	lua_pushliteral(l, "name");
	lua_pushstring(l, info->name.c_str());
	lua_settable(l, -3);
	
	lua_pushliteral(l, "rarity");
	lua_pushinteger(l, info->rarity);
	lua_settable(l, -3);
	
	lua_pushliteral(l, "growtime");
	lua_pushinteger(l, info->growTime);
	lua_settable(l, -3);
	
	lua_pushliteral(l, "breakhit");
	lua_pushinteger(l, info->breakHits/6);
	lua_settable(l, -3);
	
	lua_pushliteral(l, "clothingtype");
	lua_pushinteger(l, info->clothingType);
	lua_settable(l, -3);
	
	lua_pushliteral(l, "collisiontype");
	lua_pushinteger(l, info->collisionType);
	lua_settable(l, -3);
	
	lua_pushliteral(l, "type");
	lua_pushinteger(l, info->type);
	lua_settable(l, -3);
	
}

void lua_pushTile(lua_State* l, Tile* tile, int x, int y) {
	if (!tile) {
		lua_pushnil(l);
		return;
	}
	
	lua_newtable(l);
	
	lua_pushliteral(l, "fg");
	lua_pushinteger(l, tile->fg);
	lua_settable(l, -3);
	
	lua_pushliteral(l, "bg");
	lua_pushinteger(l, tile->bg);
	lua_settable(l, -3);
	
	lua_pushliteral(l, "ready");
	lua_pushboolean(l, tile->readyharvest);
	lua_settable(l, -3);

	lua_pushliteral(l, "x");
	lua_pushinteger(l, x);
	lua_settable(l, -3);
	
	lua_pushliteral(l, "data");
	lua_pushinteger(l, tile->data);
	lua_settable(l, -3);

	lua_pushliteral(l, "flags_1");
	lua_pushinteger(l, tile->flags_1);
	lua_settable(l, -3);

	lua_pushliteral(l, "flags_2");
	lua_pushinteger(l, tile->flags_2);
	lua_settable(l, -3);


	

}





void lua_pushWorldObject(lua_State* l, WorldObject* object) {
	if (!object) {
		lua_pushnil(l);
		return;
	}
	
	lua_newtable(l);
	
	lua_pushliteral(l, "id");
	lua_pushinteger(l, object->id);
	lua_settable(l, -3);
	
	lua_pushliteral(l, "pos");
	lua_newtable(l);
	
	lua_pushliteral(l, "x");
	lua_pushnumber(l, object->pos.m_x);
	lua_settable(l, -3);
	
	lua_pushliteral(l, "y");
	lua_pushnumber(l, object->pos.m_y);
	lua_settable(l, -3);
	
	lua_settable(l, -3);
	
	lua_pushliteral(l, "amount");
	lua_pushinteger(l, object->amount);
	lua_settable(l, -3);
	
	lua_pushliteral(l, "flags");
	lua_pushinteger(l, object->flags);
	lua_settable(l, -3);
	
	lua_pushliteral(l, "oid");
	lua_pushinteger(l, object->oid);
	lua_settable(l, -3);


}


bool lua_unpackTankPacket(lua_State* l, int i, TankPacketStruct& tps) {
	if (lua_getfield(l, i, "type") != LUA_TNIL)
		tps.type = (uint8_t)luaL_checkinteger(l, -1);
	
	if (lua_getfield(l, i, "dropped") != LUA_TNIL)
		tps.dropped = (uint8_t)luaL_checkinteger(l, -1);
	
	if (lua_getfield(l, i, "netid") != LUA_TNIL)
		tps.netid = luaL_checkinteger(l, -1);
	if (lua_getfield(l, i, "player_flag") != LUA_TNIL)
		tps.netid = luaL_checkinteger(l, -1);
	
	if (lua_getfield(l, i, "snetid") != LUA_TNIL)
		tps.snetid = luaL_checkinteger(l, -1);
	if (lua_getfield(l, i, "secondnetid") != LUA_TNIL)
		tps.snetid = luaL_checkinteger(l, -1);
	
	if (lua_getfield(l, i, "state") != LUA_TNIL)
		tps.state = luaL_checkinteger(l, -1);
	if (lua_getfield(l, i, "characterstate") != LUA_TNIL)
		tps.state = luaL_checkinteger(l, -1);
	
	if (lua_getfield(l, i, "value") != LUA_TNIL)
		tps.value = luaL_checkinteger(l, -1);
	if (lua_getfield(l, i, "int_data") != LUA_TNIL)
		tps.value = luaL_checkinteger(l, -1);
	
	if (lua_getfield(l, i, "x") != LUA_TNIL)
		tps.x = (float)luaL_checknumber(l, -1);
	if (lua_getfield(l, i, "pos_x") != LUA_TNIL)
		tps.x = (float)luaL_checknumber(l, -1);
	if (lua_getfield(l, i, "y") != LUA_TNIL)
		tps.y = (float)luaL_checknumber(l, -1);
	if (lua_getfield(l, i, "pos_y") != LUA_TNIL)
		tps.y = (float)luaL_checknumber(l, -1);
	
	if (lua_getfield(l, i, "xspeed") != LUA_TNIL)
		tps.xspeed = (float)luaL_checknumber(l, -1);
	if (lua_getfield(l, i, "yspeed") != LUA_TNIL)
		tps.yspeed = (float)luaL_checknumber(l, -1);
	
	if (lua_getfield(l, i, "px") != LUA_TNIL)
		tps.tilex = luaL_checkinteger(l, -1);
	if (lua_getfield(l, i, "tx") != LUA_TNIL)
		tps.tilex = luaL_checkinteger(l, -1);
	if (lua_getfield(l, i, "int_x") != LUA_TNIL)
		tps.tilex = luaL_checkinteger(l, -1);
	
	if (lua_getfield(l, i, "py") != LUA_TNIL)
		tps.tiley = luaL_checkinteger(l, -1);
	if (lua_getfield(l, i, "ty") != LUA_TNIL)
		tps.tiley = luaL_checkinteger(l, -1);
	if (lua_getfield(l, i, "int_y") != LUA_TNIL)
		tps.tiley = luaL_checkinteger(l, -1);
	
	if (lua_getfield(l, i, "padding1") != LUA_TNIL)
		tps.padding1 = (uint8_t) luaL_checkinteger(l, -1);
	if (lua_getfield(l, i, "padding2") != LUA_TNIL)
		tps.padding2 = (uint8_t) luaL_checkinteger(l, -1);
	if (lua_getfield(l, i, "padding4") != LUA_TNIL)
		tps.padding4 = (float) luaL_checknumber(l, -1);
	
	if (lua_getfield(l, i, "padding5") != LUA_TNIL)
		tps.padding5 = luaL_checkinteger(l, -1);
	
	if (lua_getfield(l, i, "extrasize") != LUA_TNIL)
		tps.extDataSize = luaL_checkinteger(l, -1);
	
	if (tps.type == 3) {
		if (abs(tps.x/32 - tps.tilex) > 2 || abs(tps.y/32 - tps.tiley) > 2)
			return false;
	}
	
	return true;
}

// API

int Api::L_GETINVENTORY(lua_State* l) {
	ENetClient* client = L_GC(l);
	if (client) {
		lua_newtable(l);
		for (auto& item : client->local.items) {
			lua_pushinteger(l, item.id);
			lua_pushInventoryItem(l, &item);
			lua_settable(l, -3);
		}
	} else {
		lua_pushnil(l);
	}
	return 1;
}

int Api::L_GETITEMINFO(lua_State* l) {
	if (itemDefs->Instance) {
		if (lua_isinteger(l, 1)) {
			lua_pushItemInfo(l, itemDefs->Get(luaL_checkinteger(l, 1)));
		} else if (lua_isstring(l, 1)) {
			std::string to = luaL_checkstring(l, 1);
			std::transform(to.begin(), to.end(), to.begin(), ::tolower);
			for (auto& item : itemDefs->items) {
				std::string from = item.name;
				std::transform(from.begin(), from.end(), from.begin(), ::tolower);
				if (from == to) {
					lua_pushItemInfo(l, &item);
					return 1;
				}
			}
			lua_pushnil(l);
		}
	} else {
		lua_pushnil(l);
	}
	return 1;
}

int Api::L_GETITEM(lua_State* l) {
	ENetClient* client = L_GC(l);
	if (client) {
		int id = luaL_checkinteger(l, 1);
		lua_newtable(l);
		for (auto& item : client->local.items) {
			if (item.id == id) {
				lua_pushinteger(l, item.id);
				lua_pushInventoryItem(l, &item);
				lua_settable(l, -3);
			}
		}
	} else {
		lua_pushnil(l);
	}
	return 1;
}

int Api::L_GETLOCAL(lua_State* l) {
	ENetClient* client = L_GC(l);
	if (client) {
		if (client->InWorld()) {
			lua_newtable(l);
			
			lua_pushliteral(l, "name");
			lua_pushstring(l, client->local.name.c_str());
			lua_settable(l, -3);
			
			lua_pushliteral(l, "netid");
			lua_pushinteger(l, client->local.netid);
			lua_settable(l, -3);
			
			lua_pushliteral(l, "userid");
			lua_pushinteger(l, client->local.userid);
			lua_settable(l, -3);
			
			lua_pushliteral(l, "pos");
			lua_newtable(l);
			
			lua_pushliteral(l, "x");
			lua_pushnumber(l, client->local.pos.m_x);
			lua_settable(l, -3);
			
			lua_pushliteral(l, "y");
			lua_pushnumber(l, client->local.pos.m_y);
			lua_settable(l, -3);
			
			lua_settable(l, -3);
			
		} else {
			lua_pushnil(l);
		}
	} else {
		lua_pushnil(l);
	}
	return 1;
}

int Api::L_GETOBJECTLIST(lua_State* l) {
	ENetClient* client = L_GC(l);
	if (client) {
		lua_newtable(l);
		for (auto& object : client->local.objects) {
			lua_pushinteger(l, object.oid);
			lua_pushWorldObject(l, &object);
			lua_settable(l, -3);
		}
	} else {
		lua_pushnil(l);
	}
	return 1;
}

void lua_pushplayerlist(lua_State* l, Player* player) {
	if (!player) {
		lua_pushnil(l);
		return;
	}

	lua_newtable(l);

	lua_pushliteral(l, "name");
	lua_pushstring(l, player->name.c_str());
	lua_settable(l, -3);

	lua_pushliteral(l, "pos");
	lua_newtable(l);

	lua_pushliteral(l, "x");
	lua_pushnumber(l, player->pos.m_x);
	lua_settable(l, -3);

	lua_pushliteral(l, "y");
	lua_pushnumber(l, player->pos.m_y);
	lua_settable(l, -3);

	lua_settable(l, -3);

	lua_pushliteral(l, "userid");
	lua_pushinteger(l, player->userid);
	lua_settable(l, -3);

	lua_pushliteral(l, "netid");
	lua_pushinteger(l, player->netid);
	lua_settable(l, -3);
}


int Api::L_GETPLAYERLIST(lua_State* l) {
	ENetClient* client = L_GC(l);
	if (client) {
		lua_newtable(l);
		for (auto& playerd : client->local.players) {
			lua_pushstring(l, playerd.name.c_str());
			lua_pushplayerlist(l, &playerd);
			lua_settable(l, -3);
		}
	} else {
		lua_pushnil(l);
	}
	return 1;
}

int Api::L_GETTILE(lua_State* l) {
	ENetClient* client = L_GC(l);
	if (client) {
		int x = luaL_checkinteger(l, 1);
		int y = luaL_checkinteger(l, 2);
		lua_pushTile(l, client->local.GetTile(x, y), x, y);
	} else {
		lua_pushnil(l);
	}
	return 1;
}

int Api::L_GETTILES(lua_State* l) {
	ENetClient* client = L_GC(l);
	if (client) {
		int div = client->local.width;
		lua_newtable(l);
		for (uint32_t i = 0; i < client->local.tiles.size(); i++) {
			lua_pushinteger(l, i);
			lua_pushTile(l, &client->local.tiles[i], i % div, i / div);
			lua_settable(l, -3);
		}
	} else {
		lua_pushnil(l);
	}
	return 1;
}


int Api::L_GETWORLD(lua_State* l) {
	ENetClient* client = L_GC(l);
	if (client) {
		if (client->InWorld()) {
			lua_newtable(l);
			
			lua_pushliteral(l, "name");
			lua_pushstring(l, client->local.worldname.c_str());
			lua_settable(l, -3);
			
			lua_pushliteral(l, "width");
			lua_pushinteger(l, client->local.width);
			lua_settable(l, -3);
			
			lua_pushliteral(l, "height");
			lua_pushinteger(l, client->local.height);
			lua_settable(l, -3);
			
			lua_pushliteral(l, "tilecount");
			lua_pushinteger(l, client->local.tilecount);
			lua_settable(l, -3);
			
			lua_pushliteral(l, "objectcount");
			lua_pushinteger(l, client->local.objectcount);
			lua_settable(l, -3);
			
			lua_pushliteral(l, "lastoid");
			lua_pushinteger(l, client->local.last_oid);
			lua_settable(l, -3);
		} else {
			lua_pushnil(l);
		}
	} else {
		lua_pushnil(l);
	}
	return 1;
}



int Api::L_SENDPACKET(lua_State* l) {
	ENetClient* client = L_GC(l);
	if (client)
		client->SendPacket(luaL_checkinteger(l, 1), luaL_checkstring(l, 2));
	return 1;
}

int Api::L_SENDPACKETRAW(lua_State* l) {
	int flags = 1;
	if (lua_isinteger(l, 2))
		flags = luaL_checkinteger(l, 2);
	ENetClient* client = L_GC(l);
	if (client) {
		TankPacketStruct tank;
		if (lua_unpackTankPacket(l, 1, tank))
			client->SendPacketRaw(4, (uint8_t*)&tank, flags);
	}
	return 1;
}

int Api::L_FINDPATH(lua_State* l) {
	ENetClient* client = L_GC(l);
	if (client)
		lua_pushboolean(l, client->FindPath(luaL_checkinteger(l, 1), luaL_checkinteger(l, 2)));
	return 1;
}

int Api::L_SLEEP(lua_State* l) {
	std::this_thread::sleep_for(std::chrono::milliseconds(luaL_checkinteger(l, 1)));
	return 1;
}

int Api::L_COLLECT(lua_State* l) {
	ENetClient* client = L_GC(l);
	if (client) {
		bool force = false;
		if (lua_isboolean(l, 2))
			force = lua_toboolean(l, 2);
		client->Collect(luaL_checkinteger(l, 1), force);
	}
	return 1;
}

int Api::L_DOOR(lua_State* l) {
	ENetClient* client = L_GC(l);
	if (client)
		client->Door();
	return 1;
}

int Api::L_HIT(lua_State* l) {
	ENetClient* client = L_GC(l);
	if (client)
		client->Hit(luaL_checkinteger(l, 1), luaL_checkinteger(l, 2), luaL_checkinteger(l, 3));
	return 1;
}

int Api::L_MOVE(lua_State* l) {
	ENetClient* client = L_GC(l);
	if (client)
		client->Move(luaL_checkinteger(l, 1), luaL_checkinteger(l, 2));
	return 1;
}

int L_LOGTOCONSOLE(lua_State* l) {
	ENetClient* client = L_GC(l);
	if (client)
		client->logger.Text(7, std::string(luaL_checkstring(l, 1)), 50);
	return 1;
}

int L_CHANGEHOOK(lua_State* l) {
	ENetClient* client = L_GC(l);
	if (client)
		client->hook_var = lua_toboolean(l, 1);
	return 1;
}

void Api::OPEN(lua_State* l, uintptr_t addr) {
	luaL_openlibs(l);
	
	lua_pushinteger(l, addr);
	lua_setglobal(l, "_CLIENT");
	
	lua_register(l, "ChangeHook", L_CHANGEHOOK);
	if (luaL_dostring(l, "local hook = nil;function RemoveHooks() hook = nil ChangeHook(false) end;function AddHook(str, func) if type(func) == 'function' then hook = func ChangeHook(true) end end;function CallHook(...) if type(hook) == 'function' then local status, err = pcall(hook, ...) if not status then ChangeHook(false) error(err) end end end"))
		((ENetClient*)addr)->logger.Text(7, "api error: " + std::string(luaL_checkstring(l, -1)), 50);
	
	lua_register(l, "log", L_LOGTOCONSOLE);
	if (luaL_dostring(l, "function print(...) local ret = ''; for _, v in pairs({...}) do if type(v) == 'string' or type(v) == 'number' then ret = ret .. v .. ' ' end end if ret:len() > 1 then LogToConsole(ret:sub(1, ret:len() - 1)) end end"))
		((ENetClient*)addr)->logger.Text(7, "api error: " + std::string(luaL_checkstring(l, -1)), 50);
	
	lua_register(l, "Collect", L_COLLECT);
	lua_register(l, "Door", L_DOOR);
	lua_register(l, "Hit", L_HIT);
	lua_register(l, "Move", L_MOVE);
	
	lua_register(l, "SendPacket", L_SENDPACKET);
	lua_register(l, "SendPacketRaw", L_SENDPACKETRAW);
	lua_register(l, "FindPath", L_FINDPATH);
	lua_register(l, "Sleep", L_SLEEP);
	
	lua_register(l, "GetPlayers", L_GETPLAYERLIST);
	lua_register(l, "GetInventory", L_GETINVENTORY);
	lua_register(l, "GetItemInfo", L_GETITEMINFO);
	lua_register(l, "GetLocal", L_GETLOCAL);
	lua_register(l, "GetObjects", L_GETOBJECTLIST);
	lua_register(l, "GetTile", L_GETTILE);
	lua_register(l, "GetTiles", L_GETTILES);
	lua_register(l, "GetWorld", L_GETWORLD);
	lua_register(l, "GetItem", L_GETITEM);
}


void L_TERMINATE(lua_State* L, lua_Debug *ar) {
	if (ar->event == LUA_HOOKLINE) {
		ENetClient* client = L_GC(L);
		if (client)
			client->t.push_back(L);
		luaL_error(L, "exit.");
	}
}

void Api::TERMINATE(lua_State* l) {
	if (l)
		lua_sethook(l, &L_TERMINATE, LUA_MASKLINE, 0);
}

void Api::THREAD(lua_State* l, bool* threadopen, std::string script) {
	*threadopen = true;
	if (luaL_dostring(l, script.c_str())) {
		std::string error(luaL_checkstring(l, -1));
		ENetClient* client = L_GC(l);
		if (client)
			client->logger.Text(7, "error: " + error, 50);
	}
	*threadopen = false;
}


void Api::PUSHVAR(lua_State* l, variantlist_t* varlist) {
	if (!varlist) {
		lua_pushnil(l);
		return;
	}
	
	lua_newtable(l);
	for (int index = 0; index < 7; index++) {
		lua_pushinteger(l, index);
		switch ((*varlist)[index].get_type()) {
			case variant_t::vartype_t::TYPE_STRING: {
				lua_pushstring(l, (*varlist)[index].get_string().c_str());
			} break;
			case variant_t::vartype_t::TYPE_INT32: {
				lua_pushinteger(l, (*varlist)[index].get_int32());
			} break;
			case variant_t::vartype_t::TYPE_UINT32: {
				lua_pushinteger(l, (*varlist)[index].get_uint32());
			} break;
			case variant_t::vartype_t::TYPE_FLOAT: {
				lua_pushnumber(l, (*varlist)[index].get_float());
			} break;
			case variant_t::vartype_t::TYPE_RECT: {
				lua_newtable(l);
				auto vector = (*varlist)[index].get_rect();
				lua_pushliteral(l, "x");
				lua_pushnumber(l, vector.m_x);
				lua_settable(l, -3);
				lua_pushliteral(l, "y");
				lua_pushnumber(l, vector.m_y);
				lua_settable(l, -3);
				lua_pushliteral(l, "w");
				lua_pushnumber(l, vector.m_w);
				lua_settable(l, -3);
				lua_pushliteral(l, "h");
				lua_pushnumber(l, vector.m_h);
				lua_settable(l, -3);
			} break;
			case variant_t::vartype_t::TYPE_VECTOR3: {
				lua_newtable(l);
				auto vector = (*varlist)[index].get_vector3();
				lua_pushliteral(l, "x");
				lua_pushnumber(l, vector.m_x);
				lua_settable(l, -3);
				lua_pushliteral(l, "y");
				lua_pushnumber(l, vector.m_y);
				lua_settable(l, -3);
				lua_pushliteral(l, "z");
				lua_pushnumber(l, vector.m_z);
				lua_settable(l, -3);
			} break;
			case variant_t::vartype_t::TYPE_VECTOR2: {
				lua_newtable(l);
				auto vector = (*varlist)[index].get_vector2();
				lua_pushliteral(l, "x");
				lua_pushnumber(l, vector.m_x);
				lua_settable(l, -3);
				lua_pushliteral(l, "y");
				lua_pushnumber(l, vector.m_y);
				lua_settable(l, -3);
			} break;
			default: {
				lua_pushnil(l);
			} break;
		}
		lua_settable(l, -3);
	}
}

void Api::INIT_EDITOR(TextEditor* editor) {
	const char* apis[] = {
		"log", "Sleep", "SendPacket", "SendPacketRaw", "AddHook", "FindPath", "RemoveHooks",
		"GetInventory", "GetItemInfo", "GetLocal", "GetObjects", "GetTile", "GetTiles", "GetWorld",
		"Collect", "Door", "Hit", "Move", "GetPlayers", "GetItem"
	};

	const char* structapis[] = {
		"id","amount","name","rarity","growtime","breakhit","clothingtype",
		"collisiontype","type","fg","bg","pos.x","pos.y","flags",
		"oid","netid","userid","width","height","tilecount","objectcount","lastoid",
		"dropped","padding1","padding2","secondnetid","characterstate","padding4",
		"value","x","y","xspeed","yspeed","padding5","px","py","extrasize"
	};
	auto langdef = TextEditor::LanguageDefinition::Lua();
	
	for (int i = 0; i < sizeof(apis)/sizeof(apis[0]); i++) {
		TextEditor::Identifier id;
		langdef.mIdentifiers.insert(std::make_pair(std::string(apis[i]), id));
	}
	
	for (auto& k : structapis)
		langdef.mKeywords.insert(k);

	editor->SetLanguageDefinition(langdef);
}