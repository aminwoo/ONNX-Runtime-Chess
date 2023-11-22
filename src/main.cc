#include "encoder.h"
#include "policy.h"
#include "position.h"
#include "network.h"
#include "tables.h"
#include "types.h"
#include "search.h"

#include <sstream>
#include <chrono>
#include <variant>
#include <iostream>

// This code needs refactoring 
void position(std::istringstream& is, History& history) {
	std::string token;
	is >> token;

	history.reset(); 
	if (token == "startpos") {
		is >> token; 
		while (is >> token) {
			// Templated functions leads to duplicated code 
			if (history.last().turn()) {
				MoveList<BLACK> list(history.last());
				for (Move m : list) {
					if (m.uci() == token) {
						history.push(m);
						break;
					}
				}
			}
			else {
				MoveList<WHITE> list(history.last());
				for (Move m : list) {
					if (m.uci() == token) {
						history.push(m);
						break;
					}
				}
			}
		}
	}
}

void go(std::istringstream& is, Search* search_thread, History& history) {
	run_search_thread(search_thread, history);
}

int main() {
	initialise_all_databases();
	zobrist::initialise_zobrist_keys();

	Search* search_thread = new Search;
	History history;

	std::string token, cmd;

	do {
		if (!getline(std::cin, cmd))
			cmd = "quit";

		std::istringstream is(cmd);
		std::cout << cmd << std::endl; 

		token.clear(); 
		is >> std::skipws >> token;

		if (token == "uci")             std::cout << "uciok" << std::endl;
		else if (token == "isready")	std::cout << "readyok" << std::endl;
		else if (token == "go")         go(is, search_thread, history);
		else if (token == "position")   position(is, history);

	} while (token != "quit"); 
}
