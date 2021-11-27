#pragma once
#include <SFML/Graphics.hpp>
#include <array>
#include <deque>
#include <math.h>
#include <unordered_map>
#include <stack>

class Game {
public:
	Game();
	virtual ~Game();
	bool running();

	void update();
	void render();

private:
	// Create the window and set its settings.
	void initWindow();

	// Initialize the field, place initial circles.
	void initField();

	// Refresh arrow direction to point toward pos.
	void updateArrow(sf::Vector2i pos);

	// Spawn a new circle at arrow.
	void newCircle();

	// Launch the active circle to direction of mouseclick.
	void launch();

	// Check for collisions between active circle and stale
	// circles and place the active on the stale field.
	void checkCollision();

	// Check if circles have 3 or more of same color in a row
	// at coordinate x,y.
	// Return true if something is deleted.
	bool checkRemoval(int x, int y, sf::Color color);

	// Check for islands that may be left after removing
	// surrounding circles.
	void checkIslands();

	// Check if coordinate is inside field.
	// Return true if it is.
	bool isValidCoord(int x, int y);

	// Check if circle at x,y is same color as param color.
	// Return true if it is.
	bool isSameColor(int x, int y, sf::Color color);

	// Constants
	static constexpr unsigned int C_RADIUS = 20;

	static constexpr unsigned int SHOOTING_AREA_HEIGHT = 20;
	static constexpr unsigned int C_PER_ROW = 17;
	static const unsigned int C_PER_COL = 15;
	static constexpr unsigned int FIELD_WIDTH = C_PER_ROW * C_RADIUS * 2 + C_RADIUS;
	static constexpr unsigned int FIELD_HEIGHT = C_PER_COL * C_RADIUS * 2
		+ SHOOTING_AREA_HEIGHT;

	static constexpr unsigned int INITIAL_BALLS_ROWS = 9;
	static constexpr unsigned int INITIAL_BALLS_COLS = C_PER_ROW;

	static constexpr double ARROW_LENGTH = 100;

	const double PI = atan(1) * 4;

	struct Circle {
		std::shared_ptr<sf::CircleShape> circle;
		double direction = -1;
	};


	// Gameplay
	sf::RenderWindow* window = nullptr;
	bool running_ = false;
	bool is_active_moving_ = false;

	std::array<std::array<Circle*, C_PER_COL>, C_PER_ROW> circles_;
	Circle* active_;

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

	const std::vector<std::pair<int, int>> check_ = {
		{ 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 }
	};
	const std::vector<std::pair<int, int>> check_even_ = {
		{ 1, 1 }, { 1, -1 }
	};
	const std::vector<std::pair<int, int>> check_odd_ = {
		{ -1, 1 }, { -1, -1 }
	};
};
