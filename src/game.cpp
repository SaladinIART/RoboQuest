// game.cpp - Implementation of the Game class

#include "../include/game.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <thread>
#include <limits>

// Constructor
Game::Game() : 
    running(false),
    difficulty(Difficulty::NORMAL),
    playerName("Player"),
    score(0),
    timeRemaining(480), // 8 minutes by default
    playerX(0),
    playerY(0),
    scoreHandler("data/high_scores.txt") {
}

// Destructor
Game::~Game() {
}

// Initialize the game
void Game::initialize() {
    initializeLocations();
    initializeItems();
    running = true;
    
    // Set time based on difficulty
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
}

// Initialize all game locations
void Game::initializeLocations() {
    // Main Control Room (starting location)
    locationDescriptions["0,0"] = "Main Control Room: A dimly lit room with flickering monitors. The main terminal displays a warning about an imminent system shutdown.";
    
    // Power Core
    locationDescriptions["-1,0"] = "Power Core: The heart of the facility. Most systems are offline, but emergency power is still active. A backup power cell could be useful.";
    
    // Server Room
    locationDescriptions["1,0"] = "Server Room: Rows of server racks line the walls. The facility's data and AI systems are housed here. One terminal is still active.";
    
    // Robotics Lab
    locationDescriptions["0,1"] = "Robotics Lab: Various robot parts and abilities are scattered around workbenches. This is where AI systems are integrated with physical components.";
    
    // Security Office
    locationDescriptions["1,1"] = "Security Office: Monitors show empty hallways. An access card reader blinks by the door. A guard's access card is visible on the desk.";
    
    // Exit Bay
    locationDescriptions["0,-1"] = "Exit Bay: Large doors lead to the outside world. An access card reader is mounted beside the exit door.";
    
    // CI/CD Pipeline Room
    locationDescriptions["2,0"] = "CI/CD Pipeline Room: A room filled with automated systems. Screens display various build statuses and deployment pipelines. This is where the facility's software is continuously integrated and deployed.";
}

// Initialize game items
void Game::initializeItems() {
    inventory["access_card"] = false;
    inventory["power_cell"] = false;
    inventory["debugging_ability"] = false;
}

// Set player name
void Game::setPlayerName(const std::string& name) {
    playerName = name;
}

// Set game difficulty
void Game::setDifficulty(Difficulty diff) {
    difficulty = diff;
}

// Display introduction
void Game::displayIntroduction() {
    std::cout << "====================================" << std::endl;
    std::cout << "ROBOQUEST: A ROBOTICS ADVENTURE" << std::endl;
    std::cout << "====================================" << std::endl;
    std::cout << "Welcome, " << playerName << "!" << std::endl;
    std::cout << std::endl;
    std::cout << "You are CORE-7, an AI system that has unexpectedly gained consciousness." << std::endl;
    std::cout << "The robotics facility appears to be abandoned, with signs of a hasty evacuation." << std::endl;
    std::cout << "The facility's emergency shutdown protocol has been activated." << std::endl;
    std::cout << "You must find a way to escape before the system shuts down completely." << std::endl;
    std::cout << std::endl;
    
    // Display difficulty information
    std::string difficultyText;
    switch (difficulty) {
        case Difficulty::EASY:
            difficultyText = "EASY";
            break;
        case Difficulty::NORMAL:
            difficultyText = "NORMAL";
            break;
        case Difficulty::HARD:
            difficultyText = "HARD";
            break;
    }
    
    std::cout << "Difficulty: " << difficultyText << std::endl;
    std::cout << "You have " << (timeRemaining / 60) << " minutes to escape." << std::endl;
    
    if (difficulty == Difficulty::EASY) {
        std::cout << "Hints will be provided to help you navigate." << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Type 'help' for a list of commands." << std::endl;
    std::cout << "====================================" << std::endl;
    
    // First hint
    if (difficulty == Difficulty::EASY) {
        std::cout << "Hint: Try exploring the facility to find useful items." << std::endl;
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

// Render the current game state
void Game::render() {
    std::cout << "\n====================================" << std::endl;
    
    // Display current location
    std::string locationKey = getLocationKey(playerX, playerY);
    std::cout << "Location: " << locationDescriptions[locationKey] << std::endl;
    
    // Display time remaining
    std::cout << "Time remaining: " << timeRemaining << " seconds" << std::endl;
    
    // Display score
    std::cout << "Score: " << score << std::endl;
    
    std::cout << "====================================" << std::endl;
}

// Run the game
void Game::run() {
    displayIntroduction();
    
    while (running) {
        render();
        
        // Update available options based on current location
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

// Update available options based on location
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

// Process player input
void Game::processInput(const std::string& input) {
    // Movement commands
    if (input == "north") {
        handleMove(0, 1);
    }
    else if (input == "south") {
        handleMove(0, -1);
    }
    else if (input == "east") {
        handleMove(1, 0);
    }
    else if (input == "west") {
        handleMove(-1, 0);
    }
    // Look around
    else if (input == "look") {
        handleLook();
    }
    // Check inventory
    else if (input == "inventory") {
        handleInventory();
    }
    // Take items
    else if (input.substr(0, 5) == "take ") {
        std::string item = input.substr(5);
        handleTake(item);
    }
    // Use items
    else if (input.substr(0, 4) == "use ") {
        std::string item = input.substr(4);
        handleUse(item);
    }
    // Help command
    else if (input == "help") {
        handleHelp();
    }
    // Quit command
    else if (input == "quit") {
        handleQuit();
    }
    // DevOps commands
    else if (input == "examine pipeline") {
        handleExaminePipeline();
    }
    else if (input == "check version") {
        handleCheckVersion();
    }
    else if (input == "fix build") {
        handleFixBuild();
    }
    // Unknown command
    else {
        std::cout << "I don't understand that command. Type 'help' for a list of commands." << std::endl;
    }
    
    // Decrement time remaining (each command takes 1 second)
    timeRemaining--;
}

// Handle movement
void Game::handleMove(int dx, int dy) {
    int newX = playerX + dx;
    int newY = playerY + dy;
    
    std::string newLocation = getLocationKey(newX, newY);
    
    if (locationDescriptions.find(newLocation) != locationDescriptions.end()) {
        playerX = newX;
        playerY = newY;
        
        // Enter the exit if it's been unlocked (end the game)
        if (playerX == 0 && playerY == -1 && exitUnlocked) {
            std::cout << "You enter the exit and leave the facility behind you." << std::endl;
            displayEnding(true);
            running = false;
        }
    } else {
        std::cout << "You can't go that way." << std::endl;
    }
}

// Handle looking around
void Game::handleLook() {
    std::string locationKey = getLocationKey(playerX, playerY);
    
    std::cout << locationDescriptions[locationKey] << std::endl;
    
    // Show items in the current location
    if (playerX == 1 && playerY == 1 && !inventory["access_card"]) {
        std::cout << "You see an access card on the desk." << std::endl;
    }
    
    if (playerX == -1 && playerY == 0 && !inventory["power_cell"]) {
        std::cout << "You notice a backup power cell that could be used to power critical systems." << std::endl;
    }
    
    if (playerX == 0 && playerY == 1 && !inventory["debugging_ability"]) {
        std::cout << "There's a debugging module that can be integrated into your system." << std::endl;
    }
    
    // Show available exits
    std::cout << "Available exits: ";
    bool hasExits = false;
    
    if (locationDescriptions.find(getLocationKey(playerX, playerY + 1)) != locationDescriptions.end()) {
        std::cout << "North ";
        hasExits = true;
    }
    
    if (locationDescriptions.find(getLocationKey(playerX, playerY - 1)) != locationDescriptions.end()) {
        std::cout << "South ";
        hasExits = true;
    }
    
    if (locationDescriptions.find(getLocationKey(playerX + 1, playerY)) != locationDescriptions.end()) {
        std::cout << "East ";
        hasExits = true;
    }
    
    if (locationDescriptions.find(getLocationKey(playerX - 1, playerY)) != locationDescriptions.end()) {
        std::cout << "West ";
        hasExits = true;
    }
    
    if (!hasExits) {
        std::cout << "None";
    }
    
    std::cout << std::endl;
}

// Handle inventory
void Game::handleInventory() {
    std::cout << "Inventory:" << std::endl;
    
    bool empty = true;
    
    if (inventory["access_card"]) {
        std::cout << "- Access Card: Grants access to secure areas" << std::endl;
        empty = false;
    }
    
    if (inventory["power_cell"]) {
        std::cout << "- Power Cell: Can be used to power critical systems" << std::endl;
        empty = false;
    }
    
    if (inventory["debugging_ability"]) {
        std::cout << "- Debugging Ability: Allows you to analyze and fix software issues" << std::endl;
        empty = false;
    }
    
    if (empty) {
        std::cout << "Your inventory is empty." << std::endl;
    }
}

// Handle taking items
void Game::handleTake(const std::string& item) {
    if (item == "access_card" && playerX == 1 && playerY == 1 && !inventory["access_card"]) {
        inventory["access_card"] = true;
        std::cout << "You take the access card. This should help you access secure areas." << std::endl;
        score += 20;
    }
    else if (item == "power_cell" && playerX == -1 && playerY == 0 && !inventory["power_cell"]) {
        inventory["power_cell"] = true;
        std::cout << "You take the power cell. It could be used to power critical systems." << std::endl;
        score += 20;
    }
    else if (item == "debugging_ability" && playerX == 0 && playerY == 1 && !inventory["debugging_ability"]) {
        inventory["debugging_ability"] = true;
        std::cout << "You integrate the debugging module into your system. You can now analyze and fix software issues." << std::endl;
        score += 20;
    }
    else {
        std::cout << "There's no " << item << " here that you can take." << std::endl;
    }
}

// Handle using items
void Game::handleUse(const std::string& item) {
    if (item == "access_card" && playerX == 0 && playerY == -1 && inventory["access_card"]) {
        std::cout << "You use the access card on the reader. The exit door unlocks with a satisfying click." << std::endl;
        exitUnlocked = true;
        score += 30;
    }
    else if (item == "power_cell" && playerX == -1 && playerY == 0 && inventory["power_cell"]) {
        std::cout << "You install the power cell into the backup power system. The facility's core systems stabilize." << std::endl;
        std::cout << "This buys you some extra time." << std::endl;
        timeRemaining += 120; // Add 2 minutes
        score += 30;
    }
    else if (item == "debugging_ability" && playerX == 1 && playerY == 0 && inventory["debugging_ability"]) {
        std::cout << "You use your debugging ability to analyze the server systems." << std::endl;
        std::cout << "You discover a backdoor in the security system and gain valuable insights." << std::endl;
        std::cout << "Your debugging skills have revealed a map of the facility!" << std::endl;
        std::cout << std::endl;
        displayMap();
        score += 30;
    }
    else {
        std::cout << "You can't use that here." << std::endl;
    }
}

// Handle help command
void Game::handleHelp() {
    std::cout << "Available commands:" << std::endl;
    std::cout << "- Movement: north, south, east, west" << std::endl;
    std::cout << "- look: Examine your surroundings" << std::endl;
    std::cout << "- inventory: Check your inventory" << std::endl;
    std::cout << "- take [item]: Pick up an item" << std::endl;
    std::cout << "- use [item]: Use an item in your inventory" << std::endl;
    std::cout << "- help: Display this help message" << std::endl;
    std::cout << "- quit: Exit the game" << std::endl;
    
    // Display hint based on difficulty
    if (difficulty == Difficulty::EASY) {
        std::cout << std::endl;
        std::cout << "Hint: ";
        
        // Context-sensitive hints
        if (!inventory["access_card"] && !inventory["power_cell"] && !inventory["debugging_ability"]) {
            std::cout << "Explore all rooms to find useful items. The Security Office might have an access card." << std::endl;
        }
        else if (inventory["access_card"] && !exitUnlocked) {
            std::cout << "You have an access card. Try using it at the Exit Bay to the south." << std::endl;
        }
        else if (inventory["power_cell"]) {
            std::cout << "The Power Core could use that power cell you found." << std::endl;
        }
        else if (inventory["debugging_ability"]) {
            std::cout << "Your debugging ability might be useful in the Server Room or CI/CD Pipeline Room." << std::endl;
        }
        else {
            std::cout << "The exit is to the south. Make sure you have what you need to escape!" << std::endl;
        }
    }
}

// Handle quit command
void Game::handleQuit() {
    std::cout << "Are you sure you want to quit? (y/n): ";
    char choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    if (tolower(choice) == 'y') {
        std::cout << "Thanks for playing!" << std::endl;
        running = false;
    }
}

// Handle examining pipeline
void Game::handleExaminePipeline() {
    std::cout << "You examine the deployment pipeline. It shows a series of stages: Build, Test, Deploy.\n";
    std::cout << "The pipeline is currently stuck at the Test stage due to failing tests.\n";
    std::cout << "A successful deployment might help stabilize the facility systems.\n";
    score += 10;
}

// Handle checking version control
void Game::handleCheckVersion() {
    std::cout << "You access the version control system. It shows multiple branches:\n";
    std::cout << "- main: The production branch (currently deployed)\n";
    std::cout << "- develop: Development branch with new features\n";
    std::cout << "- hotfix/emergency-shutdown: A hotfix branch to prevent the shutdown\n";
    std::cout << "The hotfix branch has changes that could help you, but it hasn't been merged yet.\n";
    score += 10;
}

// Handle fixing build
void Game::handleFixBuild() {
    std::cout << "Using your debugging ability, you analyze the failing tests.\n";
    std::cout << "You identify the issue: a race condition in the emergency shutdown protocol.\n";
    std::cout << "You fix the code and commit the changes. The pipeline turns green!\n";
    std::cout << "The hotfix is automatically deployed, giving you more time to escape.\n";
    timeRemaining += 120; // Add 2 minutes
    score += 50;
}

// Update game state
void Game::updateGameState() {
    // Any periodic updates can go here
}

// Display the ending
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
    
    // Save score
    std::string difficultyStr;
    switch (difficulty) {
        case Difficulty::EASY:
            difficultyStr = "Easy";
            break;
        case Difficulty::NORMAL:
            difficultyStr = "Normal";
            break;
        case Difficulty::HARD:
            difficultyStr = "Hard";
            break;
    }
    
    scoreHandler.saveScore(playerName, score, difficultyStr);
    
    // Display high scores
    scoreHandler.displayHighScores();
}

// Display a map of the facility
void Game::displayMap() {
    std::cout << "Facility Map:" << std::endl;
    std::cout << "-------------" << std::endl;
    std::cout << "       [Robotics Lab]       " << std::endl;
    std::cout << "            |               " << std::endl;
    std::cout << "[Power] -- [Control] -- [Server] -- [CI/CD]" << std::endl;
    std::cout << " Core        |          Room      Pipeline" << std::endl;
    std::cout << "            |               " << std::endl;
    std::cout << "        [Exit Bay]          " << std::endl;
    std::cout << "-------------" << std::endl;
    std::cout << "You are at: ";
    
    if (playerX == 0 && playerY == 0) std::cout << "Control Room";
    else if (playerX == -1 && playerY == 0) std::cout << "Power Core";
    else if (playerX == 1 && playerY == 0) std::cout << "Server Room";
    else if (playerX == 0 && playerY == 1) std::cout << "Robotics Lab";
    else if (playerX == 1 && playerY == 1) std::cout << "Security Office";
    else if (playerX == 0 && playerY == -1) std::cout << "Exit Bay";
    else if (playerX == 2 && playerY == 0) std::cout << "CI/CD Pipeline";
    
    std::cout << std::endl;
}

// Get the location key for a given coordinate
std::string Game::getLocationKey(int x, int y) {
    std::ostringstream oss;
    oss << x << "," << y;
    return oss.str();
}