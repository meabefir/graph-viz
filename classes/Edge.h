#pragma once
#include <sstream>

#include "imgui.h"
#include "imgui-SFML.h"

#include "sfLine.h"
#include "Node.h"

using namespace std;

class Edge
{
    weak_ptr<Node> from;
    weak_ptr<Node> to;
    sf::Vector2f vecFrom;
    sf::Vector2f prevVecFrom;
    sf::Vector2f vecTo;
    sf::Vector2f prevVecTo;
    int from_int;
    int to_int;
    int cost;
    int currentCost;
    char costString[20] = "";
    sf::Vertex vertex[2];
    sfLine line;
    sf::Vertex arrowVertex[3];
    sfLine arrowLine;
    sf::Text costText;

    stringstream ss;

    sf::Color normalColor;
    sf::Color hoverColor = sf::Color::Magenta;
    sf::Color currentColor = normalColor;
    sf::Color wantedColor = normalColor;

    // this decides whether to update vertex positions
    bool posChanged = true;

public:
    static float min_length;
    static float max_length;
    static float thickness;

public:
    friend class Graph;
    friend class EdgeAnimation;
    friend class EdgeAnimationReversed;
    friend class EdgeAnimationDotted;
    friend class EdgeAnimationMiddle;

    Edge(weak_ptr<Node>& _from, weak_ptr<Node>& _to, int cost = 1, const float& thicc = 2.f);
    void update(float delta);
    void draw(sf::RenderWindow* win);
    void drawGUI();

    void setCost(int value);
    int getCost() const;

    bool operator < (const Edge& other)
    {
        return cost < other.cost;
    }
};
