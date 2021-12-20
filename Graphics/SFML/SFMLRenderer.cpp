#include "SFMLRenderer.h"
#include "../../Config.h"
#include <iostream>

SFMLRenderer::SFMLRenderer() {
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8.0;
	window = new sf::RenderWindow(sf::VideoMode(Config::FIELD_WIDTH, Config::FIELD_HEIGHT),
		"Bubble shooter", sf::Style::Titlebar | sf::Style::Close, settings);
}

SFMLRenderer::~SFMLRenderer() {
	delete window;
}

void SFMLRenderer::draw(const sf::Drawable& drawable) {
	window->draw(drawable);
}


void SFMLRenderer::display() {
	window->display();
}

void SFMLRenderer::setFramerate(uint32_t framerate) {
	window->setFramerateLimit(framerate);
}

bool SFMLRenderer::pollEvent(sf::Event& event) {
	return window->pollEvent(event);
}

sf::Vector2u SFMLRenderer::getSize() {
	return window->getSize();
}

sf::Vector2i SFMLRenderer::getMousePosition() {
	return sf::Mouse::getPosition(*window);
}

void SFMLRenderer::clear() {
	window->clear();
}

void SFMLRenderer::close() {
	window->close();
}