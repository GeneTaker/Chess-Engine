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
uint64_t Board::bishop_attacks[TILES][512];

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
        bishop_masks[i] = find_bishop_mask(i);
        
    }
    init_rook_moves();
    init_bishop_moves();
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

void Board::init_pawn_attacks() {
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

        if (to_pos < 0 || to_pos >= Board::TILES) continue;

        int file = to_pos % Board::SIDE;

        if (abs(file - from_file) < DOUBLE) {
            pawn_attacks[colour][index] |= (1ULL << to_pos); 
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
    int index = __builtin_ctzll(*mask);
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

        for (int i = 0; i < (1ULL << bit_count); i++) {
            uint64_t blockers = set_occupancy(mask, bit_count, i);
            
            uint64_t attacks = find_bishop_attacks(sq, blockers);

            uint64_t index = (blockers * bishop_magic[sq]) >> (Board::TILES - bishop_shift[sq]);

            bishop_attacks[sq][index] = attacks;
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

            if (to_pos >= Board::TILES || to_pos < 0) continue;

            int to_file = to_pos % Board::SIDE;
            int from_file = i % Board::SIDE;

            if (abs(to_file - from_file) >= DOUBLE) continue;

            king_attacks[index] |= (1ULL << to_pos);
        }
    }
}

bool Board::move(Move move, bool is_white) {
    if (!is_legal_move(move, is_white)) {
        return false;
    }
    
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

                captured_piece = i + opp_offset;
                captured_square = move.to;

                break;
            }
        }
    }

    if (move.type == Board::KING && abs(move.to - move.from) == DOUBLE) {
        int castled_rook = (move.to + move.from) / 2;
        uint64_t castled_rook_mask = 1ULL << castled_rook;

        bitboards[own_offset + Board::ROOK] |= castled_rook_mask;
        *own |= castled_rook_mask;
        
        int rook_from, rook_to;

        if (move.to > move.from) {
            rook_from = move.from + 3;
            rook_to   = move.from + 1;
        } else {
            rook_from = move.from - 4;
            rook_to   = move.from - 1;
        }

        bitboards[own_offset + Board::ROOK] ^= (1ULL << rook_from);
        *own ^= (1ULL << rook_from);
    }

    //castling
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

    if (can_enpassant) {
        uint64_t capture_mask = 1ULL << captured_pawn;
        *opp ^= capture_mask;
        captured_piece = Board::PAWN + opp_offset;
        captured_square = captured_pawn;
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
    
    if (captured_piece != NONE || move.type == Board::PAWN) {
        turns_since_capture = 0;
    } else {
        turns_since_capture++;
    }

    // removes a castling right if a corner rook is captured 
    if (captured_piece % Board::PIECES == Board::ROOK) {
        if (captured_square == 0) castle_rights &= ~CASTLE_WL;
        if (captured_square == 7) castle_rights &= ~CASTLE_WS;
        if (captured_square == 56) castle_rights &= ~CASTLE_BL;
        if (captured_square == 63) castle_rights &= ~CASTLE_BS;
    }

    PastMove current_move(move, captured_piece, captured_square, old_castle_rights, turns_since_capture, enpassant);
    move_history.push_back(current_move);
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
    bool legal = false;

    switch (move.type) {
        case Board::PAWN:
            legal = legal_pawn_move(move, is_white);
            break;        
        case Board::ROOK:
            legal = legal_rook_move(move, is_white);
            break;
        case Board::KNIGHT:
            legal = legal_knight_move(move, is_white);
            break;
        case Board::BISHOP:
            legal = legal_bishop_move(move, is_white);
            break;
        case Board::QUEEN:
            legal = legal_queen_move(move, is_white);
            break;
        case Board::KING:
            legal = legal_king_move(move, is_white);
            break;
        default:
            break;
    }    

    if (!legal) return false;

    return check_evade(move, is_white);
}

bool check_evade(Move m, bool is_white) {
    make_move_unchecked(m, is_white);

    bool result = true;

    if (in_check(is_white)) {
        result = false;
    }

    unmake_move();
    return result;
}

bool Board::is_attacked(int square, bool is_white) {
    int opp_offset = (is_white) ? Board::BLACK_SHIFT : 0;
       
    uint64_t opp_pawns = bitboards[opp_offset + Board::PAWN];
    uint64_t opp_rooks = bitboards[opp_offset + Board::ROOK];
    uint64_t opp_bishops = bitboards[opp_offset + Board::BISHOP];
    uint64_t opp_knights = bitboards[opp_offset + Board::KNIGHT];
    uint64_t opp_queens = bitboards[opp_offset + Board::QUEEN];
    uint64_t opp_king = bitboards[opp_offset + Board::KING];

    int opp = (is_white) ? 1 : 0;

    uint64_t square_mask = 1ULL << square;
    if (opp_pawns & pawn_attacks[opp][square]) return true;
    if (opp_knights & knight_attacks[square]) return true;
    if (opp_king & king_attacks[square]) return true;

    uint64_t occupied = white_bitboard | black_bitboard;
    // Bishops/queens
    uint64_t bishop_blockers = occupied & bishop_masks[square];
    int bishop_index = (bishop_blockers * bishop_magic[square]) >> (Board::TILES - bishop_shift[square]);
    if (bishop_attacks[square][bishop_index] & (opp_bishops | opp_queens)) return true;
    
    // Rooks/queens
    uint64_t rook_blockers = occupied & rook_masks[square];
    int rook_index = (rook_blockers * rook_magic[square]) >> (Board::TILES - rook_shift[square]);
    if (rook_attacks[square][rook_index] & (opp_rooks | opp_queens)) return true;
    
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
    return legal_rook_move(move, is_white) || legal_bishop_move(move, is_white);
}

bool Board::legal_bishop_move(Move move, bool is_white) {
    uint64_t occupied = white_bitboard | black_bitboard;
    
    uint64_t blockers = occupied & bishop_masks[move.from];

    int index = (blockers * bishop_magic[move.from]) >> (Board::TILES - bishop_shift[move.from]);

    uint64_t to_mask = (1ULL << move.to);
    return (bishop_attacks[move.from][index] & to_mask) != 0;
}

bool Board::legal_king_move(Move move, bool is_white) {
    uint64_t attacks = king_attacks[move.from];
    uint64_t to_mask = 1ULL << move.to;

    if (is_white) {
        if (Board::CASTLE_WS & castle_rights && move.to == 6) {
            return true;
        }
        if (Board::CASTLE_WL & castle_rights && move.to == 2) {
            return true;
        }
    } else {
        if (Board::CASTLE_BS & castle_rights && move.to == 62) {
            return true;
        }
        if (Board::CASTLE_BL & castle_rights && move.to == 58) {
            return true;
        }
    }

    return ((to_mask & attacks) != 0);
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

bool Board::in_check(bool is_white) {
    int own_offset = (is_white) ? 0 : BLACK_SHIFT;

    uint64_t own_king_board = bitboards[own_offset + Board::KING];
    int king_square = pop_bit(&own_king_board);   

    return is_attacked(king_square, is_white);
}

void Board::unmake_move() {
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

    bitboards[last_move.piece_moved] |= from_mask;
    *own_board |= from_mask;
    *own_board ^= to_mask;

    if (last_move.promotion_piece != NONE) {
        bitboards[last_move.promotion_piece] ^= to_mask;
    } else {
        bitboards[last_move.piece_moved] ^= to_mask;
    }

    if (last_move.captured_square != NONE) {
        int restore = last_move.captured_piece;
        bitboards[restore] |= 1ULL << last_move.captured_square;
        *opp_board |= 1ULL << last_move.captured_square;
    }

    turns_since_capture = last_move.turn_count;

    int own_offset = colour * Board::PIECES;

    if (last_move.piece_moved == Board::KING && abs(last_move.to - last_move.from) == DOUBLE) {
        int castled_rook = (last_move.to + last_move.from) / 2;
        uint64_t castled_rook_mask = 1ULL << castled_rook;

        bitboards[own_offset + Board::ROOK] ^= castled_rook_mask;
        *own_board ^= castled_rook_mask;
        
        int rook_from, rook_to;

        if (last_move.to > last_move.from) {
            rook_from = last_move.from + 3;
            rook_to   = last_move.from + 1;
        } else {
            rook_from = last_move.from - 4;
            rook_to   = last_move.from - 1;
        }

        bitboards[own_offset + Board::ROOK] |= (1ULL << rook_from);
        *own_board |= (1ULL << rook_from);
    }

    castle_rights = last_move.castle_rights;
    enpassant = last_move.enpassant;
}

int get_turns_since_capture() {
    return turns_since_capture;
}

// bool Board::is_checkmate(bool is_white) {
//     if (!in_check()) {
//         return false;
//     }

//     for ()
// }
// bool Board::is_stalemate(bool is_white) {
//     if (!in_check()) {
//         return false;
//     }

//     for ()
// }

