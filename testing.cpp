#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <random>

int main()
{
    // Use modern C++ for random number generation
    std::random_device rd;  // Obtain a random number from hardware
    std::mt19937 gen(rd()); // Seed the generator.
    std::uniform_int_distribution<> distr(20, 40); // Define a more reasonable range for grid size.

    int rows = distr(gen);
    int cols = distr(gen);
    
    std::cout << "Randomly generated size: " << rows << "x" << cols << std::endl;
    float cellSize = 25.0f;

    unsigned int windowWidth = static_cast<unsigned int>(cols * cellSize);
    unsigned int windowHeight = static_cast<unsigned int>(rows * cellSize);
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "SFML works!");

    std::vector<std::vector<sf::RectangleShape>> grid(rows, std::vector<sf::RectangleShape>(cols));

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));
            cell.setPosition(j * cellSize, i * cellSize);
            cell.setFillColor(sf::Color::White);
            cell.setOutlineThickness(1.0f);
            cell.setOutlineColor(sf::Color::Black);
            grid[i][j] = cell;
        }
    }

    while (window.isOpen())
    {
        // Use the new SFML 3 event loop
        while (const auto event = window.pollEvent())
        {
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

        window.display();
    }
    return 0;
}