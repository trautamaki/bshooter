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

    arrowTriangle_ = sf::CircleShape(15, 3);
    arrowTriangle_.setPosition(arrowLine_[1].position.x,
        arrowLine_[1].position.y);
    arrowTriangle_.setOrigin(arrowTriangle_.getRadius(),
        3/2 * arrowTriangle_.getRadius());
}

void Game::initField() {
    circles_.reserve((int) INITIAL_BALLS_COLS * INITIAL_BALLS_ROWS);

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
    // The line
    auto wSize = window->getSize();
    double x, angle;
    double len_y = (double) wSize.y - pos.y;
    double len_x = abs(wSize.x / 2.0 - pos.x);
    double f = ARROW_LENGTH /
        (sqrt(pow(len_x, 2) + pow(len_y, 2)));
    double y = wSize.y - len_y * f;

    if (pos.x < window->getSize().x / 2) {
        x = (wSize.x / 2) - (len_x) * f;
        angle = atan2(len_y, len_x) * 180 / PI - 90;
    } else {
        x = (wSize.x / 2) + (len_x) * f;
        angle = -atan2(len_y, len_x) * 180 / PI + 90;
    }

    arrowLine_[1].position = sf::Vector2f(x, y);

    // The triangle
    arrowTriangle_.setPosition(arrowLine_[1].position.x,
        arrowLine_[1].position.y);
    arrowTriangle_.setRotation(angle);
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

    // Draw the arrow parts
    window->draw(arrowLine_);
    window->draw(arrowTriangle_);
}

void Game::render() {
    window->display();
}
