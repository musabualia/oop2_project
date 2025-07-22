#pragma once
#include "Commands/ICommand.h"
#include <SFML/Graphics.hpp>
#include <vector>

class Bomb;                                // Forward declaration

class PlaceBombCommand : public ICommand {
public:
    PlaceBombCommand(std::vector<Bomb>* bombList, const sf::Vector2f& position);   // Create command to place bomb
    virtual ~PlaceBombCommand() = default;                                         // Default destructor

    void execute() override;                          // Place the bomb
    void undo() override;                             // Undo bomb placement
    bool canUndo() const override;                    // Check if bomb can be undone
    std::string getDescription() const override;      // Get description of the command

    sf::Vector2f getPosition() const { return m_position; }     // Get bomb position
    bool hasExploded() const;                                   // Check if bomb has exploded

private:
    std::vector<Bomb>* m_bombList;           // Reference to bomb list
    sf::Vector2f m_position;                 // Bomb placement position
    size_t m_bombIndex;                      // Index of placed bomb
    bool m_bombPlaced;                       // Whether bomb was placed

    bool isBombStillValid() const;           // Validate if bomb still exists
    Bomb* getBombAtIndex() const;            // Get bomb from list by index
};
