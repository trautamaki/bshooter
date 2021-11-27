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
            delete y;
        }
    }

    delete active_;
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

            std::shared_ptr<sf::CircleShape> shape(new sf::CircleShape(C_RADIUS));
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
    active_ = new Circle{ std::make_shared<sf::CircleShape>(sf::CircleShape(C_RADIUS)) };
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
                    } else {
                        x = active_->circle->getPosition().x / (2 * C_RADIUS);
                    }
                    
                    int y = active_->circle->getPosition().y / (2 * C_RADIUS) + 1;
                    int offsetX = 0;

                    if (y % 2 == 0) offsetX = C_RADIUS;

                    active_->circle->setPosition(x * 2 * C_RADIUS + offsetX,
                        y * 2 * C_RADIUS);
                    circles_[x][y] = active_;

                    is_active_moving_ = false;
                    if (checkRemoval(x, y, active_->circle->getFillColor())) {
                        checkIslands();
                    }
                    
                    newCircle();
                }
            }
        }
    }
}

bool Game::checkRemoval(int orig_x, int orig_y, sf::Color color) {
    std::deque<std::pair<int, int>> d;
    std::map<std::pair<int, int>, Circle*> del;

    d.push_back({ orig_x, orig_y });

    while (!d.empty()) {
        int x = d.front().first;
        int y = d.front().second;
        d.pop_front();

        for (auto i : check_) {
            if (isSameColor(x + i.first, y + i.second, color)
                && del.find({ x + i.first, y + i.second }) == del.end()) {

                d.push_back({ x + i.first, y + i.second });
                del.insert({ { x + i.first, y + i.second  }, circles_[x + 1][y] });
                continue;
            }
        }

        if (y % 2 == 0) {
            for (auto i : check_even_) {
                if (isSameColor(x + i.first, y + i.second, color)
                    && del.find({ x + i.first, y + i.second }) == del.end()) {

                    d.push_back({ x + i.first, y + i.second });
                    del.insert({ { x + i.first, y + i.second  }, circles_[x + 1][y] });
                    continue;
                }
            }
        } else {
            for (auto i : check_odd_) {
                if (isSameColor(x + i.first, y + i.second, color)
                    && del.find({ x + i.first, y + i.second }) == del.end()) {

                    d.push_back({ x + i.first, y + i.second });
                    del.insert({ { x + i.first, y + i.second  }, circles_[x + 1][y] });
                    continue;
                }
            }
        }
    }

    if (del.size() >= 3) {
        for (auto& i : del) {
            delete circles_[i.first.first][i.first.second];
            circles_[i.first.first][i.first.second] = nullptr;
        }

        return true;
    }

    return false;
}

void Game::checkIslands() {
    // Find the first occupied coordinate in row y=0
    int start_y = 0;
    int start_x = -1;

    for (int i = 0; i < C_PER_ROW; i++) {
        if (circles_[0][i] != nullptr) {
            start_x = i;
            break;
        }
    }

    if (start_x == -1) return;

    std::deque<std::pair<int, int>> d;
    std::map<std::pair<int, int>, Circle*> visited;
    std::array<std::array<bool, C_PER_COL>, C_PER_ROW> res;

    for (int x = 0; x < res.size(); x++) {
        for (int y = 0; y < res[y].size(); y++) {
            res[x][y] = true;
        }
    }

    d.push_back({ start_x, start_y });

    while (!d.empty()) {
        int x = d.front().first;
        int y = d.front().second;
        d.pop_front();

        for (auto i : check_) {
            if (isValidCoord(x + i.first, y + i.second)
                && visited.find({ x + i.first, y + i.second }) == visited.end()
                && circles_[x + i.first][y + i.second] != nullptr) {

                d.push_back({ x + i.first, y + i.second });
                visited.insert({ { x + i.first, y + i.second  }, circles_[x + 1][y] });
                res[x + i.first][y + i.second] = false;
                continue;
            }
        }

        if (y % 2 == 0) {
            for (auto i : check_even_) {
                if (isValidCoord(x + i.first, y + i.second)
                    && visited.find({ x + i.first, y + i.second }) == visited.end()
                    && circles_[x + i.first][y + i.second] != nullptr) {

                    d.push_back({ x + i.first, y + i.second });
                    visited.insert({ { x + i.first, y + i.second  }, circles_[x + 1][y] });
                    res[x + i.first][y + i.second] = false;
                    continue;
                }
            }
        } else {
            for (auto i : check_odd_) {
                if (isValidCoord(x + i.first, y + i.second)
                    && visited.find({ x + i.first, y + i.second }) == visited.end()
                    && circles_[x + i.first][y + i.second] != nullptr) {

                    d.push_back({ x + i.first, y + i.second });
                    visited.insert({ { x + i.first, y + i.second  }, circles_[x + 1][y] });
                    res[x + i.first][y + i.second] = false;
                    continue;
                }
            }
        }
    }

    // Delete the ones that are marked �true�
    for (int x = 0; x < res.size(); x++) {
        for (int y = 0; y < res[y].size(); y++) {
            if (res[x][y] == true) {
                delete circles_[x][y];
                circles_[x][y] = nullptr;
            }
        }
    }
}

bool Game::isValidCoord(int x, int y) {
    if (x >= 0 && x < C_PER_ROW && y >= 0 && y < C_PER_COL) {
        return true;
    }

    return false;
}

bool Game::isSameColor(int x, int y, sf::Color color) {
    if (isValidCoord(x, y) && circles_[x][y] != nullptr
        && circles_[x][y]->circle->getFillColor() == color) {
        return true;
    }

    return false;
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
            is_active_moving_ = true;
        }
    }

    if (is_active_moving_) {
        checkCollision();
    }

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
