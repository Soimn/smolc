#if defined(_WIN32)

#if !defined(_WIN64) || _WIN64 != 1
#error "32bit is not supported"
#endif

typedef signed __int8  SC_i8;
typedef signed __int16 SC_i16;
typedef signed __int32 SC_i32;
typedef signed __int64 SC_i64;
typedef unsigned __int8  SC_u8;
typedef unsigned __int16 SC_u16;
typedef unsigned __int32 SC_u32;
typedef unsigned __int64 SC_u64;
typedef float SC_f32;
typedef double SC_f64;
#else
#error "Unsupported platform"
#endif

typedef SC_i64 SC_imm;
typedef SC_u64 SC_umm;

typedef SC_u8 SC_bool;
#define SC_false (SC_bool)(0)

typedef struct SC_String
{
    SC_u8* data;
    SC_u32 size;
} SC_String;
#define SC_STRING(S) (SC_String){ .data = (SC_u8*)(S), .size = sizeof(S) - 1 }

#define SC_ASSERT(EX) ((EX) ? 1 : *(volatile int*)0 = 0)
#define SC_NOT_IMPLEMENTED SC_ASSERT(!"SC_NOT_IMPLEMENTED")
#define SC_INVALID_DEFAULT_CASE SC_ASSERT(!"SC_INVALID_DEFAULT_CASE")

/*
text -> tokens -> preprocessed tokens -> AST
..............
- multibyte to source chars
- trigraph resolution
- backspaced newline
- universal character names
 > remember line and column info
*/

typedef struct SC_File_Pos
{
    SC_u32 file;
    SC_u32 offset;
    SC_u32 line;
    SC_u32 col;
} SC_File_Pos;

typedef struct SC_Text
{
    union { struct SC_File_Pos; SC_File_Pos file_pos; };
    SC_u32 length;
} SC_Text;

typedef enum SC_Token_Kind
{
    SC_Token_Invalid = 0,
    SC_Token_EOF,
    
    // header name
    SC_Token_Identifier,
    SC_Token_PPNumber,
    SC_Token_Char,
    SC_Token_String,
    // punctuator
    // rest
} SC_Token_Kind;

typedef struct SC_Token
{
    union { struct SC_Text; SC_Text text; };
    
    SC_Token_Kind kind;
} SC_Token;

typedef struct SC_Lexer
{
    SC_String string;
    SC_Text current_text;
    SC_Text peek_text;
    SC_File_Pos cursor;
    SC_i8 current;
    SC_i8 peek;
} SC_Lexer;

SC_Lexer
SC_Lexer_Init(SC_String string, SC_File_Pos initial_pos)
{
    return (SC_Lexer){
        .current_pos = initial_pos,
        .string      = string
    };
}

SC_bool
SC_Lexer__IsAlpha(SC_u8 c)
{
    return ((SC_u8)((c & 0xDF) - 'A') <= (SC_u8)('Z' - 'A'));
}

SC_bool
SC_Lexer__IsDigit(SC_u8 c)
{
    return ((SC_u8)(c - '0') < (SC_u8)('9' - '0'));
}

SC_bool
SC_Lexer__IsHexAlphaDigit(SC_u8 c)
{
    return ((SC_u8)((c & 0xDF) - 'A') <= (SC_u8)('F' - 'A'));
}

void
SC_Lexer__NextChar(SC_Lexer* lexer)
{
    // -1 used for errors, caught in NextToken
    // trigraphs, backspaced newlines, line and col count
    SC_NOT_IMPLEMENTED;
}

SC_Token
SC_Lexer_NextToken(SC_Lexer* lexer)
{
    SC_Token result = { .kind = SC_Token_Invalid };
    
    SC_NOT_IMPLEMENTED;
    
    if (lexer->current == '\n')
    {
        result.kind = SC_Token_Newline;
        SC_NOT_IMPLEMENTED;
    }
    else if (SC_Lexer__IsAlpha(lexer->current) || lexer->current == '\\' && (lexer->peek == 'u' || lexer->peek == 'U'))
    {
        result.kind = SC_Token_Identifier;
        SC_NOT_IMPLEMENTED;
    }
    else if (SC_Lexer__IsDigit(lexer->current))
    {
        result.kind = SC_Token_PPNumber;
        SC_NOT_IMPLEMENTED;
    }
    else if (lexer->current == '\'' || lexer->current == 'L' && lexer->peek == '\'')
    {
        result.kind = SC_Token_Char;
        SC_NOT_IMPLEMENTED;
    }
    else if (lexer->current == '"' || lexer->current == 'L' && lexer->peek == '"')
    {
        result.kind = SC_Token_String;
        SC_NOT_IMPLEMENTED;
    }
    else
    {
        SC_NOT_IMPLEMENTED;
    }
    
    if (lexer->current == -1)
    {
        SC_NOT_IMPLEMENTED;
    }
    
    return result;
}

#include <stdio.h>

int
main(int argc, char** argv)
{
    SC_Lexer_Init(SC_STRING("#include \"platform.h\"\n"
                            "#include \"math.h\"\n"
                            "#include \"font.h\"\n"
                            "\n"
                            "#define CELL_SIZE 24\n"
                            "#define CELL_BORDER_SIZE 2\n"
                            "#define PADDED_CELL_SIZE (CELL_SIZE + 2*CELL_BORDER_SIZE)\n"
                            "#define BLOCK_BORDER_SIZE 2\n"
                            "#define BLOCK_BORDER_OFFSET 2\n"
                            "#define BOARD_SIZE (PADDED_CELL_SIZE*(16 + 3))\n"
                            "\n"
                            "#define CELL_POS_BOARD_ORIGIN_X 2\n"
                            "#define CELL_POS_BOARD_ORIGIN_Y 3\n"
                            "\n"
                            "#define FONT_CHAR_SIZE 10\n"
                            "#define STIPPLE_CHAR_START 176\n"
                            "#define FLAG_CHAR 0x21\n"
                            "\n"
                            "typedef struct Cell\n"
                            "{\n"
                            "	i8 danger;\n"
                            "	bool has_bomb;\n"
                            "	bool is_revealed;\n"
                            "	bool has_flag;\n"
                            "} Cell;\n"
                            "\n"
                            "#define BOARD_FLAT_SIZE 256\n"
                            "#define BOARD_AT(coord) State.board[(coord).y*64 + (coord).x*16 + (coord).w*4 + (coord).z]\n"
                            "\n"
                            "// NOTE: This is kind of an ugly macro, but it is way better than the flat iterator approach\n"
                            "#define FOR_EACH_NEIGHBOUR(center) for (V4I it = { .y = MAX((center).y - 1, 0) }; it.y <= MIN((center).y + 1, 3); ++it.y)  \\n"
                            "								       for (it.x = MAX((center).x - 1, 0); it.x <= MIN((center).x + 1, 3); ++it.x)         \\n"
                            "									       for (it.w = MAX((center).w - 1, 0); it.w <= MIN((center).w + 1, 3); ++it.w)     \\n"
                            "										       for (it.z = MAX((center).z - 1, 0); it.z <= MIN((center).z + 1, 3); ++it.z) \\n"
                            "											       if (V4I_IsEqual(it, (center))) continue;                                \\n"
                            "												   else\n"
                            "typedef struct Game_State\n"
                            "{\n"
                            "	float timer;\n"
                            "	bool game_over;\n"
                            "	Cell board[BOARD_FLAT_SIZE];\n"
                            "} Game_State;\n"
                            "\n"
                            "Game_State State = {0};\n"
                            "\n"
                            "void\n"
                            "FillBox(umm x, umm y, umm w, umm h, u32 color)\n"
                            "{\n"
                            "	for (umm j = y; j < MIN(y + h, SCREEN_SIZE); ++j)\n"
                            "	{\n"
                            "		for (umm i = x; i < MIN(x + w, SCREEN_SIZE); ++i)\n"
                            "		{\n"
                            "			Platform->screen[j*SCREEN_SIZE + i] = color;\n"
                            "		}\n"
                            "	}\n"
                            "}\n"
                            "\n"
                            "void\n"
                            "FillBoxA(umm x, umm y, umm w, umm h, u32 color, f32 alpha)\n"
                            "{\n"
                            "	for (umm j = y; j < MIN(y + h, SCREEN_SIZE); ++j)\n"
                            "	{\n"
                            "		for (umm i = x; i < MIN(x + w, SCREEN_SIZE); ++i)\n"
                            "		{\n"
                            "			umm k = j*SCREEN_SIZE + i;\n"
                            "			Platform->screen[k] = V3_ToRGBU32(V3_Add(V3_Scale(V3_FromRGBU32(Platform->screen[k]), 1.0f - alpha), V3_Scale(V3_FromRGBU32(color), alpha)));\n"
                            "		}\n"
                            "	}\n"
                            "}\n"
                            "\n"
                            "void\n"
                            "DrawChar(u8 c, umm x, umm y, u32 color)\n"
                            "{\n"
                            "	for (umm j = y; j < MIN(y + 10, SCREEN_SIZE); ++j)\n"
                            "	{\n"
                            "		for (umm i = x; i < MIN(x + 10, SCREEN_SIZE); ++i)\n"
                            "		{\n"
                            "			u64 lo = Font[c/16][c%16][0];\n"
                            "			u64 hi = Font[c/16][c%16][1];\n"
                            "\n"
                            "			umm index = (j-y)*10 + (i-x);\n"
                            "\n"
                            "			if ((index < 64 ? (lo & (1ULL << index)) : (hi & (1ULL << (index - 64)))) != 0) Platform->screen[j*SCREEN_SIZE + i] = color;\n"
                            "		}\n"
                            "	}\n"
                            "}\n"
                            "\n"
                            "void\n"
                            "DrawCharX(u8 c, umm x, umm y, umm size, u32 color)\n"
                            "{\n"
                            "	for (umm j = y; j < MIN(y + 10*size, SCREEN_SIZE); ++j)\n"
                            "	{\n"
                            "		for (umm i = x; i < MIN(x + 10*size, SCREEN_SIZE); ++i)\n"
                            "		{\n"
                            "			u64 lo = Font[c/16][c%16][0];\n"
                            "			u64 hi = Font[c/16][c%16][1];\n"
                            "\n"
                            "			umm index = ((j-y)/size)*10 + (i-x)/size;\n"
                            "\n"
                            "			if ((index < 64 ? (lo & (1ULL << index)) : (hi & (1ULL << (index - 64)))) != 0) Platform->screen[j*SCREEN_SIZE + i] = color;\n"
                            "		}\n"
                            "	}\n"
                            "}\n"
                            "\n"
                            "void\n"
                            "DrawStringX(String string, umm x, umm y, umm char_size, u32 color)\n"
                            "{\n"
                            "	for (umm i = 0, offset = 0; i < string.size; i += 1, offset += char_size*10)\n"
                            "	{\n"
                            "		DrawCharX(string.data[i], x + offset, y, char_size, color);\n"
                            "	}\n"
                            "}\n"
                            "\n"
                            "V2I\n"
                            "CellToScreenCoord(V4I coord)\n"
                            "{\n"
                            "	return (V2I){(CELL_POS_BOARD_ORIGIN_X + coord.x*5 + coord.z)*PADDED_CELL_SIZE, (CELL_POS_BOARD_ORIGIN_Y + coord.y*5 + coord.w)*PADDED_CELL_SIZE};\n"
                            "}\n"
                            "\n"
                            "void\n"
                            "DrawBoardBackground()\n"
                            "{\n"
                            "	umm padding_x = CELL_POS_BOARD_ORIGIN_X * PADDED_CELL_SIZE;\n"
                            "	umm padding_y = CELL_POS_BOARD_ORIGIN_Y * PADDED_CELL_SIZE;\n"
                            "\n"
                            "	FillBox(padding_x, padding_y, BOARD_SIZE, BOARD_SIZE, 0);\n"
                            "\n"
                            "	for (i32 j = 0; j < 4; ++j)\n"
                            "	{\n"
                            "		for (i32 i = 0; i < 4; ++i)\n"
                            "		{\n"
                            "			umm block_size = PADDED_CELL_SIZE*4 + BLOCK_BORDER_SIZE*2;\n"
                            "\n"
                            "			umm x = padding_x + i * PADDED_CELL_SIZE*(4 + 1);\n"
                            "			umm y = padding_y + j * PADDED_CELL_SIZE*(4 + 1);\n"
                            "\n"
                            "			FillBox(x - BLOCK_BORDER_OFFSET, y - BLOCK_BORDER_OFFSET, block_size, block_size, 0xFFFFFF);\n"
                            "			FillBox(x, y, PADDED_CELL_SIZE*4, PADDED_CELL_SIZE*4, 0);\n"
                            "\n"
                            "			for (i32 cj = 0; cj < 4; ++cj)\n"
                            "			{\n"
                            "				for (i32 ci = 0; ci < 4; ++ci)\n"
                            "				{\n"
                            "					umm cx = x + ci * PADDED_CELL_SIZE;\n"
                            "					umm cy = y + cj * PADDED_CELL_SIZE;\n"
                            "\n"
                            "					Cell* cell = &BOARD_AT(Vec4I(i, j, ci, cj));\n"
                            "\n"
                            "					if (!cell->is_revealed)\n"
                            "					{\n"
                            "						FillBox(cx + CELL_BORDER_SIZE, cy + CELL_BORDER_SIZE, CELL_SIZE, CELL_SIZE, 0xFFFFFF);\n"
                            "						if (cell->has_flag) DrawCharX(FLAG_CHAR, cx + CELL_BORDER_SIZE*2, cy + CELL_BORDER_SIZE*2, 2, 0xAA1111); // extra offset of CELL_BORDER_SIZE bc. more centered visually\n"
                            "					}\n"
                            "					else if (cell->danger == 0)  DrawCharX(STIPPLE_CHAR_START, cx + CELL_BORDER_SIZE, cy + CELL_BORDER_SIZE, 2, 0xFFFFFF);\n"
                            "					else\n"
                            "					{\n"
                            "						if (cell->danger < 10) DrawChar('0' + cell->danger, cx + CELL_BORDER_SIZE + CELL_SIZE/2 - FONT_CHAR_SIZE/2, cy + CELL_BORDER_SIZE + CELL_SIZE/2 - FONT_CHAR_SIZE/2, 0xFFFFFF);\n"
                            "						else\n"
                            "						{\n"
                            "							DrawChar('0' + cell->danger / 10, cx + CELL_BORDER_SIZE, cy + CELL_BORDER_SIZE + CELL_SIZE/2 - FONT_CHAR_SIZE/2, 0xFFFFFF);\n"
                            "							DrawChar('0' + cell->danger % 10, cx + CELL_BORDER_SIZE + FONT_CHAR_SIZE, cy + CELL_BORDER_SIZE + CELL_SIZE/2 - FONT_CHAR_SIZE/2, 0xFFFFFF);\n"
                            "						}\n"
                            "					}\n"
                            "				}\n"
                            "			}\n"
                            "		}\n"
                            "	}\n"
                            "}\n"
                            "\n"
                            "void\n"
                            "UncoverZeroNeighbours(V4I center)\n"
                            "{\n"
                            "	umm stack_size = 0;\n"
                            "	V4I stack[256];\n"
                            "\n"
                            "	if (BOARD_AT(center).danger == 0) BOARD_AT(center).danger = -1;\n"
                            "	stack[stack_size++] = center;\n"
                            "\n"
                            "	while (stack_size > 0)\n"
                            "	{\n"
                            "		V4I c = stack[stack_size - 1];\n"
                            "		stack_size -= 1;\n"
                            "\n"
                            "		FOR_EACH_NEIGHBOUR(c)\n"
                            "		{\n"
                            "			Cell* neighbour = &BOARD_AT(it);\n"
                            "\n"
                            "			if (neighbour->danger == 0 && !neighbour->is_revealed)\n"
                            "			{\n"
                            "				neighbour->danger   = -1;\n"
                            "				stack[stack_size++] = it;\n"
                            "			}\n"
                            "		}\n"
                            "	}\n"
                            "\n"
                            "	for (unsigned int i = 0; i < BOARD_FLAT_SIZE; ++i)\n"
                            "	{\n"
                            "		Cell* c = &State.board[i];\n"
                            "\n"
                            "		if (c->danger == -1)\n"
                            "		{\n"
                            "			c->danger      = 0;\n"
                            "			c->is_revealed = true;\n"
                            "		}\n"
                            "	}\n"
                            "}\n"
                            "\n"
                            "void\n"
                            "UpdateDanger()\n"
                            "{\n"
                            "	for (i32 j = 0; j < 4; ++j)\n"
                            "	{\n"
                            "		for (i32 i = 0; i < 4; ++i)\n"
                            "		{\n"
                            "			for (i32 cj = 0; cj < 4; ++cj)\n"
                            "			{\n"
                            "				for (i32 ci = 0; ci < 4; ++ci)\n"
                            "				{\n"
                            "					V4I cell = Vec4I(j, i, cj, ci);\n"
                            "					FOR_EACH_NEIGHBOUR(cell) BOARD_AT(cell).danger += BOARD_AT(it).has_bomb;\n"
                            "				}\n"
                            "			}\n"
                            "		}\n"
                            "	}\n"
                            "}\n"
                            "\n"
                            "void\n"
                            "RegenerateBoard()\n"
                            "{\n"
                            "	static u16 lfsr = 22340;\n"
                            "\n"
                            "	memset(State.board, 0, BOARD_FLAT_SIZE*sizeof(Cell));\n"
                            "\n"
                            "	for (unsigned int k = 0; k < 10; ++k)\n"
                            "	{\n"
                            "		V4I coord = {\n"
                            "			.x = (lfsr / 64) % 4,\n"
                            "			.y = (lfsr / 16) % 4,\n"
                            "			.z = (lfsr / 4) % 4,\n"
                            "			.w = lfsr % 4,\n"
                            "		};\n"
                            "\n"
                            "		BOARD_AT(coord).has_bomb = true;\n"
                            "\n"
                            "		lfsr = (lfsr >> 1) | (((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) << 15);\n"
                            "	}\n"
                            "\n"
                            "	UpdateDanger();\n"
                            "}\n"
                            "\n"
                            "void\n"
                            "Tick(Platform_State* platform)\n"
                            "{\n"
                            "	if (Platform == 0)\n"
                            "	{\n"
                            "		Platform = platform;\n"
                            "		FillBox(0, 0, SCREEN_SIZE, SCREEN_SIZE, 0);\n"
                            "		RegenerateBoard();\n"
                            "	}\n"
                            "	ASSERT(Platform == platform);\n"
                            "\n"
                            "	V4I selected_cell     = {0};\n"
                            "	bool cell_is_selected = false;\n"
                            "	{ // Input\n"
                            "		i32 x = Platform->mouse_x / PADDED_CELL_SIZE - CELL_POS_BOARD_ORIGIN_X;\n"
                            "		i32 y = Platform->mouse_y / PADDED_CELL_SIZE - CELL_POS_BOARD_ORIGIN_Y;\n"
                            "\n"
                            "		u32 ox = (x % 5) & I32_MAX;\n"
                            "		u32 bx = (u32)((x - ox) / 5);\n"
                            "		u32 oy = (y % 5) & I32_MAX;\n"
                            "		u32 by = (u32)((y - oy) / 5);\n"
                            "\n"
                            "		selected_cell    = Vec4I(bx, by, ox, oy);\n"
                            "		cell_is_selected = ((bx < 4) + (ox < 4) + (by < 4) + (oy < 4) == 4);\n"
                            "	}\n"
                            "\n"
                            "	{ // Game logic\n"
                            "		if (State.game_over)\n"
                            "		{\n"
                            "			if (Platform->mouse_left == 1 || Platform->mouse_right == 1)\n"
                            "			{\n"
                            "				RegenerateBoard();\n"
                            "\n"
                            "				State.game_over = false;\n"
                            "				State.timer     = 0;\n"
                            "			}\n"
                            "		}\n"
                            "		else\n"
                            "		{\n"
                            "			State.timer += Platform->dt;\n"
                            "\n"
                            "			if (cell_is_selected)\n"
                            "			{\n"
                            "				if (Platform->mouse_left == 1)\n"
                            "				{\n"
                            "					if (BOARD_AT(selected_cell).has_bomb)\n"
                            "					{\n"
                            "						State.game_over = true;\n"
                            "						Platform->OpenURL(\"https://youtube.com/clip/UgkxKtXnhCuCDqJgnFu3ulnKMs_brNOrrkd8\");\n"
                            "					}\n"
                            "					else if (!BOARD_AT(selected_cell).has_flag)\n"
                            "					{\n"
                            "						BOARD_AT(selected_cell).is_revealed = true;\n"
                            "						if (BOARD_AT(selected_cell).danger == 0) UncoverZeroNeighbours(selected_cell);\n"
                            "					}\n"
                            "				}\n"
                            "				else if (Platform->mouse_right == 1)\n"
                            "				{\n"
                            "					if (!BOARD_AT(selected_cell).is_revealed) BOARD_AT(selected_cell).has_flag = !BOARD_AT(selected_cell).has_flag;\n"
                            "				}\n"
                            "			}\n"
                            "		}\n"
                            "	}\n"
                            "\n"
                            "	{ // Rendering\n"
                            "		{ // Draw timer\n"
                            "			umm t = MIN((umm)State.timer, 3599);\n"
                            "			String timer_string = {\n"
                            "				.data = (u8[5]){(u8)('0' + (t / 60) / 10), (u8)('0' + (t / 60) % 10), ':', (u8)('0' + (t % 60) / 10), (u8)('0' + (t % 60) % 10)},\n"
                            "				.size = 5,\n"
                            "			};\n"
                            "\n"
                            "			FillBox(CELL_POS_BOARD_ORIGIN_X*PADDED_CELL_SIZE, 20, 10*2*timer_string.size, 10*2, 0);\n"
                            "			DrawStringX(timer_string, CELL_POS_BOARD_ORIGIN_X*PADDED_CELL_SIZE, 20, 2, 0xFFFFFF);\n"
                            "		}\n"
                            "\n"
                            "		DrawBoardBackground();\n"
                            "\n"
                            "		if (State.game_over)\n"
                            "		{\n"
                            "			FillBox(0, 7*PADDED_CELL_SIZE, SCREEN_SIZE, 100, 0);\n"
                            "			DrawStringX(STRING(\"Game Over\"), 3*PADDED_CELL_SIZE, 7*PADDED_CELL_SIZE + 20, 5, 0xFFFFFF);\n"
                            "		}\n"
                            "		else if (cell_is_selected)\n"
                            "		{\n"
                            "			V2I center_cell = CellToScreenCoord(selected_cell);\n"
                            "\n"
                            "			FillBoxA(center_cell.x, center_cell.y, PADDED_CELL_SIZE, PADDED_CELL_SIZE, 0x66CCFF, 0.5f);\n"
                            "\n"
                            "			FOR_EACH_NEIGHBOUR(selected_cell)\n"
                            "			{\n"
                            "				V2I neighbour_cell = CellToScreenCoord(it);\n"
                            "				FillBoxA(neighbour_cell.x, neighbour_cell.y, PADDED_CELL_SIZE, PADDED_CELL_SIZE, 0xFFCC66, 0.5f);\n"
                            "			}\n"
                            "		}\n"
                            "	}\n"
                            "}\n"), (SC_File_Pos){0});
    
    for (;;)
    {
        SC_Token token = SC_Lexer_NextToken();
        
        if      (token.kind == SC_Token_EOF) break;
        else if (token.kind == SC_Token_Invalid) SC_NOT_IMPLEMENTED;
        else
        {
            SC_NOT_IMPLEMENTED;
        }
    }
    
    return 0;
}

// TODO: Translation phases acording to the spec
// 1. some arbitrarily encoded text -> c source character set + trigraph resolution
// 2. concatenating lines on backspaced line breaks
// 3. tokenization
// 4. preprocessing
// 5. escaped character resolution
// 6. adjacent string literals are concatenated
// 7. the rest of the fucking ~~owl~~ frontend
// 8. linking

// NOTE: Observations
// - the preprocessor seems to operate line by line
// - backspaced newlines are counted by the tokenizer, but not the preprocessor

// TODO: Problems
// - line and column counting seems tricky, source text should probably be directly turned into tokens
// - tracking macro expansions (for error messages)

// TODO: Plan
// - Go straight from file text to tokens to preserve line and column information
// - Run preprocessor on tokens, preserving line,col info
// - Concatenate and canonicalize strings and identifiers
// - Parse...

// NOTE: Stringizing is weird (see footnotes on 6.4.7)
// NOTE: Escaping a newline does not insert a space, A\\nA is counted as AA













/*

auto
break
case
char
const
continue
default
do
double
else
enum
extern
float
for
goto
if
inline
int
long
register
restrict
return
short
signed
sizeof
static
struct
switch
typedef
union
unsigned
void
volatile
while
_Bool
_Complex
_Imaginary

*/