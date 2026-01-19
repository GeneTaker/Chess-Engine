#include "../include/board.h"
#include <vector>
#include <algorithm>
#include <string>

#define UP 1
#define DOWN -1
#define DOUBLE 2
#define NONE -1
#define ENPASSANT_WHITE 5
#define ENPASSANT_BLACK 2
#define MAX_TURN_RULE 99

uint64_t Board::rook_attacks[TILES][4096];
uint64_t Board::bishop_attacks[TILES][512];

Board::Board() {
    //Initialises bitboards with initial positions
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

    // Fills the occupancy bitboards
    for (int i = 0; i < Board::PIECES; i++) {
        white_bitboard |= bitboards[i];
        black_bitboard |= bitboards[i + Board::BLACK_SHIFT];
    }

    // Initialises knight moves, rook masks, and bishop masks
    for (int i = 0; i < Board::TILES; i++) {
        init_knight_moves(i);
        rook_masks[i] = find_rook_mask(i);
        bishop_masks[i] = find_bishop_mask(i);
        
    }
    init_rook_moves();
    init_bishop_moves();
    init_pawn_attacks();
    init_king_moves();
}

void Board::init_knight_moves(int index) {
    // All conceivable knight attack offsets
    vector<int> offsets = {6, 10, 15, 17, -6, -10, -15, -17};

    for (int o : offsets) {
        int to_pos = index + o;
        
        // Ensures it's within vertical bounds
        if (to_pos >= Board::TILES || to_pos < 0) continue;

        int to_file = to_pos % Board::SIDE;
        int from_file = index % Board::SIDE;

        // Ensures within the sides of the board and that pieces do not wrap around it
        if (abs(to_file - from_file) > DOUBLE) continue;
        
        knight_attacks[index] |= (1ULL << to_pos);
    }
}

void Board::init_rook_moves() {
    for (int sq = 0; sq < Board::TILES; sq++) {
        uint64_t mask = rook_masks[sq];
        int bit_count = num_bits(mask);

        // There are 2 to the power of bit_count blocker configurations
        for (int i = 0; i < (1 << bit_count); i++) {
            // Derives the index in rook_attacks and computes
            uint64_t blockers = set_occupancy(mask, bit_count, i);
            uint64_t attacks = find_rook_attacks(sq, blockers);

            int index = (blockers * rook_magic[sq]) >> (Board::TILES - rook_shift[sq]);
            
            Board::rook_attacks[sq][index] = attacks;           
        }
    }
}

void Board::init_pawn_attacks() {
    // Possible attack offsets for each colour
    vector<int> white_offsets = {7, 9};
    vector<int> black_offsets = {-7, -9};

    for (int i = 0; i < Board::TILES; i++) {
        int white = 0;
        int black = 1;
        
        pawn_helper(white_offsets, white, i);
        pawn_helper(black_offsets, black, i);
    }
}

void Board::pawn_helper(vector<int> offsets, int colour, int index) {
    int from_file = index % Board::SIDE;

    for (int a : offsets) {
        int to_pos = a + index;

        // Within vertical bounds
        if (to_pos < 0 || to_pos >= Board::TILES) continue;

        int file = to_pos % Board::SIDE;

        // Within the sides of the board
        if (abs(file - from_file) == 1) {
            pawn_attacks[colour][index] |= (1ULL << to_pos); 
        }
    }
}

int Board::num_bits(uint64_t mask) {
    int count = 0;
    while (mask) {
        // Removes the lowest 1 bit from the mask
        mask &= (mask - 1);

        // Updates count for every 1 bit
        count++;
    }
    return count;
}

uint64_t Board::set_occupancy(uint64_t mask, int bits, int index) {
    uint64_t result = 0ULL;
    
    for (int i = 0; i < bits; i++) {
        // For every 1 bit, translate it onto a bitboard
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

        //If found a blocker, stop, the same applies for the other directions below
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
    int index = __builtin_ctzll(*mask);

    // Removes the lowest bit
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

void Board::init_bishop_moves() {
    for (int sq = 0; sq < Board::TILES; sq++) {
        uint64_t mask = bishop_masks[sq];
        int bit_count = num_bits(mask);

        for (int i = 0; i < (1 << bit_count); i++) {
            // Maps a blocker configuration onto an attack pattern for a square
            uint64_t blockers = set_occupancy(mask, bit_count, i);
            uint64_t attacks = find_bishop_attacks(sq, blockers);

            uint64_t index = (blockers * bishop_magic[sq]) >> (Board::TILES - bishop_shift[sq]);

            Board::bishop_attacks[sq][index] = attacks;
        }
    }
}

uint64_t Board::find_bishop_attacks(int sq, uint64_t blockers) {
    uint64_t result = 0;

    int file = sq % Board::SIDE;
    int rank = sq / Board::SIDE;

    //NE
    for (int x = file + 1, y = rank + 1; x < Board::SIDE && y < Board::SIDE; x++, y++) {
        uint64_t mask = 1ULL << (x + Board::SIDE * y);
        result |= mask;

        // Stops at the first blocker, the same goes for the other directions below
        if (blockers & mask) break;
    }

    //NW
    for (int x = file - 1, y = rank + 1; x >= 0 && y < Board::SIDE; x--, y++) {
        uint64_t mask = 1ULL << (x + Board::SIDE * y);
        result |= mask;
        if (blockers & mask) break;
    }

    //SW
    for (int x = file - 1, y = rank - 1; x >= 0 && y >= 0; x--, y--) {
        uint64_t mask = 1ULL << (x + Board::SIDE * y);
        result |= mask;
        if (blockers & mask) break;
    }
    
    //SE    
    for (int x = file + 1, y = rank - 1; x < Board::SIDE && y >= 0; x++, y--) {
        uint64_t mask = 1ULL << (x + Board::SIDE * y);
        result |= mask;
        if (blockers & mask) break;
    }
    
    return result;
}

uint64_t Board::find_bishop_mask(int index) {
    uint64_t result = 0;
    
    int file = index % Board::SIDE;
    int rank = index / Board::SIDE;

    //NE
    for (int x = file + 1, y = rank + 1; x < Board::SIDE - 1 && y < Board::SIDE - 1; x++, y++) {
        result |= (1ULL << (x + Board::SIDE * y));
    }

    //NW
    for (int x = file - 1, y = rank + 1; x > 0 && y < Board::SIDE - 1; x--, y++) {
        result |= (1ULL << (x + Board::SIDE * y));
    }

    //SW
    for (int x = file - 1, y = rank - 1; x > 0 && y > 0; x--, y--) {
        result |= (1ULL << (x + Board::SIDE * y));
    }
    
    //SE    
    for (int x = file + 1, y = rank - 1; x < Board::SIDE - 1 && y > 0; x++, y--) {
        result |= (1ULL << (x + Board::SIDE * y));
    }
    
    return result;
}

void Board::init_king_moves() {
    vector<int> offsets = {-9, -8, -7, -1, 1, 7, 8, 9};
    
    for (int i = 0; i < Board::TILES; i++) {
        for (int o : offsets) {
            int to_pos = i + o;

            // Vertical bounds
            if (to_pos >= Board::TILES || to_pos < 0) continue;

            int to_file = to_pos % Board::SIDE;
            int from_file = i % Board::SIDE;

            // Horizontal bounds of the board
            if (abs(to_file - from_file) >= DOUBLE) continue;

            king_attacks[i] |= (1ULL << to_pos);
        }
    }
}

bool Board::move(Move move, bool is_white) {
    // Checks it is legal
    if (!is_legal_move(move, is_white)) {
        return false;
    }
    
    // Makes move
    make_move_unchecked(move, is_white);
    
    return true;
}

void Board::make_move_unchecked(Move move, bool is_white) {
    uint64_t from_mask = 1ULL << move.from;
    uint64_t to_mask = 1ULL << move.to;
    
    uint64_t* own;
    uint64_t* opp;

    int captured_piece = NONE;
    int captured_square = NONE;

    uint8_t old_castle_rights = castle_rights;
    uint16_t old_passant = enpassant;
    
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

    // For enpassant logic, check if we can enpassant
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

    // Reset enpassant state
    enpassant = 0;
    
    // check for enpassant next turn
    if (move.type == Board::PAWN && abs(move.to - move.from) == DOUBLE * Board::SIDE) {
        int offset = (is_white) ? Board::SIDE : 0;
        enpassant = 1 << ((move.to % Board::SIDE) + offset); 
    }   

    // For regular captures
    if (to_mask & *opp && !can_enpassant) {
        *opp ^= to_mask;
        for (int i = 0; i < Board::PIECES; i++) {
            if (bitboards[i + opp_offset] & to_mask) {
                bitboards[i + opp_offset] ^= to_mask; 

                captured_piece = i + opp_offset;
                captured_square = move.to;

                break;
            }
        }
    }

    // Castling
    if (move.type == Board::KING &&
        ((move.from == 4 && (move.to == 6 || move.to == 2)) ||
        (move.from == 60 && (move.to == 62 || move.to == 58)))) {
        
        int rook_from, rook_to;

        if (move.to > move.from) {
            rook_from = move.from + 3;
            rook_to   = move.from + 1;
        } else {
            rook_from = move.from - 4;
            rook_to   = move.from - 1;
        }

        uint64_t rook_from_mask = 1ULL << rook_from;
        uint64_t rook_to_mask = 1ULL << rook_to;

        // Removes rook from it's initial position and adds it to its new one
        bitboards[own_offset + Board::ROOK] ^= rook_from_mask;
        bitboards[own_offset + Board::ROOK] ^= rook_to_mask;
        
        // Update occupancy
        *own ^= rook_from_mask;
        *own ^= rook_to_mask;
    }

    //castling, remove castling rights upon moving the king
    if (move.type == Board::KING) {
        uint8_t remove = (is_white) ? ~(CASTLE_WS | CASTLE_WL) : ~(CASTLE_BS | CASTLE_BL);
        castle_rights &= remove;
    }

    // remove a castling right if a corner rook moves 
    if (move.type == Board::ROOK) {
        if (move.from == 0) castle_rights &= ~Board::CASTLE_WL;
        if (move.from == 7) castle_rights &= ~Board::CASTLE_WS;
        if (move.from == 56) castle_rights &= ~Board::CASTLE_BL;
        if (move.from == 63) castle_rights &= ~Board::CASTLE_BS;
    }

    // From earlier, if we were capable of enpassant, we execute the move now
    if (can_enpassant) {
        uint64_t capture_mask = 1ULL << captured_pawn;
        // Remove the enpassant'ed piece from the opposing occupancy
        *opp ^= capture_mask;
        captured_piece = Board::PAWN + opp_offset;
        captured_square = captured_pawn;

        // Remove the captured piece during enpassant
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

    // removes a castling right if a corner rook is captured 
    if (captured_piece % Board::PIECES == Board::ROOK) {
        if (captured_square == 0) castle_rights &= ~CASTLE_WL;
        if (captured_square == 7) castle_rights &= ~CASTLE_WS;
        if (captured_square == 56) castle_rights &= ~CASTLE_BL;
        if (captured_square == 63) castle_rights &= ~CASTLE_BS;
    }

    // Adds the current move to the move history vector
    PastMove current_move(move, own_offset + move.type, captured_piece, captured_square, old_castle_rights, old_passant);
    move_history.push_back(current_move);
}

bool Board::is_legal_move(Move move, bool is_white) {
    int piece_index = bb_index(move.type, is_white);

    uint64_t move_mask = 1ULL << move.from;

    // check if there is a piece there first
    if (!(move_mask & bitboards[piece_index])) return false;

    uint64_t own = (is_white) ? white_bitboard : black_bitboard;
    uint64_t to_mask = 1ULL << move.to;

    // Checks if the destination square is occupied by another of our own pieces
    if (own & to_mask) return false;
    
    // now check if it is a valid destination
    if (move.to >= TILES || move.to < 0) return false;

    // check if it is a valid move
    bool legal = false;

    // checks the move satisfies the movement constraints of the particular piece
    switch (move.type) {
        case Board::PAWN:
            legal = legal_pawn_move(move, is_white);
            break;        
        case Board::ROOK:
            legal = legal_rook_move(move);
            break;
        case Board::KNIGHT:
            legal = legal_knight_move(move);
            break;
        case Board::BISHOP:
            legal = legal_bishop_move(move);
            break;
        case Board::QUEEN:
            legal = legal_queen_move(move);
            break;
        case Board::KING:
            legal = legal_king_move(move, is_white);
            break;
        default:
            break;
    }    

    if (!legal) return false;

    // Ensure our move does not illegally put/leave our king in check
    return check_evade(move, is_white);
}

bool Board::check_evade(Move m, bool is_white) {
    // Store initial board state
    uint64_t saved_white = white_bitboard;
    uint64_t saved_black = black_bitboard;
    uint64_t saved_bitboards[Board::UNIQUE_PIECES];
    uint8_t saved_castle = castle_rights;
    uint16_t saved_enpassant = enpassant;

    for (int i = 0; i < Board::UNIQUE_PIECES; i++) {
        saved_bitboards[i] = bitboards[i];
    }

    // Make move
    make_move_unchecked(m, is_white);

    bool result = true;

    // Ensure move does not put us in check
    if (in_check(is_white)) {
        result = false;
    }

    // Restore board state
    white_bitboard = saved_white;
    black_bitboard = saved_black;
    castle_rights = saved_castle;
    enpassant = saved_enpassant;
    for (int i = 0; i < Board::UNIQUE_PIECES; i++) {
        bitboards[i] = saved_bitboards[i];
    }
    move_history.pop_back();
    return result;
}

bool Board::is_attacked(int square, bool is_white) {
    int opp_offset = (is_white) ? Board::BLACK_SHIFT : 0;
    
    // Opposing pieces
    uint64_t opp_pawns = bitboards[opp_offset + Board::PAWN];
    uint64_t opp_rooks = bitboards[opp_offset + Board::ROOK];
    uint64_t opp_bishops = bitboards[opp_offset + Board::BISHOP];
    uint64_t opp_knights = bitboards[opp_offset + Board::KNIGHT];
    uint64_t opp_queens = bitboards[opp_offset + Board::QUEEN];
    uint64_t opp_king = bitboards[opp_offset + Board::KING];

    int own = (is_white) ? 0 : 1;

    // check if the opposing piece can attack the square
    if (opp_pawns & pawn_attacks[own][square]) return true;
    if (opp_knights & knight_attacks[square]) return true;
    if (opp_king & king_attacks[square]) return true;

    // Bishops/queens
    uint64_t attack_bishops = get_bishop_attacks(square);
    if (attack_bishops & (opp_bishops | opp_queens)) return true;
    
    // Rooks/queens
    uint64_t attack_rooks = get_rook_attacks(square);
    if (attack_rooks & (opp_rooks | opp_queens)) return true;
    
    return false;
}

bool Board::legal_pawn_move(Move move, bool is_white) {
    // Direction that the pawn pushes
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

        // Ensure our move does not move through a piece, nor does it move onto a piece
        if (!((own | other) & (blocker | to_mask))) {
            return true;
        }
    }
    
    // Capture
    uint64_t capture_left = (is_white) ? (from_mask & ~Board::FILE_A) << (Board::SIDE - 1) : (from_mask & ~Board::FILE_A) >> (Board::SIDE + 1);
    uint64_t capture_right = (is_white) ? (from_mask & ~Board::FILE_H) << (Board::SIDE + 1) : (from_mask & ~Board::FILE_H) >> (Board::SIDE - 1);

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
            // check the enpassant is valid, that the pawn moves to the correct rank, given the file
            int expected_rank = (is_white) ? ENPASSANT_WHITE : ENPASSANT_BLACK;

            if (to_rank == expected_rank) {
                return true;
            }
        }
    }

    return false;
}

bool Board::legal_rook_move(Move move) {
    uint64_t attacks = get_rook_attacks(move.from);

    // Validate move by ensuring it is included in rook_attacks
    return !((attacks & (1ULL << move.to)) == 0);
}

uint64_t Board::get_rook_attacks(int square) {
    // Maps a blocker configuration to an index, and retrieves the attacks
    uint64_t occupancy = white_bitboard | black_bitboard;
    uint64_t blockers = occupancy & rook_masks[square];

    int index = (blockers * rook_magic[square]) >> (Board::TILES - rook_shift[square]);

    uint64_t attacks = Board::rook_attacks[square][index];

    return attacks;
}

bool Board::legal_knight_move(Move move) {
    uint64_t legal_moves = knight_attacks[move.from];
    uint64_t to_mask = 1ULL << move.to;

    // Validate the move against knight_attacks
    return !((legal_moves & to_mask) == 0);
}

bool Board::legal_queen_move(Move move) {
    // A queen can only move anywhere a rook/bishop could move to
    return legal_rook_move(move) || legal_bishop_move(move);
}

uint64_t Board::get_queen_attacks(int square) {
    // A queen can only move anywhere a rook/bishop could move to
    return get_rook_attacks(square) | get_bishop_attacks(square);
}

bool Board::legal_bishop_move(Move move) {
    uint64_t attacks = get_bishop_attacks(move.from);
    uint64_t to_mask = (1ULL << move.to);

    // Validate move against bishop_attacks
    return (attacks & to_mask) != 0;
}

uint64_t Board::get_bishop_attacks(int square) {
    // Translates a blocker configuration onto an attack pattern
    uint64_t occupied = white_bitboard | black_bitboard;
    uint64_t blockers = occupied & bishop_masks[square];

    int index = (blockers * bishop_magic[square]) >> (Board::TILES - bishop_shift[square]);

    return Board::bishop_attacks[square][index];
}

bool Board::legal_king_move(Move move, bool is_white) {
    uint64_t attacks = king_attacks[move.from];
    uint64_t to_mask = 1ULL << move.to;

    uint64_t occupied = white_bitboard | black_bitboard;
    
    // Cannot castle if you are in check
    if (!in_check(is_white)) {
        if (is_white) {

            if (Board::CASTLE_WS & castle_rights && move.to == 6) {
                uint64_t short_mask = (1ULL << 5) | (1ULL << 6);
                if (is_attacked(6, is_white) || is_attacked(5, is_white)) return false;

                return ((short_mask & occupied) == 0);
            }
            if (Board::CASTLE_WL & castle_rights && move.to == 2) {
                uint64_t long_mask = (1ULL << 1) | (1ULL << 2) | (1ULL << 3);
                if (is_attacked(3, is_white) || is_attacked(2, is_white)) return false;
                return (long_mask & occupied) == 0;
            }
        } else {
            if (Board::CASTLE_BS & castle_rights && move.to == 62) {
                uint64_t short_mask = (1ULL << 61) | (1ULL << 62);
                if (is_attacked(61, is_white) || is_attacked(62, is_white)) return false;
                return (short_mask & occupied) == 0;
            }
            if (Board::CASTLE_BL & castle_rights && move.to == 58) {
                uint64_t long_mask = (1ULL << 57) | (1ULL << 58) | (1ULL << 59);
                if (is_attacked(59, is_white) || is_attacked(58, is_white)) return false;
                return (long_mask & occupied) == 0;
            }
        }
    }

    // If not castling, then validate against king_attacks
    return ((to_mask & attacks) != 0);
}

bool Board::can_promote(Move move, bool is_white) {
    // Checks if the piece is a pawn
    if (move.type != Board::PAWN && move.type != Board::BLACK_PAWN) return false;

    // Check it reaches the final rank
    uint64_t end_rank = (is_white) ? 0xFF00000000000000ULL : 0x00000000000000FFULL;
    uint64_t to_mask = 1ULL << move.to;
    
    if (to_mask & end_rank) return true;
    return false;
}

void Board::print_board(bool is_white) {
    // Board edge
    cout << "\n  +---+---+---+---+---+---+---+---+" << endl;

    if (is_white) {
        for (int r = Board::SIDE; r >= 1; r--) {
            print_row(r);
        }
    } else {
        for (int r = 1; r <= Board::SIDE; r++) {
            print_row(r);
        }
    }

    // File indicators
    cout <<  "    a   b   c   d   e   f   g   h\n" << endl;
    
}

void Board::print_row(int r) {
    // Rank indicators
    cout << r << " ";
        
    for (int f = 0; f < Board::SIDE; f++) {
        int square = (r - 1) * Board::SIDE + f;
        
        cout << "| " << get_piece_at(square) << " ";           
    }

    cout << "|" << endl;
    cout << "  +---+---+---+---+---+---+---+---+" << endl;
}

std::string Board::get_piece_at(int square) {
    uint64_t square_mask = 1ULL << square;

    // Iterates through all bitboards to look for a piece there
    for (int i = 0; i < Board::UNIQUE_PIECES; i++) {
        if (square_mask & bitboards[i]) {            
            switch (i) {
                case Board::PAWN:
                    return "P";               
                case Board::KNIGHT:
                    return "N";                
                case Board::BISHOP:
                    return "B";                
                case Board::ROOK:
                    return "R";               
                case Board::QUEEN:
                    return "Q";
                case Board::KING:
                    return "K";                
                case Board::BLACK_PAWN:
                    return "p";                
                case Board::BLACK_KNIGHT:
                    return "n";                
                case Board::BLACK_BISHOP:
                    return "b";                
                case Board::BLACK_ROOK:
                    return "r";                
                case Board::BLACK_QUEEN:
                    return "q";                
                case Board::BLACK_KING:
                    return "k";
                default:
                    break;                
            }

        }
    }
    return ".";
}

uint64_t Board::get_bitboard(int index) {
    return bitboards[index];
}

uint64_t Board::get_occupancy(bool is_white) {
    return (is_white) ? white_bitboard : black_bitboard;
}

int Board::bb_index(int type, bool is_white) {
    int offset = 0;
    if (!is_white) {
        offset = BLACK_SHIFT;
    }

    return offset + type;
}

bool Board::in_check(bool is_white) {
    int own_offset = (is_white) ? 0 : BLACK_SHIFT;

    uint64_t own_king_board = bitboards[own_offset + Board::KING];
    if (own_king_board == 0) return false;
    int king_square = __builtin_ctzll(own_king_board);   

    // Checks if the king is attacked
    return is_attacked(king_square, is_white);
}

void Board::unmake_move() {
    // Pops from the move history for the last move
    PastMove last_move = move_history.back();
    move_history.pop_back();

    uint64_t to_mask = 1ULL << last_move.to;
    uint64_t from_mask = 1ULL << last_move.from;

    int colour = last_move.piece_moved / Board::PIECES;

    uint64_t* own_board;
    uint64_t* opp_board;

    if (colour == 0) {
        own_board = &white_bitboard;
        opp_board = &black_bitboard;
    } else {
        own_board = &black_bitboard;
        opp_board = &white_bitboard;
    }

    int own_offset = colour * Board::PIECES;

    if (last_move.promotion_piece != NONE) {
        // Reverts promotion
        bitboards[last_move.promotion_piece + own_offset] &= ~to_mask;
        bitboards[last_move.piece_moved] |= from_mask;
    } else {
        bitboards[last_move.piece_moved] &= ~to_mask;
        bitboards[last_move.piece_moved] |= from_mask;
    }

    // Updates occupancies
    *own_board &= ~to_mask;
    *own_board |= from_mask;

    // If the last move was a capture
    if (last_move.captured_square != NONE) {
        int restore = last_move.captured_piece;

        // Restore captured piece
        bitboards[restore] |= 1ULL << last_move.captured_square;
        *opp_board |= 1ULL << last_move.captured_square;
    }

    // Restore castling
    if ((last_move.piece_moved % Board::PIECES == Board::KING) &&
        ((last_move.from == 4 && (last_move.to == 6 || last_move.to == 2)) ||
        (last_move.from == 60 && (last_move.to == 62 || last_move.to == 58)))) {        
        int rook_from, rook_to;

        // Find where our rook belongs, depending on colour
        if (last_move.to > last_move.from) {
            rook_from = last_move.from + 3;
            rook_to   = last_move.from + 1;
        } else {
            rook_from = last_move.from - 4;
            rook_to   = last_move.from - 1;
        }

        uint64_t rook_from_mask = 1ULL << rook_from;
        uint64_t rook_to_mask = 1ULL << rook_to;

        // Places rook where it belongs
        bitboards[own_offset + Board::ROOK] |= rook_from_mask;
        bitboards[own_offset + Board::ROOK] &= ~rook_to_mask;

        // Updates occupancy for rook
        *own_board |= rook_from_mask;
        *own_board &= ~rook_to_mask;
    }

    // Reverts castling and enpassant rights
    castle_rights = last_move.castle_rights;
    enpassant = last_move.enpassant;
}

bool Board::is_checkmate(bool is_white) {
    // Can't be in checkmate without at least being in check
    if (!in_check(is_white)) {
        return false;
    }

    MoveGenerator generator;
    // generate moves to see if we can evade check
    vector<Move> all_moves = generator.generate_moves(*this, is_white);

    for (Move m : all_moves) {
        // If there is such a move, then we are not in checkmate
        if (is_legal_move(m, is_white)) return false;
    }

    return true;
}
bool Board::is_stalemate(bool is_white) {
    // Cannot be in stalemate if in check
    if (in_check(is_white)) {
        return false;
    }

    MoveGenerator generator;
    vector<Move> all_moves = generator.generate_moves(*this, is_white);
    
    for (Move m : all_moves) {
        // If we have a move, we are not in stalemate
        if (is_legal_move(m, is_white)) return false;
    }

    return true;
}

vector<Move> Board::find_legal_moves(bool is_white) {
    MoveGenerator generator;
    vector<Move> all_moves = generator.generate_moves(*this, is_white);
    vector<Move> filter_moves;

    // For every legal move, add it to our resultant list of legal moves
    for (Move m : all_moves) {
        if (is_legal_move(m, is_white)) filter_moves.push_back(m);
    }

    return filter_moves;
}

vector<Move> Board::find_legal_attacks(bool is_white) {
    vector<Move> legal_moves = find_legal_moves(is_white);
    vector<Move> result;

    uint64_t opp_board = (is_white) ? black_bitboard : white_bitboard;

    for (auto m : legal_moves) {
        uint64_t to_mask = 1ULL << m.to;
        
        // If it is a capture, add it to our resultant list of legal attacks
        if (opp_board & to_mask) {
            result.push_back(m);
        }
    }

    return result;
}

bool Board::is_three_fold() {
    int count = 0;

    int n = move_history.size();
    Board temp = *this;

    // Iterates through our past moves for similar board states
    for (int i = 0; i < n; i++) {
        temp.unmake_move();
        if (bitboards_equal(temp) && temp.enpassant == enpassant && temp.castle_rights == castle_rights) {
            if (++count >= 2) return true;
        }
    }
    return false;
}

bool Board::bitboards_equal(Board board) {
    for (int i = 0; i < Board::UNIQUE_PIECES; i++) {
        if (board.bitboards[i] != bitboards[i]) return false;
    }
    return true;
}

bool Board::insufficient_material() {
    uint64_t white_pawns   = bitboards[PAWN];
    uint64_t white_rooks   = bitboards[ROOK];
    uint64_t white_queens  = bitboards[QUEEN];
    uint64_t white_knights = bitboards[KNIGHT];
    uint64_t white_bishops = bitboards[BISHOP];

    uint64_t black_pawns   = bitboards[BLACK_PAWN];
    uint64_t black_rooks   = bitboards[BLACK_ROOK];
    uint64_t black_queens  = bitboards[BLACK_QUEEN];
    uint64_t black_knights = bitboards[BLACK_KNIGHT];
    uint64_t black_bishops = bitboards[BLACK_BISHOP];

    // If we have any of these pieces, we do not have insufficient material
    if (white_pawns || black_pawns || white_rooks || black_rooks || white_queens || black_queens)
        return false;

    // Finds number of pieces which could affect if we have insufficent material
    int white_knight_count = __builtin_popcountll(white_knights);
    int white_bishop_count = __builtin_popcountll(white_bishops);
    int black_knight_count = __builtin_popcountll(black_knights);
    int black_bishop_count = __builtin_popcountll(black_bishops);

    int total_minors = white_knight_count + white_bishop_count + black_knight_count + black_bishop_count;

    // No minor pieces means insufficient material
    if (total_minors == 0) return true;

    // Only one minor means insufficient material
    if (total_minors == 1) return true;

    // Only configurations where we could have insufficent material despite
    // having 2 minors 

    if (total_minors == DOUBLE &&
        white_bishop_count == 1 && black_bishop_count == 1 &&
        white_knight_count == 0 && black_knight_count == 0) {

        int w_square = __builtin_ctzll(white_bishops);
        int b_square = __builtin_ctzll(black_bishops);

        bool w_color = (w_square / Board::SIDE + w_square % Board::SIDE) % 2;
        bool b_color = (b_square / Board::SIDE + b_square % Board::SIDE) % 2;

        if (w_color == b_color) return true;
    }

    // Only 2 knights
    if (total_minors == DOUBLE &&
        white_knight_count == 1 && black_knight_count == 1 &&
        white_bishop_count == 0 && black_bishop_count == 0)
        return true;

    return false;
}

bool Board::is_over(bool is_white) {
    return is_draw(is_white) || is_checkmate(is_white);
}

bool Board::is_draw(bool is_white) {
    return insufficient_material() || is_three_fold() || is_stalemate(is_white);
}

uint64_t Board::king_adjacent(bool is_white) {
    int position = king_position(is_white);

    uint64_t result = king_attacks[position];
    return result;
}

int Board::king_position(bool is_white) {
    uint64_t king_board = bitboards[bb_index(Board::KING, is_white)];
    if (king_board == 0) return 0;

    int position = __builtin_ctzll(king_board);
    return position;
}

int Board::find_piece_moved(int from) {
    for (int i = 0; i < Board::UNIQUE_PIECES; i++) {
        uint64_t from_mask = 1ULL << from;

        if (bitboards[i] & from_mask) {
            return i;
        }
    }

    return NONE;
}