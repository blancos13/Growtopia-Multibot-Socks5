

#pragma once

#include <map>
#include <string>
#include <vector>

struct Logger {
	std::map<int, std::vector<std::string>> textList;
	void Text(int type, std::string text, int optional = 20);
};