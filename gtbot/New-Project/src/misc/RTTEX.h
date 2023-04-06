#ifndef RTTEX_H_
#define RTTEX_H_

#include <fstream>
#include "BitMap.h"

struct RTTEXINFO {
	int Height;
	int Width;
	int Format;
	int RealHeight;
	int RealWidth;
	bool useAlpha;
	bool isCompressed;
	short Flags;
	int MipMapCount;
};

struct RTTEX {
private:
	BitMap* bitMap = nullptr;

public:
	RTTEXINFO Info;
	RTTEX(const char* File) {
		std::fstream stream(File, std::ios_base::in | std::ios::binary);
		stream.seekg(8, std::ios_base::beg);
		stream.read(reinterpret_cast<char*>(&Info), sizeof(Info));
		bitMap = new BitMap(Info.Height, Info.Width);
		stream.seekg(88, std::ios_base::cur);
		int* Bits = bitMap->GetBitData();

		for (int y = Info.Height - 1; y >= 0; y--) {
			for (int x = 0; x < Info.Width; x++)
			{
				if (Info.useAlpha) {
					RGB_A rgba;
					stream.read(reinterpret_cast<char*>(&rgba), sizeof(rgba));
					Bits[x + y * Info.Width] = (rgba.r << 16 | rgba.a << 8 | rgba.b | rgba.g << 24);
				}
				else {
					ColorRGB rgb;
					stream.read(reinterpret_cast<char*>(&rgb), sizeof(rgb));
					Bits[x + y * Info.Width] = (rgb.r << 16 | rgb.g << 8 | rgb.b | -16777216);
				}
			}
		}
		stream.close();
	}

	~RTTEX() {
		delete bitMap;
	}
	BitMap* GetMap() {
		return bitMap;
	}
};

#endif
