#pragma once
#include <vector>

#pragma pack(push, 1)

struct ColorRGB {
	uint8_t b;
	uint8_t g;
	uint8_t r;

	inline bool operator==(const ColorRGB& other) {
		if (this->r == other.r &&
			this->g == other.g &&
			this->b == other.b) return true;
		return false;
	}
	ColorRGB() { }
	ColorRGB(uint8_t r, uint8_t b, uint8_t g) : r(r), g(g), b(b) { }
};

struct RGB_A {
	uint8_t g;
	uint8_t b;
	uint8_t a;
	uint8_t r;

	inline bool operator==(const RGB_A& other) {
		if (this->r == other.r &&
			this->g == other.g &&
			this->b == other.b &&
			this->a == other.a) return true;
		return false;
	}
	inline bool operator!=(const RGB_A& other) {
		if (this->r != other.r ||
			this->g != other.g ||
			this->b != other.b ||
			this->a != other.a) return true;
		return false;
	}

	inline bool operator>(const RGB_A& other) {
		if (this->r > other.r &&
			this->g > other.g &&
			this->b > other.b) return true;
		return false;
	}
	RGB_A() { }
	RGB_A(uint8_t r, uint8_t b, uint8_t g, uint8_t a) : r(r), g(g), b(b), a(a) { }
};

#pragma pack(pop)

struct BitMap {
private:
	std::vector<int> Bits;
public:

	int Height;
	int Width;

	BitMap(int h, int w) {
		Height = h;
		Width = w;
		Bits.resize(h * w);
	}

	int* GetBitData() {
		return Bits.data();
	}

	int* GetBitData(int x, int y) {
		return (&Bits[x + y * Width]);
	}

	RGB_A GetPixelRGBA(int x, int y) {
		return *reinterpret_cast<RGB_A*>(&Bits[x + y * Width]);
	}
	ColorRGB GetPixelRGB(int x, int y) {
		return *reinterpret_cast<ColorRGB*>(&Bits[x + y * Width]);
	}
};