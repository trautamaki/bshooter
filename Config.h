#pragma once

class Config {
public:
	// Constants
	static constexpr unsigned int C_RADIUS = 20;

	static constexpr unsigned int SHOOTING_AREA_HEIGHT = 20;
	static constexpr unsigned int C_PER_ROW = 17;
	static const unsigned int C_PER_COL = 15;
	static constexpr unsigned int FIELD_WIDTH = C_PER_ROW * C_RADIUS * 2 + C_RADIUS;
	static constexpr unsigned int FIELD_HEIGHT = C_PER_COL * C_RADIUS * 2
		+ SHOOTING_AREA_HEIGHT;

	static constexpr unsigned int INITIAL_C_ROWS = 9;
	static constexpr unsigned int INITIAL_C_COLS = C_PER_ROW;

	static constexpr double ARROW_LENGTH = 100;
};