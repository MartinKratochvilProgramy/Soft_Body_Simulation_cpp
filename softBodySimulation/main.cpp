#include <SFML/Graphics.hpp>
#include <iostream>
#include <math.h>
#include "Geometry.h"


float gy = 600.;
float gx = 0.;
float ks = 7550.0;
float kd = 40050.0;
float P0 = -200000;

Vector2D midPoint(0, 0);

std::vector<Vertex> vertices;
std::vector<Spring> springs;

sf::RenderWindow window{ sf::VideoMode(WIDTH, HEIGHT), "My Program" };

static const int number_of_elements = 50;


float calculateBodyArea() {
	float vol = 0;
	float x1, y1;
	float x2, y2;

	//CALCULATE AREA USING GAUSS THEOREM
	for (int i = 0; i < springs.size(); i++)
	{
		x1 = springs[i].sx;
		y1 = springs[i].sy;
		x2 = springs[i].ex;
		y2 = springs[i].ey;

		float L = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));

		vol += 0.5 * fabs(x1 - x2) * fabs(springs[i].nx) * L;
	}
	return vol;
}

void accumulateForces() {
	//ASSIGN NEW VERTEX POSITIONS TO SPRINGS AND INITIALIZE GRAVITY FORCES
	for (int i = 0; i < springs.size(); i++)
	{
		if (i < springs.size() - 1) {
			springs[i].sx = vertices[i].x;
			springs[i].sy = vertices[i].y;
			springs[i].ex = vertices[i + 1].x;
			springs[i].ey = vertices[i + 1].y;
		}	//last spring:
		else {
			springs[i].sx = vertices[i].x;
			springs[i].sy = vertices[i].y;
			springs[i].ex = vertices[0].x;
			springs[i].ey = vertices[0].y;
		}

		vertices[i].fx = gx;
		vertices[i].fy = gy;
	}

	for (int i = 0; i < springs.size(); i++)
	{

		float x1 = springs[i].sx;
		float y1 = springs[i].sy;
		float x2 = springs[i].ex;
		float y2 = springs[i].ey;

		//CALCULATE SPRING LENGHT
		float L = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));


//*****************SPRING FORCES*****************
		float vx12, vy12;
		float F;
		float Fxs, Fys;

		//CALCULATE RELATIVE VELOCITIES
		vx12 = springs[i].svx - springs[i].evx;
		vy12 = springs[i].svy - springs[i].evy;

		//CALCULATE FORCE
		F = (L - springs[i].L0) * ks + (vx12 * (x1 - x2) + vy12 * (y1 - y2)) * kd / L;

		Fxs = ((x1 - x2) / L) * F;
		Fys = ((y1 - y2) / L) * F;

		//ACCUMULATE FORCES TO VERTICES
		vertices[i].fx -= Fxs;			
		vertices[i].fy -= Fys;

		if (i < vertices.size() - 1) {
			vertices[i + 1].fx += Fxs;
			vertices[i + 1].fy += Fys;
		}	//last spring
		else {
			vertices[0].fx += Fxs;
			vertices[0].fy += Fys;
		}

		//CALCULATE NORMAL VECTOR 
		springs[i].nx = +(y1 - y2) / L;
		springs[i].ny = -(x1 - x2) / L;



//*****************PRESSURE FORCES*****************
		float volume = calculateBodyArea();
		float pressurev = L * P0 * (1.0f / volume);

		//ACCUMULATE FORCES TO VERTICES
		vertices[i].fx += springs[i].nx * pressurev;
		vertices[i].fy += springs[i].ny * pressurev;

		if (i < vertices.size() - 1) {
			vertices[i + 1].fx += springs[i].nx * pressurev;
			vertices[i + 1].fy += springs[i].ny * pressurev;
		}	//last spring
		else {
			vertices[0].fx += springs[springs.size() - 1].nx * pressurev;
			vertices[0].fy += springs[springs.size() - 1].ny * pressurev;
		}
	}
}

void integrateEuler() {
	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i].vx = 0.99 * vertices[i].vx + (vertices[i].fx / vertices[i].MASS) * dt;
		vertices[i].vy = 0.99 * vertices[i].vy + (vertices[i].fy / vertices[i].MASS) * dt;

		//DEFINE BOUNDS
		if (vertices[i].y > HEIGHT - 1) {
			vertices[i].vy = -0.5 * abs(vertices[i].vy);
			vertices[i].vx = 0.1 * (vertices[i].vx);
		}
		if (vertices[i].x > WIDTH - 1) {
			vertices[i].vy = 0.1 * (vertices[i].vy);
			vertices[i].vx = -0.5 * abs(vertices[i].vx);
		}
		if (vertices[i].x < 1) {
			vertices[i].vy = 0.1 * (vertices[i].vy);
			vertices[i].vx = 0.5 * abs(vertices[i].vx);
		}
		if (vertices[i].y < 1) {
			vertices[i].vy = 0.5 * abs(vertices[i].vy);
			vertices[i].vx = 0.1 * (vertices[i].vx);
		}

		vertices[i].x = vertices[i].x + vertices[i].vx * dt;
		vertices[i].y = vertices[i].y + vertices[i].vy * dt;
	}
}

void integrateHeun() {
	float fx_saved[number_of_elements];
	float fy_saved[number_of_elements];	
	
	float vx_saved[number_of_elements];
	float vy_saved[number_of_elements];

	for (int i = 0; i < vertices.size(); i++)
	{
		fx_saved[i] = vertices[i].fx;
		fy_saved[i] = vertices[i].fy;		
		
		vx_saved[i] = vertices[i].vx;
		vy_saved[i] = vertices[i].vy;

		vertices[i].vx += vertices[i].fx / vertices[i].MASS * dt;
		vertices[i].vy += vertices[i].fy / vertices[i].MASS * dt;

		vertices[i].x += vertices[i].vx * dt;
		vertices[i].y += vertices[i].vy * dt;
	}

	accumulateForces();

	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i].vx = vx_saved[i] + (vertices[i].fx + fx_saved[i]) / vertices[i].MASS * dt / 2;
		vertices[i].vy = vy_saved[i] + (vertices[i].fy + fy_saved[i]) / vertices[i].MASS * dt / 2;

		if (vertices[i].y > 550) {
			vertices[i].vy = -0.5 * abs(vertices[i].vy);
			vertices[i].vx = 0.1 * (vertices[i].vx);
		}

		vertices[i].x += vertices[i].vx * dt;
		vertices[i].y += vertices[i].vy * dt;
	}

}

void drawMidPoint() {
	//calculates mid point as an average of x and y coordinate of all vertices
	float x0 = 0;
	float y0 = 0;

	for (int i = 0; i < vertices.size(); i++)
	{
		x0 += vertices[i].x;
		y0 += vertices[i].y;
	}

	midPoint.x = x0 / vertices.size();
	midPoint.y = y0 / vertices.size();

	sf::CircleShape mid;
	mid.setPosition(midPoint.x-3, midPoint.y-3);
	mid.setRadius(3);
	mid.setFillColor(sf::Color::Green);

	window.draw(mid);
}



int main()
{
	float time = 0.0;
	window.setFramerateLimit(120);
	sf::Event event;


//*****************INIT SOFT BODY*****************
	for (int i = 0; i < number_of_elements; i++)
	{
		float x = WIDTH / 2 + 20 * cos(i * 2 * 3.1415 / number_of_elements);
		float y = HEIGHT / 2 + 20 * sin(i * 2 * 3.1415 / number_of_elements);

		Vertex new_vertex(x, y);
		vertices.push_back(new_vertex);
	}
	for (int i = 0; i < number_of_elements; i++)
	{
		if (i < number_of_elements-1) {
			Spring new_spring(vertices[i].x, vertices[i].y, vertices[i + 1].x, vertices[i + 1].y);
			springs.push_back(new_spring);
		}
		else {
			Spring s(vertices[i].x, vertices[i].y, vertices[0].x, vertices[0].y);
			springs.push_back(s);
		}
	}

	bool change_gravity_on_mouse_click = false;

//*****************RUN MAIN SIMULATION LOOP*****************
	while (window.isOpen()) {
		sf::Vector2i mousePos = sf::Mouse::getPosition(window);
		window.clear();

		while (window.pollEvent(event))
		{

			if (event.type == sf::Event::Closed) {
				window.close();
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
				window.close();
			}

			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				change_gravity_on_mouse_click = true;
			}
			else if (event.type == sf::Event::MouseButtonReleased) {
				if (event.key.code == sf::Mouse::Left) {
					change_gravity_on_mouse_click = false;

				}
			}
		}

		if (change_gravity_on_mouse_click)
		{
			gx = (mousePos.x - midPoint.x)/0.9;
			gy = (mousePos.y - midPoint.y)/0.9;
		}
		else {
			gx = 0;
			gy = 600;
		}

		accumulateForces();

		integrateEuler();

		//integrateHeun(vertices, springs);

		if (time > 5.){
//*****************DISPLAY SPRINGS, VERTICES, FORCE DIRECTION*****************
			for (int i = 0; i < vertices.size(); i++) {
				//vertices[i].draw(window);
				springs[i].draw(window);
			}

			drawMidPoint();

			sf::Vertex mouseLine[] =
				{
				sf::Vertex(sf::Vector2f(midPoint.x, midPoint.y), sf::Color::Green),
				sf::Vertex(sf::Vector2f(mousePos.x, mousePos.y), sf::Color::Green)
				};
			window.draw(mouseLine, 2, sf::Lines);

			window.display();

			
		}
		time += dt;
	}
}


