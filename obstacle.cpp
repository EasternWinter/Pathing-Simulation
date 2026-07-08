#include "obstacle.h"

// Constructor to initialize the obstacle's properties.
// It uses a member initializer list, which is more efficient.
Obstacle::Obstacle(float cellSize, float speed, sf::Vector2f startPos, sf::Color color)
    : m_speed(speed)
{
    m_shape.setSize(sf::Vector2f(cellSize, cellSize));
    m_shape.setPosition(startPos);
    m_shape.setFillColor(color);
}

// Moves the obstacle and makes it bounce off the vertical window edges.
void Obstacle::move(unsigned int windowWidth, unsigned int windowHeight)
{
    // Get current position and size of the obstacle
    sf::Vector2f pos = m_shape.getPosition();
    sf::Vector2f size = m_shape.getSize();

    // Check for collision with the top or bottom edge of the window
    if ((pos.y + size.y >= windowHeight && m_speed > 0) || (pos.y <= 0 && m_speed < 0))
    {
        m_speed = -m_speed; // Reverse the vertical speed
    }
    m_shape.setPosition(m_shape.getPosition() + sf::Vector2f(0, m_speed)); // Move the obstacle vertically
}

// Draws the obstacle to the render window.
void Obstacle::draw(sf::RenderWindow& window) const
{
    window.draw(m_shape);
}

// Returns a constant reference to the shape.
// Useful for collision detection or other interactions.
const sf::RectangleShape& Obstacle::getShape() const
{
    return m_shape;
}