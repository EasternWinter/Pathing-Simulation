#include "robot.h"
#include <queue>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <functional> // For std::function

Robot::Robot(float cellSize, sf::Vector2i start) : m_currentPos(start){
    m_shape.setSize({cellSize, cellSize});
    m_shape.setFillColor(sf::Color::Blue);
    m_shape.setPosition(sf::Vector2f(start.x * cellSize, start.y * cellSize));
}

int heuristic(sf::Vector2i a, sf::Vector2i b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

void Robot::findPath(sf::Vector2i goal, int rows, int cols, const std::vector<Obstacle>& obstacles) {
    m_path.clear();

    float cellSize = m_shape.getSize().x;

    // --- Create a time-aware obstacle checker ---
    // This function checks if a cell (x, y) is occupied at a given time step (gCost)
    auto isObstacleAtTime = [&](int x, int y, int time) -> bool {
        sf::FloatRect cellBounds({x * cellSize, y * cellSize}, {cellSize, cellSize});
        for (const auto& obs : obstacles) {
            sf::RectangleShape predictedObsShape = obs.getShape();
            sf::Vector2f currentSpeed = obs.getSpeed(); // This now returns sf::Vector2f
            sf::Vector2f currentPos = predictedObsShape.getPosition();
            
            // The robot moves once every 0.2 seconds. Assuming 60 FPS, the obstacles
            // move 12 times (60 * 0.2) for each robot step.
            // We simulate this movement 'time' steps into the future.
            const int framesPerRobotStep = 12;
            
            for (int t = 0; t < time * framesPerRobotStep; ++t) {
                // Check for bounce against top or bottom walls
                if ((currentPos.y + cellSize >= rows * cellSize && currentSpeed.y > 0) || (currentPos.y <= 0 && currentSpeed.y < 0)) {
                    currentSpeed.y = -currentSpeed.y; // Reverse vertical direction
                }
                // Note: This prediction only considers vertical movement.
                currentPos += currentSpeed;
            }

            // Clamp position to be within bounds, in case of overshooting in a single frame
            if (currentPos.y < 0) currentPos.y = 0;
            if (currentPos.y + cellSize > rows * cellSize) currentPos.y = rows * cellSize - cellSize;


            predictedObsShape.setPosition(currentPos);

            if (cellBounds.findIntersection(predictedObsShape.getGlobalBounds())) {
                return true; // Collision detected at this time step
            }
        }
        return false;
    };

    // --- A* PATHFINDING ---
    // Always use the time-aware A* for full-range prediction.
    std::priority_queue<Node*, std::vector<Node*>, std::function<bool(Node*, Node*)>> openList(
        [](Node* a, Node* b) { return a->getFCost() > b->getFCost(); });

    std::vector<std::vector<Node*>> allNodes(rows, std::vector<Node*>(cols, nullptr));
    std::vector<Node*> nodeRegistry; // To manage memory

    Node* startNode = new Node(m_currentPos.y, m_currentPos.x);
    startNode->gCost = 0;
    startNode->hCost = heuristic(m_currentPos, goal);
    allNodes[m_currentPos.y][m_currentPos.x] = startNode;
    nodeRegistry.push_back(startNode);
    openList.push(startNode);

    int dy[] = {-1, 1, 0, 0}; // Up, Down
    int dx[] = {0, 0, -1, 1}; // Left, Right

    while (!openList.empty()) {
        Node* currentNode = openList.top();
        openList.pop();

        if (currentNode->y == goal.y && currentNode->x == goal.x) {
            Node* temp = currentNode;
            while (temp != nullptr) {
                m_path.push_back({temp->x, temp->y});
                temp = temp->parent;
            }
            std::reverse(m_path.begin(), m_path.end());
            if (!m_path.empty()) m_path.erase(m_path.begin());
            
            for (Node* node : nodeRegistry) delete node;
            return;
        }

        for (int i = 0; i < 4; ++i) {
            int newY = currentNode->y + dy[i];
            int newX = currentNode->x + dx[i];

            // Check if the neighbor is valid and not occupied at the time we would arrive there
            if (newY < 0 || newY >= rows || newX < 0 || newX >= cols || isObstacleAtTime(newX, newY, currentNode->gCost + 1)) {
                continue;
            }

            int newGCost = currentNode->gCost + 1;
            Node* neighborNode = allNodes[newY][newX];
            if (neighborNode == nullptr) {
                neighborNode = new Node(newY, newX);
                allNodes[newY][newX] = neighborNode;
                nodeRegistry.push_back(neighborNode);
            } else if (newGCost >= neighborNode->gCost) {
                continue;
            }

            neighborNode->parent = currentNode;
            neighborNode->gCost = newGCost;
            neighborNode->hCost = heuristic({newX, newY}, goal);
            openList.push(neighborNode);
        }
    }
    // A* failed to find a path, so wait.
    std::cout << "No path found! Waiting for an opening." << std::endl;
    m_path.push_back(m_currentPos); // Set path to current position to make it wait
    for (Node* node : nodeRegistry) delete node;
}

// Removed unused 'grid' parameter
void Robot::update(int rows, int cols, const std::vector<Obstacle>& obstacles) {
    float cellSize = m_shape.getSize().x;

    if (!m_path.empty()) {
        m_path_history.push_back(m_currentPos); // Record current position before moving
        sf::Vector2i nextPos = m_path.front();

        // If the next step is not the current position, it's a real move.
        if (nextPos != m_currentPos) {
            // Create a bounding box for where the robot WILL BE
            sf::FloatRect nextRobotBounds({nextPos.x * cellSize, nextPos.y * cellSize}, {m_shape.getSize().x, m_shape.getSize().y});

            // --- Predictive Collision Check ---
            bool collision_imminent = false;
            for (const auto& obs : obstacles) {
                // 1. Check for collision at current obstacle positions
                if (nextRobotBounds.findIntersection(obs.getShape().getGlobalBounds())) {
                    collision_imminent = true;
                    break;
                }

                // 2. Predict next obstacle position and check for collision there
                sf::RectangleShape predictedObsShape = obs.getShape();
                sf::Vector2f obsSpeed = obs.getSpeed();
                predictedObsShape.move(obsSpeed); // Simulate one step of obstacle movement

                if (nextRobotBounds.findIntersection(predictedObsShape.getGlobalBounds())) {
                    collision_imminent = true;
                    break;
                }
            }

            if (collision_imminent) {
                // Collision on path detected! Invalidate the path to force a replan
                // on the next call to findPath(). Stay in the current position for this frame.
                std::cout << "Collision ahead! Attempting to dodge." << std::endl;
                m_path.clear();

                // --- Intelligent Dodge Maneuver ---
                // Evaluate all 8 adjacent cells to find the best one to move to.
                // The "best" cell is the one that is safe and maximizes the distance to the nearest obstacle.
                sf::Vector2i bestDodgePos = m_currentPos;
                float maxMinDistance = -1.0f;

                // Directions: 8 directions + staying still (as a last resort)
                int dy[] = {-1, 1, 0, 0, -1, -1, 1, 1, 0};
                int dx[] = {0, 0, -1, 1, -1, 1, -1, 1, 0};

                for (int i = 0; i < 9; ++i) {
                    sf::Vector2i dodgePos = {m_currentPos.x + dx[i], m_currentPos.y + dy[i]};

                    // Basic check: is the move within the grid?
                    if (dodgePos.x < 0 || dodgePos.x >= cols || dodgePos.y < 0 || dodgePos.y >= rows) {
                        continue;
                    }

                    sf::FloatRect dodgeBounds({dodgePos.x * cellSize, dodgePos.y * cellSize}, {m_shape.getSize().x, m_shape.getSize().y});

                    // Safety check: will this spot be safe in the next frame?
                    bool isSafe = true;
                    float minDistanceToObstacle = std::numeric_limits<float>::max();

                    for (const auto& obs : obstacles) {
                        sf::RectangleShape predictedObsShape = obs.getShape();
                        predictedObsShape.move(obs.getSpeed());

                        if (dodgeBounds.findIntersection(predictedObsShape.getGlobalBounds())) {
                            isSafe = false;
                            break;
                        }
                        // Calculate distance from dodge position to this predicted obstacle
                        sf::Vector2f obsCenter = predictedObsShape.getPosition() + predictedObsShape.getSize() / 2.f;
                        sf::Vector2f dodgeCenter = dodgeBounds.position + dodgeBounds.size / 2.f;
                        float dist = std::hypot(obsCenter.x - dodgeCenter.x, obsCenter.y - dodgeCenter.y);
                        minDistanceToObstacle = std::min(minDistanceToObstacle, dist);
                    }

                    if (isSafe && minDistanceToObstacle > maxMinDistance) {
                        maxMinDistance = minDistanceToObstacle;
                        bestDodgePos = dodgePos;
                    }
                }
                m_currentPos = bestDodgePos;
                return; // End update for this frame after dodging.
            }
            m_currentPos = nextPos;
        }
        m_path.erase(m_path.begin());

        // Keep path history from growing indefinitely
        if (m_path_history.size() > 20) { // Keep a history of the last 20 moves
            m_path_history.erase(m_path_history.begin());
        }
    } else {
        // Path is empty, meaning we are waiting (e.g., at a blocked goal).
        // Stay defensive and check for collisions on our CURRENT spot.
        sf::FloatRect currentRobotBounds = m_shape.getGlobalBounds();
        for (const auto& obs : obstacles) {
            if (currentRobotBounds.findIntersection(obs.getShape().getGlobalBounds())) {
                std::cout << "Collision while waiting! Attempting to dodge." << std::endl;
                // Attempt to find a safe adjacent cell to move to.
                int dy[] = {-1, 1, 0, 0}; // Up, Down
                int dx[] = {0, 0, -1, 1}; // Left, Right
                for (int i = 0; i < 4; ++i) {
                    sf::Vector2i dodgePos = {m_currentPos.x + dx[i], m_currentPos.y + dy[i]};
                    sf::FloatRect dodgeBounds({dodgePos.x * cellSize, dodgePos.y * cellSize}, {m_shape.getSize().x, m_shape.getSize().y});
                    bool isSafe = true;
                    for (const auto& inner_obs : obstacles) {
                        if (dodgeBounds.findIntersection(inner_obs.getShape().getGlobalBounds())) {
                            isSafe = false;
                            break;
                        }
                    }
                    if (isSafe) {
                        m_currentPos = dodgePos; // Move to safe spot
                        break; // Stop after finding one safe spot
                    }
                }
                break; // Stop checking for collisions after dodging
            }
        }
    }
    m_shape.setPosition(sf::Vector2f(static_cast<float>(m_currentPos.x) * cellSize, static_cast<float>(m_currentPos.y) * cellSize));
}

void Robot::draw(sf::RenderWindow& window) const {
    window.draw(m_shape);
}

sf::Vector2i Robot::getCurrentPos() const {
    return m_currentPos;
}