#include "Helper.h"

namespace Buttons
{
	sf::Keyboard::Key deleteButton = sf::Keyboard::Key::D;
}

std::random_device Random::rd;
std::default_random_engine Random::eng(Random::rd());
std::uniform_real_distribution<float> Random::distr(0.f, 1.f);