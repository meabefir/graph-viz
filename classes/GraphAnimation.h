#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"

class AnimationComponent;

using namespace std;

class GraphAnimation
{
    vector<AnimationComponent*> components;
    vector<AnimationComponent*> finishedButStillDrawing;
    bool finished = false;
    char name[100];
    bool playing = true;
    float timeScale = 1.f;

public:
    friend class Graph;
    ~GraphAnimation();
    bool isPlaying();
    void pause();
    void unpause();
    void setName(const char* _name);
    char* getName();
    size_t size();
    void update(float delta);
    void draw(sf::RenderWindow* win);
    void drawGUI();
    GraphAnimation& addAnimation(AnimationComponent* component);
    void setLastStopsQueue();
    void removeAnimationComponentBefore(AnimationComponent* ac);
};

