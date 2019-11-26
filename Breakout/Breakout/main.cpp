#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Main.hpp>
#include <iostream>
#include <string>

const float pi = 3.14159f;

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

	void checkCollision(paddle leftP, sf::Time dt) {
		//bounce off left, right, and upper walls
		if (circle.getPosition().y < 0) {
			//bounce off top
			angle = -angle;
			circle.setPosition(sf::Vector2f(circle.getPosition().x - (cosf(angle) * speed * dt.asSeconds() / 2), 0.1f));
//			boingSound.play();
		}
		if (circle.getPosition().x < 0) {
			//bounce off left wall
			angle = pi - angle;
			circle.setPosition(sf::Vector2f(0.1f, circle.getPosition().y - (sinf(angle) * speed * dt.asSeconds() / 2)));
//			boingSound.play();
		}
		else if (circle.getPosition().x + 10.f > 600) {
			//bounce off right wall
			angle = pi - angle;
			circle.setPosition(sf::Vector2f(599.9f - circle.getRadius() * 2, circle.getPosition().y - (sinf(angle) * speed * dt.asSeconds() / 2)));
//			boingSound.play();
		}

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
//			whoaSound.play();
		}
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

class brick {

};

int main() {
	//game init
	bool playing = true;
	bool ballShot = false;
	float radius = 5.0f;
	int lives = 3;
	sf::RenderWindow window(sf::VideoMode(600, 600), "Wolfe's Big Box o' Breakout");
	paddle paddle(275.f, 550.f);
	ball playBall(sf::Vector2f(paddle.x() + (paddle.width()/2.f), paddle.y() - 5.f), radius);

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
		if (playing) {
			//control paddle
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
				paddle.move(false, dt);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
				paddle.move(true, dt);
			}

			//update ball
			if (ballShot) {
				//move ball independently
				if (playBall.y() < 650.f) {
					playBall.move(dt);
					playBall.checkCollision(paddle, dt);
				}
			}
			else {
				//move ball with paddle
				playBall.circle.setPosition(sf::Vector2f(paddle.x() + (paddle.width() / 2.f) - radius, paddle.y() - radius * 2 - 0.5f));
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
					//shoot ball
					ballShot = true;
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
						playBall.angle += .6f;
					}
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
						playBall.angle -= .6f;
					}
				}
			}

			//return ball if goes below screen
			if (playBall.y() > 600) {
				ballShot = false;
				--lives;
				playBall.angle = pi * 1.5f;
			}

			if (lives == 0) {
				//REPLACE THIS WITH END GAME LOGIC
				window.close();
			}

		}

		//game draw loop
		window.clear();
		if (playing) {
			window.draw(paddle.rectangle);
//			window.draw(leftScoreText);
//			window.draw(rightScoreText);
			window.draw(playBall.circle);
//			if (leftScore >= 5 || rightScore >= 5) {
//				window.draw(playAgain);
//			}
//			else {
//				window.draw(midPaddle.rectangle);
//			}
		}
		else {
//			window.draw(gameMode);
//			window.draw(credits);
//			window.draw(title2);
//			window.draw(title);
		}
		window.display();
	}
	
	//game end
	return 0;
}

//sf::SoundBuffer boingBuffer;
//sf::Sound boingSound;
//sf::SoundBuffer cheersBuffer;
//sf::Sound cheersSound;
//sf::SoundBuffer ahhhBuffer;
//sf::Sound ahhhSound;
//sf::SoundBuffer whoaBuffer;
//sf::Sound whoaSound;
//sf::Font font;
//sf::Texture paddleTexture;
//sf::Music soundtrack;
//

//int main()
//{
//	int leftScore = 0;
//	int rightScore = 0;
//	sf::RenderWindow window(sf::VideoMode(600, 600), "Wolfe's Big Box o' Pong");
//
//	sf::Text leftScoreText;
//	leftScoreText.setFont(font);
//	leftScoreText.setCharacterSize(48);
//	leftScoreText.setPosition(sf::Vector2f(40, 40));
//	sf::Text rightScoreText;
//	rightScoreText.setFont(font);
//	rightScoreText.setFillColor(sf::Color::White);
//	rightScoreText.setCharacterSize(48);
//	rightScoreText.setPosition(sf::Vector2f(522, 40));
//
//	sf::Text playAgain;
//	playAgain.setFont(font);
//	playAgain.setFillColor(sf::Color::Green);
//	playAgain.setCharacterSize(36);
//	playAgain.setPosition(sf::Vector2f(60, 300));
//	playAgain.setString("Press the spacebar to play again or esc to quit.");
//	playAgain.setRotation(-30.f);
//
//	sf::Text gameMode;
//	gameMode.setFont(font);
//	gameMode.setFillColor(sf::Color::Green);
//	gameMode.setCharacterSize(36);
//	gameMode.setPosition(sf::Vector2f(60, 300));
//	gameMode.setString("Press 1 for single-player (arrow key controls). \nPress 2 for multi-player (arrow keys vs WASD).");
//
//	sf::Text credits;
//	credits.setFont(font);
//	credits.setFillColor(sf::Color::Green);
//	credits.setCharacterSize(18);
//	credits.setPosition(sf::Vector2f(180, 500));
//	credits.setString("Font: Steelfish Rounded (from public domain) \nMusic: 'Battle Against a Clueless Foe' by Shane Mesa (used with CC license) \nAll else: Mitchell F. Wolfe");
//
//	sf::Text title;
//	title.setFont(font);
//	title.setFillColor(sf::Color::Green);
//	title.setCharacterSize(90);
//	title.setPosition(sf::Vector2f(60, 50));
//	title.setString("Wolfe's Big Box o' Pong!");
//	title.setRotation(15.f);
//
//	sf::Text title2;
//	title2.setFont(font);
//	title2.setFillColor(sf::Color::Cyan);
//	title2.setCharacterSize(90);
//	title2.setPosition(sf::Vector2f(57, 47));
//	title2.setString("Wolfe's Big Box o' Pong!");
//	title2.setRotation(15.f);
//
//	if (!boingBuffer.loadFromFile("boingPong.wav")) {
//		return -1;
//	}
//	boingSound.setBuffer(boingBuffer);
//
//	if (!cheersBuffer.loadFromFile("cheers.wav")) {
//		return -1;
//	}
//	cheersSound.setBuffer(cheersBuffer);
//
//	if (!ahhhBuffer.loadFromFile("ahhh.wav")) {
//		return -1;
//	}
//	ahhhSound.setBuffer(ahhhBuffer);
//
//	if (!whoaBuffer.loadFromFile("whoaPong.wav")) {
//		return -1;
//	}
//	whoaSound.setBuffer(whoaBuffer);
//
//	if (!font.loadFromFile("steelfish.ttf"))
//	{
//		return -1;
//	}
//	if (!paddleTexture.loadFromFile("paddleTexture.jpg")) {
//		std::cout << "oops";
//		return -1;
//	}
//	if (!soundtrack.openFromFile("Battle_Against_a_Clueless_Foe.wav")) {
//		return -1;
//	}
//	soundtrack.setLoop(true);
//	soundtrack.play();
//
//	paddle leftPaddle(10.f, 295.f);
//	paddle rightPaddle(580.f, 295.f);
//	rightPaddle.rectangle.setTexture(&paddleTexture, true);
//	paddle midPaddle(295.f, 0.f);
//	midPaddle.rectangle.setFillColor(sf::Color::Magenta);
//
//	ball playBall(sf::Vector2f(295, 295));
//
//	bool addedScore = false;
//	bool playing = false;
//	bool AI = false;
//	bool midUp = false;
//
//	sf::Time dt;
//	sf::Clock clock;
//	while (window.isOpen())
//	{
//		dt = clock.getElapsedTime();
//		clock.restart();
//		sf::Event event;
//		while (window.pollEvent(event))
//		{
//			if (event.type == sf::Event::Closed) {
//				window.close();
//			}
//		}
//		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
//			window.close();
//		}
//
//		if (playing) {
//			if (AI) {
//				//do left paddle AI here
//				if (playBall.y() < leftPaddle.y() - 5.f) {
//					leftPaddle.move(true, dt);
//				}
//				else if (playBall.y() > (leftPaddle.y() + leftPaddle.height() + 5.f)) {
//					leftPaddle.move(false, dt);
//				}
//			}
//			else {
//				if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
//					leftPaddle.move(true, dt);
//				}
//				if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
//					leftPaddle.move(false, dt);
//				}
//			}
//			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
//				rightPaddle.move(true, dt);
//			}
//			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
//				rightPaddle.move(false, dt);
//			}
//
//			if (midUp) {
//				midPaddle.move(true, dt);
//			}
//			else {
//				midPaddle.move(false, dt);
//			}
//
//			if (midPaddle.y() <= 2.f) {
//				midUp = false;
//			}
//			else if (midPaddle.y() + midPaddle.height() >= 598.f) {
//				midUp = true;
//			}
//
//			if (playBall.x() < 650.f && playBall.x() > -50.f) {
//				playBall.move(dt);
//				playBall.checkCollision(leftPaddle, rightPaddle, midPaddle, dt);
//			}
//		}
//		else {
//			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1) || sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad1)) {
//				//set to single player
//				AI = true;
//				playing = true;
//				leftPaddle.rectangle.setFillColor(sf::Color::Blue);
//				leftScoreText.setFillColor(sf::Color::Blue);
//			}
//			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2) || sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad2)) {
//				//set to multiplayer
//				AI = false;
//				playing = true;
//				leftPaddle.rectangle.setTexture(&paddleTexture, true);
//				leftScoreText.setFillColor(sf::Color::White);
//			}
//		}
//
//		if (playBall.x() < -5.f || playBall.x() > 605.f) {
//			if (playBall.x() < -5.f && !addedScore) {
//				//add score to right
//				++rightScore;
//				cheersSound.play();
//				addedScore = true;
//			}
//			else if (playBall.x() > 605.f && !addedScore) {
//				//add score to left
//				++leftScore;
//				ahhhSound.play();
//				addedScore = true;
//			}
//			if (leftScore >= 5 || rightScore >= 5) {
//				window.draw(playAgain);
//				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
//					//reset whole game
//					leftScore = 0;
//					rightScore = 0;
//					playBall.returnBall();
//					leftPaddle.setPosition(sf::Vector2f(leftPaddle.x(), (300 - leftPaddle.height() / 2)));
//					rightPaddle.setPosition(sf::Vector2f(rightPaddle.x(), (300 - rightPaddle.height() / 2)));
//					midPaddle.setPosition(sf::Vector2f(295.f, 0.f));
//					addedScore = false;
//				}
//				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
//					window.close();
//				}
//			}
//			else {
//				playBall.returnBall();
//				addedScore = false;
//			}
//		}
//
//		leftScoreText.setString(std::to_string(leftScore));
//		rightScoreText.setString(std::to_string(rightScore));
//
//		window.clear();
//		if (playing) {
//			window.draw(leftPaddle.rectangle);
//			window.draw(rightPaddle.rectangle);
//			window.draw(leftScoreText);
//			window.draw(rightScoreText);
//			window.draw(playBall.circle);
//			if (leftScore >= 5 || rightScore >= 5) {
//				window.draw(playAgain);
//			}
//			else {
//				window.draw(midPaddle.rectangle);
//			}
//		}
//		else {
//			window.draw(gameMode);
//			window.draw(credits);
//			window.draw(title2);
//			window.draw(title);
//		}
//		window.display();
//	}
//
//	return 0;
//}