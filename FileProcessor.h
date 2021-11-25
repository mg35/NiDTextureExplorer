#pragma once
#include <string>
#include <vector>
#define FOUR_BIT 4
#define EIGHT_BIT 8

class FileProcessor {
public:
	FileProcessor(std::wstring fileName) {
		this->fileName = fileName;
	}
	void setupImage(int offset, int paletteOffset);
	void loadGameFile4Bit(int offset);
	void loadGameFile(int offset);
	void writeImgFile(std::wstring outFileName);
	void WidenArray();
	void loadPalette(int paletteOffset, int paletteSize);
	void genPixelArray();
	void nullifyPixelArray();
	void setDims(int width, int height, int mode);
	void FindPaletteCandidates();
	int getPrevPalette(int currPalette);
	int getPalette(int paletteIndex) {
		return paletteOffsets.at(paletteIndex);
	}
	unsigned char* getPaletteArray();
	int getNextPalette(int currPalette);
	unsigned char* getPixelArray();
	
private:
	unsigned char* getBMPHeader();
	std::wstring fileName = L"";
	std::vector<int> paletteOffsets;
	unsigned char* RGBpointer = NULL;
	unsigned char RGBpalette[256][3];
	int width = 0;
	int height = 0;
	unsigned char* hexArray;
	unsigned char* narrowArray;
	unsigned char* pixelArray;
	int mode = FOUR_BIT;
};

