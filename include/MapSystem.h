#ifndef MAPSYSTEM_H
#define MAPSYSTEM_H

#include <string>

class MapSystem
{
    public:
        ~MapSystem(); // Destructor

        int loadMap(std::string filePath); // Loads map file from specified file
        int unloadMap(); // Removes all the map data allocated on heap
        int saveMap(std::string filePath); // Saves map file to specified file

        int generateLuaScript(std::string filePath); // Generates tile generation script in Lua and stores it into file
        int removeTiles(); // Removes tiles from currently loaded map

        std::string generateSpecialString(); // Returns special string used in saveMap() function
    private:
        // Misc variables
        bool mapLoaded = false; // Is map loaded?

        // Map related variables
        int scrollMapLikeTiles; // Will map scroll like tiles?
        int useModifiers; // Will modifiers be used?

        int upTime; // Uptime of system when map was created
        int USGNID; // USGNID of the author

        std::string authorName; // Username of the author
        std::string tilesetFileName; // Tileset filename
        std::string backgroundFileName; // Background filename

        int requiredTilesCount; // Number of tiles required
        int entityCount = 0; // Number of entities in the map

        int mapWidth; // Width of the map
        int mapHeight; // Height of the map
        int mapScrollXSpeed; // Scroll x speed of the map
        int mapScrollYSpeed; // Scroll y speed of the map
        int backgroundColorRed; // Background red color value
        int backgroundColorGreen; // Background green color value
        int backgroundColorBlue; // Background blue color value
        int* tileType; // Tile types
        int** tileFrame; // Tile frames

        int** tileModifier; // Tile modifiers
        int** tileModificationFrame; // Tile modification frame
        int** tileColorRed; // Tile red color value
        int** tileColorGreen; // Tile green color value
        int** tileColorBlue; // Tile blue color value
        int** tileOverlayFrame; // Tile overlay frame

        std::string* entityName; // Entity name input
        std::string* entityTrigger; // Entity trigger input
        int* entityType; // Entity type
        int* entityX; // Entity x position
        int* entityY; // Entity y position

        int** entitySettingInt; // Entity settings ints
        std::string** entitySettingString; // Entity setting strings
};

#endif // MAPSYSTEM_H
