#include "Game.h"
#include <iostream>

Game::Game() {
    for (int i = 0; i < C_PER_ROW; i++) {
        for (int j = 0; j < C_PER_COL; j++) {
            circles_[i][j] = nullptr;
        }
    }

    initWindow();
    initField();
    window->setFramerateLimit(60);
    running_ = true;
}

Game::~Game() {
    delete window;
    
    for (const auto& x : circles_) {
        for (const auto& y : x) {
            delete y->circle;
            delete y;
        }
    }

    delete active_->circle;
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
    for (int x = 0; x < INITIAL_BALLS_COLS; x++) {
        for (int y = 0; y < INITIAL_BALLS_ROWS; y++) {
            int offsetX = 0;
            if (y % 2 == 0) {
                offsetX = C_RADIUS;
            }

            sf::CircleShape* shape = new sf::CircleShape(C_RADIUS);
            shape->setFillColor(colors_.at(rand() % colors_.size()));
            shape->setPosition(x * C_RADIUS * 2 + offsetX,
                y * C_RADIUS * 2);

            circles_[x][y] = new Circle { shape };
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
    active_ = new Circle{ new sf::CircleShape(C_RADIUS) };
    active_->circle->setFillColor(colors_.at(rand() % colors_.size()));
    active_->circle->setPosition(window->getSize().x / 2 - C_RADIUS,
        window->getSize().y - C_RADIUS);
}

void Game::launch() {
    // Get launch angle
    active_->direction = arrowTriangle_.getRotation();
}

void Game::checkCollision() {
    for (const auto& x : circles_) {
        for (const auto& y : x) {
            if (y != nullptr) {
                float a_square = pow(y->circle->getPosition().x
                    - active_->circle->getPosition().x, 2);
                float b_square = pow(y->circle->getPosition().y
                    - active_->circle->getPosition().y, 2);

                if (sqrt(a_square + b_square) <= 2 * C_RADIUS) {
                    double old_direction = active_->direction;
                    active_->direction = -1;

                    int x;
                    if (old_direction > 0 && old_direction < 180) {
                        x = std::round(active_->circle->getPosition().x / (2 * C_RADIUS));
                    }
                    else {
                        x = active_->circle->getPosition().x / (2 * C_RADIUS);
                    }
                    
                    int y = active_->circle->getPosition().y / (2 * C_RADIUS) + 1;
                    int offsetX = 0;

                    if (y % 2 == 0)
                        offsetX = C_RADIUS;

                    active_->circle->setPosition(x * 2 * C_RADIUS + offsetX,
                        y * 2 * C_RADIUS);
                    circles_[x][y] = active_;
                    checkRemoval(x, y, active_->circle->getFillColor());
                    newCircle();
                }
            }
        }
    }
}

void Game::checkRemoval(int orig_x, int orig_y, sf::Color color) {
    std::deque<std::pair<int, int>> d;
    std::deque<std::pair<int, int>> b;
    d.push_back({ orig_x, orig_y });
    b.push_back({ orig_x, orig_y });

    while (!d.empty()) {
        int x = d.front().first;
        int y = d.front().second;
        d.pop_front();

        if (circles_[x + 1][y] != nullptr
            && circles_[x + 1][y]->circle->getFillColor() == color) {
            d.push_back({ x + 1, y });
            b.push_back({ x + 1, y });
        }

        if (circles_[x - 1][y] != nullptr
            && circles_[x - 1][y]->circle->getFillColor() == color) {
            d.push_back({ x - 1, y });
            b.push_back({ x - 1, y });
        }

        if (circles_[x][y + 1] != nullptr
            && circles_[x][y + 1]->circle->getFillColor() == color) {
            d.push_back({ x, y + 1 });
            b.push_back({ x, y + 1 });
        }

        if (circles_[x][y - 1] != nullptr
            && circles_[x][y - 1]->circle->getFillColor() == color) {
            d.push_back({ x, y - 1 });
            b.push_back({ x, y - 1 });
        }

        if (y % 2 == 0) {
            if (circles_[x + 1][y + 1] != nullptr
                && circles_[x + 1][y + 1]->circle->getFillColor() == color) {
                d.push_back({ x + 1, y + 1 });
                b.push_back({ x + 1, y + 1 });
            }

            if (circles_[x + 1][y - 1] != nullptr
                && circles_[x + 1][y - 1]->circle->getFillColor() == color) {
                d.push_back({ x + 1, y - 1 });
                b.push_back({ x + 1, y - 1 });
            }
        } else {
            if (circles_[x - 1][y + 1] != nullptr
                && circles_[x - 1][y + 1]->circle->getFillColor() == color) {
                d.push_back({ x - 1, y + 1 });
                b.push_back({ x - 1, y + 1 });
            }

            if (circles_[x - 1][y - 1] != nullptr
                && circles_[x - 1][y - 1]->circle->getFillColor() == color) {
                d.push_back({ x - 1, y - 1 });
                b.push_back({ x - 1, y - 1 });
            }
        }

        if (b.size() >= 3) {
            for (auto& i : b) {
                delete circles_[i.first][i.second];
                circles_[i.first][i.second] = nullptr;
            }
        }
    }
}

void Game::checkIslands() {

}

void Game::update() {
    sf::Event event;
    while (window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window->close();

        } else if (event.type == sf::Event::MouseMoved) {
            updateArrow(sf::Mouse::getPosition(*window));

        } else if (event.type == sf::Event::MouseButtonReleased
            && active_->direction < 0) {
            launch();
        }
    }

    checkCollision();
    checkIslands();

    window->clear();

    for (const auto& x : circles_) {
        for (const auto& y : x) {
            if (y != nullptr) {
                window->draw(*y->circle);
            }
        }
    }

    // Move the active circle
    if (active_->circle != nullptr) {
        if (active_->direction > 0) {
            double angle = active_->direction;
            double move_x = 5 * -sin(angle * PI / 180);
            double move_y = 5 * cos(angle * PI / 180);
            auto current_pos = active_->circle->getPosition();

            if (active_->circle->getPosition().x <= 0 ||
                active_->circle->getPosition().x >= window->getSize().x - 2 * C_RADIUS) {
                active_->direction = 360 - angle;
                move_x = -move_x;
            }

            active_->circle->setPosition(current_pos.x - move_x,
                current_pos.y - move_y);
        }

        window->draw(*active_->circle);
    }

    // Draw the arrow parts
    window->draw(arrowLine_);
    window->draw(arrowTriangle_);
}

void Game::render() {
    window->display();
}
