// ================================
// Dynamic Wave Generator - Strategy Pattern for Progressive Difficulty Scaling
// ================================
#include "Managers/DynamicWaveGenerator.h"
#include <algorithm>
#include <cmath>
#include <random>
#include <iostream>

// Main wave generation method - creates balanced waves based on level and wave number
DynamicWaveGenerator::WaveComposition DynamicWaveGenerator::generateWave(int level, int wave) {
    WaveComposition composition;

    // Calculate total robot count based on progression formula
    composition.totalRobots = calculateTotalRobots(level, wave);

    // Distribute robots by type based on level progression
    if (level <= 8) {
        // Early levels: Only Basic and Stealth robots for learning curve
        distributeEarlyLevelRobots(composition, level, wave);
    }
    else {
        // Later levels: Introduce Fire robots for increased difficulty
        distributeLaterLevelRobots(composition, level, wave);
    }

    // Generate randomized spawn order for unpredictable gameplay
    generateRandomSpawnOrder(composition);

    return composition;
}

// Calculate total robot count with gradual difficulty progression
int DynamicWaveGenerator::calculateTotalRobots(int level, int wave) {
    int baseRobots = 3;                          // Minimum robots per wave
    int levelBonus = (level - 1) * 1;           // +1 robot per level (gradual increase)
    int waveBonus = wave - 1;                   // +1 robot per wave within level
    int difficultySpike = (level > 5) ? (level - 5) / 2 : 0;  // Extra robots after level 5

    return baseRobots + levelBonus + waveBonus + difficultySpike;
}

// Distribute robots for levels 1-8 (Basic + Stealth only)
void DynamicWaveGenerator::distributeEarlyLevelRobots(WaveComposition& comp, int level, int wave) {
    if (level == 1) {
        // Level 1: Gentle introduction to stealth robots
        if (wave == 1) {
            comp.basicRobots = comp.totalRobots;  // All basic robots
            comp.stealthRobots = 0;
        }
        else if (wave == 2) {
            comp.basicRobots = comp.totalRobots - 1;  // Mostly basic
            comp.stealthRobots = 1;                   // Introduce 1 stealth
        }
        else { // wave 3
            comp.basicRobots = comp.totalRobots - 2;  // Less basic
            comp.stealthRobots = 2;                   // More stealth
        }
    }
    else if (level <= 3) {
        // Levels 2-3: Gradual increase of stealth robots (25-50%)
        float stealthPercentage = 0.2f + (level * 0.1f) + (wave * 0.05f);
        stealthPercentage = std::min(stealthPercentage, 0.5f);  // Cap at 50%

        comp.stealthRobots = std::max(1, static_cast<int>(comp.totalRobots * stealthPercentage));
        comp.basicRobots = comp.totalRobots - comp.stealthRobots;
    }
    else if (level <= 5) {
        // Levels 4-5: More balanced mix (30-60%)
        float stealthPercentage = 0.3f + (wave * 0.1f);
        stealthPercentage = std::min(stealthPercentage, 0.6f);

        comp.stealthRobots = static_cast<int>(comp.totalRobots * stealthPercentage);
        comp.basicRobots = comp.totalRobots - comp.stealthRobots;
    }
    else { // levels 6-8
        // Levels 6-8: Stealth robots become dominant (40-70%)
        float stealthPercentage = 0.4f + (level - 6) * 0.1f + (wave * 0.05f);
        stealthPercentage = std::min(stealthPercentage, 0.7f);

        comp.stealthRobots = static_cast<int>(comp.totalRobots * stealthPercentage);
        comp.basicRobots = comp.totalRobots - comp.stealthRobots;
    }

    // Ensure minimum counts for balanced gameplay
    if (comp.basicRobots < 1 && comp.totalRobots > 1) {
        comp.basicRobots = 1;
        comp.stealthRobots = comp.totalRobots - 1;
    }
}

// Distribute robots for levels 9+ (Basic + Stealth + Fire)
void DynamicWaveGenerator::distributeLaterLevelRobots(WaveComposition& comp, int level, int wave) {
    // Calculate Fire robot percentage (15-35% based on level and wave)
    float firePercentage = 0.15f + ((level - 9) * 0.03f) + (wave * 0.02f);
    firePercentage = std::min(firePercentage, 0.35f);  // Cap at 35%

    // Calculate Stealth robot percentage (35-50%)
    float stealthPercentage = 0.35f + (wave * 0.03f);
    stealthPercentage = std::min(stealthPercentage, 0.5f);  // Cap at 50%

    // Distribute robots based on calculated percentages
    comp.fireRobots = std::max(1, static_cast<int>(comp.totalRobots * firePercentage));
    comp.stealthRobots = static_cast<int>((comp.totalRobots - comp.fireRobots) *
        (stealthPercentage / (1.0f - firePercentage)));
    comp.basicRobots = comp.totalRobots - comp.fireRobots - comp.stealthRobots;

    // Ensure minimum basic robots for balanced gameplay
    if (comp.basicRobots < 1) {
        comp.basicRobots = 1;
        if (comp.stealthRobots > 1) {
            comp.stealthRobots--;
        }
        else if (comp.fireRobots > 1) {
            comp.fireRobots--;
        }
    }
}

// Generate random spawn order for unpredictable enemy waves
void DynamicWaveGenerator::generateRandomSpawnOrder(WaveComposition& comp) {
    comp.spawnOrder.clear();
    comp.spawnOrder.reserve(comp.totalRobots);

    // Add all robot types to spawn order list
    for (int i = 0; i < comp.basicRobots; ++i) {
        comp.spawnOrder.push_back(GameTypes::RobotType::Basic);
    }
    for (int i = 0; i < comp.stealthRobots; ++i) {
        comp.spawnOrder.push_back(GameTypes::RobotType::Stealth);
    }
    for (int i = 0; i < comp.fireRobots; ++i) {
        comp.spawnOrder.push_back(GameTypes::RobotType::Fire);
    }

    // Shuffle the spawn order for unpredictable gameplay
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(comp.spawnOrder.begin(), comp.spawnOrder.end(), gen);
}