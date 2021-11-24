#include "Game.h"
#include <iostream>

Game::Game() {
    initWindow();
    initField();
	running_ = true;
}

Game::~Game() {
    delete window;
}

bool Game::running() {
	return running_;
}

void Game::initWindow() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8.0;
	window = new sf::RenderWindow(sf::VideoMode(FIELD_WIDTH, FIELD_HEIGHT),
		"Bubble shooter", sf::Style::Titlebar | sf::Style::Close, settings);

    arrowLine_ = sf::VertexArray(sf::LinesStrip, 2);
    arrowLine_[0].position = sf::Vector2f(window->getSize().x / 2,
        window->getSize().y);
    arrowLine_[1].position = sf::Vector2f(window->getSize().x / 2,
        window->getSize().y - ARROW_LENGTH);
    arrowLine_[0].color = sf::Color::White;
    arrowLine_[1].color = sf::Color::White;
}

void Game::initField() {
    circles_.reserve(INITIAL_BALLS_COLS * INITIAL_BALLS_ROWS);

    for (int x = 0; x < INITIAL_BALLS_COLS; x++) {
        for (int y = 0; y < INITIAL_BALLS_ROWS; y++) {
            int offsetY = 0;
            if (y % 2 == 0) {
                offsetY = C_RADIUS;
            }

            sf::CircleShape shape(C_RADIUS);
            shape.setFillColor(colors_.at(rand() % AMOUNT_COLORS));
            shape.setPosition(x * C_RADIUS * 2 + offsetY,
                y * C_RADIUS * 2);

            circles_.push_back(Circle{ shape });
        }
    }
}

void Game::updateArrow(sf::Vector2i pos) {
    auto wSize = window->getSize();
    double f, x;

    if (pos.x < window->getSize().x / 2) {
        double len_x = wSize.x / 2.0 - pos.x;
        f = ARROW_LENGTH /
            (sqrt(pow(len_x, 2) + pow(wSize.y - pos.y, 2)));
        x = (wSize.x / 2) - (len_x) * f;
    } else {
        double len_x = pos.x - wSize.x / 2.0;
        f = ARROW_LENGTH /
            (sqrt(pow(len_x, 2) + pow(wSize.y - pos.y, 2)));
        x = (wSize.x / 2) + (len_x) * f;
    }

    double y = wSize.y - ((double) wSize.y - pos.y) * f;
    arrowLine_[1].position = sf::Vector2f(x, y);
}

void Game::update() {
    sf::Event event;
    while (window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window->close();

        } else if (event.type == sf::Event::MouseMoved) {
            updateArrow(sf::Mouse::getPosition(*window));
        }
    }

    window->clear();
    for (auto b : circles_) {
        window->draw(b.circle);
    }

    window->draw(arrowLine_);
}

void Game::render() {
    window->display();
}
