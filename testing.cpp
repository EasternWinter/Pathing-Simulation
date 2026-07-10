#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "obstacle.h"
#include "robot.h"

int main()
{
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
            cell.setPosition({static_cast<float>(j) * cellSize, static_cast<float>(i) * cellSize});
            cell.setFillColor(sf::Color::White);
            cell.setOutlineThickness(1.0f);
            cell.setOutlineColor(sf::Color::Black);
            grid[i][j] = cell;
        }
    }

    // Create a vector to hold the obstacles
    std::vector<Obstacle> obstacles;
    float obsSpeed = 3.0f;

    // Add obstacles to the vector using the Obstacle class
    obstacles.emplace_back(cellSize, obsSpeed, sf::Vector2f(2 * cellSize, 0 * cellSize), sf::Color::Red);
    obstacles.emplace_back(cellSize, -obsSpeed, sf::Vector2f(7 * cellSize, 19 * cellSize), sf::Color::Red);
    obstacles.emplace_back(cellSize, obsSpeed, sf::Vector2f(10 * cellSize, 0 * cellSize), sf::Color::Red);
    obstacles.emplace_back(cellSize, -obsSpeed, sf::Vector2f(15 * cellSize, 19 * cellSize), sf::Color::Red);

    // Create the robot
    sf::Vector2i startPos(0, rows - 1); // Bottom-left
    sf::Vector2i goalPos(cols - 1, 0);   // Top-right
    Robot robot(cellSize, startPos);

    robot.findPath(startPos, goalPos, rows, cols, obstacles);

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

        // Move and draw all obstacles
        for (auto& obstacle : obstacles) {
            obstacle.move(windowWidth, windowHeight);
            obstacle.draw(window);
        }

        // Update and draw robot
        // Move robot every 0.2 seconds
        if (deltaClock.getElapsedTime().asSeconds() > 0.2f) {
            robot.update();
            deltaClock.restart();
        }
        robot.draw(window);
        window.display();
    }
    return 0;
}