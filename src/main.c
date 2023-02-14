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

typedef enum SC_Lexeme_Kind
{
    SC_Lexeme_Invalid = 0,
    SC_Lexeme_Unknown,
    SC_Lexeme_EOF,
    
    SC_Lexeme_Newline,
    SC_Lexeme_Identifier,
    SC_Lexeme_PPNumber,
    SC_Lexeme_Char,
    SC_Lexeme_String,
    SC_Lexeme_Punctuator,
} SC_Lexeme_Kind;

typedef struct SC_Lexeme
{
    SC_Lexeme_Kind kind;
} SC_Lexeme;

typedef struct SC_Lexer
{
    SC_String string;
    char current_char;
} SC_Lexer;

SC_u8 SC_Lexer__NextChar(SC_Lexer* lexer);

SC_Lexer
SC_Lexer_Init(SC_String string)
{
    SC_Lexer lexer = {
        .string       = string,
        .current_char = 0,
    };
    
    SC_Lexer__NextChar(&lexer);
    return lexer;
}

// NOTE: Doing this instead of the more recognizable c >= min && c <= max is kind of stupid on
//       compilers like clang, but seems to be needed on msvc to produce the same level of assembly.
//       Not that it matters anyway, and it might very well hurt optimization when inlined, but it is
//       fun.
SC_bool
SC_Lexer__IsAlpha(SC_u8 c)
{
    // NOTE: The only bit difference between upper- and lower case is the 5th bit, hence mask by 0xDF to map lower into upper case
    return ((SC_u8)((c & 0xDF) - 'A') <= (SC_u8)('Z' - 'A'));
}

SC_bool
SC_Lexer__IsDigit(SC_u8 c)
{
    return ((SC_u8)(c - '0') < 10u);
}

SC_bool
SC_Lexer__IsAlphaHexDigit(SC_u8 c)
{
    return ((SC_u8)((c & 0xDF) - 'A') <= (SC_u8)('F' - 'A'));
}

SC_bool
SC_Lexer__IsHexDigit(SC_u8 c)
{
    return (SC_Lexer__IsDigit(c) | SC_Lexer__IsAlphaHexDigit(c));
}

SC_u8
SC_Lexer__NextChar(SC_Lexer* lexer)
{
    // trigraphs
    // escaped newline
    // different newlines
    // different whitespace chars
    // column and line counting
    
    // TODO: ^ + set current char
    SC_NOT_IMPLEMENTED;
    return lexer->current_char;
}

SC_u8
SC_Lexer__GetChar(SC_Lexer* lexer)
{
    return lexer->current_char;
}

SC_Lexeme
SC_Lexer_NextLexeme(SC_Lexer* lexer)
{
    SC_Lexeme lexeme = { .kind = SC_Lexeme_Invalid };
    
    char curr = SC_Lexer__GetChar(lexer);
    char peek = SC_Lexer__NextChar(lexer);
    
    // NOTE: Skip whitespace
    for (;;)
    {
        if (curr == ' ')
        {
            while (peek == ' ') peek = SC_Lexer__NextChar(lexer);
        }
        else if (curr == '/' && peek == '/')
        {
            while (peek != 0 && peek != '\n') peek = SC_Lexer__NextChar(lexer);
        }
        else if (curr == '/' && peek == '*')
        {
            curr = SC_Lexer__NextChar(lexer);
            peek = SC_Lexer__NextChar(lexer);
            
            for (;;)
            {
                if (curr == 0)
                {
                    //// ERROR: Unterminated comment
                    SC_NOT_IMPLEMENTED;
                }
                else if (curr == '*' && peek == '/')
                {
                    SC_Lexer__NextChar(); // NOTE: Skip the slash ending the comment
                    break;
                }
                else
                {
                    curr = peek;
                    peek = SC_Lexer__NextChar(lexer);
                }
            }
        }
        else break;
    }
    
    if (curr == '\n')
    {
        lexeme.kind = SC_Lexeme_Newline;
    }
    else if (SC_Lexer__IsAlpha(curr) || curr == '_') // TODO: Universal character name
    {
        lexeme.kind = SC_Lexeme_Identifier;
        SC_NOT_IMPLEMENTED;
    }
    else if (SC_Lexer__IsDigit(curr) || curr == '.' && SC_Lexer__IsDigit(peek))
    {
        lexeme.kind = SC_Lexeme_PPNumber;
        SC_NOT_IMPLEMENTED;
    }
    else if (curr == '\'' || curr == 'L' && peek == '\'')
    {
        lexeme.kind = SC_Lexeme_Char;
        SC_NOT_IMPLEMENTED;
    }
    else if (curr == '"' || curr == 'L' && peek == '"')
    {
        lexeme.kind = SC_Lexeme_String;
        
        SC_bool is_wide = SC_false;
        if (curr == 'L')
        {
            is_wide = !SC_false;
            SC_Lexer__NextChar(lexer); // NOTE: Skip L
        }
        
        curr = SC_Lexer__NextChar(lexer);
        
        for (;;)
        {
            if (curr == 0 || curr == '\n')
            {
                //// ERROR: Unterminated string literal
                SC_NOT_IMPLEMENTED;
            }
            else if (curr == '"')
            {
                // TODO: Save string length
                SC_Lexer__NextChar(lexer); // NOTE: Skip "
                break;
            }
            else if (curr != '\\')
            {
                SC_NOT_IMPLEMENTED;
            }
            else
            {
                curr = SC_Lexer__NextChar(lexer);
                
                if (curr == 0) continue; // NOTE: Unterminated string literal, continue to next iteration to handle this in one place
                else
                {
                    switch (curr)
                    {
                        case '\'': = '\''; break;
                        case '\"': = '\"'; break;
                        case '\?': = '\?'; break;
                        case '\\': = '\\'; break;
                        
                        case 'a': = '\a'; break;
                        case 'b': = '\b'; break;
                        case 'f': = '\f'; break;
                        case 'n': = '\n'; break;
                        case 'r': = '\r'; break;
                        case 't': = '\t'; break;
                        case 'v': = '\v'; break;
                        
                        default:
                        {
                            if (curr >= '0' && curr <= '7')
                            {
                                SC_u16 value = 0;
                                for (SC_umm i = 0; i < 3; ++i)
                                {
                                    if (!SC_Lexer__IsDigit(curr) || curr >= 8) break;
                                    else
                                    {
                                        value = (value << 3) | (curr & 0xF));
                                        curr = SC_Lexer__NextChar(lexer);
                                    }
                                }
                                
                                // TODO: limits
                                SC_NOT_IMPLEMENTED;
                            }
                            else if (curr == 'x')
                            {
                                curr = SC_Lexer__NextChar(lexer);
                                
                                if (!SC_Lexer__IsHexDigit(curr))
                                {
                                    //// ERROR: Missing hex digits
                                    SC_NOT_IMPLEMENTED;
                                }
                                else
                                {
                                    SC_bool has_overflown = SC_false;
                                    SC_u64 value          = 0;
                                    for (;;)
                                    {
                                        SC_u8 digit;
                                        if      (SC_Lexer__IsDigit(curr))         digit = curr & 0xF;
                                        else if (SC_Lexer__IsAlphaHexDigit(curr)) digit = (curr & 0x1F) + 0x9;
                                        else break;
                                        
                                        has_overflown = ((value & (0xFULL << 60)) > 0);
                                        value         = (value << 4) | digit;
                                    }
                                    
                                    // TODO: limits
                                    SC_NOT_IMPLEMENTED;
                                    if (has_overflown)
                                    {
                                        SC_NOT_IMPLEMENTED;
                                    }
                                }
                            }
                            else
                            {
                                //// ERROR: Unknown escape sequence
                                SC_NOT_IMPLEMENTED;
                            }
                            
                        } break;
                    }
                }
                SC_NOT_IMPLEMENTED;
            }
        }
    }
    else
    {
        SC_Lexeme_Punctuator,
        SC_NOT_IMPLEMENTED;
    }
    
    return lexeme;
}

#include <stdio.h>

int
main(int argc, char** argv)
{
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
// - Parse...A

// NOTE: Stringizing is weird (see footnotes on 6.4.7)
// NOTE: Escaping a newline does not insert a space A\\nA is counted as AA













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