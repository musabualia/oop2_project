#pragma once
#include "Core/Constants.h"
#include <vector>

class DynamicWaveGenerator {
public:
    // Structure representing the composition of a wave
    struct WaveComposition {
        int basicRobots = 0;  // Number of basic robots
        int fireRobots = 0;   // Number of fire robots
        int stealthRobots = 0; // Number of stealth robots
        int totalRobots = 0;  // Total robots in the wave
        std::vector<GameTypes::RobotType> spawnOrder; // ✅ Randomized spawn order
    };

    // Main method to generate wave composition based on level and wave number
    static WaveComposition generateWave(int level, int wave);

private:
    // Calculate total number of robots for a given level and wave
    static int calculateTotalRobots(int level, int wave);

    // Distribute robot types for early levels
    static void distributeEarlyLevelRobots(WaveComposition& comp, int level, int wave);

    // Distribute robot types for later levels
    static void distributeLaterLevelRobots(WaveComposition& comp, int level, int wave);

    // Generate a randomized order of robot spawning
    static void generateRandomSpawnOrder(WaveComposition& comp);
};
