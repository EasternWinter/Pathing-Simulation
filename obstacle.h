#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <SFML/Graphics.hpp>

class Obstacle
{
public:
    // Constructor to initialize the obstacle's properties.
    Obstacle(float cellSize, float speed, sf::Vector2f startPos, sf::Color color);

    // Moves the obstacle and makes it bounce off the horizontal and vertical window edges.
    void move(unsigned int windowWidth, unsigned int windowHeight);

    // Draws the obstacle to the render window.
    void draw(sf::RenderWindow& window) const;

    // Returns a constant reference to the shape for interactions like collision detection.
    const sf::RectangleShape& getShape() const;

private:
    sf::RectangleShape m_shape;
    float m_speed;
};

#endif // OBSTACLE_H