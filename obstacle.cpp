#include "obstacle.h"
#include <random>

// Constructor to initialize the obstacle's properties.
// It uses a member initializer list, which is more efficient.
Obstacle::Obstacle(float cellSize, sf::Vector2f speed, sf::Vector2f startPos, sf::Color color)
    : m_speed(speed)
{
    m_shape.setSize(sf::Vector2f(cellSize, cellSize));
    m_shape.setPosition(startPos);
    m_shape.setFillColor(color);
}

// Moves the obstacle and makes it bounce off the vertical window edges.
void Obstacle::move(unsigned int windowWidth, unsigned int windowHeight){
    // Get current position and size of the obstacle
    sf::Vector2f pos = m_shape.getPosition();
    sf::Vector2f size = m_shape.getSize();

    // Check for collision with the top or bottom edge of the window
    if ((pos.y + size.y >= windowHeight && m_speed.y > 0) || (pos.y <= 0 && m_speed.y < 0)){
        m_speed.y = -m_speed.y; // Reverse the vertical speed
    }
    if ((pos.x + size.x >= windowWidth && m_speed.x > 0) || (pos.x <= 0 && m_speed.x < 0)){
        m_speed.x = -m_speed.x; // Reverse the horizontal speed
    }
    m_shape.setPosition(m_shape.getPosition() + m_speed); // Move the obstacle
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> go(0, 25);
    if (go(gen) == 0) {
        m_speed.y = std::uniform_int_distribution(-5, 5)(gen);
        m_speed.x = std::uniform_int_distribution(-5, 5)(gen);
    }
}

// Draws the obstacle to the render window.
void Obstacle::draw(sf::RenderWindow& window) const{
    window.draw(m_shape);
}

// Returns a constant reference to the shape.
// Useful for collision detection or other interactions.
const sf::RectangleShape& Obstacle::getShape() const{
    return m_shape;
}

// Sets the speed of the obstacle.
void Obstacle::setSpeed(sf::Vector2f speed) {
    m_speed = speed;
}

// Returns the current speed of the obstacle.
sf::Vector2f Obstacle::getSpeed() const {
    return m_speed;
}