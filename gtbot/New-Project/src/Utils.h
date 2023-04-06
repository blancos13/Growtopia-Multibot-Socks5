#pragma once

#include "misc/RTTEX.h"

#include <algorithm>
#include <iostream>
#include <vector>
#include <string>


namespace Utils {
	std::vector<std::string> Split(const std::string &str,const std::string &delimiter);
	std::string Random(int len);
	std::string GenerateMac();
	bool Replace(std::string& str, const std::string& from, const std::string& to);
	bool HandleDelay(uint64_t& alpha, int beta);
	uint64_t GetTime();
	float SolveCaptcha(RTTEX& image);
	bool is_number(const std::string& s);
};