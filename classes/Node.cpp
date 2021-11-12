#include "Node.h"

sf::Color Node::normalColor = sf::Color::Cyan;
sf::Color Node::hoverColor = sf::Color(sf::Uint8(224), sf::Uint8(255), sf::Uint8(255), sf::Uint8(255));
float Node::radius = 30.f;
float Node::radiusSquared = Node::radius * Node::radius;
size_t Node::pointCount = 20;

Node::Node(const char* _name, sf::Vector2f _pos, int _idx) :
    shape(radius, pointCount), name(_name), idx(_idx), text(_name, defFont, 20)
{
    shape.setOrigin(sf::Vector2f(radius, radius));
    shape.setFillColor(sf::Color::Cyan);

    // font
    // name
    idText.setFillColor(sf::Color::Black);
    idText.setOutlineColor(sf::Color::White);
    idText.setOutlineThickness(2.f);
    idText.setCharacterSize((int)Node::radius);
    // idx
    _itoa_s(idx, idxChar, 10);
    idText.setString(idxChar);
    idText.setFont(defFont);
    idText.setOutlineColor(sf::Color::White);
    idText.setOutlineThickness(2.f);
    this->idText.setOrigin(sf::Vector2f(this->idText.getGlobalBounds().width / 2.f,
        this->idText.getGlobalBounds().height));

    setPos(_pos);
}

void Node::capture()
{
    capturedOffset = mouseWorldPos - pos;
    captured = true;
    isStatic = true;
}

void Node::release()
{
    captured = false;
    isStatic = false;
}

void Node::setPos(sf::Vector2f _pos)
{
    pos = _pos;
    text.setPosition(_pos);
    shape.setPosition(_pos);
    this->text.setPosition(pos);
    this->idText.setPosition(pos);
}

void Node::addImpulse(sf::Vector2f _imp)
{
    if (isStatic)
        return;
    setPos(pos + _imp);
}

void Node::update(float delta)
{
    if (captured)
    {
        setPos(mouseWorldPos - capturedOffset);
    }
}

void Node::draw(sf::RenderWindow* win)
{
    // if mouse over
    if (Vector::getLengthSquared(mouseWorldPos - pos) <= radiusSquared)
        shape.setFillColor(hoverColor);
    else
        shape.setFillColor(normalColor);

    if (shape.getRadius() != Node::radius)
    {
        // update shape radius and text font size
        shape.setRadius(Node::radius);
        shape.setOrigin(sf::Vector2f(Node::radius, Node::radius));

        idText.setCharacterSize((int)Node::radius);
        this->idText.setOrigin(sf::Vector2f(this->idText.getGlobalBounds().width / 2.f,
            this->idText.getGlobalBounds().height));
    }
    win->draw(shape);
    if (drawNodeNames)
        win->draw(text);
    if (drawNodeId)
        win->draw(idText);
}