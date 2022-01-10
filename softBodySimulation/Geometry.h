#pragma once
#define WIDTH 800
#define HEIGHT 800
#define dt 0.01

struct Vertex {
	float x, y;
	float vx, vy;
	float fx, fy;
	float MASS = 1.0;

	Vertex(float ix, float iy) {
		x = ix;
		y = iy;
		vx = 0;
		vy = 0;
	}

	void draw(sf::RenderWindow& window) {
		sf::Vector2i mousePos = sf::Mouse::getPosition(window);

		sf::CircleShape circle;
		circle.setPosition(x - 3, y - 3);
		circle.setRadius(3);
		circle.setFillColor(sf::Color::Red);
		
		window.draw(circle);
	}

};

struct Spring {
	float sx, sy;
	float ex, ey;
	float svx, svy;
	float evx, evy;
	float L0;
	float nx, ny;

	Spring(float isx, float isy, float iex, float iey) {
		sx = isx;
		sy = isy;
		ex = iex;
		ey = iey;
		L0 = sqrt((sx - ex) * (sx - ex) + (sy - ey) * (sy - ey));
		nx = (sy - ey) / L0;
		ny =  -(sx - ex) / L0;
	}

	void draw(sf::RenderWindow& window) {
		sf::Vertex line[] =
		{
			sf::Vertex(sf::Vector2f(sx, sy)),
			sf::Vertex(sf::Vector2f(ex, ey))
		};
		window.draw(line, 2, sf::Lines);

	}
};

struct Vector2D {
	float x, y;

	Vector2D(float ix, float iy) {
		x = ix;
		y = iy;
	}
};