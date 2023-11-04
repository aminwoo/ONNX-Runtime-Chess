#include "encoder.h"
#include "policy.h"
#include "position.h"
#include "network.h"
#include "tables.h"
#include "types.h"

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

	}
}

void go(istringstream& is) {
	Network nn;
	InputPlanes planes = EncodePositionForNN(history);

	float* policy = nn.forward(planes);
	auto it = max_element(policy, policy + 1858);
	cout << "bestmove ";
	if (history.back().turn()) {
		cout << mirror_uci(policy_index[it - policy]) << '\n';
	}
	else {
		cout << policy_index[it - policy] << '\n';
	}
}


int main() {
	initialise_all_databases();
	zobrist::initialise_zobrist_keys();

	string token, cmd;

	do {
		if (!getline(cin, cmd))
			cmd = "quit";

		istringstream is(cmd);

		token.clear(); 
		is >> skipws >> token;

		if (token == "uci")             cout << "uciok" << endl;
		else if (token == "isready")	cout << "readyok" << endl;
		else if (token == "go")         go(is);
		else if (token == "position")   position(is);

	} while (token != "quit"); 
}

