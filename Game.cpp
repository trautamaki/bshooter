#include "Game.h"
#include <iostream>

Game::Game() {
    initWindow();
    initField();
    window->setFramerateLimit(60);
	running_ = true;
}

Game::~Game() {
    delete window;
    
    for (const auto& c : circles_) {
        delete c.circle;
    }

    delete active_.circle;
}

bool Game::running() {
    return running_;
}

void Game::initWindow() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8.0;
	window = new sf::RenderWindow(sf::VideoMode(FIELD_WIDTH, FIELD_HEIGHT),
		"Bubble shooter", sf::Style::Titlebar | sf::Style::Close, settings);

    // Setup the arrow
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
    circles_.reserve(INITIAL_BALLS_COLS * INITIAL_BALLS_ROWS);

    for (int x = 0; x < INITIAL_BALLS_COLS; x++) {
        for (int y = 0; y < INITIAL_BALLS_ROWS; y++) {
            int offsetY = 0;
            if (y % 2 == 0) {
                offsetY = C_RADIUS;
            }

            sf::CircleShape* shape = new sf::CircleShape(C_RADIUS);
            shape->setFillColor(colors_.at(rand() % colors_.size()));
            shape->setPosition(x * C_RADIUS * 2 + offsetY,
                y * C_RADIUS * 2);

            circles_.push_back(Circle{ shape });
        }
    }

    newCircle();
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

void Game::newCircle() {
    active_ = Circle{ new sf::CircleShape(C_RADIUS) };
    active_.circle->setFillColor(colors_.at(rand() % colors_.size()));
    active_.circle->setPosition(window->getSize().x / 2 - C_RADIUS,
        window->getSize().y - 2 * C_RADIUS);
}

void Game::launch() {
    // Get launch angle
    active_.direction = arrowTriangle_.getRotation();
}

void Game::update() {
    sf::Event event;
    while (window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window->close();

        } else if (event.type == sf::Event::MouseMoved) {
            updateArrow(sf::Mouse::getPosition(*window));

        } else if (event.type == sf::Event::MouseButtonReleased) {
            launch();
        }
    }

    window->clear();
    for (const auto& c : circles_) {
        window->draw(*c.circle);
    }

    // Move the active circle
    if (active_.circle != nullptr && active_.direction > 0) {
        double angle = active_.direction;
        double move_x = 5 * -sin(angle * PI / 180);
        double move_y = 5 * cos(angle * PI / 180);
        auto current_pos = active_.circle->getPosition();

        if (active_.circle->getPosition().x <= 0 ||
            active_.circle->getPosition().x >= window->getSize().x - 2 * C_RADIUS) {
            active_.direction = 360 - angle;
            move_x = -move_x;
        }

        active_.circle->setPosition(current_pos.x - move_x,
            current_pos.y - move_y);

        window->draw(*active_.circle);
    }

    // Draw the arrow parts
    window->draw(arrowLine_);
    window->draw(arrowTriangle_);
}

void Game::render() {
    window->display();
}
