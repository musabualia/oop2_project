// ================================
// include/Core/Constants.h - UPDATED with Economy System
// ================================
#pragma once
#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

// ================================
// ENUMS - All Game Type Definitions
// ================================
namespace GameTypes {
    // Object identification for collision detection
    enum class ObjectCategory {
        Robot,
        SquadMember,
        Projectile,
        Collectible,
        Unknown
    };

    // Robot types and states
    enum class RobotType {
        Basic, Fire, Stealth
    };

    enum class RobotAIState {
        Moving, Attacking, UsingAbility, Stunned, Dying
    };

    enum class FireRobotState {
        Walking,                    // Robot walks for 4-5 seconds
        Shooting,
        Dead// Brief pause before resuming walking
    };

    // Squad member types and targeting
    enum class SquadMemberType {
        HeavyGunner, Sniper, ShieldBearer
    };

    enum class TargetPriority {
        Closest, Strongest, Weakest, First, Last
    };

    // Projectile types and targeting
    enum class ProjectileType {
        Bullet, SniperBullet, Explosion, RobotBullet
    };

    enum class ProjectileTargetType {
        Robot, Position, SquadMember
    };

    // Collectible types and power-ups
    enum class CollectibleType {
        Coin, HealthPack, BonusPoints
    };

    // Healing types for health packs
    enum class HealType {
        Instant, OverTime, AreaOfEffect
    };

    // Game states
    enum class GameState {
        Menu, Playing, Paused, GameOver, Victory, Settings
    };

    // UI and input
    enum class ButtonState {
        Normal, Hovered, Pressed, Disabled
    };

    enum class PlacementState {
        None, Selecting, Placing, Upgrading
    };

    // Wave countdown system
    enum class CountdownPhase {
        None,
        GetReady,
        WaveDisplay,
        Complete
    };

    enum class WaveState {
        Preparing,
        Active,
        Completed,
        AllCompleted
    };
}

// ================================
// CORE GAME CONSTANTS
// ================================
namespace GameConstants {

    // ================================
    // PHYSICS CONSTANTS
    // ================================
    namespace Physics {
        const float DEFAULT_PIXELS_PER_METER = 30.0f;
        const b2Vec2 DEFAULT_GRAVITY(0.0f, 9.8f);
        const float DEFAULT_TIMESTEP = 1.0f / 60.0f;
        const int DEFAULT_VELOCITY_ITERATIONS = 6;
        const int DEFAULT_POSITION_ITERATIONS = 2;

        // Runtime values loaded from config
        extern float PIXELS_PER_METER;
        extern b2Vec2 GRAVITY;
        extern int VELOCITY_ITERATIONS;
        extern int POSITION_ITERATIONS;
    }

    // ================================
    // TIMING CONSTANTS
    // ================================
    const float FRAME_RATE = 60.0f;
    const float FIXED_TIMESTEP = 1.0f / FRAME_RATE;

    constexpr unsigned int WINDOW_WIDTH = 1200;
    constexpr unsigned int WINDOW_HEIGHT = 800;

    // ================================
    // GAME GRID CONFIGURATION
    // ================================
    constexpr int GRID_COLUMNS = 7;                   // Number of cells horizontally
    constexpr int GRID_ROWS = 3;                      // Number of lanes vertically

    constexpr float GRID_CELL_WIDTH = 100.0f;         // Width of each grid cell
    constexpr float GRID_CELL_HEIGHT = 120.0f;        // Height of each grid cell (matching lane height)

    constexpr float GRID_OFFSET_X = 100.0f;           // X offset from the left screen edge
    constexpr float GRID_OFFSET_Y = 185.0f;           // Y offset from the top screen edge

    // ================================
    // COMBAT SYSTEM
    // ================================
    const float PROJECTILE_LIFETIME = 5.0f;

    // ================================
    // ECONOMY SYSTEM
    // ================================
    namespace Economy {
        // Harder level-based starting coins calculation
        inline int calculateStartingCoins(int level) {
            if (level <= 5) {
                // Levels 1-5: start at 200, +50 per level
                // Level 1: 200, Level 2: 250, ..., Level 5: 400
                return 200 + ((level - 1) * 50);
            }
            else if (level <= 10) {
                // Levels 6-10: slower growth, +30 per level from level 6
                // Level 6: 430, ..., Level 10: 550
                return 430 + ((level - 6) * 30);
            }
            else {
                // Levels 11+: very slow growth, +20 per level
                // Level 11: 570, Level 12: 590, etc.
                return 570 + ((level - 11) * 20);
            }
        }

        // Ensure coins are multiples of 10 for consistent spending
        inline int roundCoins(int coins) {
            return ((coins + 5) / 10) * 10;
        }

        // Final starting coins per level
        inline int getStartingCoinsForLevel(int level) {
            return roundCoins(calculateStartingCoins(level));
        }
    }


    const int BASE_HEALTH = 10;

    // ================================
    // UPGRADE SYSTEM
    // ================================
    const float UPGRADE_MULTIPLIER = 1.5f;
    const int MAX_UPGRADE_LEVEL = 3;
    const float UPGRADE_COST_MULTIPLIER = 1.5f;

    // ================================
    // COMBAT SYSTEM (Extended)
    // ================================
    const float EXPLOSION_RADIUS = 50.0f;
    const float ROBOT_SPAWN_INTERVAL = 4.5f;
    const float WAVE_SPAWN_INTERVAL = 20.0f;

    // ================================
    // COLLECTIBLE SYSTEM
    // ================================
    const int HEALTH_PACK_VALUE = 50;
    const float COLLECTIBLE_LIFETIME = 5.0f;
    const float COLLECTION_RADIUS = 30.0f;

    // ================================
    // AUDIO SYSTEM
    // ================================
    namespace Audio {
        const float MASTER_VOLUME = 100.0f;
        const float MUSIC_VOLUME = 70.0f;
        const float SFX_VOLUME = 80.0f;
        const float UI_VOLUME = 90.0f;
    }

    // ================================
    // UI CONFIGURATION
    // ================================
    const int LEVEL_ICON_SIZE = 190;
    const float LEVEL_ICON_SCALE = 0.09f;

    // ================================
    // COLLISION (LEGACY - Use Box2D instead)
    // ================================
    const float COLLISION_TOLERANCE = 5.0f;

    // ================================
    // FIRE ROBOT SPECIFIC CONSTANTS  
    // ================================
    namespace FireRobot {
        constexpr float DETECTION_UPDATE_INTERVAL = 0.1f;   // Detection check frequency
    }

    // ================================
    // COUNTDOWN SYSTEM CONSTANTS
    // ================================
    namespace Countdown {
        const float GET_READY_DURATION = 2.0f;    // "GET READY FOR COMBAT!" duration
        const float COUNTDOWN_DURATION = 1.0f;    // Each countdown number duration
        const float FADE_DURATION = 0.3f;         // Fade in/out duration
    }
}