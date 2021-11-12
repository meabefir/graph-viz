#include <iostream>
#include <queue>
#include <stack>
#include <unordered_map>
#include <math.h>

#include "classes/Edge.h"
#include "classes/AnimationComponent.h"
#include "classes/Graph.h"

using namespace std;

sf::Font defFont;
sf::RenderWindow* window;
sf::Vector2i mousePos;
sf::Vector2f mouseWorldPos;
sf::Vector2f lastMouseWorldPos;

bool drawNodeNames = true;
bool drawNodeId = true;
bool drawEdges = true;
bool drawCosts = true;
bool randomEdgeCosts = true;
bool nodeRadiusChanged = false;
bool edgeThicknessChanged = false;

int main()
{
    defFont.loadFromFile("./fonts/def.otf");

    sf::ContextSettings settings(0, 0, 4);
    window = new sf::RenderWindow(sf::VideoMode(1400, 850), "graph app", sf::Style::Close, settings);
    window->setFramerateLimit(120);
    sf::View view(sf::FloatRect(0.f, 0.f, 1400.f, 850.f));
    window->setView(view);

    Graph g;
    ImGui::SFML::Init(*window);

    sf::Clock deltaClock;
    while (window->isOpen())
    {
        sf::Event event;
        while (window->pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);
            g.handleEvent(event);
            if (event.type == sf::Event::Closed)
                window->close();
        }
        mousePos = sf::Mouse::getPosition(*window);
        lastMouseWorldPos = mouseWorldPos;
        mouseWorldPos = window->mapPixelToCoords(mousePos);

        auto imgui_delta = deltaClock.restart();
        ImGui::SFML::Update(*window, imgui_delta);
        float delta = imgui_delta.asSeconds();

        g.updateGUI();
        g.update(delta);

        window->clear(sf::Color(18, 33, 43));
        g.draw(window);
        ImGui::SFML::Render(*window);
        window->display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}