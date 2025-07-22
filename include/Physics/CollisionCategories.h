
#pragma once
#include <box2d/box2d.h>
#include <cstdint>

namespace CollisionCategory {
    static constexpr uint16_t ROBOT = 0x0001;          
    static constexpr uint16_t SQUAD_MEMBER = 0x0002;     
    static constexpr uint16_t PROJECTILE = 0x0004;      
    static constexpr uint16_t ROBOT_BULLET = 0x0008;    
    static constexpr uint16_t COLLECTIBLE = 0x0010;     
    static constexpr uint16_t SENSOR = 0x0040;          
}

namespace CollisionMask {

    static constexpr uint16_t ROBOT = CollisionCategory::SQUAD_MEMBER | CollisionCategory::PROJECTILE | CollisionCategory::COLLECTIBLE;
    static constexpr uint16_t SQUAD_MEMBER = CollisionCategory::ROBOT | CollisionCategory::ROBOT_BULLET | CollisionCategory::COLLECTIBLE;
    static constexpr uint16_t PROJECTILE = CollisionCategory::ROBOT;
    static constexpr uint16_t ROBOT_BULLET = CollisionCategory::SQUAD_MEMBER;
    static constexpr uint16_t COLLECTIBLE = CollisionCategory::ROBOT | CollisionCategory::SQUAD_MEMBER;
    static constexpr uint16_t WALLS = CollisionCategory::ROBOT | CollisionCategory::PROJECTILE | CollisionCategory::ROBOT_BULLET;
}