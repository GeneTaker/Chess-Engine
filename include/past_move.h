#ifndef PAST_MOVE_H
#define PAST_MOVE_H

#include "move.h"

struct PastMove {
    int from;
    int to;

    int captured_piece;
    int piece_moved;
    int captured_square; 

    uint8_t castle_rights;
    // for 50 move rule
    int turn_count;

    int promotion_piece;
    uint16_t enpassant;
    
    PastMove(Move m, int pmi, int cp, int cs, uint8_t cr, int tc, int ep):
        from(m.from), to(m.to), captured_piece(cp), piece_moved(pmi), captured_square(cs),
        castle_rights(cr), turn_count(tc), promotion_piece(m.promotion), enpassant(ep) {}
};

#endif