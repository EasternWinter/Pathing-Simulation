#ifndef ROBOT_H
#define ROBOT_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "obstacle.h"

// Node for A* pathfinding
struct Node {
    int y, x;
    int gCost, hCost;
    Node* parent;

    Node(int r, int c) : y(r), x(c), gCost(0), hCost(0), parent(nullptr) {}

    int getFCost() const { return gCost + hCost; }

    // For priority queue comparison
    bool operator>(const Node& other) const {
        return getFCost() > other.getFCost();
    }
};

class Robot {
public:
    Robot(float cellSize, sf::Vector2i start);

    void findPath(sf::Vector2i goal, int rows, int cols, const std::vector<Obstacle>& obstacles);
    void update(int rows, int cols, const std::vector<Obstacle>& obstacles);
    void draw(sf::RenderWindow& window) const;
    sf::Vector2i getCurrentPos() const;

private:
    sf::RectangleShape m_shape;
    sf::Vector2i m_currentPos;
    std::vector<sf::Vector2i> m_path;
    std::vector<sf::Vector2i> m_path_history; // To enable retreating
};

#endif // ROBOT_H