#pragma once
#include "GameObject.h"

class StaticObject : public GameObject {
public:
    virtual ~StaticObject() = default;                         // Default destructor

    GameTypes::ObjectCategory getObjectCategory() const override {
        return GameTypes::ObjectCategory::Unknown;             // Default category for static objects
    }

    void createPhysicsBody(b2World* world) override {          // Create default static box body
        createBoxPhysicsBody(world, false, 40.0f, 40.0f, 1.0f, 0.3f);
    }

    void setLane(int lane);                                    // Set object's lane
    int getLane() const;                                       // Get object's lane

    void setGridPosition(int x, int y);                        // Set grid position
    sf::Vector2i getGridPosition() const;                      // Get grid position

    virtual bool canBePlacedAt(int lane, int gridX) const;     // Check if can be placed at position
    static bool isValidGridPosition(int gridX, int lane);      // Check if grid position is valid

    static sf::Vector2f gridToWorldPosition(int gridX, int lane);                // Convert grid to world position
    static sf::Vector2i worldToGridPosition(const sf::Vector2f& worldPos);       // Convert world to grid position

protected:
    int m_lane = 0;                                            // Object's lane index
    sf::Vector2i m_gridPosition{ 0, 0 };                       // Object's grid position
    bool m_isPlaced = false;                                  // Is object placed on the grid

    void snapToGrid();                                        // Align position to grid
};
