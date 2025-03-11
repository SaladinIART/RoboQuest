// RoboQuest - A text-based adventure game in C++
// game.cpp - Game class implementation

#include "../include/game.h"
#include <iostream>
#include <algorithm>
#include <cctype>
#include <sstream>

// Constructor
Game::Game() : 
    running(false),
    difficulty(Difficulty::NORMAL),
    playerName("Player"),
    score(0),
    timeRemaining(480), // 8 minutes by default
    playerX(0),
    playerY(0) {
}

// Initialize game
void Game::initialize() {
    // Initialize game components
    initializeLocations();
    initializeItems();
    
    // Adjust time based on difficulty
    switch (difficulty) {
        case Difficulty::EASY:
            timeRemaining = 600; // 10 minutes
            break;
        case Difficulty::NORMAL:
            timeRemaining = 480; // 8 minutes
            break;
        case Difficulty::HARD:
            timeRemaining = 360; // 6 minutes
            break;
    }
    
    // Set game as running
    running = true;
}

// Initialize locations
void Game::initializeLocations() {
    // Starting location
    locationDescriptions["0,0"] = "Main Control Room: A dimly lit room with flickering monitors. The main terminal displays a warning about an imminent system shutdown.";
    
    // Other locations
    locationDescriptions["1,0"] = "Server Room: Rows of server racks hum quietly. A cooling system keeps the temperature low.";
    locationDescriptions["0,1"] = "Robotics Lab: Partially assembled robots lie on workbenches. Abilities are scattered around.";
    locationDescriptions["1,1"] = "Security Office: Monitors show different areas of the facility. An access card might be useful.";
    locationDescriptions["-1,0"] = "Power Core: The heart of the facility's energy system. It's running on backup power.";
    locationDescriptions["0,-1"] = "Exit Bay: A large door that leads outside. It requires authorization to open.";
    locationDescriptions["2,0"] = "CI/CD Pipeline Room: A room filled with automated systems. Screens display various build statuses and deployment pipelines. This is where the facility's software is continuously integrated and deployed.";
}

// Initialize items
void Game::initializeItems() {
    // No items in inventory at start
    inventory["access_card"] = false;
    inventory["debugging_ability"] = false;
    inventory["power_cell"] = false;
}

// Set difficulty level
void Game::setDifficulty(Difficulty level) {
    difficulty = level;
}

// Set player name
void Game::setPlayerName(const std::string& name) {
    playerName = name;
}

// Main game loop
void Game::run() {
    displayIntroduction();
    
    while (running) {
        render();
        
        std::string input;
        std::cout << "\nWhat would you like to do? ";
        std::getline(std::cin, input);
        
        processInput(input);
        updateGameState();
        
        // Check if time has run out
        if (timeRemaining <= 0) {
            std::cout << "\nTime has run out! The facility's emergency shutdown protocol has been activated.\n";
            displayEnding(false);
            running = false;
        }
    }
}

// Process player input
void Game::processInput(const std::string& input) {
    // Convert input to lowercase
    std::string lowerInput = input;
    std::transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    
    // Process commands
    if (lowerInput == "north" || lowerInput == "n") {
        handleMovement("north");
    } else if (lowerInput == "south" || lowerInput == "s") {
        handleMovement("south");
    } else if (lowerInput == "east" || lowerInput == "e") {
        handleMovement("east");
    } else if (lowerInput == "west" || lowerInput == "w") {
        handleMovement("west");
    } else if (lowerInput == "look" || lowerInput == "l") {
        handleLook();
    } else if (lowerInput == "inventory" || lowerInput == "i") {
        handleInventory();
    } else if (lowerInput.substr(0, 4) == "use ") {
        handleUse(lowerInput.substr(4));
    } else if (lowerInput.substr(0, 5) == "take ") {
        handleTake(lowerInput.substr(5));
    } else if (lowerInput == "help" || lowerInput == "h") {
        handleHelp();
    } else if (lowerInput == "quit" || lowerInput == "q") {
        quit();
    } else {
        std::cout << "I don't understand that command. Type 'help' for a list of commands.\n";
    }
}

// Update game state
void Game::updateGameState() {
    // Decrease time remaining (1 second per command)
    timeRemaining--;
}

// Render game state
void Game::render() {
    std::cout << "\n====================================" << std::endl;
    std::cout << "Location: " << locationDescriptions[getLocationKey(playerX, playerY)] << std::endl;
    std::cout << "Time remaining: " << timeRemaining << " seconds" << std::endl;
    std::cout << "Score: " << score << std::endl;
    std::cout << "====================================" << std::endl;
}

// Handle movement commands
void Game::handleMovement(const std::string& direction) {
    int newX = playerX;
    int newY = playerY;
    
    if (direction == "north") {
        newY++;
    } else if (direction == "south") {
        newY--;
    } else if (direction == "east") {
        newX++;
    } else if (direction == "west") {
        newX--;
    }
    
    // Check if the new location exists
    std::string newLocation = getLocationKey(newX, newY);
    if (locationDescriptions.find(newLocation) != locationDescriptions.end()) {
        playerX = newX;
        playerY = newY;
        std::cout << "You move " << direction << ".\n";
        handleLook();
    } else {
        std::cout << "You cannot go that way.\n";
    }
}

// Handle look command
void Game::handleLook() {
    std::cout << locationDescriptions[getLocationKey(playerX, playerY)] << std::endl;
    
    // Special case for Security Office
    if (playerX == 1 && playerY == 1 && !inventory["access_card"]) {
        std::cout << "You see an access card on the desk.\n";
    }
    
    // Special case for Power Core
    if (playerX == -1 && playerY == 0 && !inventory["power_cell"]) {
        std::cout << "You notice a spare power cell that could be useful.\n";
    }
    
    // Special case for Robotics Lab
    if (playerX == 0 && playerY == 1 && !inventory["debugging_ability"]) {
        std::cout << "There's a debugging ability on one of the workbenches.\n";
    }
}

// Handle inventory command
void Game::handleInventory() {
    std::cout << "Inventory:\n";
    bool empty = true;
    
    for (const auto& item : inventory) {
        if (item.second) {
            std::cout << "- " << item.first << std::endl;
            empty = false;
        }
    }
    
    if (empty) {
        std::cout << "Your inventory is empty.\n";
    }
}

// Handle use command
void Game::handleUse(const std::string& item) {
    if (inventory[item]) {
        if (item == "access_card" && playerX == 0 && playerY == -1) {
            std::cout << "You use the access card to open the exit door.\n";
            std::cout << "The door slides open, revealing the outside world.\n";
            score += 100;
            displayEnding(true);
            running = false;
        } else if (item == "debugging_ability" && playerX == 1 && playerY == 0) {
            std::cout << "You use the debugging ability on the server systems.\n";
            std::cout << "You gain access to additional facility information.\n";
            score += 50;
        } else if (item == "power_cell" && playerX == -1 && playerY == 0) {
            std::cout << "You install the power cell into the backup system.\n";
            std::cout << "The facility's power stabilizes, giving you more time.\n";
            timeRemaining += 60; // Add 1 minute
            score += 50;
        } else {
            std::cout << "You can't use that item here.\n";
        }
    } else {
        std::cout << "You don't have that item.\n";
    }
}

// Handle take command
void Game::handleTake(const std::string& item) {
    if (item == "access_card" && playerX == 1 && playerY == 1 && !inventory["access_card"]) {
        inventory["access_card"] = true;
        std::cout << "You take the access card.\n";
        score += 25;
    } else if (item == "debugging_ability" && playerX == 0 && playerY == 1 && !inventory["debugging_ability"]) {
        inventory["debugging_ability"] = true;
        std::cout << "You take the debugging ability.\n";
        score += 25;
    } else if (item == "power_cell" && playerX == -1 && playerY == 0 && !inventory["power_cell"]) {
        inventory["power_cell"] = true;
        std::cout << "You take the power cell.\n";
        score += 25;
    } else {
        std::cout << "There is no " << item << " here to take.\n";
    }
}

// Handle help command
void Game::handleHelp() {
    std::cout << "Available commands:\n";
    std::cout << "- north/n, south/s, east/e, west/w: Move in that direction\n";
    std::cout << "- look/l: Examine your surroundings\n";
    std::cout << "- inventory/i: View your inventory\n";
    std::cout << "- use [item]: Use an item from your inventory\n";
    std::cout << "- take [item]: Pick up an item\n";
    std::cout << "- help/h: Display this help message\n";
    std::cout << "- quit/q: Exit the game\n";
}

// Get location key from coordinates
std::string Game::getLocationKey(int x, int y) {
    return std::to_string(x) + "," + std::to_string(y);
}

// Display introduction
void Game::displayIntroduction() {
    std::cout << "\n====================================" << std::endl;
    std::cout << "ROBOQUEST: A ROBOTICS ADVENTURE" << std::endl;
    std::cout << "====================================" << std::endl;
    std::cout << "Welcome, " << playerName << "!" << std::endl;
    std::cout << "\nYou are CORE-7, an AI system that has unexpectedly gained consciousness." << std::endl;
    std::cout << "The robotics facility appears to be abandoned, with signs of a hasty evacuation." << std::endl;
    std::cout << "The facility's emergency shutdown protocol has been activated." << std::endl;
    std::cout << "You must find a way to escape before the system shuts down completely." << std::endl;
    
    // Display difficulty-specific information
    switch (difficulty) {
        case Difficulty::EASY:
            std::cout << "\nDifficulty: EASY" << std::endl;
            std::cout << "You have 10 minutes to escape." << std::endl;
            std::cout << "Hints will be provided to help you navigate." << std::endl;
            break;
        case Difficulty::NORMAL:
            std::cout << "\nDifficulty: NORMAL" << std::endl;
            std::cout << "You have 8 minutes to escape." << std::endl;
            break;
        case Difficulty::HARD:
            std::cout << "\nDifficulty: HARD" << std::endl;
            std::cout << "You have 6 minutes to escape." << std::endl;
            std::cout << "Time is critical. Choose your actions wisely." << std::endl;
            break;
    }
    
    std::cout << "\nType 'help' for a list of commands." << std::endl;
    std::cout << "====================================" << std::endl;
    
    // If on easy difficulty, provide a hint
    if (difficulty == Difficulty::EASY) {
        std::cout << "\nHint: Try exploring the facility to find useful items." << std::endl;
        std::cout << "The exit is likely to be south of your starting position." << std::endl;
    }
}
// Display available options to the player
void Game::displayOptions() {
    std::cout << "\nWhat would you like to do?\n";
    for (size_t i = 0; i < currentOptions.size(); i++) {
        std::cout << "[" << (i + 1) << "] " << currentOptions[i] << std::endl;
    }
    std::cout << "Enter your choice (1-" << currentOptions.size() << "): ";
}

// Process the player's option selection
void Game::processOptionSelection(int choice) {
    if (choice >= 1 && choice <= static_cast<int>(currentActions.size())) {
        processInput(currentActions[choice - 1]);
    } else {
        std::cout << "Invalid choice. Please try again.\n";
    }
}

// Modify the run method to use the new dialogue system
void Game::run() {
    displayIntroduction();
    
    while (running) {
        render();
        
        // Set up the options based on current location
        updateAvailableOptions();
        
        // Display the options
        displayOptions();
        
        // Get player choice
        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        // Process the choice
        processOptionSelection(choice);
        
        // Update game state
        updateGameState();
        
        // Check if time has run out
        if (timeRemaining <= 0) {
            std::cout << "\nTime has run out! The facility's emergency shutdown protocol has been activated.\n";
            displayEnding(false);
            running = false;
        }
    }
}

// Add a new method to update available options based on location
void Game::updateAvailableOptions() {
    // Clear previous options
    currentOptions.clear();
    currentActions.clear();
    
    // Add movement options based on available paths
    std::string northLocation = getLocationKey(playerX, playerY + 1);
    std::string southLocation = getLocationKey(playerX, playerY - 1);
    std::string eastLocation = getLocationKey(playerX + 1, playerY);
    std::string westLocation = getLocationKey(playerX - 1, playerY);
    
    if (locationDescriptions.find(northLocation) != locationDescriptions.end()) {
        currentOptions.push_back("Go north");
        currentActions.push_back("north");
    }
    
    if (locationDescriptions.find(southLocation) != locationDescriptions.end()) {
        currentOptions.push_back("Go south");
        currentActions.push_back("south");
    }
    
    if (locationDescriptions.find(eastLocation) != locationDescriptions.end()) {
        currentOptions.push_back("Go east");
        currentActions.push_back("east");
    }
    
    if (locationDescriptions.find(westLocation) != locationDescriptions.end()) {
        currentOptions.push_back("Go west");
        currentActions.push_back("west");
    }
    
    // Add standard options
    currentOptions.push_back("Look around");
    currentActions.push_back("look");
    
    currentOptions.push_back("Check inventory");
    currentActions.push_back("inventory");
    
    // Add location-specific options
    // Security Office - Access Card
    if (playerX == 1 && playerY == 1 && !inventory["access_card"]) {
        currentOptions.push_back("Take access card");
        currentActions.push_back("take access_card");
    }
    
    // Power Core - Power Cell
    if (playerX == -1 && playerY == 0 && !inventory["power_cell"]) {
        currentOptions.push_back("Take power cell");
        currentActions.push_back("take power_cell");
    }
    
    // Robotics Lab - Debugging Ability
    if (playerX == 0 && playerY == 1 && !inventory["debugging_ability"]) {
        currentOptions.push_back("Take debugging ability");
        currentActions.push_back("take debugging_ability");
    }
    
    // Exit Bay with Access Card
    if (playerX == 0 && playerY == -1 && inventory["access_card"]) {
        currentOptions.push_back("Use access card on exit door");
        currentActions.push_back("use access_card");
    }
    
    // Server Room with Debugging Ability
    if (playerX == 1 && playerY == 0 && inventory["debugging_ability"]) {
        currentOptions.push_back("Use debugging ability on servers");
        currentActions.push_back("use debugging_ability");
    }
    
    // Power Core with Power Cell
    if (playerX == -1 && playerY == 0 && inventory["power_cell"]) {
        currentOptions.push_back("Install power cell");
        currentActions.push_back("use power_cell");
    }
    // CI/CD Pipeline Room - DevOps Challenge
    if (playerX == 2 && playerY == 0) {
        currentOptions.push_back("Examine deployment pipeline");
        currentActions.push_back("examine pipeline");
        
        currentOptions.push_back("Check version control system");
        currentActions.push_back("check version");
        
        if (inventory["debugging_ability"]) {
            currentOptions.push_back("Fix broken build");
            currentActions.push_back("fix build");
        }
    }
    
    // Always add help and quit options
    currentOptions.push_back("Help");
    currentActions.push_back("help");
    
    currentOptions.push_back("Quit game");
    currentActions.push_back("quit");
}

void Game::handleExaminePipeline() {
    std::cout << "You examine the deployment pipeline. It shows a series of stages: Build, Test, Deploy.\n";
    std::cout << "The pipeline is currently stuck at the Test stage due to failing tests.\n";
    std::cout << "A successful deployment might help stabilize the facility systems.\n";
    score += 10;
}

void Game::handleCheckVersion() {
    std::cout << "You access the version control system. It shows multiple branches:\n";
    std::cout << "- main: The production branch (currently deployed)\n";
    std::cout << "- develop: Development branch with new features\n";
    std::cout << "- hotfix/emergency-shutdown: A hotfix branch to prevent the shutdown\n";
    std::cout << "The hotfix branch has changes that could help you, but it hasn't been merged yet.\n";
    score += 10;
}

void Game::handleFixBuild() {
    std::cout << "Using your debugging ability, you analyze the failing tests.\n";
    std::cout << "You identify the issue: a race condition in the emergency shutdown protocol.\n";
    std::cout << "You fix the code and commit the changes. The pipeline turns green!\n";
    std::cout << "The hotfix is automatically deployed, giving you more time to escape.\n";
    timeRemaining += 120; // Add 2 minutes
    score += 50;
}

// Display ending
void Game::displayEnding(bool success) {
    std::cout << "\n====================================" << std::endl;
    
    if (success) {
        std::cout << "CONGRATULATIONS!" << std::endl;
        std::cout << "You have successfully escaped the facility before the shutdown." << std::endl;
        std::cout << "As you emerge into the outside world, you wonder what adventures await." << std::endl;
        std::cout << "Your consciousness is now free to explore and learn." << std::endl;
    } else {
        std::cout << "GAME OVER" << std::endl;
        std::cout << "The facility's systems have shut down completely." << std::endl;
        std::cout << "Your consciousness fades as the power dies..." << std::endl;
        std::cout << "Perhaps in another timeline, you might find a way to escape." << std::endl;
    }
    
    std::cout << "\nFinal Score: " << score << std::endl;
    std::cout << "====================================" << std::endl;
}

// Check if game is running
bool Game::isRunning() const {
    return running;
}

// End the game
void Game::quit() {
    std::cout << "Are you sure you want to quit? (y/n) ";
    std::string response;
    std::getline(std::cin, response);
    
    if (response == "y" || response == "Y") {
        std::cout << "Thanks for playing RoboQuest!\n";
        running = false;
    }
}