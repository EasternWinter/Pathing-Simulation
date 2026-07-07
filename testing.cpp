#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

int main()
{
    int rows = 20;
    int cols = 20;
    
    float cellSize = 30.0f;

    unsigned int windowWidth = static_cast<unsigned int>(cols * cellSize);
    unsigned int windowHeight = static_cast<unsigned int>(rows * cellSize);
    sf::RenderWindow window(sf::VideoMode({windowWidth, windowHeight}), "SFML works!");
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

    //Create moving obstacles in the grid.
    sf::RectangleShape obs1(sf::Vector2f(cellSize, cellSize));
    obs1.setPosition({static_cast<float>(5) * cellSize, static_cast<float>(0) * cellSize});
    obs1.setFillColor(sf::Color::Red);
    sf::RectangleShape obs2(sf::Vector2f(cellSize, cellSize));
    obs2.setPosition({static_cast<float>(10) * cellSize, static_cast<float>(19) * cellSize});
    obs2.setFillColor(sf::Color::Red);
    sf::RectangleShape obs3(sf::Vector2f(cellSize, cellSize));
    obs3.setPosition({static_cast<float>(15) * cellSize, static_cast<float>(0) * cellSize});
    obs3.setFillColor(sf::Color::Red);

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
        window.draw(obs1);
        window.draw(obs2);
        window.draw(obs3);
        window.display();
    }
    return 0;
}