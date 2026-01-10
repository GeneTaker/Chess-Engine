#include "../include/board.h"
#include <vector>
#include <algorithm>

#define UP 1
#define DOWN -1
#define DOUBLE 2
#define NONE -1
#define ENPASSANT_WHITE 5
#define ENPASSANT_BLACK 2

uint64_t Board::rook_attacks[TILES][4096];

Board::Board() {
    bitboards[Board::PAWN] = 0x000000000000FF00ULL;
    bitboards[Board::BLACK_PAWN] = 0x00FF000000000000ULL;

    bitboards[Board::KNIGHT] = 0x0000000000000042ULL;
    bitboards[Board::BLACK_KNIGHT] = 0x4200000000000000ULL;

    bitboards[Board::ROOK] = 0x0000000000000081ULL;
    bitboards[Board::BLACK_ROOK] = 0x8100000000000000ULL;

    bitboards[Board::BISHOP] = 0x0000000000000024ULL;
    bitboards[Board::BLACK_BISHOP] = 0x2400000000000000ULL;

    bitboards[Board::QUEEN] = 0x0000000000000008ULL;
    bitboards[Board::BLACK_QUEEN] = 0x0800000000000000ULL;

    bitboards[Board::KING] = 0x0000000000000010ULL;
    bitboards[Board::BLACK_KING] = 0x1000000000000000ULL;

    for (int i = 0; i < Board::PIECES; i++) {
        white_bitboard |= bitboards[i];
        black_bitboard |= bitboards[i + Board::BLACK_SHIFT];
    }
    for (int i = 0; i < Board::TILES; i++) {
        init_knight_moves(i);
        rook_masks[i] = find_rook_mask(i);
        
    }
    init_rook_moves();
}

void Board::init_knight_moves(int index) {
    vector<int> offsets = {6, 10, 15, 17, -6, -10, -15, -17};

    for (int o : offsets) {
        int to_pos = index + o;
        
        if (to_pos >= Board::TILES || to_pos < 0) continue;

        int to_file = to_pos % Board::SIDE;
        int from_file = index % Board::SIDE;

        if (abs(to_file - from_file) > DOUBLE) continue;
        
        knight_attacks[index] |= (1ULL << to_pos);
    }
}

void Board::init_rook_moves() {
    for (int sq = 0; sq < Board::TILES; sq++) {
        uint64_t mask = rook_masks[sq];
        int bit_count = num_bits(mask);

        for (int i = 0; i < (1ULL << bit_count); i++) {
            uint64_t blockers = set_occupancy(mask, bit_count, i);

            uint64_t attacks = find_rook_attacks(sq, blockers);

            int index = (blockers * rook_magic[sq]) >> (Board::TILES - rook_shift[sq]);
            
            Board::rook_attacks[sq][index] = attacks;           
        }
    }
}

int Board::num_bits(uint64_t mask) {
    int count = 0;
    while (mask) {
        mask &= (mask - 1);
        count++;
    }
    return count;
}

uint64_t Board::set_occupancy(uint64_t mask, int bits, int index) {
    uint64_t result = 0ULL;

    for (int i = 0; i < bits; i++) {
        int square = pop_bit(&mask);
        if (index & (1ULL << i)) {
            result |= (1ULL << square);
        }
    }

    return result;
}

uint64_t Board::find_rook_attacks(int sq, uint64_t blockers) {
    uint64_t result = 0;
    int rank = sq / Board::SIDE;
    int file = sq % Board::SIDE;

    // Scan north
    for (int i = rank + 1; i < Board::SIDE; i++) {
        int s = i * Board::SIDE + file;
        result |= (1ULL << s);
        if (blockers & (1ULL << s)) break;
    }

    // Scan east
    for (int i = file + 1; i < Board::SIDE; i++) {
        int s = i + rank * Board::SIDE;
        result |= (1ULL << s);
        if (blockers & (1ULL << s)) break;
    }
    
    // Scan south
    for (int i = rank - 1; i >= 0; i--) {
        int s = i * Board::SIDE + file;
        result |= (1ULL << s);
        if (blockers & (1ULL << s)) break;
    }
    
    // Scan west
    for (int i = file - 1; i >= 0; i--) {
        int s = i + rank * Board::SIDE;
        result |= (1ULL << s);
        if (blockers & (1ULL << s)) break;
    }

    return result;
}

int Board::pop_bit(uint64_t *mask) {
    int index = 0;
    uint64_t b = *mask;

    while ((b & 1) == 0) {
        b >>= 1;
        index++;
    }

    *mask &= (*mask - 1);

    return index;
}

uint64_t Board::find_rook_mask(int index) {
    int file = index % Board::SIDE;
    int rank = index / Board::SIDE;

    uint64_t result = 0ULL;

    // Horizontal file
    for (int i = 1; i < Board::SIDE - 1; i++) {
        if (i != file) {
            result |= (1ULL << (rank * Board::SIDE + i));
        }
    }   

    // Vertical rank
    for (int i = 1; i < Board::SIDE - 1; i++) {
        if (i != rank) {
            result |= (1ULL << (file + Board::SIDE * i));
        }
    }

    return result;
}


bool Board::move(Move move, bool is_white) {
    if (!is_legal_move(move, is_white)) {
        return false;
    }
    
    uint64_t from_mask = 1ULL << move.from;
    uint64_t to_mask = 1ULL << move.to;
    
    uint64_t* own;
    uint64_t* opp;
    
    // check if we can promote
    if (is_white) {
        own = &white_bitboard;
        opp = &black_bitboard;
    } else {
        own = &black_bitboard;
        opp = &white_bitboard;
    }

    int own_offset = (is_white) ? 0 : Board::BLACK_SHIFT;
    int opp_offset = (is_white) ? Board::BLACK_SHIFT : 0;

    bool can_enpassant = false;
    int captured_pawn = -1;

    if (move.type == Board::PAWN) {
        int to_file = move.to % Board::SIDE;
        int to_rank = move.to / Board::SIDE;

        int check_offset = is_white ? 0 : Board::SIDE;
        uint16_t opp_mask = 1 << (to_file + check_offset);

        if (enpassant & opp_mask) {
            int from_file = move.from % Board::SIDE;
            
            if (abs(to_file - from_file) == 1) {
                int expected_rank = is_white ? ENPASSANT_WHITE : ENPASSANT_BLACK;
                
                if (to_rank == expected_rank) {
                    can_enpassant = true;
                    captured_pawn = (is_white) ? (move.to - Board::SIDE) : (move.to + Board::SIDE);
                }
            }
        }
    }

    enpassant = 0;
    
    // check for enpassant next turn
    if (move.type == Board::PAWN && abs(move.to - move.from) == DOUBLE * Board::SIDE) {
        int offset = (is_white) ? Board::SIDE : 0;
        enpassant = 1 << ((move.to % Board::SIDE) + offset); 
    }   

    if (to_mask & *opp && !can_enpassant) {
        *opp ^= to_mask;
        for (int i = 0; i < Board::PIECES; i++) {
            if (bitboards[i + opp_offset] & to_mask) {
                bitboards[i + opp_offset] ^= to_mask; 
                break;
            }
        }
    }

    if (can_enpassant) {
        uint64_t capture_mask = 1ULL << captured_pawn;
        *opp ^= capture_mask;
        bitboards[Board::PAWN + opp_offset] ^= capture_mask;
    }

    // Remove and add bits to simulate a move
    bitboards[move.type + own_offset] |= to_mask;
    bitboards[move.type + own_offset] ^= from_mask;
    
    // Remove and add from our occupancy bitboard
    *own |= to_mask;
    *own ^= from_mask;    
    
    if (move.promotion != NONE && can_promote(move, is_white)) {
        bitboards[move.type + own_offset] ^= to_mask;
        bitboards[move.promotion + own_offset] |= to_mask;
    }
    
    return true;
}

bool Board::is_legal_move(Move move, bool is_white) {
    int piece_index = bb_index(move.type, is_white);

    uint64_t move_mask = 1ULL << move.from;

    // check if there is a piece there first
    if (!(move_mask & bitboards[piece_index])) return false;

    uint64_t own = (is_white) ? white_bitboard : black_bitboard;
    uint64_t to_mask = 1ULL << move.to;

    if (own & to_mask) return false;
    
    // now check if it is a valid destination
    if (move.to >= TILES || move.to < 0) return false;

    // check if it is a valid move
    switch (move.type) {
        case Board::PAWN:
            return legal_pawn_move(move, is_white);
        
        case Board::ROOK:
            return legal_rook_move(move, is_white);
        
        case Board::KNIGHT:
            return legal_knight_move(move, is_white);
        
        case Board::BISHOP:
            return legal_bishop_move(move, is_white);
        
        case Board::QUEEN:
            return legal_queen_move(move, is_white);
        
        case Board::KING:
            return legal_king_move(move, is_white);
        default:
            break;
    }    
    return false;
}

bool Board::legal_pawn_move(Move move, bool is_white) {
    int direction = UP * Board::SIDE;
    if (!is_white) {
        direction *= DOWN;
    }
    
    uint64_t from_mask = 1ULL << move.from;
    uint64_t to_mask = 1ULL << move.to;
    uint64_t own = (is_white) ? white_bitboard : black_bitboard;
    uint64_t other = (is_white) ? black_bitboard : white_bitboard;

    // Single push
    if ((move.to == move.from + direction)) {
        if ((!(other & to_mask))) {
            return true;
        }
    }

    // Double push
    uint64_t rank = (is_white) ? 0x000000000000FF00 : 0x00FF000000000000;
    
    if ((rank & from_mask) && move.to == (move.from + DOUBLE * direction)) {
        uint64_t blocker = 1ULL << (move.from + direction);

        if (!((own | other) & (blocker | to_mask))) {
            return true;
        }
    }
    
    // Capture
    uint64_t capture_left = (is_white) ? (from_mask & ~Board::FILE_A) << (Board::SIDE - 1) : (from_mask & ~Board::FILE_H) >> (Board::SIDE + 1);
    uint64_t capture_right = (is_white) ? (from_mask & ~Board::FILE_H) << (Board::SIDE + 1) : (from_mask & ~Board::FILE_A) >> (Board::SIDE - 1);


    if ((capture_left | capture_right) & to_mask & other) {
        return true;
    }

    // En passant
    int to_file = move.to % Board::SIDE;
    int to_rank = move.to / Board::SIDE;

    int check_offset = (is_white) ? 0 : Board::SIDE;
    uint64_t opp_mask = 1 << (to_file + check_offset);

    if (enpassant & opp_mask) {
        int from_file = move.from % Board::SIDE;

        if (abs(to_file - from_file) == 1) {
            int expected_rank = (is_white) ? ENPASSANT_WHITE : ENPASSANT_BLACK;

            if (to_rank == expected_rank) {
                return true;
            }
        }
    }

    return false;
}

bool Board::legal_rook_move(Move move, bool is_white) {
    uint64_t occupancy = white_bitboard | black_bitboard;
    
    uint64_t blockers = occupancy & rook_masks[move.from];

    int index = (blockers * rook_magic[move.from]) >> (Board::TILES - rook_shift[move.from]);

    uint64_t attacks = Board::rook_attacks[move.from][index];

    return !((attacks & (1ULL << move.to)) == 0);
}

bool Board::legal_knight_move(Move move, bool is_white) {
    uint64_t legal_moves = knight_attacks[move.from];
    uint64_t to_mask = 1ULL << move.to;

    return !((legal_moves & to_mask) == 0);
}

bool Board::legal_queen_move(Move move, bool is_white) {
    return false;
}

bool Board::legal_bishop_move(Move move, bool is_white) {
    return false;
}

bool Board::legal_king_move(Move move, bool is_white) {
    return false;
}


bool Board::can_promote(Move move, bool is_white) {
    if (move.type != Board::PAWN && move.type != Board::BLACK_PAWN) return false;

    uint64_t end_rank = (is_white) ? 0xFF00000000000000ULL : 0x00000000000000FFULL;
    uint64_t to_mask = 1ULL << move.to;
    
    if (to_mask & end_rank) return true;
    return false;
}

void Board::print_bitboards() {
    for (auto& b : bitboards) {
        cout << b << endl;
    }
}

uint64_t Board::get_bitboard(int index) {
    return bitboards[index];
}

uint64_t Board::get_occupancy(bool is_white) {
    return (is_white) ? white_bitboard : black_bitboard;
}

constexpr int Board::bb_index(int type, bool is_white) {
    int offset = 0;
    if (!is_white) {
        offset = BLACK_SHIFT;
    }

    return offset + type;
}


