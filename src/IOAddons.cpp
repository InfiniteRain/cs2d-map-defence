#include "IOAddons.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

// See header file for more information on the functions!

void IOAddons::writeByte(std::ofstream& file, int8_t value) {
    file.write(reinterpret_cast<const char*>(&value), sizeof(value));
}

void IOAddons::writeShort(std::ofstream& file, int16_t value) {
    file.write(reinterpret_cast<const char*>(&value), sizeof(value));
}

void IOAddons::writeInt(std::ofstream& file, int32_t value) {
    file.write(reinterpret_cast<const char*>(&value), sizeof(value));
}

void IOAddons::writeString(std::ofstream& file, std::string value) {
    std::string returnString = value;
    returnString.insert(returnString.length(), "\r\n");
    file << (returnString);
}

int IOAddons::readByte(std::ifstream& file) {
    int8_t returnValue;
    file.read((char*)&returnValue, sizeof(returnValue));

    return (unsigned int8_t)returnValue;
}

int IOAddons::readShort(std::ifstream& file) {
    int16_t returnValue;
    file.read((char*)&returnValue, sizeof(returnValue));

    return (unsigned int16_t)returnValue;
}

int IOAddons::readInt(std::ifstream& file) {
    int32_t returnValue;
    file.read((char*)&returnValue, sizeof(returnValue));

    return returnValue;
}

std::string IOAddons::readString(std::ifstream& file) {
    std::string returnString;
    std::getline(file, returnString);

    returnString.erase(std::remove(returnString.begin(), returnString.end(), '\n'), returnString.end());
    returnString.erase(std::remove(returnString.begin(), returnString.end(), '\r'), returnString.end());

    return returnString;
}
