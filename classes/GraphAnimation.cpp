#include "GraphAnimation.h"
#include <iostream>

#include "AnimationComponent.h"

GraphAnimation::~GraphAnimation()
{
    for (auto& comp : components)
        delete comp;
    components.clear();
    for (auto& comp : finishedButStillDrawing)
        delete comp;
    finishedButStillDrawing.clear();
}

bool GraphAnimation::isPlaying()
{
    return playing;
}

void GraphAnimation::pause()
{
    playing = false;
}

void GraphAnimation::unpause()
{
    playing = true;
}

void GraphAnimation::setName(const char* _name)
{
    strcpy_s(name, _name);
}

char* GraphAnimation::getName()
{
    return name;
}

size_t GraphAnimation::size()
{
    return components.size();
}

void GraphAnimation::update(float delta)
{
    if (!playing)
        return;
    vector<AnimationComponent*> finished_now;
    /*for (auto& animation_comp : components)
    {
        animation_comp->update(delta);
        if (animation_comp->finished)
            finished_now.push_back(animation_comp);
    }*/
    // update only the first one
    if (components.size())
    {
        for (auto& an_comp : components)
        {
            an_comp->update(delta * timeScale);
            if (an_comp->finished)
                finished_now.push_back(an_comp);
            if (an_comp->stopAnimationsInQueue)
                break;
        }
        /*components[0]->update(delta * timeScale);
        if (components[0]->finished)
            finished_now.push_back(components[0]);*/
    }
    for (auto& an : finished_now)
    {
        finishedButStillDrawing.push_back(an);
        components.erase(find(components.begin(), components.end(), an));
        if (components.size() == 0)
        {
            // cout << " finished\n";
            finished = true;
        }
    }
}

void GraphAnimation::draw(sf::RenderWindow* win)
{
    for (auto& animation_comp : finishedButStillDrawing)
    {
        animation_comp->draw(win);
    }
    for (auto& animation_comp : components)
    {
        animation_comp->draw(win);
    }
}

void GraphAnimation::drawGUI()
{
    ImGui::PushID(this);
    ImGui::Text(name);
    if (isPlaying())
    {
        if (ImGui::Button("PAUSE"))
        {
            pause();
        }
    }
    else
    {
        if (ImGui::Button("PLAY"))
        {
            unpause();
        }
    }
    ImGui::SameLine();
    ImGui::SliderFloat("time scale", &timeScale, .2f, 5.f);
    ImGui::PopID();
}

GraphAnimation& GraphAnimation::addAnimation(AnimationComponent* component)
{
    components.push_back(component);
    return *this;
}

void GraphAnimation::setLastStopsQueue()
{
    components[components.size() - 1]->setStopAnimationsInQueue(true);
}

void GraphAnimation::removeAnimationComponentBefore(AnimationComponent* ac)
{
    finishedButStillDrawing.erase(finishedButStillDrawing.end() - 1);
}