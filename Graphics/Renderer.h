#pragma once
#include <SFML/Graphics.hpp> //Temporarily use SFML types
class Renderer
{
public:
	virtual ~Renderer() = default;
	virtual void draw(const sf::Drawable& drawable) = 0;
	virtual void clear() = 0;
	virtual void display() = 0;
	virtual void setFramerate(uint32_t frames) = 0;
	virtual bool pollEvent(sf::Event& event) = 0;
	virtual sf::Vector2u getSize() = 0;
	virtual sf::Vector2i getMousePosition() = 0;
	virtual void close() = 0;
};

