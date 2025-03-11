// RoboQuest - A text-based adventure game in C++
// game.h - Game class declaration

#ifndef GAME_H
#define GAME_H

#include <string>
#include <map>
#include <vector>

// Difficulty levels
enum class Difficulty {
    EASY,
    NORMAL,
    HARD
};

// Game class to manage the game state and logic
class Game {
private:
    // Game state variables
    bool running;
    Difficulty difficulty;
    std::string playerName;
    int score;
    int timeRemaining; // in seconds
    
    // Current player location
    int playerX;
    int playerY;
    
    // Map of location descriptions
    std::map<std::string, std::string> locationDescriptions;
    
    // Player inventory
    std::map<std::string, bool> inventory;
    
    // Initialize game components
    void initializeLocations();
    void initializeItems();
    
    // Game loop helpers
    void processInput(const std::string& input);
    void updateGameState();
    void render();
    
    // Command handlers
    void handleMovement(const std::string& direction);
    void handleLook();
    void handleInventory();
    void handleUse(const std::string& item);
    void handleTake(const std::string& item);
    void handleHelp();
    
    // Utility functions
    std::string getLocationKey(int x, int y);
    void displayIntroduction();
    void displayEnding(bool success);
    
public:
    // Constructor
    Game();
    
    // Game initialization
    void initialize();
    
    // Set difficulty level
    void setDifficulty(Difficulty level);
    
    // Set player name
    void setPlayerName(const std::string& name);
    
    // Main game loop
    void run();
    
    // Check if game is running
    bool isRunning() const;
    
    // End the game
    void quit();
};

#endif // GAME_H