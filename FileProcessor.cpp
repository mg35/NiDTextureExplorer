#include "FileProcessor.h"
#include <fstream>
#include <list>
#include <iostream>


void FileProcessor::setupImage(int offset, int paletteOffset) {    
    //filepath?
    if (mode == FOUR_BIT) {
        loadGameFile4Bit(offset);
        WidenArray();
    }
    else {
        loadGameFile(offset);
    }
    loadPalette(paletteOffset, pow(2.0, (double) mode));
}

void FileProcessor::loadGameFile4Bit(int offset) {
    std::ifstream imgFile;
    imgFile.open(fileName, std::ios::binary);
    char hex[1] = { 0 };
    int counter = 0;
    while (!imgFile.eof()) {
        imgFile.read(hex, 1);
        if (counter >= offset && counter < offset + (width * height / 2)) {
            *(narrowArray + counter - offset) = *hex;
        }
        else if (counter > offset + (width * height / 2)) {
            break;
        }
        counter++;
    }
    imgFile.close();
}

void FileProcessor::loadGameFile(int offset) {
    std::ifstream imgFile;
    imgFile.open(fileName, std::ios::binary);
    char hex[1] = { 0 };
    int counter = 0;
    while (!imgFile.eof()) {
        imgFile.read(hex, 1);
        if (counter >= offset && counter < offset + (width * height)) {
            *(hexArray + counter - offset) = *hex;
        }
        else if (counter > offset + (width * height)) {
            break;
        }
        counter++;
    }
    imgFile.close();
}

unsigned char* FileProcessor::getPaletteArray() {
    if (RGBpointer != NULL) {
        free(RGBpointer);
        RGBpointer = NULL;
    }
    RGBpointer = (unsigned char*)malloc(pow(2.0,(double)mode) * 3);
    for (int i = 0; i < pow(2.0, (double)mode); i++) {
        for (int j = 0; j < 3; j++) {
            *(RGBpointer + i * 3 + j) = RGBpalette[i][j];
        }
    }
    return RGBpointer;
}

void FileProcessor::FindPaletteCandidates() {
    paletteOffsets.clear();
    std::ifstream imgFile;
    imgFile.open(fileName, std::ios::binary);
    std::list<unsigned char> paletteCandidate;
    std::list<unsigned char>::iterator ptr;
    unsigned short* potentialPalette = (unsigned short*)malloc(pow(2.0, (double)mode) * 2);
    unsigned char* buildPalette = (unsigned char*)malloc(pow(2.0, (double)mode) * 2);
    bool zeroFirst = true;
    unsigned char hex[1];
    int counter = 0;
    while (!imgFile.eof()) {
        imgFile.read((char*)hex, 1);
        paletteCandidate.push_back(*hex);
        if (paletteCandidate.size() == pow(2.0, (double)mode) * 2) {
            if (counter % 8 == 1) {
                ptr = paletteCandidate.begin();
                zeroFirst = true;
                for (int i = 0; i < pow(2.0, (double)mode); i++) {
                    if (*ptr >> 7 == 1) {
                        zeroFirst = false;
                        for (int j = 0; j < i * 2 + 1; j++) {
                            imgFile.read((char*)hex, 1);
                            paletteCandidate.pop_front();
                            paletteCandidate.push_back(*hex);
                            counter++;
                        }
                        break;
                    }
                    ptr++;
                    ptr++;
                }
                if (zeroFirst == true) {
                    ptr = paletteCandidate.begin();
                    for (int i = 0; i < pow(2.0, (double)mode) * 2; i++) {
                        if (i % 2 == 0) {
                            buildPalette[i + 1] = *ptr;
                        }
                        else {
                            buildPalette[i - 1] = *ptr;
                            potentialPalette[i / 2] = *(unsigned short*)(buildPalette + i - 1);
                        }
                        ptr++;
                    }
                    int skipTo = 0;
                    for (int i = 0; i < pow(2.0, (double)mode); i++) {
                        if (potentialPalette[i] != 0) {
                            for (int j = i + 1; j < pow(2.0, (double)mode); j++) {
                                if (potentialPalette[i] == potentialPalette[j]) {
                                    skipTo = i + 1;
                                }
                            }
                            if (skipTo != 0) {
                                for (int j = 0; j < skipTo * 2; j++) {
                                    imgFile.read((char*)hex, 1);
                                    paletteCandidate.pop_front();
                                    paletteCandidate.push_back(*hex);
                                    counter++;
                                }
                                break;
                            }
                        }
                    }
                    if (skipTo == 0 || mode == EIGHT_BIT) {
                        paletteOffsets.push_back(counter - pow(2.0, (double)mode) * 2 - 1);

                    }
                }
            }
            paletteCandidate.pop_front(); 
        }
        counter++;
    }
    free(buildPalette);
    free(potentialPalette);
    if (paletteOffsets.size() == 0) {
        paletteOffsets.push_back(0);
    }
    imgFile.close();
}

int FileProcessor::getPrevPalette(int currPalette) {
    int paletteIndex = paletteOffsets.size() - 1;
    while (currPalette <= paletteOffsets.at(paletteIndex)) {
        paletteIndex -= 1;
        if (paletteIndex < 0) {
            paletteIndex = paletteOffsets.size() - 1;
            break;
        }
    }
    return paletteOffsets.at(paletteIndex);
}
int FileProcessor::getNextPalette(int currPalette) {
    int paletteIndex = 0;
    while (currPalette >= paletteOffsets.at(paletteIndex)) {
        paletteIndex += 1;
        if (paletteIndex > paletteOffsets.size() - 1) {
            paletteIndex = 0;
            break;
        }
    }
    return paletteOffsets.at(paletteIndex);
}

void FileProcessor::setDims(int width, int height, int mode) {
    this->mode = mode;
    this->width = width;
    this->height = height;
    if (hexArray != NULL) {
        free(hexArray);
    }
    if (narrowArray != NULL) {
        free(narrowArray);
    }
    if (pixelArray != NULL) {
        free(pixelArray);
    }
    hexArray = (unsigned char*)malloc(width * height);
    narrowArray = (unsigned char*)malloc(width * height / 2);
    pixelArray = (unsigned char*)malloc(3 * width * height);
}

void FileProcessor::genPixelArray() {
    for (int i = 0; i < width * height; i++) {
        *(pixelArray + i * 3) = RGBpalette[hexArray[i]][0];
        *(pixelArray + i * 3 + 1) = RGBpalette[hexArray[i]][1];
        *(pixelArray + i * 3 + 2) = RGBpalette[hexArray[i]][2];
    }
}

unsigned char* FileProcessor::getPixelArray() {
    return pixelArray;
}

void FileProcessor::nullifyPixelArray() {
    pixelArray = NULL;
}

void FileProcessor::WidenArray() {
    int arraySize = width * height;
    for (int i = 0; i < arraySize / 2; i++) {
        *(hexArray + i * 2) = (*(narrowArray + i) >> 4) & 0x0F;
        *(hexArray + i * 2 + 1) = *(narrowArray + i) & 0x0F;
    }
}


void FileProcessor::loadPalette(int paletteOffset, int paletteSize) {
    unsigned char color[2];
    unsigned char temp;
    std::ifstream paletteReader;
    paletteReader.open(fileName, std::ios::binary);
    paletteReader.seekg(paletteOffset);
    for (int i = 0; i < paletteSize; i++) {
        paletteReader.read((char*)color, 2);
        temp = color[0];
        color[0] = color[1];
        color[1] = temp;
        RGBpalette[i][0] = (*(unsigned short*)color & 0x7c00) >> 7;
        RGBpalette[i][1] = (*(unsigned short*)color & 0x03e0) >> 2;
        RGBpalette[i][2] = (*(unsigned short*)color & 0x001f) << 3;
    }
}

void FileProcessor::writeImgFile(std::wstring outFileName) {
    int bmpNumBytes = width * height + 0x36;
    unsigned char* bmpNumBytesP = (unsigned char*)&bmpNumBytes;
    unsigned char* heightP = (unsigned char*)&height;
    unsigned char* widthP = (unsigned char*)&width;
    unsigned char bmpHeader[0x36] = { 0x42, 0x4D, *bmpNumBytesP, *(bmpNumBytesP + 1), *(bmpNumBytesP + 2),
                                         *(bmpNumBytesP + 3),
                                         0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00,
                                         *widthP, *(widthP + 1), *(widthP + 2), *(widthP + 3),
                                         *heightP, *(heightP + 1), *(heightP + 2), *(heightP + 3),
                                         0x01, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00,
                                         0x00, 0x00,
                                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    std::ofstream imgConverted;
    imgConverted.open(outFileName, std::ios::binary);
    imgConverted.write((char*)bmpHeader, 0x36);
    imgConverted.write((char*)pixelArray, 3*width*height);
    imgConverted.close();
}

