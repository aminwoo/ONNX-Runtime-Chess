#include "encoder.h"

#include <algorithm>


InputPlanes EncodePositionForNN(std::vector<Position>& history) {
    InputPlanes result(kAuxPlaneBase + 8);

    const Position& position = history.back();
    const bool we_are_black = position.turn();

    // "Legacy" input planes with:
    // - Plane 104 (0-based) filled with 1 if we can castle queenside.
    // - Plane 105 filled with ones if we can castle kingside.
    // - Plane 106 filled with ones if they can castle queenside.
    // - Plane 107 filled with ones if they can castle kingside.
    if (we_are_black) {
        if ((position.castling() & BLACK_OOO_MASK) == 0) result[kAuxPlaneBase + 0].SetAll();
        if ((position.castling() & BLACK_OO_MASK) == 0) result[kAuxPlaneBase + 1].SetAll();
        if ((position.castling() & WHITE_OOO_MASK) == 0) result[kAuxPlaneBase + 2].SetAll();
        if ((position.castling() & WHITE_OO_MASK) == 0) result[kAuxPlaneBase + 3].SetAll();
    }
    else {
        if ((position.castling() & WHITE_OOO_MASK) == 0) result[kAuxPlaneBase + 0].SetAll();
        if ((position.castling() & WHITE_OO_MASK) == 0) result[kAuxPlaneBase + 1].SetAll();
        if ((position.castling() & BLACK_OOO_MASK) == 0) result[kAuxPlaneBase + 2].SetAll();
        if ((position.castling() & BLACK_OO_MASK) == 0) result[kAuxPlaneBase + 3].SetAll();
    }

    if (we_are_black) result[kAuxPlaneBase + 4].SetAll();

    // Plane kAuxPlaneBase + 7 is all ones to help NN find board edges.
    result[kAuxPlaneBase + 7].SetAll();

        
    int history_idx = history.size() - 1;
    for (int i = 0; i < kMoveHistory; ++i, --history_idx) {
        const Position& position = history[std::max(0, history_idx)];

        Color us = position.turn(); 
        Color them = ~us; 

        const int base = i * kPlanesPerBoard;
        result[base + 0].mask = position.all_pieces(us) & position.bitboard_of(us, PAWN);
        result[base + 1].mask = position.all_pieces(us) & position.bitboard_of(us, KNIGHT);
        result[base + 2].mask = position.all_pieces(us) & position.bitboard_of(us, BISHOP);
        result[base + 3].mask = position.all_pieces(us) & position.bitboard_of(us, ROOK);
        result[base + 4].mask = position.all_pieces(us) & position.bitboard_of(us, QUEEN);
        result[base + 5].mask = position.all_pieces(us) & position.bitboard_of(us, KING);

        result[base + 6].mask = position.all_pieces(them) & position.bitboard_of(them, PAWN);
        result[base + 7].mask = position.all_pieces(them) & position.bitboard_of(them, KNIGHT);
        result[base + 8].mask = position.all_pieces(them) & position.bitboard_of(them, BISHOP);
        result[base + 9].mask = position.all_pieces(them) & position.bitboard_of(them, ROOK);
        result[base + 10].mask = position.all_pieces(them) & position.bitboard_of(them, QUEEN);
        result[base + 11].mask = position.all_pieces(them) & position.bitboard_of(them, KING);

        for (int j = 0; j < history_idx; j++) {
            if (history[j].get_hash() == position.get_hash()) {
                result[base + 12].SetAll();
                break; 
            }
        }
    }

    if (we_are_black) {
        for (int i = 0; i < kAuxPlaneBase; i++) {
            auto v = result[i].mask;
            if (v == 0 || v == ~0ULL) continue;
            result[i].mask = mirror(v);
        }
    }
    return result;
}

