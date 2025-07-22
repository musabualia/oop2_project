#include "Entities/Base/SquadMember.h"
#include "Entities/Base/Robot.h"
#include "Core/Constants.h"
#include "Utils/ConfigLoader.h"
#include <cmath>
#include <algorithm>
#include <limits>

SquadMember::SquadMember(GameTypes::SquadMemberType type, int cost, float range, int damage)
    : m_type(type), m_baseCost(cost), m_damage(damage), m_range(range), m_attackCooldown(1.0f) {
    m_attackTimer.setDuration(m_attackCooldown);
}

SquadMember::~SquadMember() {
    if (!m_attackingRobots.empty()) {
        m_attackingRobots.clear();
        m_underAttack = false;
    }
}

void SquadMember::update(float dt) {
    if (m_isDying) {
        m_deathAnimTimer += dt;
        syncSpriteFromPhysics();
        if (m_deathAnimTimer >= m_deathAnimDuration) {
            setActive(false);
        }
        return;
    }

    if (isDestroyed() && !m_isDying) {
        m_isDying = true;
        playDeathAnimation();
        return;
    }

    m_attackTimer.update(dt);
    m_abilityTimer.update(dt);
    syncSpriteFromPhysics();
}

void SquadMember::draw(sf::RenderWindow& window) const {
    drawHealthBar(window);
}

void SquadMember::drawHealthBar(sf::RenderWindow& window) const {
    if (m_isDying || isDestroyed()) return;

    const float barWidth = 50.0f;
    const float barHeight = 8.0f;
    const float barOffsetX = 25.0f;
    const float barOffsetY = 95.0f;

    float healthPercent = static_cast<float>(m_health) / static_cast<float>(m_maxHealth);
    sf::Vector2f unitPos = getPosition();
    sf::Vector2f barPos(unitPos.x - barOffsetX, unitPos.y - barOffsetY);

    sf::RectangleShape healthBarBg(sf::Vector2f(barWidth, barHeight));
    healthBarBg.setPosition(barPos);
    healthBarBg.setFillColor(sf::Color::Transparent);
    healthBarBg.setOutlineColor(sf::Color(50, 50, 50, 150));
    healthBarBg.setOutlineThickness(1.0f);

    sf::RectangleShape healthBarFg(sf::Vector2f(barWidth * healthPercent, barHeight));
    healthBarFg.setPosition(barPos);

    sf::Color healthColor;
    if (healthPercent > 0.75f) {
        healthColor = sf::Color::Green;
    }
    else if (healthPercent > 0.5f) {
        healthColor = sf::Color(128, 255, 0);
    }
    else if (healthPercent > 0.25f) {
        healthColor = sf::Color::Yellow;
    }
    else {
        healthColor = sf::Color::Red;
    }

    healthBarFg.setFillColor(healthColor);

    window.draw(healthBarBg);
    if (healthPercent > 0.0f) {
        window.draw(healthBarFg);
    }
}

GameTypes::SquadMemberType SquadMember::getType() const { return m_type; }
int SquadMember::getCost() const { return m_baseCost; }
int SquadMember::getBaseCost() const { return m_baseCost; }
float SquadMember::getRange() const { return m_range; }
int SquadMember::getDamage() const { return m_damage; }

void SquadMember::playDeathAnimation() {
    m_deathAnimDuration = 1.0f;
}

bool SquadMember::canAttack() const { return m_attackTimer.isElapsed(); }
void SquadMember::resetAttackTimer() { m_attackTimer.restart(); }
float SquadMember::getAttackCooldown() const { return m_attackCooldown; }

void SquadMember::setTargetPriority(GameTypes::TargetPriority priority) { m_targetPriority = priority; }
GameTypes::TargetPriority SquadMember::getTargetPriority() const { return m_targetPriority; }
Robot* SquadMember::getCurrentTarget() const { return m_currentTarget; }

Robot* SquadMember::findTarget(const std::vector<std::unique_ptr<Robot>>& robots) const {
    Robot* bestTarget = nullptr;
    float bestValue = 0.0f;

    for (const auto& robot : robots) {
        if (!robot || !robot->isActive() || !isInRange(robot.get()) || !isValidTarget(robot.get()))
            continue;

        float value = 0.0f;
        float distance = distanceToTarget(robot.get());

        switch (m_targetPriority) {
        case GameTypes::TargetPriority::Closest:  value = 1000.0f - distance; break;
        case GameTypes::TargetPriority::Strongest: value = static_cast<float>(robot->getHealth()); break;
        case GameTypes::TargetPriority::Weakest:   value = 1000.0f - static_cast<float>(robot->getHealth()); break;
        case GameTypes::TargetPriority::First:     value = 1000.0f; break;
        case GameTypes::TargetPriority::Last:      value = distance; break;
        }

        if (value > bestValue) {
            bestValue = value;
            bestTarget = robot.get();
        }

        if (m_targetPriority == GameTypes::TargetPriority::First && bestTarget)
            break;
    }

    return bestTarget;
}

void SquadMember::takeDamage(int damage) {
    if (isDestroyed() || m_isDying) return;

    m_health = std::max(0, m_health - damage);

    if (m_health <= 0) {
        if (m_underAttack && !m_attackingRobots.empty()) {
            std::vector<Robot*> attackersCopy = m_attackingRobots;
            m_attackingRobots.clear();
            m_underAttack = false;

            for (Robot* robot : attackersCopy) {
                if (robot) {
                    robot->stopAttackingSquadMember();
                }
            }
        }

        if (!m_isDying) {
            m_isDying = true;
            playDeathAnimation();
        }
    }
}

void SquadMember::removeAttacker(Robot* attacker) {
    if (!attacker) return;

    auto it = std::find(m_attackingRobots.begin(), m_attackingRobots.end(), attacker);
    if (it != m_attackingRobots.end()) {
        m_attackingRobots.erase(it);
    }

    m_underAttack = !m_attackingRobots.empty();
}

void SquadMember::heal(int amount) {
    if (isDestroyed() || m_isDying || amount <= 0) return;

    m_health = std::min(m_maxHealth, m_health + amount);
}

int SquadMember::getHealth() const { return m_health; }
int SquadMember::getMaxHealth() const { return m_maxHealth; }
bool SquadMember::isDestroyed() const { return m_health <= 0; }

bool SquadMember::canBePlacedAt(int lane, int gridX) const {
    return isValidGridPosition(gridX, lane);
}

void SquadMember::onRobotCollision(Robot* attacker) {
    if (!attacker || isDestroyed() || m_isDying) return;

    auto it = std::find(m_attackingRobots.begin(), m_attackingRobots.end(), attacker);
    if (it == m_attackingRobots.end()) {
        m_attackingRobots.push_back(attacker);
    }

    m_underAttack = !m_attackingRobots.empty();
}

void SquadMember::onRobotBulletHit(int damage) {
    if (isDestroyed() || m_isDying) return;

    takeDamage(damage);
}

void SquadMember::updateTargeting(const std::vector<std::unique_ptr<Robot>>& robots) {
    m_currentTarget = findTarget(robots);
}

void SquadMember::updateAttack(float dt) {}

float SquadMember::distanceToTarget(const Robot* robot) const {
    return robot ? distanceTo(*robot) : std::numeric_limits<float>::max();
}

bool SquadMember::isInRange(const Robot* robot) const {
    return distanceToTarget(robot) <= m_range;
}

bool SquadMember::isValidTarget(const Robot* robot) const {
    return robot && robot->isActive() && !robot->isDead();
}

void SquadMember::healByPercentage(float percentage) {
    if (isDestroyed() || m_isDying || percentage <= 0.0f) return;

    float currentPercent = (static_cast<float>(m_health) / static_cast<float>(m_maxHealth)) * 100.0f;
    float newPercent = std::min(100.0f, currentPercent + percentage);
    int newHealth = static_cast<int>((newPercent / 100.0f) * m_maxHealth);

    int actualHealAmount = newHealth - m_health;
    if (actualHealAmount > 0) {
        m_health = newHealth;
    }
}

bool SquadMember::canBeHealed() const {
    return !isDestroyed() && !m_isDying && m_health < m_maxHealth;
}

void SquadMember::initializeConfigFromName(const std::string& configName, float defaultAttackSpeed) {
    auto& config = ConfigLoader::getInstance();

    try {
        m_health = config.getInt(configName, "health", 100);
        m_maxHealth = m_health;
        m_range = config.getFloat(configName, "range", 200.0f);
        m_baseCost = config.getInt(configName, "cost", 40);
        m_attackCooldown = config.getFloat(configName, "attackSpeed", defaultAttackSpeed);

        m_attackTimer.setDuration(m_attackCooldown);
    }
    catch (const std::exception& e) {
        // Keep existing defaults set in constructor
    }
}