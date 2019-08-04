#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

#include "IOAddons.h"
#include "MapSystem.h"

enum AppState {MAIN_MENU, EXIT, SELECT_FILE, SELECT_FILE_PROCEED, INFO_PROCEED, OPERATION};

const std::string DATE_OF_COMPLETION = "08.05.2015";
const std::string VERSION = "v2.0";

int main()
{
    MapSystem *mapSystem = new MapSystem;

    int appState = MAIN_MENU;
    std::string input;
    std::string mapPath;

    // Printing out the header on the start of the application
    std::cout << "================================\n";
    std::cout << "=Welcome to Rain's Map Defense!=\n";
    std::cout << "=    Current version: " << VERSION << "     =\n";
    std::cout << "================================\n\n";

    // Main loop
    // If appState will be equaled to EXIT, then the application will close
    while (appState != EXIT) {
        if (appState == MAIN_MENU) {
            // Main menu
            std::cout << "Choose one of the following options:\n";
            std::cout << "[S] - Protect a map\n";
            std::cout << "[A] - About\n";
            std::cout << "[E] - Exit\n";
            std::cout << "Option: ";

            std::cin >> input;

            // Making the input lowercase
            std::transform(input.begin(), input.end(), input.begin(), ::tolower);
            if (input == "s") {
                appState = SELECT_FILE;
                std::cout << "\n";
            } else if (input == "a") {
                std::cout << "\n== About the application ==\n";
                std::cout << "Author: Infinite Rain (USGN: 16770)\n";
                std::cout << "Version: " << VERSION << "\n";
                std::cout << "Date of completion: " << DATE_OF_COMPLETION << "\n\n";
            } else if (input == "e") {
                appState = EXIT;
            } else {
                std::cout << "\nIncorrect input!\n\n";
            }
        } else if (appState == SELECT_FILE) {
            // Selection of .map file
            std::cout << "Please specify a path to the .map file you wish to protect.\n";
            std::cout << "Input [ABORT] to abort the operation and return to the main menu.\n";
            std::cout << "Path: ";

            std::cin >> input;
            std::string lowerInput = input; // Making a lowerInput variable which will hold input in lower case
            // But also keeping the unchanged input to get the specified path
            std::transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(), ::tolower);
            if (lowerInput == "abort") {
                appState = MAIN_MENU;
                std::cout << "\n";
            } else {
                // Checking if file on specified path exists
                std::ifstream inputStream;
                inputStream.open(input);
                if (inputStream.fail()) {
                    std::cout << "\nSpecified file doesn't exist! (" << input << ")\n\n";
                } else {
                    inputStream.close(); // Closing the input stream
                    mapPath = input; // Saving the path to the map
                    appState = SELECT_FILE_PROCEED;
                    std::cout << "\n";
                }
            }
        } else if (appState == SELECT_FILE_PROCEED) {
            // Making sure that specified path is correct
            std::cout << "Path you specified is: " << mapPath << ". Proceed?\n";
            std::cout << "Input [ABORT] to abort the operation and return to the main menu.\n";
            std::cout << "Option [Y/N/ABORT]: ";

            std::cin >> input;
            // Making input lowercase
            std::transform(input.begin(), input.end(), input.begin(), ::tolower);
            if (input == "y") {
                // Checking if map file is valid as well as loading the map itself
                if (mapSystem->loadMap(mapPath) == 0) {
                    appState = INFO_PROCEED;
                    std::cout << "\n";
                } else {
                    appState = SELECT_FILE;
                    std::cout << "\nSpecified map file contains invalid map data!\n\n";
                }
            } else if (input == "n") {
                appState = SELECT_FILE; // Sending user to re-select the .map file
                std::cout << "\n";
            } else if (input == "abort") {
                appState = MAIN_MENU; // Sending user back to main menu
                std::cout << "\n";
            } else {
                std::cout << "\nIncorrect input!\n\n";
            }
        } else if (appState == INFO_PROCEED) {
            // Printing out the info and asking if user wants to proceed
            std::cout << "This application will generate a tileless copy of the map and a\n";
            std::cout << "Lua script which will regenerate the correct tiles. If you will\n";
            std::cout << "use the generated .map and Lua files on your server, it means\n";
            std::cout << "that anyone who will join the server will download a blank map,\n";
            std::cout << "while your server will have the correct map.\n\n";
            std::cout << "This application WILL NOT overwrite the existing map.\n\n";

            std::cout << "Proceed?\n";
            std::cout << "Input [ABORT] to abort the operation and return to the main menu.\n";
            std::cout << "Option [Y/N/ABORT]: ";

            std::cin >> input;
            std::transform(input.begin(), input.end(), input.begin(), ::tolower);
            if (input == "y") {
                // If yes, proceed to the operation
                appState = OPERATION;
                std::cout << "\n";
            } else if (input == "n") {
                appState = SELECT_FILE; // Sending user to re-select the .map file
                mapSystem->unloadMap(); // Unloading the currently loaded map
                std::cout << "\n";
            } else if (input == "abort") {
                appState = MAIN_MENU; // Sending user back to main menu
                mapSystem->unloadMap(); // Unloading the currently loaded map
                std::cout << "\n";
            } else {
                std::cout << "\nIncorrect input!\n\n";
            }
        } else if (appState == OPERATION) {
            // Operation of generating the tileless map and Lua script
            // Getting rid of full path and extension to leave out map name only
            std::string name = mapPath;
            if (name.find_last_of("\\/") != std::string::npos) {
                name.erase(0, name.find_last_of("\\/") + 1);
            }

            if (name.find_last_of(".") != std::string::npos) {
                name.erase(name.find_last_of("."));
            }

            // Getting path to the folder where map file is located
            std::string folderPath = mapPath;
            if (folderPath.find_last_of("\\/") != std::string::npos) {
                folderPath.erase(folderPath.find_last_of("\\/")+1);
            } else {
                folderPath = "";
            }

            // Generating Lua script
            std::cout << "Generating the Lua script...\n";
            mapSystem->generateLuaScript(folderPath + name + " (Map generation script).lua");
            std::cout << "Done! Saved as \"" << name << " (Map generation script).lua\".\n\n";

            // Generating tileless copy of the map
            std::cout << "Generating a tileless copy of the map...\n";
            mapSystem->removeTiles();
            mapSystem->saveMap(folderPath + name + " (Tileless version).map");
            std::cout << "Done! Saved as \"" << name << " (Tileless version).map\".\n";

            mapSystem->unloadMap(); // Unloading the currently loaded map
            appState = MAIN_MENU; // Sending user back to main menu
            std::cout << "\nOperation was successful!\n\n";
        }
    }

    return 0;
}
