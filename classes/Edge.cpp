#include "Edge.h"

float Edge::min_length = 100.f;
float Edge::max_length = 280.f;
float Edge::thickness = 3.5f;

extern bool drawCosts;
extern bool nodeRadiusChanged;

Edge::Edge(weak_ptr<Node>& _from, weak_ptr<Node>& _to, int cost, const float& thicc) :
    from(_from), to(_to), from_int(obj(_from).idx), to_int(obj(_to).idx),
    cost(cost), currentCost(cost),
    line(vertex, 2), arrowLine(arrowVertex, 3)
{
    normalColor = sf::Color::Yellow;

    line.setColor(normalColor);
    arrowLine.setColor(normalColor);

    _itoa_s(cost, costString, 10);

    costText.setCharacterSize(20);
    costText.setFont(defFont);
    costText.setString(costString);
    costText.setOrigin(sf::Vector2f(costText.getGlobalBounds().width / 2.f, costText.getGlobalBounds().height / 2.f));
    costText.setFillColor(sf::Color::Black);
    costText.setOutlineColor(sf::Color::White);
    costText.setOutlineThickness(2.f);

    // init the stringstream
    ss << from_int << " to " << to_int;
}

void Edge::setCost(int value)
{
    cost = value;
    _itoa_s(cost, costString, 10);
}

void Edge::update(float delta)
{
    auto& _from = obj(from);
    auto& _to = obj(to);

    if (prevVecFrom != _from.pos or prevVecTo != _to.pos)
        posChanged = true;
    else
        posChanged = false;

    prevVecFrom = _from.pos;
    prevVecTo = _to.pos;

    if (!posChanged and !nodeRadiusChanged)
        return;

    float length = Vector::getLength(_to.pos - _from.pos);
    if (length >= min_length && length <= max_length)
        return;

    sf::Vector2f from_to(_to.pos - _from.pos);
    // stc maybe just negate from_to
    sf::Vector2f to_from(_from.pos - _to.pos);

    float impulse_mod = 2.f * delta;
    if (length > max_length)
    {
        _from.addImpulse(from_to * impulse_mod);
        _to.addImpulse(to_from * impulse_mod);
    }
    else if (length < min_length)
    {
        _to.addImpulse(from_to * impulse_mod);
        _from.addImpulse(to_from * impulse_mod);
    }

}

void Edge::draw(sf::RenderWindow* win)
{
    if (posChanged or nodeRadiusChanged)
    {
        auto _from = obj(from);
        auto _to = obj(to);

        float offset_angle = 30.f;
        sf::Vector2f from_to_rotated_norm = Vector::normalizeVec(Vector::rotateVec((_to.pos - _from.pos), offset_angle));
        sf::Vector2f to_from_rotated_norm = Vector::normalizeVec(Vector::rotateVec((_from.pos - _to.pos), -offset_angle));

        vecFrom = _from.pos + from_to_rotated_norm * Node::radius;
        vecTo = _to.pos + to_from_rotated_norm * Node::radius;

        vertex[0].position = vecFrom;
        vertex[1].position = vecTo;
        line.setVertices(vertex);

        arrowVertex[1].position = vecFrom + (vecTo - vecFrom) / 2.f;
        arrowVertex[2].position = arrowVertex[1].position + from_to_rotated_norm * (-max(Node::radius, 30.f) / 2.f);
        arrowVertex[0].position = arrowVertex[1].position + to_from_rotated_norm * (max(Node::radius, 30.f) / 2.f);
        arrowLine.setVertices(arrowVertex);
    }

    // change color if needed
    if (wantedColor != currentColor)
    {
        currentColor = wantedColor;
        /*vertex[0].color = currentColor;
        vertex[1].color = currentColor;*/
        line.setColor(currentColor);

        /*arrowVertex[0].color = currentColor;
        arrowVertex[1].color = currentColor;
        arrowVertex[2].color = currentColor;*/
        arrowLine.setColor(currentColor);
    }

    // win->draw(vertex, 2, sf::Lines);
    line.draw(*win);
    arrowLine.draw(*win);
    // win->draw(arrowVertex, 3, sf::LinesStrip);

    if (drawCosts)
    {
        // check if cost changed and update costText
        if (cost != currentCost)
        {
            costText.setOrigin(sf::Vector2f(costText.getGlobalBounds().width / 2.f, costText.getGlobalBounds().height / 2.f));
            currentCost = cost;
            _itoa_s(cost, costString, 10);
            costText.setString(costString);
        }
        // set cost text pos
        costText.setPosition(arrowVertex[1].position);
        win->draw(costText);
    }
}

void Edge::drawGUI()
{
    ImGui::PushID(this);
    const string tmp = ss.str();
    const char* cstr = tmp.c_str();
    ImGui::DragInt(cstr, &cost, 1, -999999, 999999);
    ImGui::PopID();
    if (ImGui::IsItemHovered())
    {
        wantedColor = hoverColor;
    }
    else
    {
        wantedColor = normalColor;
    }
}

int Edge::getCost() const
{
    return cost;
}