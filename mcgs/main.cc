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

using namespace std;


vector<Position> history;


void position(istringstream& is) {
	string token;
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

void go(istringstream& is) {
	Search* t = new Search;
	run_search_thread(t, history);
}


int main() {
	initialise_all_databases();
	zobrist::initialise_zobrist_keys();

	string token, cmd;

	do {
		if (!getline(cin, cmd))
			cmd = "quit";
		cout << cmd << '\n'; 

		istringstream is(cmd);

		token.clear(); 
		is >> skipws >> token;

		if (token == "uci")             cout << "uciok" << endl;
		else if (token == "isready")	cout << "readyok" << endl;
		else if (token == "go")         go(is);
		else if (token == "position")   position(is);

	} while (token != "quit"); 
}

