

#pragma once
#include <algorithm>
#include "../vendor/lua/lua.hpp"
#include "proton/variant.hpp"
#include "wrapper/TextEditor.h"

namespace Api {
	int L_GETINVENTORY(lua_State* l);
	int L_GETITEMINFO(lua_State* l);
	int L_GETITEM(lua_State* l);
	int L_GETLOCAL(lua_State* l);
	int L_GETOBJECTLIST(lua_State* l);
	int L_GETTILE(lua_State* l);
	int L_GETTILES(lua_State* l);
	int L_GETWORLD(lua_State* l);
	int L_GETPLAYERLIST(lua_State* l);


	int L_SENDPACKET(lua_State* l);
	int L_SENDPACKETRAW(lua_State* l);
	int L_FINDPATH(lua_State* l);
	int L_SLEEP(lua_State* l);
	
	int L_COLLECT(lua_State* l);
	int L_DOOR(lua_State* l);
	int L_HIT(lua_State* l);
	int L_MOVE(lua_State* l);
	
	void OPEN(lua_State* l, uintptr_t address);
	void TERMINATE(lua_State* l);
	void THREAD(lua_State* l, bool* threadopen, std::string script);
	
	void PUSHVAR(lua_State* l, variantlist_t* varlist);
	
	void INIT_EDITOR(TextEditor* editor);
};