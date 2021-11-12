#pragma once
#include "Helper.h"

extern sf::Font defFont;
extern sf::Vector2f mouseWorldPos;
extern bool drawNodeNames;
extern bool drawNodeId;

class Node
{
    sf::Text text;
    sf::Text idText;
    sf::CircleShape shape;
    sf::Vector2f pos;
    const char* name;
    int idx;
    char idxChar[10];

    static float radius;
    static float radiusSquared;
    static size_t pointCount;
    static sf::Color normalColor;
    static sf::Color hoverColor;

    sf::Vector2f vel;
    bool captured = false;
    sf::Vector2f capturedOffset;
    bool isStatic = false;

public:
    friend class Graph;
    friend class Edge;
    friend class EdgeAnimation;
    friend class EdgeAnimationDotted;
    friend class EdgeAnimationMiddle;

    Node(const char* _name, sf::Vector2f _pos, int _idx);
    void capture();
    void release();
    void setPos(sf::Vector2f _pos);
    void addImpulse(sf::Vector2f _imp);
    void update(float delta);
    void draw(sf::RenderWindow* win);
};
