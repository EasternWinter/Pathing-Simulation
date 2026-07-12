#include "robot.h"
#include <queue>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <functional> // For std::function

Robot::Robot(float cellSize, sf::Vector2i start) {
    m_shape.setSize({cellSize, cellSize});
    m_shape.setFillColor(sf::Color::Blue);
    m_shape.setPosition(sf::Vector2f(static_cast<float>(start.x) * cellSize, static_cast<float>(start.y) * cellSize));
}

int heuristic(sf::Vector2i a, sf::Vector2i b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

void Robot::findPath(sf::Vector2i start, sf::Vector2i goal, int rows, int cols, const std::vector<Obstacle>& obstacles) {
    // Clear path and manage memory from previous runs
    m_path.clear();
    
    // Create a grid to represent obstacle locations for efficient lookup
    std::vector<std::vector<bool>> obstacleGrid(rows, std::vector<bool>(cols, false));
    float cellSize = m_shape.getSize().x;
    for (const auto& obs : obstacles) {
        sf::Vector2f pos = obs.getShape().getPosition();
        int gridX = static_cast<int>(pos.x / cellSize);
        int gridY = static_cast<int>(pos.y / cellSize);
        if (gridY >= 0 && gridY < rows && gridX >= 0 && gridX < cols) {
            obstacleGrid[gridY][gridX] = true;
        }
    }

    std::priority_queue<Node*, std::vector<Node*>, std::function<bool(Node*, Node*)>> openList(
        [](Node* a, Node* b) { return a->getFCost() > b->getFCost(); });

    std::vector<std::vector<Node*>> allNodes(rows, std::vector<Node*>(cols, nullptr));
    std::vector<Node*> nodeRegistry; // To manage memory

    Node* startNode = new Node(start.y, start.x);
    startNode->gCost = 0;
    startNode->hCost = heuristic(start, goal);
    allNodes[start.y][start.x] = startNode;
    nodeRegistry.push_back(startNode);

    openList.push(startNode);

    int dy[] = {-1, 1, 0, 0}; // Up, Down
    int dx[] = {0, 0, -1, 1}; // Left, Right

    while (!openList.empty()) {
        Node* currentNode = openList.top();
        openList.pop();

        if (currentNode->y == goal.y && currentNode->x == goal.x) {
            // Path found, reconstruct it
            m_path.clear();
            Node* temp = currentNode;
            while (temp != nullptr) {
                m_path.push_back({temp->x, temp->y});
                temp = temp->parent;
            }
            std::reverse(m_path.begin(), m_path.end());
            
            // Clean up allocated nodes
            for (Node* node : nodeRegistry) {
                delete node;
            }
            return;
        }

        for (int i = 0; i < 4; ++i) {
            int newY = currentNode->y + dy[i];
            int newX = currentNode->x + dx[i];

            if (newY < 0 || newY >= rows || newX < 0 || newX >= cols) {
                continue; // Out of bounds
            }

            // Use the pre-computed obstacle grid for collision checks
            if (obstacleGrid[newY][newX]) {
                continue;
            }

            int newGCost = currentNode->gCost + 1;

            Node* neighborNode = allNodes[newY][newX];
            if (neighborNode == nullptr) {
                neighborNode = new Node(newY, newX);
                allNodes[newY][newX] = neighborNode;
                nodeRegistry.push_back(neighborNode);
            } else if (newGCost >= neighborNode->gCost) {
                continue; // Not a better path
            }

            neighborNode->parent = currentNode;
            neighborNode->gCost = newGCost;
            neighborNode->hCost = heuristic({newX, newY}, goal);
            
            // Removed redundant/unused inOpenList check
            openList.push(neighborNode);
        }
    }

    std::cout << "No path found!" << std::endl;
    // Clean up allocated nodes
    for (Node* node : nodeRegistry) {
        delete node;
    }
}

// Removed unused 'grid' parameter
void Robot::update() {
    if (!m_path.empty()) {
        sf::Vector2i nextPos = m_path.front();
        m_path.erase(m_path.begin());

        float cellSize = m_shape.getSize().x;
        m_shape.setPosition(sf::Vector2f(static_cast<float>(nextPos.x) * cellSize, static_cast<float>(nextPos.y) * cellSize));
    }
}

void Robot::draw(sf::RenderWindow& window) const {
    window.draw(m_shape);
}