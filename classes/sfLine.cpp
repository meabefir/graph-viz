#include "sfLine.h"
#include "Edge.h"

extern bool edgeThicknessChanged;

sfLine::sfLine(const sf::Vertex _points[], int _size, float thicc, const sf::Color& c) :
    color(c), thickness(thicc), size(_size)
{
    for (int i = 0; i < size; i++)
        points.push_back(_points[i].position);

    vertices = new sf::Vertex[size * 4];

    for (int i = 0; i < size * 4; ++i)
        vertices[i].color = color;
    if (thickness != Edge::thickness)
        setThickness(Edge::thickness);
}

void sfLine::update()
{
    for (int i = 0; i < points.size() - 1; i++)
    {
        sf::Vector2f from = points[i];
        sf::Vector2f to = points[i + 1];

        sf::Vector2f direction = to - from;
        sf::Vector2f unitDirection = direction / std::sqrt(direction.x * direction.x + direction.y * direction.y);
        sf::Vector2f unitPerpendicular(-unitDirection.y, unitDirection.x);

        sf::Vector2f offset = (thickness / 2.f) * unitPerpendicular;

        vertices[i * 4].position = from + offset;
        vertices[i * 4 + 1].position = to + offset;
        vertices[i * 4 + 2].position = to - offset;
        vertices[i * 4 + 3].position = from - offset;
    }
}

void sfLine::draw(sf::RenderTarget& target)
{
    if (edgeThicknessChanged)
    {
        setThickness(Edge::thickness);
    }
    target.draw(vertices, size * 4, sf::Quads);
}

void sfLine::setVertices(const sf::Vertex _points[])
{
    for (int i = 0; i < points.size(); i++)
        points[i] = _points[i].position;
    update();
}

void sfLine::setColor(sf::Color& c)
{
    color = c;
    for (int i = 0; i < 4; ++i)
        vertices[i].color = color;
}

void sfLine::setThickness(const float& t)
{
    thickness = t;
    update();
}