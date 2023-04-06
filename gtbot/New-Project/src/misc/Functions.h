
#pragma once

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>

struct CheatData {
	void* client = NULL;
	
	bool auto_aa = false;
	
	bool auto_c = false;
	bool auto_c_f = false;
	int auto_c_d = 200;
	int auto_c_r = 0;
	uint64_t auto_c_last = 0;
	
	
	int auto_f_m = 0;
	int auto_f_id = -1;
	int auto_f_pd = 250;
	int auto_f_bd = 230;
	bool auto_f = false;
	bool auto_f_p = false;
	bool auto_f_b = false;
	uint8_t auto_f_bhit = 0;
	uint8_t auto_f_step = 0;
	uint64_t auto_f_last = 0;
	
	std::vector<std::string> auto_s_t;
	bool auto_s = false;
	int auto_s_step = 0;
	int auto_s_d = 4000;
	uint64_t auto_s_last = 0;
	
	void OnUpdate();
};

namespace opt {
	extern bool enable_blacklist;
	extern std::vector<std::string> accnames;
	extern std::vector<std::string> banlogs;
	extern char details_rpc[200];
};