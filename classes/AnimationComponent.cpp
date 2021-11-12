#include "AnimationComponent.h"
#include <iostream>

using namespace std;

AnimationComponent::AnimationComponent(float duration, bool _stop_animations_in_queue) :
    totalDuration(duration), stopAnimationsInQueue(_stop_animations_in_queue)
{

}

void AnimationComponent::update(float delta)
{
    if (totalDuration == 0.f)
        progress = 1.f;
    else
        progress += delta / totalDuration;
    progress = min(progress, 1.f);

    if (progress >= 1.f)
    {
        finished = true;
    }
}

void AnimationComponent::draw(sf::RenderWindow* win)
{

}

void AnimationComponent::setStopAnimationsInQueue(bool val)
{
    stopAnimationsInQueue = val;
}

////////////////////////////////////////////////////////////////////////////////////////

DrawableAnimationComponent::DrawableAnimationComponent(sf::Color c, float duration) :
    AnimationComponent(duration),
    baseColor(c)
{

}

void DrawableAnimationComponent::draw(sf::RenderTarget* win)
{

}

////////////////////////////////////////////////////////////////////////////////////////


EdgeAnimation::EdgeAnimation(weak_ptr<Edge> e, sf::Color c, float dur) :
    DrawableAnimationComponent(c, dur),
    edge(e), line(vertexArr, 2)
{
    line.setColor(baseColor);
}

void EdgeAnimation::draw(sf::RenderWindow* win)
{
    if (edge.expired())
        return;
    if (progress == 0.f)
        return;
    vertexArr[0].position = objp(edge)->vecFrom;
    sf::Vector2f dir = objp(edge)->vecTo - objp(edge)->vecFrom;
    vertexArr[1].position = vertexArr[0].position + dir * progress;
    line.setVertices(vertexArr);
    line.draw(*win);
}

////////////////////////////////////////////////////////////////////////////////////////

EdgeAnimationReversed::EdgeAnimationReversed(weak_ptr<Edge> e, sf::Color c, float dur) :
    DrawableAnimationComponent(c, dur),
    edge(e), line(vertexArr, 2)
{
    line.setColor(baseColor);
}

void EdgeAnimationReversed::draw(sf::RenderWindow* win)
{
    if (edge.expired())
        return;
    if (progress == 0.f)
        return;
    float current_progress = 1.f - progress;
    vertexArr[0].position = objp(edge)->vecFrom;
    sf::Vector2f dir = objp(edge)->vecTo - objp(edge)->vecFrom;
    vertexArr[1].position = vertexArr[0].position + dir * current_progress;
    line.setVertices(vertexArr);
    line.draw(*win);
}

////////////////////////////////////////////////////////////////////////////////////////

EdgeAnimationDotted::EdgeAnimationDotted(weak_ptr<Edge> e, sf::Color _c, float dur) :
    EdgeAnimation(e, _c, dur)
{

}

void EdgeAnimationDotted::draw(sf::RenderWindow* win)
{
    if (edge.expired())
        return;
    if (progress == 0.f)
        return;

    sf::Vector2f _from = objp(edge)->vecFrom;
    sf::Vector2f _to = objp(edge)->vecTo;

    float total_len = Vector::getLength(_to - _from);
    float total_steps = total_len / 5.f;
    float fraction_len = total_len / total_steps;
    sf::Vector2f fraction_vec = (_to - _from) / total_steps;
    sf::Vector2f unit_vec = (_to - _from) / total_len;
    float current_max_len = total_len * progress;
    float current_len = .0f;
    int idx = 0;
    float current_step = 0.f;
    while (current_len <= current_max_len)
    {
        vertexArr[idx].position = _from + fraction_vec * current_step;
        current_len += fraction_len;
        if (idx == 1)
        {
            line.setVertices(vertexArr);
            line.draw(*win);
        }
        current_step += 1.f;
        idx += 1;
        idx %= 2;
    }
    vertexArr[idx].position = _from + unit_vec * current_max_len;
    if (idx == 1)
    {
        line.setVertices(vertexArr);
        line.draw(*win);
    }
}

////////////////////////////////////////////////////////////////////////////////////////


EdgeAnimationMiddle::EdgeAnimationMiddle(weak_ptr<Edge> e, sf::Color c, float dur) :
    EdgeAnimation(e, c, dur)
{

}

void EdgeAnimationMiddle::draw(sf::RenderWindow* win)
{
    if (edge.expired())
        return;
    if (progress == 0.f)
        return;

    sf::Vector2f _from = obj(edge).vecFrom;
    sf::Vector2f _to = obj(edge).vecTo;
    float total_dist = Vector::getLength(_to - _from);
    sf::Vector2f from_to_norm = (_to - _from) / total_dist;
    sf::Vector2f mid = (_from + _to) / 2.f;
    float half_dist = total_dist / 2.f;

    vertexArr[0].position = mid - from_to_norm * half_dist * progress;
    vertexArr[1].position = mid + from_to_norm * half_dist * progress;
    line.setVertices(vertexArr);

    line.draw(*win);
}

////////////////////////////////////////////////////////////////////////////////////////

GraphAlteringAnimationComponent::GraphAlteringAnimationComponent(Graph* g, float duration) :
    AnimationComponent(duration),
    graph(g)
{

}

////////////////////////////////////////////////////////////////////////////////////////

GraphAlterRemoveEdge::GraphAlterRemoveEdge(Graph* g, weak_ptr<Edge> e, float duration) :
    GraphAlteringAnimationComponent(g, duration),
    edge(e)
{

}

void GraphAlterRemoveEdge::update(float delta)
{
    AnimationComponent::update(delta);
    if (finished)
        alter();
}

void GraphAlterRemoveEdge::alter()
{
    if (edge.expired())
        return;
    graph->deleteEdge(edge);
}

////////////////////////////////////////////////////////////////////////////////////////

GraphAnimationAlteringAnimationComponent::GraphAnimationAlteringAnimationComponent(GraphAnimation* ga, float duration) :
    AnimationComponent(duration),
    graphAnimation(ga)
{

}

////////////////////////////////////////////////////////////////////////////////////////

GraphAnimationAlterRemoveBefore::GraphAnimationAlterRemoveBefore(GraphAnimation* ga, float duration) :
    GraphAnimationAlteringAnimationComponent(ga, duration)
{

}

void GraphAnimationAlterRemoveBefore::update(float delta)
{
    AnimationComponent::update(delta);
    if (finished)
        alter();
}

void GraphAnimationAlterRemoveBefore::alter()
{
    graphAnimation->removeAnimationComponentBefore(this);
}