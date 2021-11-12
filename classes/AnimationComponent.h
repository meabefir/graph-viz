#pragma once
#include <SFML/Graphics.hpp>
#include "Edge.h"
#include "Graph.h"
#include "GraphAnimation.h"

class AnimationComponent
{
protected:
    float progress = .0f;
    bool finished = false;
    float totalDuration = 1.f;
    bool stopAnimationsInQueue;
public:
    friend class GraphAnimation;
    AnimationComponent(float duration = 1.f, bool _stop_animations_in_queue = true);
    virtual void update(float delta);
    virtual void draw(sf::RenderWindow* win);
    void setStopAnimationsInQueue(bool);
};

////////////////////////////////////////////////////////////////////////////////////////

class PauseAnimation : public AnimationComponent
{

};

////////////////////////////////////////////////////////////////////////////////////////

class DrawableAnimationComponent : public AnimationComponent
{
protected:
    sf::Color baseColor;
public:
    DrawableAnimationComponent(sf::Color c = sf::Color::White, float duration = 1.f);
    virtual void draw(sf::RenderTarget* win);
};

////////////////////////////////////////////////////////////////////////////////////////

class EdgeAnimation : public DrawableAnimationComponent
{
protected:
    weak_ptr<Edge> edge;
    sf::Vertex vertexArr[2];
    sfLine line;

public:
    friend class GraphAnimation;

    EdgeAnimation(weak_ptr<Edge> e, sf::Color c = sf::Color::White, float duration = 1.f);
    virtual void draw(sf::RenderWindow* win);
};

////////////////////////////////////////////////////////////////////////////////////////

class EdgeAnimationReversed : public DrawableAnimationComponent
{
protected:
    weak_ptr<Edge> edge;
    sf::Vertex vertexArr[2];
    sfLine line;

public:
    friend class GraphAnimation;

    EdgeAnimationReversed(weak_ptr<Edge> e, sf::Color c = sf::Color::White, float duration = 1.f);
    virtual void draw(sf::RenderWindow* win);
};

////////////////////////////////////////////////////////////////////////////////////////

class EdgeAnimationDotted : public EdgeAnimation
{
public:
    EdgeAnimationDotted(weak_ptr<Edge> e, sf::Color, float duration = 1.f);
    virtual void draw(sf::RenderWindow* win);
};

////////////////////////////////////////////////////////////////////////////////////////

class EdgeAnimationMiddle : public EdgeAnimation
{
public:
    EdgeAnimationMiddle(weak_ptr<Edge> e, sf::Color, float duration = 1.f);
    virtual void draw(sf::RenderWindow* win);
};

////////////////////////////////////////////////////////////////////////////////////////

class GraphAlteringAnimationComponent : public AnimationComponent
{
protected:
    Graph* graph;
public:
    GraphAlteringAnimationComponent(Graph* g, float duration = .0f);
    virtual void alter() = 0;
};

////////////////////////////////////////////////////////////////////////////////////////

class GraphAlterRemoveEdge : public GraphAlteringAnimationComponent
{
    weak_ptr<Edge> edge;
public:
    GraphAlterRemoveEdge(Graph* g, weak_ptr<Edge> e, float duration = .0f);
    void update(float delta);
    void alter();
};

////////////////////////////////////////////////////////////////////////////////////////

class GraphAnimationAlteringAnimationComponent : public AnimationComponent
{
protected:
    GraphAnimation* graphAnimation;
public:
    GraphAnimationAlteringAnimationComponent(GraphAnimation* ga, float duration = .0f);
    virtual void alter() = 0;
};

////////////////////////////////////////////////////////////////////////////////////////

class GraphAnimationAlterRemoveBefore : public GraphAnimationAlteringAnimationComponent
{
public:
    GraphAnimationAlterRemoveBefore(GraphAnimation* ga, float duration = .0f);
    void update(float delta);
    void alter();
};