#include "Entities/Base/Robot.h"
#include "Entities/Base/SquadMember.h"
#include "Managers/SquadMemberManager.h"
#include "Core/Constants.h"
#include "Physics/PhysicsUtils.h"
#include "Systems/EventSystem.h"
#include "Utils/ConfigLoader.h"
#include <cmath>
#include <random>
#include <limits>

Robot::Robot(GameTypes::RobotType type, int health, float speed, int damage)
    : m_type(type)
    , m_health(health)
    , m_maxHealth(health)
    , m_damage(damage)
    , m_rewardValue(25)
    , m_attackDamage(damage)
{
    // Apply speed variation
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> speedVariation(0.7f, 1.4f);
    m_originalSpeed = speed * speedVariation(gen);

    m_attackTimer.setDuration(m_attackCooldown);
    loadAttackRangeFromConfig();
}

void Robot::update(float dt) {
    if (isDead()) {
        handleDeathState(dt);
        return;
    }

    updateSquadMemberDetection(dt);
    updateMovement(dt);
    updateCombat(dt);
    updateAnimation(dt);
    updatePhysics(dt);
}

//  movement logic
void Robot::updateMovement(float dt) {
    bool shouldStop = isPerformingSpecialAction() ||
        isAttackingSquadMember() ||
        shouldStopForSquadMember();

    if (m_movementEnabled && !shouldStop) {
        updatePhysicsMovement(dt);
        setFacingDirection(true);
    }
    else if (shouldStop || !m_movementEnabled) {
        // Stop movement
        if (m_physicsBody) {
            m_physicsBody->SetLinearVelocity(b2Vec2(0, 0));
            m_physicsBody->SetAngularVelocity(0.0f);
        }
    }
}

//  physics movement
void Robot::updatePhysicsMovement(float dt) {
    if (!m_physicsBody) return;

    b2Vec2 currentVel = m_physicsBody->GetLinearVelocity();
    float expectedVelX = PhysicsUtils::sfmlToBox2D(-m_originalSpeed);

    if (std::abs(currentVel.x - expectedVelX) > 0.1f) {
        b2Vec2 correctVelocity(expectedVelX, 0.0f);
        m_physicsBody->SetLinearVelocity(correctVelocity);
    }

    // Keep robot stable
    m_physicsBody->SetGravityScale(0.0f);
    m_physicsBody->SetAngularVelocity(0.0f);
    m_physicsBody->SetTransform(m_physicsBody->GetPosition(), 0.0f);
}

//  combat logic
void Robot::updateCombat(float dt) {
    updateAttackBehavior(dt);
}

//  death handling
void Robot::handleDeathState(float dt) {
    if (m_physicsBody) {
        m_physicsBody->SetLinearVelocity(b2Vec2(0, 0));
        m_physicsBody->SetAngularVelocity(0.0f);
    }
    updateAnimation(dt);
}

void Robot::updateAttackBehavior(float dt) {
    if (!m_isAttackingSquadMember || !m_targetSquadMember) {
        if (m_isAttackingSquadMember && !m_targetSquadMember) {
            cleanupInvalidTarget();
        }
        return;
    }

    if (!isValidAttackTarget(m_targetSquadMember)) {
        cleanupInvalidTarget();
        return;
    }

    processAttackTarget(dt);
}

// Process attack on current target
void Robot::processAttackTarget(float dt) {
    float distance = calculateDistanceToMember(m_targetSquadMember);

    if (distance > m_attackRange + 20.0f) {
        stopAttackingSquadMember();
        return;
    }

    m_attackTimer.update(dt);
    if (m_attackTimer.isElapsed()) {
        executeAttack();
        m_attackTimer.restart();
    }
}

// Execute attack on target
void Robot::executeAttack() {
    if (m_targetSquadMember && !m_targetSquadMember->isDestroyed()) {
        try {
            m_targetSquadMember->takeDamage(m_attackDamage);
        }
        catch (...) {
            cleanupInvalidTarget();
        }
    }
}

// Check if target is valid for attack
bool Robot::isValidAttackTarget(SquadMember* target) const {
    if (!target) return false;

    try {
        return !target->isDestroyed();
    }
    catch (...) {
        return false;
    }
}

// Cleanup invalid attack target
void Robot::cleanupInvalidTarget() {
    m_targetSquadMember = nullptr;
    m_isAttackingSquadMember = false;
    setMovementEnabled(true);
    switchToWalkingAnimation();
}

void Robot::updateSquadMemberDetection(float dt) {
    if (!m_squadMemberManager) return;

    updateDetectionState();
}

// Update detection state
void Robot::updateDetectionState() {
    auto nearbyUnits = m_squadMemberManager->getActiveSquadMembersInRange(
        getPosition(), m_attackRange);

    bool foundValidTarget = false;
    for (SquadMember* unit : nearbyUnits) {
        if (checkSquadMemberInRange(unit)) {
            foundValidTarget = true;
            break;
        }
    }

    m_hasDetectedTarget = foundValidTarget;
}

// Check if squad member is in attack range
bool Robot::checkSquadMemberInRange(const SquadMember* member) const {
    if (!member || member->isDestroyed()) return false;

    sf::Vector2f robotPos = getPosition();
    sf::Vector2f memberPos = member->getPosition();

    float horizontalDiff = robotPos.x - memberPos.x;
    float verticalDiff = std::abs(robotPos.y - memberPos.y);

    bool isToLeft = horizontalDiff >= 20.0f;
    bool inSameLane = verticalDiff <= GameConstants::GRID_CELL_HEIGHT * 0.4f;
    bool inRange = horizontalDiff <= m_attackRange;

    return isToLeft && inSameLane && inRange;
}

// Calculate distance to squad member
float Robot::calculateDistanceToMember(const SquadMember* member) const {
    if (!member) return std::numeric_limits<float>::max();

    try {
        sf::Vector2f myPos = getPosition();
        sf::Vector2f targetPos = member->getPosition();
        return std::sqrt(
            std::pow(myPos.x - targetPos.x, 2) +
            std::pow(myPos.y - targetPos.y, 2)
        );
    }
    catch (...) {
        return std::numeric_limits<float>::max();
    }
}

void Robot::draw(sf::RenderWindow& window) const {
    if (!isActive()) return;
    drawHealthBar(window);
}

void Robot::drawHealthBar(sf::RenderWindow& window) const {
    if (isDead()) return;

    const float barWidth = 50.0f;
    const float barHeight = 8.0f;
    const float barOffsetX = 25.0f;
    const float barOffsetY = 95.0f;

    float healthPercent = static_cast<float>(m_health) / static_cast<float>(m_maxHealth);
    sf::Vector2f robotPos = getPosition();
    sf::Vector2f barPos(robotPos.x - barOffsetX, robotPos.y - barOffsetY);

    sf::RectangleShape healthBarBg(sf::Vector2f(barWidth, barHeight));
    healthBarBg.setPosition(barPos);
    healthBarBg.setFillColor(sf::Color::Transparent);
    healthBarBg.setOutlineColor(sf::Color(50, 50, 50, 150));
    healthBarBg.setOutlineThickness(1.0f);

    sf::RectangleShape healthBarFg(sf::Vector2f(barWidth * healthPercent, barHeight));
    healthBarFg.setPosition(barPos);

    sf::Color healthColor;
    if (healthPercent > 0.75f) healthColor = sf::Color::Green;
    else if (healthPercent > 0.5f) healthColor = sf::Color(128, 255, 0);
    else if (healthPercent > 0.25f) healthColor = sf::Color::Yellow;
    else healthColor = sf::Color::Red;

    healthBarFg.setFillColor(healthColor);

    window.draw(healthBarBg);
    if (healthPercent > 0.0f) {
        window.draw(healthBarFg);
    }
}

void Robot::takeDamage(int damage) {
    if (isDead()) return;

    m_health -= damage;
    if (m_health < 0) m_health = 0;

    if (m_health <= 0) {
        if (isAttackingSquadMember()) {
            stopAttackingSquadMember();
        }

        EventSystem::getInstance().publish<RobotDestroyedEvent>(
            this, getPosition(), m_rewardValue, m_type
        );
    }
}

void Robot::setFacingDirection(bool facingLeft) {
    if (m_facingLeft != facingLeft) {
        m_facingLeft = facingLeft;
        float scaleX = facingLeft ? -1.0f : 1.0f;
        float scaleY = m_sprite.getScale().y;
        m_sprite.setScale(scaleX, scaleY);
    }
}

void Robot::setSquadMemberManager(SquadMemberManager* manager) {
    m_squadMemberManager = manager;
}

bool Robot::shouldStopForSquadMember() const {
    return m_hasDetectedTarget;
}

bool Robot::isSquadMemberInFront(const SquadMember* member) const {
    return checkSquadMemberInRange(member);
}

std::vector<SquadMember*> Robot::findNearbySquadMembers() const {
    if (!m_squadMemberManager) return {};
    return m_squadMemberManager->getActiveSquadMembersInRange(getPosition(), m_attackRange);
}

void Robot::onSquadMemberCollision(SquadMember* target) {
    if (!target || target->isDestroyed()) return;
    startAttackingSquadMember(target);
}

void Robot::startAttackingSquadMember(SquadMember* target) {
    if (!target || m_isAttackingSquadMember || isDead()) return;

    m_isAttackingSquadMember = true;
    m_targetSquadMember = target;
    m_attackTimer.restart();
    setMovementEnabled(false);
    switchToAttackAnimation();
}

void Robot::stopAttackingSquadMember() {
    if (!m_isAttackingSquadMember) return;

    if (m_targetSquadMember) {
        m_targetSquadMember->removeAttacker(this);
    }

    cleanupInvalidTarget();

    if (m_physicsBody) {
        b2Vec2 correctVelocity(PhysicsUtils::sfmlToBox2D(-m_originalSpeed), 0.0f);
        m_physicsBody->SetLinearVelocity(correctVelocity);
    }
}

void Robot::loadAttackRangeFromConfig() {
    auto& config = ConfigLoader::getInstance();
    std::string robotSection;

    switch (m_type) {
    case GameTypes::RobotType::Basic: robotSection = "BasicRobot"; break;
    case GameTypes::RobotType::Fire: robotSection = "FireRobot"; break;
    case GameTypes::RobotType::Stealth: robotSection = "StealthRobot"; break;
    default: robotSection = "BasicRobot"; break;
    }

    try {
        int configHealth = config.getInt(robotSection, "health", m_health);
        if (configHealth != m_health) {
            m_health = configHealth;
            m_maxHealth = configHealth;
        }

        float configSpeed = config.getFloat(robotSection, "speed", m_originalSpeed);
        if (configSpeed != m_originalSpeed) {
            float currentVariation = m_originalSpeed / configSpeed;
            m_originalSpeed = configSpeed * currentVariation;
        }

        m_rewardValue = config.getInt(robotSection, "reward", m_rewardValue);
        m_attackRange = config.getFloat(robotSection, "attackRange", 100.0f);
        m_damage = config.getInt(robotSection, "damage", m_damage);
        m_attackDamage = m_damage;
    }
    catch (const std::exception&) {
        // Use constructor defaults on error
    }
}

// Getters and simple methods
void Robot::heal(int amount) { m_health += amount; if (m_health > m_maxHealth) m_health = m_maxHealth; }
int Robot::getHealth() const { return m_health; }
int Robot::getMaxHealth() const { return m_maxHealth; }
bool Robot::isDead() const { return m_health <= 0; }
GameTypes::RobotType Robot::getType() const { return m_type; }
int Robot::getDamage() const { return m_damage; }
int Robot::getRewardValue() const { return m_rewardValue; }
void Robot::setLane(int lane) { m_lane = lane; }
int Robot::getLane() const { return m_lane; }
void Robot::moveToLane(int newLane) { m_lane = newLane; }
void Robot::useSpecialAbility() {}
bool Robot::canUseAbility() const { return hasSpecialAbility() && m_abilityTimer.isElapsed(m_abilityCooldown); }
void Robot::updateAnimation(float dt) {}
void Robot::switchToAttackAnimation() {}
void Robot::switchToWalkingAnimation() {}