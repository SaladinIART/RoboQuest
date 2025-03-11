// RoboQuest - A text-based adventure game in C++
// main.cpp - Entry point for the application

#include <iostream>
#include <string>
#include <limits>
#include "../include/game.h"

// Function to clear the input buffer
void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main() {
    // Display welcome message
    std::cout << "====================================" << std::endl;
    std::cout << "Welcome to RoboQuest - A Robotics Adventure" << std::endl;
    std::cout << "====================================" << std::endl;
    std::cout << "Version 0.1 - Initial Setup" << std::endl;
    std::cout << "====================================" << std::endl;
    
    // Create game instance
    Game game;
    
    // Get player name
    std::string playerName;
    std::cout << "Enter your name: ";
    std::getline(std::cin, playerName);
    game.setPlayerName(playerName);
    
    // Select difficulty
    int difficultyChoice = 0;
    std::cout << "\nSelect difficulty:" << std::endl;
    std::cout << "1. Easy (10 minutes, hints provided)" << std::endl;
    std::cout << "2. Normal (8 minutes)" << std::endl;
    std::cout << "3. Hard (6 minutes, no hints)" << std::endl;
    std::cout << "Enter your choice (1-3): ";
    
    std::cin >> difficultyChoice;
    clearInputBuffer(); // Clear the input buffer
    
    // Set difficulty based on choice
    switch (difficultyChoice) {
        case 1:
            game.setDifficulty(Difficulty::EASY);
            break;
        case 2:
            game.setDifficulty(Difficulty::NORMAL);
            break;
        case 3:
            game.setDifficulty(Difficulty::HARD);
            break;
        default:
            std::cout << "Invalid choice. Setting difficulty to Normal." << std::endl;
            game.setDifficulty(Difficulty::NORMAL);
            break;
    }
    
    // Initialize and run the game
    game.initialize();
    game.run();
    
    // Game has ended
    std::cout << "\nThank you for playing RoboQuest!" << std::endl;
    
    // Wait for user input before closing
    std::cout << "\nPress Enter to exit...";
    std::cin.get();
    
    return 0;
}