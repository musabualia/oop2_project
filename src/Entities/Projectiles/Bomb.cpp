#include "Entities/Projectiles/Bomb.h"
#include "Managers/ResourceManager.h"
#include "Managers/AudioManager.h"
#include "Systems/AnimationSystem.h"
#include "Systems/EventSystem.h"
#include "Core/Constants.h"

Bomb::Bomb(const sf::Vector2f& pos)
    : m_position(pos), m_animComponent(std::make_unique<AnimationComponent>(nullptr))
{
    m_sprite.setTexture(ResourceManager::getInstance().getTexture("bomb.png"));
    m_sprite.setOrigin(m_sprite.getTexture()->getSize().x / 2.f, m_sprite.getTexture()->getSize().y / 2.f);
    m_sprite.setPosition(pos);
}

void Bomb::update(float dt) {
    m_timer += dt;

    if (!m_exploded && m_timer >= m_fuseTime) {
        explode();
    }

    if (m_exploded && m_animComponent) {
        m_animComponent->update(dt);
        if (m_animComponent->isAnimationComplete()) {
            m_animationFinished = true;
        }
    }
}

void Bomb::render(sf::RenderWindow& window) const {
    if (!m_exploded) {
        window.draw(m_sprite);
    }
    else if (m_animComponent) {
        m_animComponent->draw(window);
    }
}

bool Bomb::isDone() const {
    return m_exploded && m_animationFinished;
}

void Bomb::explode() {
    AudioManager::getInstance().playSound("explosion", AudioManager::AudioCategory::UI);
    if (m_exploded) return;

    m_exploded = true;

    EventSystem::getInstance().publish<BombExplosionEvent>(m_position, m_explosionRadius, m_damage);

    // Start explosion animation
    auto& animSystem = AnimationSystem::getInstance();
    if (animSystem.hasAnimation("Explosion")) {
        const Animation* anim = animSystem.getAnimation("Explosion");
        if (anim) {
            m_animComponent->play(*anim);
            m_animComponent->setPosition(m_position);
        }
    }
}

sf::Vector2f Bomb::getPosition() const {
    return m_position;
}

float Bomb::getExplosionRadius() const {
    return m_explosionRadius;
}

int Bomb::getDamage() const {
    return m_damage;
}

bool Bomb::hasExploded() const {
    return m_exploded;
}
