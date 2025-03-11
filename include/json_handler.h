// Create a new file: include/json_handler.h
#ifndef JSON_HANDLER_H
#define JSON_HANDLER_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <ctime>

struct ScoreEntry {
    std::string playerName;
    int score;
    std::string difficulty;
    std::string date;
};

class JsonHandler {
private:
    std::string filePath;
    std::vector<ScoreEntry> scores;
    
    std::string getCurrentDate() {
        time_t now = time(0);
        struct tm* timeinfo = localtime(&now);
        char buffer[80];
        strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);
        return std::string(buffer);
    }
    
public:
    JsonHandler(const std::string& path) : filePath(path) {
        loadScores();
    }
    
    void loadScores() {
        scores.clear();
        
        std::ifstream file(filePath);
        if (!file.is_open()) {
            return;
        }
        
        // Simple parsing of our custom format
        std::string line;
        while (std::getline(file, line)) {
            size_t nameEnd = line.find(',');
            if (nameEnd == std::string::npos) continue;
            
            std::string name = line.substr(0, nameEnd);
            
            size_t scoreEnd = line.find(',', nameEnd + 1);
            if (scoreEnd == std::string::npos) continue;
            
            int score = std::stoi(line.substr(nameEnd + 1, scoreEnd - nameEnd - 1));
            
            size_t diffEnd = line.find(',', scoreEnd + 1);
            if (diffEnd == std::string::npos) continue;
            
            std::string difficulty = line.substr(scoreEnd + 1, diffEnd - scoreEnd - 1);
            std::string date = line.substr(diffEnd + 1);
            
            scores.push_back({name, score, difficulty, date});
        }
        
        file.close();
    }
    
    void saveScore(const std::string& playerName, int score, const std::string& difficulty) {
        ScoreEntry entry;
        entry.playerName = playerName;
        entry.score = score;
        entry.difficulty = difficulty;
        entry.date = getCurrentDate();
        
        scores.push_back(entry);
        
        // Save to file
        std::ofstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file for writing: " << filePath << std::endl;
            return;
        }
        
        for (const auto& entry : scores) {
            file << entry.playerName << "," 
                 << entry.score << "," 
                 << entry.difficulty << "," 
                 << entry.date << std::endl;
        }
        
        file.close();
    }
    
    void displayHighScores() {
        if (scores.empty()) {
            std::cout << "No high scores yet!" << std::endl;
            return;
        }
        
        // Sort scores (highest first)
        std::sort(scores.begin(), scores.end(), 
                 [](const ScoreEntry& a, const ScoreEntry& b) {
                     return a.score > b.score;
                 });
        
        std::cout << "\n===== HIGH SCORES =====\n";
        int count = 0;
        for (const auto& entry : scores) {
            std::cout << (count + 1) << ". " << entry.playerName 
                     << " - " << entry.score << " points"
                     << " (" << entry.difficulty << ")"
                     << " on " << entry.date << std::endl;
            
            count++;
            if (count >= 5) break; // Show only top 5
        }
        std::cout << "======================\n";
    }
};

#endif // JSON_HANDLER_H