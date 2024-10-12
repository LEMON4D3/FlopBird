#include <raylib.h>
#include <vector>
#include <chrono>
#include <random>
#include <stdio.h>

#define screenW 1080
#define screenH 720

typedef enum  userLoc {
	menu,
	start
};

static int user = menu;

int getRandomVal(int min, int max) {
	std::random_device rd;
	std::mt19937::result_type seed = rd() ^ (
		(std::mt19937::result_type)
		std::chrono::duration_cast<std::chrono::seconds>(
			std::chrono::system_clock::now().time_since_epoch()
		).count() +
		(std::mt19937::result_type)
		std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
		).count());

	std::mt19937 gen(seed);
	std::uniform_int_distribution<unsigned> distrib(min, max);

	return distrib(gen);
}

static struct player {
	Vector2 location = { 50, screenH / 2 };
	
	float width = 50;
	float height = 50;

	Rectangle playerRec = { location.x, location.y, width, height };

	void gravity() {
		playerRec.y += 10;
	}

	void gameOver() {
		user = menu;
	}

	void checker() {
		if (playerRec.y - (height / 2) <= 0)
			playerRec.y = 0;
	}

};

static player* Player = new player();

static struct structure {
	float width = Player->width;
	float height = 0;
	
	float nextBatch = 0;

	Rectangle squareTop = {};
	Rectangle squareDown = {};

	void create() {
		SetRandomSeed((unsigned int)time(NULL));
		height = getRandomVal(0, 420);

		nextBatch = getRandomVal(100, 800);
		printf("%f", nextBatch);

		squareTop = { screenW + 50, 0, width, height };
		squareDown = { screenW + 50, height + 150, width, screenH - height };
	}

	void draw() {
		DrawRectangleRec(squareTop, GRAY);
		DrawRectangleRec(squareDown, GRAY);
	}

	void move() {
		squareTop.x -= 10;
		squareDown.x -= 10;
	}

	void deleteRec() {
		delete this;
	}

	void gameOver() {
		if (CheckCollisionRecs(Player->playerRec, squareTop) || CheckCollisionRecs(Player->playerRec, squareDown)) {
			Player->gameOver();
		} else if(Player->playerRec.y + (Player->height / 2) > 720)
			Player->gameOver();
	}
};

static class gameStart {

private:
	
	int playerScore = 0;

	static void updateRect(std::vector<structure*> rect) {

		for (structure* i : rect) {
			i->move();
			i->gameOver();
			i->draw();
		}

	}

public:

	std::vector<structure*> rect = {};

	void engine() {

		
		if (Player->playerRec.x + (Player->width / 2) > rect[playerScore]->squareTop.x + (rect[playerScore]->width / 2))
			playerScore++;

		if (rect[rect.size() - 1]->nextBatch >= rect[rect.size() - 1]->squareTop.x) {
			rect.push_back(new structure());
			rect[rect.size() - 1]->create();
		}

		Player->gravity();

		if (IsKeyDown(KEY_SPACE))
			Player->playerRec.y -= 20;
	}

	void draw() {
		BeginDrawing();
		ClearBackground(RAYWHITE);
		
		DrawText(TextFormat("%d", playerScore), (screenW / 2), 0, 100, BLACK);
		DrawRectangleRec(Player->playerRec, GREEN);
		updateRect(rect);

		EndDrawing();
	}
};

static gameStart* gamestart = new gameStart();

class gameMenu {

public:

	gameMenu() {
		if (IsKeyPressed(KEY_SPACE)) {
			gamestart = new gameStart();
			Player = new player();
			gamestart->rect.push_back(new structure());
			gamestart->rect[0]->create();
			user = start;

		}

		BeginDrawing();
		ClearBackground(RAYWHITE);

		DrawText("Press Space to begin", screenW / 2, screenH / 2, 20, BLACK);

		EndDrawing();


	}
};
int main() {

	InitWindow(screenW, screenH, "Flop Bird");
	system("cls");

	SetTargetFPS(30);

	while (!WindowShouldClose()) {

		switch (user) {
			case menu:
				gameMenu();
				break;
			case start: {
				gamestart->engine();
				gamestart->draw();
				break;
				}
			default:
				gameMenu();
				break;
		}
	}

	CloseWindow();
}