#ifndef IOADDONS_H
#define IOADDONS_H

#include <fstream>
#include <cstdint>

class IOAddons
{
    public:
        static void writeByte(std::ofstream& file, int8_t value); // Writes unsigned 8-bit byte to the file stream
        static void writeShort(std::ofstream& file, int16_t value); // Write unsigned 16-bit short to the file stream
        static void writeInt(std::ofstream& file, int32_t value); // Writes signed 32-bit integer to the file stream
        static void writeString(std::ofstream& file, std::string value); // Writes a string with a linebreak in the end

        static int readByte(std::ifstream& file); // Reads an unsigned 8-bit short from a file stream
        static int readShort(std::ifstream& file); // Reads an unsigned 16-bit short from a file stream
        static int readInt(std::ifstream& file); // Reads an unsigned 32-bit integer from a file stream
        static std::string readString(std::ifstream& file); // Reads a string up to line break from the file stream
};

#endif // IOADDONS_H
