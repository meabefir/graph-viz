#pragma once
#include <SFML/Graphics.hpp>
#include <random>
#include <memory>

# define PI           3.14159265358979323846

using namespace std;

namespace Buttons
{
    extern sf::Keyboard::Key deleteButton;
}

// these to get the object or the adress of a weakptr
template <class T>
T& obj(weak_ptr<T>& wp);

template <class T>
shared_ptr<T> objp(weak_ptr<T>& wp);

template <class T>
T& obj(weak_ptr<T>& wp)
{
    return (*wp.lock());
}

template <class T>
shared_ptr<T> objp(weak_ptr<T>& wp)
{
    return wp.lock();
}

class Vector
{
public:
    static sf::Vector2f normalizeVec(const sf::Vector2f& v)
    {
        float length = getLength(v);
        return { v.x / length, v.y / length };
    }
    static float getLength(const sf::Vector2f& v)
    {
        return sqrt(v.x * v.x + v.y * v.y);
    }
    static float getLengthSquared(const sf::Vector2f& v)
    {
        return v.x * v.x + v.y * v.y;
    }
    static sf::Vector2f rotateVec(const sf::Vector2f& v, float deg)
    {
        //cout << v.x << ' ' << v.y << '\n';
        float rad = deg * PI / 180.f;
        float new_x = v.x * cos(rad) - v.y * sin(rad);
        float new_y = v.x * sin(rad) + v.y * cos(rad);
        //cout << new_x << ' ' << new_y << '\n';
        return sf::Vector2f(new_x, new_y);
    }
    static bool ccw(sf::Vector2f& a, sf::Vector2f& b, sf::Vector2f& c)
    {
        return ((c.y - a.y) * (b.x - a.x) > (b.y - a.y) * (c.x - a.x));
    }
    static bool segmentsIntersect(sf::Vector2f& a, sf::Vector2f& b, sf::Vector2f& c, sf::Vector2f& d)
    {
        return ((ccw(a, c, d) != ccw(b, c, d)) and (ccw(a, b, c) != ccw(a, b, d)));
    }
};

class Random
{
    static std::random_device rd;
    static std::default_random_engine eng;
    static std::uniform_real_distribution<float> distr;
public:
    static float randf()
    {
        return distr(eng);
    }
};

//__declspec(selectany) std::random_device Random::rd;
//__declspec(selectany) std::default_random_engine Random::eng(Random::rd());
//__declspec(selectany) std::uniform_real_distribution<float> Random::distr(0.f, 1.f);