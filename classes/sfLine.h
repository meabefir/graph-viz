#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Edge;

class sfLine : public sf::Drawable
{
private:
    std::vector<sf::Vector2f> points;
    sf::Vertex* vertices;
    int size;
    float thickness;
    sf::Color color;
public:
    sfLine(const sf::Vertex _points[], int _size, float thicc = 3.5f, const sf::Color& c = sf::Color::Yellow);
    void update();
    void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {}
    void draw(sf::RenderTarget& target);
    void setVertices(const sf::Vertex _points[]);
    void setColor(sf::Color& c);
    void setThickness(const float& t);
};
