#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <SFML/Graphics.hpp>

class Obstacle
{
public:
    // Constructor to initialize the obstacle's properties.
    Obstacle(float cellSize, sf::Vector2f speed, sf::Vector2f startPos, sf::Color color);

    // Moves the obstacle and makes it bounce off the horizontal and vertical window edges.
    void move(unsigned int windowWidth, unsigned int windowHeight);

    // Draws the obstacle to the render window.
    void draw(sf::RenderWindow& window) const;

    // Returns a constant reference to the shape for interactions like collision detection.
    const sf::RectangleShape& getShape() const;

    // Sets the speed of the obstacle.
    void setSpeed(sf::Vector2f speed);

    // Returns the current speed of the obstacle.
    sf::Vector2f getSpeed() const;

private:
    sf::RectangleShape m_shape;
    sf::Vector2f m_speed;
};

#endif // OBSTACLE_H