#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <random>
#include <unordered_set>
#include "obstacle.h"
#include "robot.h"

int main(int argc, char* argv[])
{
    int numObstacles = 50; // Default number of obstacles

    // Check for command-line arguments first
    if (argc > 1) {
        try {
            numObstacles = std::stoi(argv[1]);
        } catch (const std::exception& e) {
            std::cerr << "Invalid number for obstacles. Using default: " << numObstacles << std::endl;
        }
    } else {
        // If no command-line argument, prompt the user for input
        std::cout << "Number of Obstacles: ";
        std::cin >> numObstacles;
        if (std::cin.fail() || numObstacles < 0) {
            std::cerr << "Invalid input. Using default: 50" << std::endl;
            numObstacles = 50;
        }
    }
    int rows = 20;
    int cols = 20;
    
    float cellSize = 30.0f;

    unsigned int windowWidth = static_cast<unsigned int>(cols * cellSize);
    unsigned int windowHeight = static_cast<unsigned int>(rows * cellSize);
    sf::RenderWindow window(sf::VideoMode({windowWidth, windowHeight}), "SFML works!");
    sf::Clock deltaClock;
    window.setFramerateLimit(60);

    std::vector<std::vector<sf::RectangleShape>> grid(rows, std::vector<sf::RectangleShape>(cols));
    
    //Initialize grid cells.
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));
            cell.setPosition({j * cellSize, i * cellSize});
            cell.setFillColor(sf::Color::White);
            cell.setOutlineThickness(1.0f);
            cell.setOutlineColor(sf::Color::Black);
            grid[i][j] = cell;
        }
    }

    // Create a vector to hold the obstacles
    std::vector<Obstacle> obstacles;

    // Setup for random number generation
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> rowDist(0, rows - 1);
    std::uniform_int_distribution<> colDist(0, cols - 1);
    std::uniform_int_distribution<int> spdX(-5, 5);
    std::uniform_int_distribution<int> spdY(-5, 5);

    // Create the robot
    sf::Vector2i startPos(0, rows - 1); // Bottom-left
    sf::Vector2i goalPos(cols - 1, 0);   // Top-right
    Robot robot(cellSize, startPos);

    // Keep track of occupied cells to avoid duplicates and overwriting start/goal
    std::unordered_set<int> occupied_cells;
    occupied_cells.insert(startPos.y * cols + startPos.x);

    for (int i = 0; i < numObstacles; ++i) {
        int randRow, randCol, x, y;
        do {
            randRow = rowDist(gen);
            randCol = colDist(gen);
            x = spdX(gen);
            y = spdY(gen);
        } while (occupied_cells.count(randRow * cols + randCol));

        occupied_cells.insert(randRow * cols + randCol);
        obstacles.emplace_back(cellSize, sf::Vector2f(x, y), sf::Vector2f(randCol * cellSize, randRow * cellSize), sf::Color::Red);
    }

    bool goalReached = false;

    while (window.isOpen())
    {
        // The new SFML 3 event loop.
        // window.pollEvent() returns an std::optional<sf::Event>.
        while (const auto event = window.pollEvent())
        {
            // Check for the Closed event or if the Escape key was pressed.
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        window.clear(sf::Color::Black);
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)
            {
                window.draw(grid[i][j]);
            }
        }

        // Draw all obstacles
        for (auto& obstacle : obstacles) {
            obstacle.draw(window);
        }

        // Move robot every 0.2 seconds, then move the obstacles.
        if (!goalReached && deltaClock.getElapsedTime().asSeconds() > 0.2f) {
            robot.findPath(goalPos, rows, cols, obstacles);
            robot.update(rows, cols, obstacles);
            deltaClock.restart();

            // Check if goal is reached and the goal cell is clear
            if (robot.getCurrentPos() == goalPos)
            {
                bool goalIsClear = true;
                for (const auto& obstacle : obstacles) {
                    sf::Vector2f obsPos = obstacle.getShape().getPosition();
                    if (static_cast<int>(obsPos.x / cellSize) == goalPos.x && static_cast<int>(obsPos.y / cellSize) == goalPos.y) {
                        goalIsClear = false;
                        break;
                    }
                }
                if (goalIsClear) {
                    goalReached = true;
                    std::cout << "Goal Reached! Stopping obstacles." << std::endl;
                } else {
                    // Robot is at the goal, but it's blocked.
                    // Invalidate the path to make the robot wait and stay defensive.
                    robot.findPath(goalPos, rows, cols, {}); // Pass empty obstacles to clear path
                }
            }
        }

        // Move obstacles every frame if the goal has not been reached
        if (!goalReached) {
            for (auto& obstacle : obstacles) {
                obstacle.move(windowWidth, windowHeight);
            }
        }
        robot.draw(window);
        window.display();
    }
    return 0;
}