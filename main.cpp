#include <iostream>
#include <utility>
#include <array>
#include <random>

enum SquareOwner { none, red, blue};

enum Error {null, noPermission, outOfBounds};

struct Square {
	SquareOwner owner = none;
	int level = 0;
};

typedef std::array<std::array<Square, 5>, 5> BoardArray;
typedef std::array<int, 2> position;

class Player {
public:
	void setColor(SquareOwner color_){
		color = color_;
	}
	virtual position Move(BoardArray board, Error error) { return { -1,-1 }; }
protected:
	SquareOwner color;
};

class Board {
public:
	void Display() {
		std::cout << std::endl << "  0 1 2 3 4" << std::endl;
		int i = 0;
		for (auto& row : board) {
			std::cout << i++ << " ";
			for (auto& square : row) {
				;
				std::cout << char(32 + 32 * square.owner + square.level) << " ";
			}
			std::cout << std::endl;
		}
	}
	Error Set(position pos, SquareOwner owner, bool force = false, int level = 1) {
		for (int i : {0, 1}) {
			if (pos[i] < 0 || pos[i] > 4) {
				return outOfBounds;
			}
		}

		Square* square = &board[pos[1]][pos[0]];

		if (((*square).owner != owner && !force)) {
			return noPermission;
		}

		(*square).level += level;
		(*square).owner = owner;
		if ((*square).level == 4) {
			(*square).level = 0;
			(*square).owner = none;
			for (int d : {-1, 1}) {
				Set({ pos[0] + d, pos[1] }, owner, true);
				Set({ pos[0], pos[1] + d }, owner, true);
			}
		}
		return null;
	}
	SquareOwner CheckWin() {
		SquareOwner winner = none;
		for (auto& row : board) {
			for (auto& square : row) {
				if (square.owner != none) {
					if (winner == none) {
						winner = square.owner;
					}
					if (square.owner != winner) {
						return none;
					}
					winner = square.owner;
				}
			}
		}
		return winner;
	}
	BoardArray GetBoard() {
		return board;
	}
private:
	BoardArray board;
};

class GameHandler {
public:
	GameHandler(Player* redPlayer_, Player* bluePlayer_) : redPlayer{ redPlayer_ }, bluePlayer{ bluePlayer_ } {
		bluePlayer->setColor(blue);
		redPlayer->setColor(red);
	}
	void TryMove(Player* player, SquareOwner color, bool first = false) {
		Error error = null;
		board.Display();
		while (error != null) {
			error = board.Set(player->Move(board.GetBoard(), error), color, first,1+first*2);
		}
	}
	void Play() {
		TryMove(redPlayer, red, true);
		TryMove(bluePlayer, blue, true);
		while (board.CheckWin() == none) {
			TryMove(redPlayer, red, false);
			if(board.CheckWin() != none) {
				break;
			}
			TryMove(bluePlayer, blue, false);
		}
		std::cout << "Player " << board.CheckWin() << " wins!";
	}
private:
	Player* redPlayer;
	Player* bluePlayer;
	Board board;
};

class Human : public Player {
	position Move(BoardArray board, Error error) {
		if (error != null) {
			std::cout << "That move is invalid. Enter a different move." << std::endl;
		}
		int x, y;
		std::cout << std::endl << "Which x position would you like to move to?" << std::endl;
		std::cin >> x;
		std::cout << "Which y position would you like to move to?" << std::endl;
		std::cin >> y;
		return { x,y };
	}
};

class BadBot : public Player {
	position Move(BoardArray board, Error error) {
		std::array<int, 2> pos = { 3,3 };
		int max = 0;
		for (int y = 0; y < 5; y++) {
			for (int x = 0; x < 5; x++) {
				Square square = board[y][x];
				if (square.owner == color) {
					if (square.level > max) {
						max = square.level;
						pos = { x,y };
					}
				}
			}
		}
		return pos;
	}
};

int main() {
	Human h1;
	BadBot h2;
	Player* p1 = &h1;
	Player* p2 = &h2;
	GameHandler Game(p1,p2);
	Game.Play();
	return 0;
}
