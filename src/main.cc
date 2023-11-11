#include "encoder.h"
#include "policy.h"
#include "position.h"
#include "network.h"
#include "tables.h"
#include "types.h"
#include "search.h"

#include <sstream>
#include <chrono>
#include <iostream>

void position(std::istringstream& is, std::vector<Position>& history) {
	std::string token;
	is >> token;

	Position p;
	history.clear(); 
	if (token == "startpos") {
		Position::set("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -", p);
		history.push_back(p);

		is >> token; 
		while (is >> token) {
			if (p.turn()) {
				MoveList<BLACK> list(p);
				for (Move m : list) {
					if (m.uci() == token) {
						p.play<BLACK>(m);
						break;
					}
				}
			}
			else {
				MoveList<WHITE> list(p);
				for (Move m : list) {
					if (m.uci() == token) {
						p.play<WHITE>(m);
						break;
					}
				}
			}

			history.push_back(p);
		}
	}
	else if (token == "fen") {
		std::string fen; 
		while (is >> token) {
			fen += token + " ";
		}
		Position::set(fen, p);
		history.push_back(p);
	}
}

void go(std::istringstream& is, Search* search_thread, std::vector<Position>& history) {
	run_search_thread(search_thread, history);
}

int main() {
	initialise_all_databases();
	zobrist::initialise_zobrist_keys();

	Search* search_thread = new Search;
	std::vector<Position> history;

	std::string token, cmd;

	do {
		if (!getline(std::cin, cmd))
			cmd = "quit";

		std::istringstream is(cmd);

		token.clear(); 
		is >> std::skipws >> token;

		if (token == "uci")             std::cout << "uciok" << std::endl;
		else if (token == "isready")	std::cout << "readyok" << std::endl;
		else if (token == "go")         go(is, search_thread, history);
		else if (token == "position")   position(is, history);

	} while (token != "quit"); 
}
