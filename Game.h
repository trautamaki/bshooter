#pragma once
#include <SFML/Graphics.hpp>
#include <math.h>

class Game {
public:
	Game();
	virtual ~Game();
	bool running();

	void update();
	void render();

private:
	void initWindow();
	void initField();
	void updateArrow(sf::Vector2i pos);

	// Constants
	const unsigned int C_RADIUS = 20;

	const unsigned int SHOOTING_AREA_HEIGHT = 20;
	const unsigned int C_PER_ROW = 17;
	const unsigned int C_PER_COL = 15;
	const unsigned int FIELD_WIDTH = C_PER_ROW * C_RADIUS * 2 + C_RADIUS;
	const unsigned int FIELD_HEIGHT = C_PER_COL * C_RADIUS * 2
		+ SHOOTING_AREA_HEIGHT;

	const unsigned int INITIAL_BALLS_ROWS = 9;
	const unsigned int INITIAL_BALLS_COLS = C_PER_ROW;

	const double ARROW_LENGTH = 100;

	const double PI = atan(1) * 4;

	enum Color {
		GREEN,
		YELLOW,
		MAGENTA,
		BLUE,
		RED,
		CYAN,
		AMOUNT_COLORS,
	};

	struct Circle {
		sf::CircleShape circle;

		// A moving circle
		bool stale = false;
		int direction = -1;
		int x = -1;
		int y = -1;
	};


	// Gameplay
	sf::RenderWindow* window = nullptr;
	bool running_ = false;

	std::vector<Circle> circles_;

	std::vector<sf::Color> colors_ = {
		sf::Color::Green,
		sf::Color::Yellow,
		sf::Color::Magenta,
		sf::Color::Blue,
		sf::Color::Red,
		sf::Color::Cyan,
	};

	// The arrow
	sf::VertexArray arrowLine_;
	sf::CircleShape arrowTriangle_;
};
