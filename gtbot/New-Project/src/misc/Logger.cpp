

#include "Logger.h"


void Logger::Text(int type, std::string text, int optional) {
	textList[type].push_back(text);
	if (textList[type].size() > optional)
		textList[type].erase(textList[type].begin());
}