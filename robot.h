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

    void findPath(sf::Vector2i start, sf::Vector2i goal, int rows, int cols, const std::vector<Obstacle>& obstacles);
    void update();
    void draw(sf::RenderWindow& window) const;

private:
    sf::RectangleShape m_shape;
    std::vector<sf::Vector2i> m_path;
};

#endif // ROBOT_H