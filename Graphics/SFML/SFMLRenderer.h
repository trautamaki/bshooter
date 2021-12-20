#pragma once
#include "../Renderer.h"
#include <SFML/Graphics.hpp>


class SFMLRenderer : public Renderer {
public:
	SFMLRenderer();
	~SFMLRenderer();
	void draw(const sf::Drawable& drawable);
	void clear();
	void display();
	void setFramerate(uint32_t frames);
	bool pollEvent(sf::Event& event);
	sf::Vector2u getSize();
	sf::Vector2i getMousePosition();
	void close();
private:
	sf::RenderWindow* window = nullptr;
};