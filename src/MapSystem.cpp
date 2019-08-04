#include "MapSystem.h"
#include "IOAddons.h"

#include <fstream>
#include <iostream>
#include <ctime>
#include <cstdio>
#include <cstdint>
#include <windows.h>

MapSystem::~MapSystem() {
    if (mapLoaded) { // /Checks if map is loaded
        unloadMap(); // Unloads map thus removing any heap allocated memory
    }
}

// Following function will load map data from .map file and store it in private variables defined in header file
// All the heap allocated memory gets cleaned once saveMap() function or destructor is called
// Returns 0 if operation was successful
// Returns 1 if map is already loaded
// Returns 2 if map file was not found (failure)
// Returns 3 if map has failed first header check (failure)
// Returns 4 if map has failed second header check (failure)
int MapSystem::loadMap(std::string filePath) {
    if (!(mapLoaded)) {
        std::ifstream file(filePath, std::ios::binary); // Opening input stream
        if (!file.fail()) { // Checks if file stream was successfully opened
            if (IOAddons::readString(file) == "Unreal Software's Counter-Strike 2D Map File (max)") { // First header check
                // Byte settings
                scrollMapLikeTiles = IOAddons::readByte(file); // Will map scroll like tiles?
                useModifiers = IOAddons::readByte(file); // Will map use modifiers?
                for (int i = 0; i < 8; i++) { // Skips through unused settings bytes
                    IOAddons::readByte(file);
                }

                // Int settings
                upTime = IOAddons::readInt(file); // Gets up time of the system when map was created
                USGNID = IOAddons::readInt(file); // Gets USGN ID of the author
                if (USGNID > 0) { // If USGN ID is not 0, then user was registered
                    USGNID -= 51; // USGN ID has an offset of +51 (or 0 if he was not registered)
                }
                for (int i = 0; i < 8; i++) { // Skips through unused settings ints
                    IOAddons::readInt(file);
                }

                // String settings
                authorName = IOAddons::readString(file);  // Gets author username he used during the creation of the map
                for (int i = 0; i < 9; i++) { // Skips through unused settings strings
                    IOAddons::readString(file);
                }

                // More map settings
                IOAddons::readString(file); // Reads special string which is not used in this application so it isn't saved
                tilesetFileName = IOAddons::readString(file); // Gets tileset filename
                requiredTilesCount = IOAddons::readByte(file); // Gets count of required tiles
                mapWidth = IOAddons::readInt(file); // Gets map width
                mapHeight = IOAddons::readInt(file); // Gets map height
                backgroundFileName = IOAddons::readString(file); // Gets background filename
                mapScrollXSpeed = IOAddons::readInt(file); // Gets map scroll x speed
                mapScrollYSpeed = IOAddons::readInt(file); // Gets map scroll y speed
                backgroundColorRed = IOAddons::readByte(file); // Gets background red color
                backgroundColorGreen = IOAddons::readByte(file); // Gets background green color
                backgroundColorBlue = IOAddons::readByte(file); // Gets background blue color

                if (IOAddons::readString(file) == "ed.erawtfoslaernu") { // Second header check
                    // Tile types
                    tileType = new int[requiredTilesCount+1];
                    for (int i = 0; i <= requiredTilesCount; i++) {
                        tileType[i] = IOAddons::readByte(file); // Saving tile types into an array
                    }

                    // Tile frames
                    tileFrame = new int*[mapWidth+1]; // Setting up array to store tile frame
                    for (int x = 0; x <= mapWidth; x++) {
                        tileFrame[x] = new int[mapHeight+1]; // Adding a second dimension to declared array
                        for (int y = 0; y <= mapHeight; y++) {
                            tileFrame[x][y] = IOAddons::readByte(file); // Saving tile frames into a 2D array
                        }
                    }

                    // Map modifiers
                    if (useModifiers == 1) {

                        // Setting up arrays to store modifier data
                        tileModifier = new int*[mapWidth+1];
                        tileModificationFrame = new int*[mapWidth+1];
                        tileColorRed = new int*[mapWidth+1];
                        tileColorGreen = new int*[mapWidth+1];
                        tileColorBlue = new int*[mapWidth+1];
                        tileOverlayFrame = new int*[mapWidth+1];
                        for (int x = 0; x <= mapWidth; x++) {

                            // Adding a second dimension to declared arrays
                            tileModifier[x] = new int[mapHeight+1];
                            tileModificationFrame[x] = new int[mapHeight+1];
                            tileColorRed[x] = new int[mapHeight+1];
                            tileColorGreen[x] = new int[mapHeight+1];
                            tileColorBlue[x] = new int[mapHeight+1];
                            tileOverlayFrame[x] = new int[mapHeight+1];
                            for (int y = 0; y <= mapHeight; y++) {
                                tileModifier[x][y] = IOAddons::readByte(file); // Gets tile modifier
                                int modifier = tileModifier[x][y]; // Variable for shorter usage

                                if ((modifier & 128) || (modifier & 64)) {
                                    if ((modifier & 64) && (modifier & 128)) {
                                        IOAddons::readString(file); // Reads unused string
                                    } else if ((modifier & 64) || !(modifier & 128)) {
                                        tileModificationFrame[x][y] = IOAddons::readByte(file); // Gets modification frame of that tile
                                        tileColorRed[x][y] = 0;
                                        tileColorGreen[x][y] = 0;
                                        tileColorBlue[x][y] = 0;
                                        tileOverlayFrame[x][y] = 0;
                                    } else {
                                        tileColorRed[x][y] = IOAddons::readByte(file); // Gets red color value of that tile
                                        tileColorGreen[x][y] = IOAddons::readByte(file); // Gets green color value of that tile
                                        tileColorBlue[x][y] = IOAddons::readByte(file); // Gets blue color value of that tile
                                        tileOverlayFrame[x][y] = IOAddons::readByte(file); // Gets overlay frame of that tile
                                        tileModificationFrame[x][y] = 0;
                                    }
                                }
                            }
                        }
                    }

                    // Entities
                    entityCount = IOAddons::readInt(file); // Gets a number of entities used in the map

                    // Setting up arrays to store entity data
                    entityName = new std::string[entityCount];
                    entityTrigger = new std::string[entityCount];
                    entityType = new int[entityCount];
                    entityX = new int[entityCount];
                    entityY = new int[entityCount];

                    // Setting up arrays to store setting inputs
                    entitySettingInt = new int*[entityCount];
                    entitySettingString = new std::string*[entityCount];
                    for (int i = 0; i < entityCount; i ++) {
                        entityName[i] = IOAddons::readString(file); // Gets name input of the entity
                        entityType[i] = IOAddons::readByte(file); // Gets entity type
                        entityX[i] = IOAddons::readInt(file); // Gets x position of the entity
                        entityY[i] = IOAddons::readInt(file); // Gets y position of the entity
                        entityTrigger[i] = IOAddons::readString(file); // Gets trigger input of the entity

                        // Adding second dimension to setting inputs arrays
                        entitySettingInt[i] = new int[10];
                        entitySettingString[i] = new std::string[10];
                        for (int j = 0; j < 10; j++) {
                            entitySettingInt[i][j] = IOAddons::readInt(file); // Gets int setting input
                            entitySettingString[i][j] = IOAddons::readString(file); // Gets string setting input
                        }
                    }

                    mapLoaded = true; // Map is loaded

                    return 0; // Map loaded; operation was successful
                } else {
                    return 4; // Second header check failed; operation failed
                }
            } else {
                return 3; // First header check failed; operation failed
            }
        } else {
            return 2; // Map file wasn't found; operation failed
        }
    } else {
        return 1; // Map is already loaded; operation failed
    }
}

// Following function will remove all the map data allocated on heap
// Returns 0 if operation was successful
// Returns 1 if map wasn't loaded (failure)
int MapSystem::unloadMap() {
    // If we have memory allocated in heap, remove it to prevent memory leaks
    if (mapLoaded) { // Checks if map is loaded
        delete[] tileType; // Removing tile types array

        // Removing tile frames array
        for (int x = 0; x <= mapWidth; x++) {
            delete[] tileFrame[x];
        }
        delete[] tileFrame;

        // Removing modifier related arrays
        if (useModifiers) { // Checks if map is using modifiers
            for (int x = 0; x <= mapWidth; x++) {
                delete[] tileModifier[x];
                delete[] tileModificationFrame[x];
                delete[] tileColorRed[x];
                delete[] tileColorGreen[x];
                delete[] tileColorBlue[x];
                delete[] tileOverlayFrame[x];
            }
            delete[] tileModifier;
            delete[] tileModificationFrame;
            delete[] tileColorRed;
            delete[] tileColorGreen;
            delete[] tileColorBlue;
            delete[] tileOverlayFrame;
        }

        // Removing entity related arrays
        delete[] entityName;
        delete[] entityTrigger;
        delete[] entityType;
        delete[] entityX;
        delete[] entityY;
        for (int i = 0; i < entityCount; i++) {
            delete[] entitySettingInt[i];
            delete[] entitySettingString[i];
        }
        delete[] entitySettingInt;
        delete[] entitySettingString;

        mapLoaded = false;

        return 0; // Heap allocated memory got removed, operation successful
    } else {
        return 1; // Map wasn't even loaded, nothing to remove (failure)
    }
}

// Following function will store map data into .map file
// Returns 0 if operation succeeded
// Returns 1 if file was not found
int MapSystem::saveMap(std::string filePath) {
    std::ofstream file(filePath, std::ios::binary); // Opening output stream
    if (!file.fail()) {
        IOAddons::writeString(file, "Unreal Software's Counter-Strike 2D Map File (max)"); // Writes first header

        // Byte settings
        IOAddons::writeByte(file, scrollMapLikeTiles); // Will map scroll like tiles?
        IOAddons::writeByte(file, useModifiers); // Will map use modifiers?
        for (int i = 0; i < 8; i++) { // Fills in unused settings bytes
            IOAddons::writeByte(file, 0);
        }

        // Int settings
        IOAddons::writeInt(file, upTime); // Stores uptime
        IOAddons::writeInt(file, USGNID); // Stores USGN ID of the map author
        for (int i = 0; i < 8; i++) { // Fills in unused settings ints
            IOAddons::writeInt(file, 0);
        }

        // String settings
        IOAddons::writeString(file, authorName); // Stores author's name
        for (int i = 0; i < 9; i++) { // Fills in unused settings strings
            IOAddons::writeString(file, "");
        }

        // More map settings
        IOAddons::writeString(file, generateSpecialString()); // Stores specially generated string
        IOAddons::writeString(file, tilesetFileName); // Stores tileset filename
        IOAddons::writeByte(file, requiredTilesCount); // Stores number of required tiles
        IOAddons::writeInt(file, mapWidth); // Stores map width
        IOAddons::writeInt(file, mapHeight); // Stores map height
        IOAddons::writeString(file, backgroundFileName); // Stores background filename
        IOAddons::writeInt(file, mapScrollXSpeed); // Stores map scroll x speed
        IOAddons::writeInt(file, mapScrollYSpeed); // Stores map scroll y speed
        IOAddons::writeByte(file, backgroundColorRed); // Stores background red value
        IOAddons::writeByte(file, backgroundColorGreen); // Stores background green value
        IOAddons::writeByte(file, backgroundColorBlue); // Stores background blue value

        // Second header
        IOAddons::writeString(file, "ed.erawtfoslaernu"); // Writes second header

        // Tile types
        for (int i = 0; i <= requiredTilesCount; i++) {
            IOAddons::writeByte(file, tileType[i]); // Stores tile types
        }

        // Tile frames
        for (int x = 0; x <= mapWidth; x++) {
            for (int y = 0; y <= mapHeight; y++) {
                IOAddons::writeByte(file, tileFrame[x][y]); // Stores tile frames
            }
        }

        // Tile modifiers
        if (useModifiers == 1) {
            for (int x = 0; x <= mapWidth; x++) {
                for (int y = 0; y <= mapHeight; y++) {
                    IOAddons::writeByte(file, tileModifier[x][y]); // Stores tile modifier
                    int modifier = tileModifier[x][y]; // Variable for shorter usage

                    if ((modifier & 128) || (modifier & 64)) {
                        if ((modifier & 64) && (modifier & 128)) {
                            IOAddons::writeString(file, ""); // Writes empty string
                        } else if ((modifier & 64) || !(modifier & 128)) {
                            IOAddons::writeByte(file, tileModificationFrame[x][y]); // Stores modification frame
                        } else {
                            IOAddons::writeByte(file, tileColorRed[x][y]); // Stores red color value of that tile
                            IOAddons::writeByte(file, tileColorGreen[x][y]); // Stores green color value of that tile
                            IOAddons::writeByte(file, tileColorBlue[x][y]); // Stores blue color value of that tile
                            IOAddons::writeByte(file, tileOverlayFrame[x][y]); // Stores tile overlay frame
                        }
                    }
                }
            }
            // Removing modifier related arrays from heap memory
        }

        // Entities
        IOAddons::writeInt(file, entityCount); // Stores number of entities used in this map

        for (int i = 0; i < entityCount; i++) {
            IOAddons::writeString(file, entityName[i]); // Stores entity name input
            IOAddons::writeByte(file, entityType[i]); // Stores entity type
            IOAddons::writeInt(file, entityX[i]); // Stores entity x position
            IOAddons::writeInt(file, entityY[i]); // Stores entity y position
            IOAddons::writeString(file, entityTrigger[i]); // Stores entity trigger input

            for (int j = 0; j < 10; j++) {
                IOAddons::writeInt(file, entitySettingInt[i][j]); // Stores entity settings ints
                IOAddons::writeString(file, entitySettingString[i][j]); // Stores entity settings strings
            }
        }

        return 0; // Map saved; operation was successful
    } else {
        return 1; // File wasn't found; operation failed
    }
}

// Following function will generate the tile generation script in Lua
// Should be called BEFORE the removeTiles() function!!!
// Returns 0 if operation was successful
// Returns 1 if map is not loaded (failure)
// Returns 2 if file was failed to load (failure)
int MapSystem::generateLuaScript(std::string filePath) {
    if (mapLoaded) { // Checks if map is loaded
        std::ofstream file;
        file.open(filePath); // Opens output file stream

        if (!(file.fail())) { // Checks if loading file was successful
            // Generates the Lua script
            file << "mapProtection = {\n";
            file << "    map = {\n";
            for (int x = 0; x <= mapWidth; x++) {
                file << "        [" << x << "] = {\n";
                for (int y = 0; y <= mapHeight; y++) {
                    file << "            [" << y << "] = " << tileFrame[x][y] << ";\n";
                }
                file << "        };\n";
            }
            file << "    };\n\n";
            file << "    generateMap = function()\n";
            file << "        for x = 0, map'xsize' do\n";
            file << "            for y = 0, map'ysize' do\n";
            file << "                parse('settile '.. x ..' '.. y ..' '.. mapProtection.map[x][y])\n";
            file << "            end\n";
            file << "        end\n";
            file << "    end;\n";
            file << "}\n\n";
            file << "mapProtection.generateMap()";

            return 0; // Script was generated, operation was successful
        } else {
            return 2; // File wasn't found, operation failed
        }
    } else {
        return 1; // Map isn't loaded, operation failed
    }
}

// Following function will remove all the tiles from loaded map with the exception of tiles which have modifiers in it
// Returns 0 if operation was successful
// Returns 1 if map is not loaded (failure)
int MapSystem::removeTiles() {
    if (mapLoaded) { // If map is loaded
        int mapException[mapWidth + 1][mapHeight + 1]; // Declares a 2D array which will decide which tile WON'T get removed
        memset(mapException, 0, sizeof(mapException[0][0]) * (mapWidth+1) * (mapHeight+1)); // Defaulting everything in the array to 0
        // Checking tiles for modifiers, if they do have modifiers, add them to the exception array
        if (useModifiers) { // Does map have modifiers enabled?
            for (int x = 0; x <= mapWidth; x++) {
                for (int y = 0; y <= mapHeight; y++) {
                    if (tileModifier[x][y] != 0) { // Checks if the modifier in this tile is not equals to zero
                        int m = tileModificationFrame[x][y]; // Shortcut for faster usage
                        if (m == 7 || m == 15 || m == 23 || m == 31 || m == 39) {
                            mapException[x-1][y-1] = 1;
                        } else if (m == 1 || m == 9 || m == 17 || m == 25 || m == 33) {
                            mapException[x+1][y-1] = 1;
                        } else if (m == 3 || m == 11 || m == 19 || m == 27 || m == 35) {
                            mapException[x+1][y+1] = 1;
                        } else if (m == 5 || m == 13 || m == 21 || m == 29 || m == 37) {
                            mapException[x-1][y+1] = 1;
                        } else if (m == 6 || m == 14 || m == 22 || m == 30 || m == 38) {
                            mapException[x-1][y] = 1;
                        } else if (m == 0 || m == 8 || m == 16 || m == 24 || m == 32) {
                            mapException[x][y-1] = 1;
                        } else if (m == 4 || m == 12 || m == 20 || m == 28 || m == 36) {
                            mapException[x][y+1] = 1;
                        } else if (m == 2 || m == 10 || m == 18 || m == 26 || m == 34) {
                            mapException[x+1][y] = 1;
                        }
                        mapException[x][y] = 1;
                    }
                }
            }
        }

        // Removing tiles from map
        for (int x = 0; x <= mapWidth; x++) {
            for (int y = 0; y <= mapHeight; y++) {
                if (mapException[x][y] == 0) { // Checks if this tile is not in the exceptions array
                    tileFrame[x][y] = 0; // Removes tile
                }
            }
        }
        return 0; // Map tiles are removed, operation was successful
    } else {
        return 1; // Map is not loaded, operation failed
    }
}

// Returns special string used in saveMap() function
std::string MapSystem::generateSpecialString() {
    // Too much complicated crap is going on here, cba explaining...
    // Although it's probably not even complicated
    std::time_t rawtime;
    std::tm* timeinfo;
    char timeString[7];
    char resultString[256];
    unsigned int upTime = GetTickCount();

    std::time(&rawtime);
    timeinfo = std::localtime(&rawtime);

    std::strftime(timeString, 80, "%H%M%S", timeinfo);

    sprintf(resultString, "%dx%d$%s%%%d", mapWidth*mapHeight, requiredTilesCount, timeString, upTime);
    return resultString;
}
