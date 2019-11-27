#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Main.hpp>
#include <iostream>
#include <string>

sf::Font font;
sf::SoundBuffer boingBuffer;
sf::Sound boingSound;
sf::SoundBuffer cheersBuffer;
sf::Sound cheersSound;
sf::SoundBuffer ahhhBuffer;
sf::Sound ahhhSound;
sf::SoundBuffer whoaBuffer;
sf::Sound whoaSound;
sf::SoundBuffer boopBuffer;
sf::Sound boopSound;
sf::SoundBuffer dunnBuffer;
sf::Sound dunnSound;
sf::SoundBuffer pewBuffer;
sf::Sound pewSound;
sf::SoundBuffer screamBuffer;
sf::Sound screamSound;
sf::Texture brickTexture;
sf::Texture greyTexture;
sf::Texture yellowTexture;
sf::Texture eyesTexture;
const float pi = 3.14159f;
int score = 0;

//level1positions
float level1x[32] = {1,61,121,181,241,301,361,421,481,541,1,61,121,181,241,301,361,421,481,541,1,61,121,181,241,301,361,421,481,541,61,481};
float level1y[32] = {50,50,50,50,50,50,50,50,50,50,75,75,75,75,75,75,75,75,75,75,100,100,100,100,100,100,100,100,100,100,125,125};
int level1types[32] = {1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

//level2positions
float level2x[32] = { 1,61,121,181,241,301,361,421,481,541,1,61,121,181,241,301,361,421,481,541,1,61,121,181,241,301,361,421,481,541,61,481 };
float level2y[32] = {250,250,250,250,250,250,250,250,250,250,275,275,275,275,275,275,275,275,275,275,300,300,300,300,300,300,300,300,300,300,325,325 };
int level2types[32] = { 1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2 };

//level3positions
float level3x[32] = { 1,61,121,181,241,301,361,421,481,541,1,61,121,181,241,301,361,421,481,541,1,61,121,181,241,301,361,421,481,541,61,481 };
float level3y[32] = { 50,50,50,50,50,50,50,50,50,50,75,75,75,75,75,75,75,75,75,75,300,300,300,300,300,300,300,300,300,300,325,325 };
int level3types[32] = { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,3,3,3,3,3,3,3,3,3,2,2 };

class paddle {
public:
	sf::RectangleShape rectangle;
	float speed = 300.f;

	paddle(float x, float y) {
		rectangle.setPosition(x, y);
		rectangle.setSize(sf::Vector2f(50, 10));
	}

	void move(bool right, sf::Time dt) {
		if (right)
		{
			//do moving left
			if (rectangle.getPosition().x - (speed * dt.asSeconds()) < 0) {
				rectangle.setPosition(sf::Vector2f(0, rectangle.getPosition().y));
			}
			else {
				rectangle.move(sf::Vector2f(-(speed * dt.asSeconds()), 0));
			}
		}
		else {
			//do moving right
			if (rectangle.getPosition().x + (speed * dt.asSeconds()) > 600 - rectangle.getSize().x - 1) {
				rectangle.setPosition(sf::Vector2f(600 - rectangle.getSize().x - 1, rectangle.getPosition().y));
			}
			else {
				rectangle.move(sf::Vector2f((speed * dt.asSeconds()), 0));
			}
		}
	}

	void setPosition(sf::Vector2f position) {
		rectangle.setPosition(position);
	}

	float x() {
		return rectangle.getPosition().x;
	}

	float y() {
		return rectangle.getPosition().y;
	}

	float width() {
		return rectangle.getSize().x;
	}

	float height() {
		return rectangle.getSize().y;
	}
};

class bricks {
public:
	sf::Vector2f positions[32];
	int healths[32] = {0};
	float width = 55.f;
	float height = 20.f;
	int brickType[32] = {0};
	sf::RectangleShape brick[32];

	void setUp(int level) {
		for (int i = 0; i < 32; ++i) {
			switch (level) {
			case 1:
				positions[i] = sf::Vector2f(level1x[i], level1y[i]);
				brickType[i] = level1types[i];
				break;
			case 2:
				positions[i] = sf::Vector2f(level2x[i], level2y[i]);
				brickType[i] = level2types[i];
				break;
			case 3:
				positions[i] = sf::Vector2f(level3x[i], level3y[i]);
				brickType[i] = level3types[i];
				break;
			default:
				break;
			}
			brick[i].setPosition(positions[i]);
			brick[i].setSize(sf::Vector2f(width, height));
			switch (brickType[i]) {
			case 0:
				brick[i].setTexture(&brickTexture);
				healths[i] = 1;
				break;
			case 1:
				brick[i].setTexture(&greyTexture);
				healths[i] = 2;
				break;
			case 2:
				brick[i].setTexture(&yellowTexture);
				healths[i] = 1;
				break;
			case 3:
				brick[i].setTexture(&eyesTexture);
				healths[i] = 1;
				break;
			default:
				break;
			}
		}
	}

	void doCollision(int collisionId) {
		--healths[collisionId];
		if (brickType[collisionId] == 3) {
			//SCREAM
			screamSound.play();
			score += 1;
		}
		else if (healths[collisionId] > 0) {
			//bounce off brick, don't destroy
			boopSound.play();
		}
		else {
			//destroy brick
			dunnSound.play();
			score += 1;
		}
	}
};

class ball {
public:
	sf::CircleShape circle;
	float angle;
	float speed;

	ball(sf::Vector2f startPos, float radius) {

		angle = pi * 1.5f;
		speed = 300.00f;
		circle.setRadius(radius);
		circle.setPosition(startPos);
		circle.move(sf::Vector2f(-5.f,-5.f));
	}

	void move(sf::Time dt) {
		circle.move(sf::Vector2f(cosf(angle) * speed * dt.asSeconds(), sinf(angle) * speed * dt.asSeconds()));
	}

	int checkCollision(paddle leftP, bricks bricks, sf::Time dt) {
		int collision = -1;
		
		//bounce off left, right, and upper walls
		if (circle.getPosition().y < 0) {
			//bounce off top
			angle = -angle;
			circle.setPosition(sf::Vector2f(circle.getPosition().x - (cosf(angle) * speed * dt.asSeconds() / 2), 0.1f));
			boingSound.play();
		}
		if (circle.getPosition().x < 0) {
			//bounce off left wall
			angle = pi - angle;
			circle.setPosition(sf::Vector2f(0.1f, circle.getPosition().y - (sinf(angle) * speed * dt.asSeconds() / 2)));
			boingSound.play();
		}
		else if (circle.getPosition().x + 10.f > 600) {
			//bounce off right wall
			angle = pi - angle;
			circle.setPosition(sf::Vector2f(599.9f - circle.getRadius() * 2, circle.getPosition().y - (sinf(angle) * speed * dt.asSeconds() / 2)));
			boingSound.play();
		}

		//test against paddle
		float centerx = circle.getPosition().x + circle.getRadius();
		float centery = circle.getPosition().y + circle.getRadius();
		float testx = centerx;
		float testy = centery;
		float distx2 = 0;
		float disty2 = 0;

		if (centerx < leftP.x()) {
			testx = leftP.x();
		}
		else if (centerx > leftP.x() + leftP.width()) {
			testx = leftP.x() + leftP.width();
		}
		if (centery < leftP.y()) {
			testy = leftP.y();
		}
		else if (centery > leftP.y() + leftP.height()) {
			testy = leftP.y() + leftP.height();
		}
		distx2 = (centerx - testx) * (centerx - testx);
		disty2 = (centery - testy) * (centery - testy);
		if ((distx2 + disty2) <= (circle.getRadius() * circle.getRadius())) {
			//bounce off paddle here
			angle = -angle;
			circle.setPosition(sf::Vector2f(circle.getPosition().x, leftP.y() - 10.05f));
			angle += magnifyAngle(leftP);
			whoaSound.play();
		}

		//test against bricks
		for (int i = 0; i < 32; ++i) {
			if (bricks.healths[i] > 0) {
				testx = centerx;
				testy = centery;
				distx2 = 0;
				disty2 = 0;

				if (centerx < bricks.positions[i].x) {
					//left side
					testx = bricks.positions[i].x;
				}
				else if (centerx > bricks.positions[i].x + bricks.width) {
					//right side
					testx = bricks.positions[i].x + bricks.width;
				}
				if (centery < bricks.positions[i].y) {
					//top
					testy = bricks.positions[i].y;
				}
				else if (centery > bricks.positions[i].y + bricks.height) {
					//bottom
					testy = bricks.positions[i].y + bricks.height;
				}
				distx2 = (centerx - testx) * (centerx - testx);
				disty2 = (centery - testy) * (centery - testy);
				if ((distx2 + disty2) <= (circle.getRadius() * circle.getRadius())) {
					//there is collision, bounce off
					collision = i;
					if (bricks.brickType[i] == 2) {
						//speed up ball from hitting rubber brick
						speed += 25.f;
					}
					if (centerx < bricks.positions[i].x) {
						//left side
						angle = pi - angle;
						circle.move(sf::Vector2f(0.f, -0.5f));
					}
					else if (centerx > bricks.positions[i].x + bricks.width) {
						//right side
						angle = pi - angle;
						circle.move(sf::Vector2f(0.f, 0.5f));
					}
					if (centery < bricks.positions[i].y) {
						//top
						angle = -angle;
						circle.move(sf::Vector2f(-0.5f, 0.f));
					}
					else if (centery > bricks.positions[i].y + bricks.height) {
						//bottom
						angle = -angle;
						circle.move(sf::Vector2f(0.5f, 0.f));
					}
				}
			}
		}

		return collision;
	}

	float x() {
		return circle.getPosition().x + circle.getRadius();
	}

	float y() {
		return circle.getPosition().y + circle.getRadius();
	}

	float magnifyAngle(paddle p) {
		float magnification;
		magnification = (circle.getPosition().x + circle.getRadius() - (p.x() + p.width() / 2)) * 2 / p.width();
		return magnification * 40 * pi / 360;
	}
};

int main() {
	//game init
	bool playing = false;
	bool ballShot = false;
	bool mouseMode = true;
	bool emptyBoard = false;
	float radius = 5.0f;
	int lives = 3;
	int level = 1;
	int collisionId = -1;

	sf::Text lifeCount;
	lifeCount.setFont(font);
	lifeCount.setCharacterSize(24);
	lifeCount.setFillColor(sf::Color::Red);
	lifeCount.setPosition(sf::Vector2f(20, 10));

	sf::Text scoreDisp;
	scoreDisp.setFont(font);
	scoreDisp.setCharacterSize(24);
	scoreDisp.setFillColor(sf::Color::Red);
	scoreDisp.setPosition(sf::Vector2f(500, 10));

	sf::Text title;
	title.setFont(font);
	title.setFillColor(sf::Color::Green);
	title.setCharacterSize(64);
	title.setPosition(sf::Vector2f(60, 50));
	title.setString("Wolfe's Big Box o' Breakout!");
	title.setRotation(15.f);

	sf::Text title2;
	title2.setFont(font);
	title2.setFillColor(sf::Color::Cyan);
	title2.setCharacterSize(64);
	title2.setPosition(sf::Vector2f(58, 48));
	title2.setString("Wolfe's Big Box o' Breakout!");
	title2.setRotation(15.f);

	sf::Text playAgain;
	playAgain.setFont(font);
	playAgain.setFillColor(sf::Color::Green);
	playAgain.setCharacterSize(36);
	playAgain.setPosition(sf::Vector2f(60, 300));
	playAgain.setString("Press the spacebar to play again or esc to quit.");
	playAgain.setRotation(-30.f);

	sf::Text gameMode;
	gameMode.setFont(font);
	gameMode.setFillColor(sf::Color::Green);
	gameMode.setCharacterSize(36);
	gameMode.setPosition(sf::Vector2f(60, 300));
	gameMode.setString("Press Space for arrow key controls. \nRight click for mouse controls.");

	sf::Text credits;
	credits.setFont(font);
	credits.setFillColor(sf::Color::Green);
	credits.setCharacterSize(18);
	credits.setPosition(sf::Vector2f(180, 200));
	credits.setString("by Mitchell F. Wolfe");

	if (!font.loadFromFile("steelfish.ttf"))
	{
		return -1;
	}

	if (!boingBuffer.loadFromFile("boingPong.wav")) {
		return -1;
	}
	boingSound.setBuffer(boingBuffer);

	if (!cheersBuffer.loadFromFile("cheers.wav")) {
		return -1;
	}
	cheersSound.setBuffer(cheersBuffer);

	if (!ahhhBuffer.loadFromFile("ahhh.wav")) {
		return -1;
	}
	ahhhSound.setBuffer(ahhhBuffer);

	if (!whoaBuffer.loadFromFile("whoaPong.wav")) {
		return -1;
	}
	whoaSound.setBuffer(whoaBuffer);

	if (!boopBuffer.loadFromFile("boop.wav")) {
		return -1;
	}
	boopSound.setBuffer(boopBuffer);

	if (!dunnBuffer.loadFromFile("dunn.wav")) {
		return -1;
	}
	dunnSound.setBuffer(dunnBuffer);

	if (!pewBuffer.loadFromFile("pew.wav")) {
		return -1;
	}
	pewSound.setBuffer(pewBuffer);

	if (!screamBuffer.loadFromFile("scream.wav")) {
		return -1;
	}
	screamSound.setBuffer(screamBuffer);

	if (!brickTexture.loadFromFile("brick.jpg")) {
		return -1;
	}

	if (!greyTexture.loadFromFile("greybrick.jpg")) {
		return -1;
	}

	if (!yellowTexture.loadFromFile("yellowbrick.png")) {
		return -1;
	}

	if (!eyesTexture.loadFromFile("eyes.jpg")) {
		return -1;
	}

	sf::RenderWindow window(sf::VideoMode(600, 600), "Wolfe's Big Box o' Breakout");
	paddle paddle(275.f, 550.f);
	ball playBall(sf::Vector2f(paddle.x() + (paddle.width()/2.f), paddle.y() - 5.f), radius);
	bricks bricks;

	sf::Time dt;
	sf::Clock clock;
	//gameloop
	while (window.isOpen())
	{
		//record frame length from last loop
		dt = clock.getElapsedTime();
		clock.restart();

		//window close logic
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
			window.close();
		}

		//game update loop
		sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
		if (playing) {
			//control paddle
			if (mouseMode) {
				if (mousePosition.x > paddle.x() + paddle.width() / 2) {
					//go right
					paddle.move(false, dt);
				}
				if (mousePosition.x < paddle.x() + paddle.width() / 2) {
					//go left
					paddle.move(true, dt);
				}
			}
			else {
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
					paddle.move(false, dt);
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
					paddle.move(true, dt);
				}
			}

			//update ball
			if (ballShot) {
				//move ball independently
				if (playBall.y() < 650.f) {
					playBall.move(dt);
					collisionId = playBall.checkCollision(paddle, bricks, dt);
					if (!(collisionId == -1)) {
						bricks.doCollision(collisionId);
					}
				}
			}
			else {
				//move ball with paddle
				playBall.circle.setPosition(sf::Vector2f(paddle.x() + (paddle.width() / 2.f) - radius, paddle.y() - radius * 2 - 0.5f));
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
					//shoot ball
					ballShot = true;
					pewSound.play();
					if (mouseMode) {
						if (mousePosition.x > paddle.x() - 1.f + paddle.width() / 2) {
							playBall.angle += .6f;
						}
						if (mousePosition.x < paddle.x() + 1.f + paddle.width() / 2) {
							playBall.angle -= .6f;
						}
					}
					else {
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
							playBall.angle += .6f;
						}
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
							playBall.angle -= .6f;
						}
					}
				}
			}

			//return ball if goes below screen
			if (playBall.y() > 600) {
				ballShot = false;
				--lives;
				ahhhSound.play();
				playBall.angle = pi * 1.5f;
			}

			//set up next level
			for (int i = 0; i < 32; ++i) {
				if (bricks.healths[i] > 0) {
					emptyBoard = false;
					break;
				}
				else if (i == 31) {
					emptyBoard = true;
				}
			}
			if (emptyBoard) {
				level++;
				if (level > 3) {
					level = 1;
				}
				bricks.setUp(level);
				ballShot = false;
				playBall.angle = pi * 1.5f;
				cheersSound.play();
			}

			//reset game if all lives are lost
			if (lives == 0) {
				playing = false;
			}

		}
		else {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
				//set to key control
				mouseMode = false;
				playing = true;
				lives = 3;
				bricks.setUp(1);
				playBall.speed = 300.f;
				score = 0;
			}
			if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
				//set to multiplayer
				mouseMode = true;
				playing = true;
				lives = 3;
				bricks.setUp(1);
				playBall.speed = 300.f;
				score = 0;
			}
		}

		lifeCount.setString("x" + std::to_string(lives));
		scoreDisp.setString("Score: " + std::to_string(score));

		//game draw loop
		window.clear();
		window.draw(scoreDisp);
		if (playing) {
			window.draw(paddle.rectangle);
			window.draw(lifeCount);
			window.draw(playBall.circle);
			for (int i = 0; i < 32; ++i) {
				if (bricks.healths[i] > 0) {
					window.draw(bricks.brick[i]);
				}
							
			}
			if (lives == 0) {
				window.draw(playAgain);
			}
		}
		else {
			window.draw(gameMode);
			window.draw(credits);
			window.draw(title2);
			window.draw(title);
		}
		window.display();
	}
	
	//game end
	return 0;
}