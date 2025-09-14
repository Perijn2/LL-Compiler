//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include <ctype.h>
#include <stdlib.h>

#include "raw_lexer.h"
#include "lex_numbers.h"

//------------------------------------------------------------------------------
// Public API
//------------------------------------------------------------------------------

uint32_t rl_init(raw_lexer_t* rl, const char* filename, const uint8_t* begin, size_t len)
{

}

uint32_t rl_from_file(raw_lexer_t* rl, const char* path)
{
	file_buf_handle* hdl = malloc(sizeof(file_buf_handle));
	if (!hdl) return LEX_STATUS_OUT_OF_MEMORY;

	if (fb_open(path, hdl) != FILE_STATUS_OK)
	{ 
		return LEX_STATUS_IO_ERROR; 
	}

	raw_lexer_t* raw_lex = malloc(sizeof(raw_lexer_t));
	if (!raw_lex) { return LEX_STATUS_OUT_OF_MEMORY; }

	char* data;
	size_t len = rl_trim_comments(hdl->data, hdl->len, data);

	memset(rl, 0, sizeof * rl);
	rl->begin = data;
	rl->cur = data;
	rl->end = data + len;
	rl->filename = path;
	rl->fb = hdl;
	rl->line = 1;
	rl->col = 1;
}

void rl_destroy(raw_lexer_t* rl)
{
	if (!rl || !rl->fb) return;
	fb_close(rl->fb);      /* unmap / free file buffer */
	free(rl->fb);          /* free the FileBuf struct  */
	memset(rl, 0, sizeof * rl);
}

static inline token_t* make_tok(token_type_t type, operator_subtype_t sub,
	raw_lexer_t* rl,
	const uint8_t* start,
	uint32_t start_col)
{
	token_t* t = (token_t*)malloc(sizeof(token_t));
	t->type = type;         /* your main token kind */
	t->subtype = sub;                  /* operator_subtype_t */
	t->text = (const char*)start;
	t->len = (size_t)(rl->cur - start);
	t->line = rl->line;
	//t->col = start_col;
	t->bol_hash = false;
	return t;
}

lexer_status_t rl_next(raw_lexer_t* rl, token_t* tk)
{
	// Trimming await the whitespaces
	rl_trim_whitespaces(rl);

	// if curser hits the end return a EOF token
	if (rl->cur >= rl->end)
	{
		static token_t eof_tok = { .type = TOK_EOF };
		tk = &eof_tok;                 /* sets len/col internally */
		return LEX_STATUS_EOF;
	}

	// Record the start and start_col before lexing
	const uint8_t* start = rl->cur;
	uint32_t start_col = rl->col;

	/* detect # at beginning of logical line */
	bool bol_hash_flag = (CUR == '#' && start_col == 1);

	/* identifier / keyword */
	if (is_indent_start(CUR)) {
		tk = rl_lex_identifier(rl);                 /* sets len/col internally */
		return LEX_STATUS_OK;
	}

	/* number */
	if (rl_is_number_start(rl)) {
		tk = rl_lex_number(rl);                 /* sets len/col internally */
		return LEX_STATUS_OK;
	}

	/* string / char literal */
	if (CUR == '"' || CUR == '\'') {
		tk = rl_lex_string(rl);                 /* handles escapes, updates pos */
		return LEX_STATUS_OK;
	}

	/* single char punctuators a la {}[]();,< > ; etc. */
	switch (CUR) {
	case '{': case '}': case '[': case ']':
	case '(': case ')': case ';': case ',':
	{
		ADV();

		token_t* tok = (token_t*)malloc(sizeof(token_t));
		tok->type = get_punct_kind(start[0]);
		tok->text = (const char*)start;
		tok->len = 1;
		tok->line = rl->line;
		tok->bol_hash = false;
		tk = tok;                 /* sets len/col internally */
		return LEX_STATUS_OK;
	}
	default: break;
	}

	/* operators (+ - * / % ! = < > & | ^ ~ etc; handle multi char) */
	if (is_op_start(CUR)) {
		tk = rl_lex_operator(rl);
		return LEX_STATUS_OK;/* consumes, sets type/len */
	}

	/* hash at BOL return it, PP layer will consume directive */
	if (bol_hash_flag) {
		ADV();

		token_t* tok = (token_t*)malloc(sizeof(token_t));
		tok->type = TOK_HASH;
		tok->bol_hash = true;
		return LEX_STATUS_OK;
	}

	/* unknown / invalid char */
	ADV();

	static token_t bad_tok = { .type = TOK_INVALID };
	tk = &bad_tok;                 /* sets len/col internally */
	return LEX_STATUS_INVALID_CHAR;
}


//token_t* rl_lex_operator(raw_lexer_t* rl)
//{
//	init_operator_hashes();
//
//	const uint8_t* start = rl->cur;
//	uint32_t start_col = rl->col;
//
//	/* Longest match first (up to 3 here—add 4 if you have any) */
//	size_t max_len = (size_t)(rl->end - rl->cur);
//	if (max_len > 3) max_len = 3;
//
//	for (size_t len = max_len; len >= 1; --len) {
//		uint32_t h = hashOperator((const char*)rl->cur, len);
//
//		for (const operater_def* op = OPERATORS; op->text; ++op) {
//			if (op->len != len) continue;
//			if (op->hash != h)  continue;
//			if (memcmp(rl->cur, op->text, len) != 0) continue;
//
//			/* match! */
//			rl->cur += len;
//			rl->col += (uint32_t)len;
//
//			return make_tok(TOK_OPERATOR, op->type, rl, start, start_col);
//		}
//	}
//
//	/* nothing matched – consume one char and return invalid */
//	rl->cur++;
//	rl->col++;
//	return make_tok(TOK_INVALID, OP_INVALID, rl, start, start_col);
//}

size_t rl_trim_comments(char* src, size_t len, char* ptr)
{
	char* ret = src;
	char* dst = src;
	while (*src) {
		if (*src == '/' && src[1] == '*') {
			{
				src += 2;
				len -= 2;
			}
			while (!(*src == '*' && src[1] == '/')) { src++; len--; }
			src += 2;
			len -= 2;
		}
		else if (*src == '/' && src[1] == '/') {
			while (*src != '\n' && *src != '\0') { src++; len--; }
		}
		else {
			*(dst++) = *(src++);
		}
	}
	*dst = '\0';
	ptr = ret;
	return len;
}

void rl_trim_whitespaces(raw_lexer_t* rl)
{
	if (rl->cur >= rl->end) return;

	while (isspace(CUR))
	{
		ADV();
	}
}

token_t* rl_lex_identifier(raw_lexer_t* rl)
{
	const uint8_t* start = rl->cur;
	uint32_t start_col = rl->col;

	/* caller usually checked id_start(CUR); do it anyway for safety */
	if (!is_indent_start(CUR)) {
		/* return an invalid token or assert – your choice */
		ADV();
		static token_t bad_tok = { .type = TOK_INVALID };
		return &bad_tok;
	}

	ADV();  /* consume first char */

	/* consume the rest */
	while (rl->cur < rl->end && is_ident_char(CUR))
		ADV();

	token_t* tok = (token_t*)malloc(sizeof(token_t));
	tok->type = TOK_IDENTIFIER;                 /* no keyword classification here */
	tok->text = (const char*)start;
	tok->len = (size_t)(rl->cur - start);
	tok->line = rl->line;
	//tok->col = start_col;
	tok->bol_hash = false;                     /* identifiers can’t start PP lines */

	/* we’re no longer at beginning of logical line */
	/* if you track a flag: rl->at_bol = false; */

	return tok;
}

token_t* rl_lex_operator(raw_lexer_t* rl)
{
	const uint8_t* start = rl->cur;
	uint32_t start_col = rl->col;

	unsigned char c = CUR;
	unsigned char c1 = PEEK(1);
	unsigned char c2 = PEEK(2);

#define ONE(sub)                  do { ADV();    return make_tok(TOK_OPERATOR, sub, rl, start, start_col); } while(0)
#define TWO(sub)                  do { ADV_N(2); return make_tok(TOK_OPERATOR, sub, rl, start, start_col); } while(0)
#define THREE(sub)                do { ADV_N(3); return make_tok(TOK_OPERATOR, sub, rl, start, start_col); } while(0)

	switch (c) {
	case '+':
		if (c1 == '+')          TWO(OP_INC);
		if (c1 == '=')          TWO(OP_ADD_ASSIGN);
		ONE(OP_ADD);
	case '-':
		if (c1 == '-')          TWO(OP_DEC);
		if (c1 == '=')          TWO(OP_MIN_ASSIGN);
		if (c1 == '>')          TWO(OP_ARROW);
		ONE(OP_MIN);
	case '*':
		if (c1 == '=')          TWO(OP_MUL_ASSIGN);
		ONE(OP_MUL);
	case '/':
		if (c1 == '=')          TWO(OP_DIV_ASSIGN);
		ONE(OP_DIV);
	case '%':
		if (c1 == '=')          TWO(OP_MOD_ASSIGN);
		ONE(OP_MOD);
	case '&':
		if (c1 == '&')          TWO(OP_LOG_AND);
		if (c1 == '=')          TWO(OP_AND_ASSIGN);
		ONE(OP_BIT_AND);
	case '|':
		if (c1 == '|')          TWO(OP_LOG_OR);
		if (c1 == '=')          TWO(OP_OR_ASSIGN);
		ONE(OP_BIT_OR);
	case '^':
		if (c1 == '=')          TWO(OP_XOR_ASSIGN);
		ONE(OP_BIT_XOR);
	case '~':
		ONE(OP_BIT_NOT);
	case '!':
		if (c1 == '=')          TWO(OP_NEQ);
		ONE(OP_LOG_NOT);
	case '=':
		if (c1 == '=')          TWO(OP_EQ);
		ONE(OP_ASSIGN);
	case '<':
		if (c1 == '<') {
			if (c2 == '=')      THREE(OP_SHL_ASSIGN);
			TWO(OP_SHL);
		}
		if (c1 == '=')          TWO(OP_LESS_EQUAL);
		ONE(OP_LESS);
	case '>':
		if (c1 == '>') {
			if (c2 == '=')      THREE(OP_SHR_ASSIGN);
			TWO(OP_SHR);
		}
		if (c1 == '=')          TWO(OP_GREATER_EQUAL);
		ONE(OP_GREATER);
	case '.':
		if (c1 == '.' && c2 == '.') THREE(OP_ELLIPSIS);
		ONE(OP_DOT);
	case '#':
		if (c1 == '#')          TWO(OP_HASHHASH);
		ONE(OP_HASH);
	case '?': ONE(OP_QUESTION);
	case ':': ONE(OP_COLON);
	case ',': ONE(OP_COMMA);
	default:
		/* not an operator start – consume 1 char as invalid */
		ADV();
		return make_tok(TOK_OPERATOR, OP_INVALID, rl, start, start_col);
	}

#undef ONE
#undef TWO
#undef THREE
}

token_t* rl_lex_string(raw_lexer_t* rl)
{
	const uint8_t* start = rl->cur;
	uint32_t start_col = rl->col;
	
	bool is_char = CUR == '\'';

	unsigned char quote = CUR;
	ADV();

	uint64_t char_acc = 0;   /* for character constants */
	int char_units = 0;      /* how many code units seen in char literal */

	while (!AT_END) {
		unsigned char c = CUR;

		/* end quote */
		if (c == quote) {
			ADV();
			token_t *tok = make_tok(is_char ? TOK_CHAR : TOK_STRING, OP_INVALID, rl, start, start_col);
			if (is_char) tok->intValue = 1;
			return tok;
		}

		/* ordinary byte in literal */
		if (is_char) { char_acc = (char_acc << 8) | (uint64_t)c; ++char_units; }
		ADV();

		ADV();
	}
}


static inline bool rl_is_number_start(const raw_lexer_t* rl)
{
	if (rl->cur >= rl->end)               /* past EOF safety */
		return false;

	unsigned char c = *rl->cur;

	/* 1. Ordinary decimal   1  9 */
	if (isdigit(c) && c != '0')
		return true;

	/* 2. 0x  /  0b  /  0 octal  */
	if (c == '0' && rl->cur + 1 < rl->end) {
		unsigned char nxt = (unsigned char)tolower(*(rl->cur + 1));

		/* 0x  or  0b  */
		if (nxt == 'x' || nxt == 'b')
			return true;

		/* 0octal (0 followed by another digit) */
		if (isdigit(nxt))
			return true;

		/* 0.123 (float starting with zero) */
		if (nxt == '.')
			return true;
	}

	/* 3. .123  (float that starts with a dot) */
	if (c == '.' && rl->cur + 1 < rl->end && isdigit(*(rl->cur + 1)))
		return true;

	return false;
}

static inline bool is_indent_start(unsigned char c)
{
	return (c == '_') ||
		isalpha(c) ||
		(c >= 0x80);                 /* allow UTF-8 lead bytes if you want */
}

static inline bool is_ident_char(char c) {
	return isalnum((unsigned char)c) || c == '_';
}

static inline token_type_t get_punct_kind(unsigned char c)
{
	switch (c) {
	case '{': return TOK_LBRACE;
	case '}': return TOK_RBRACE;
	case '[': return TOK_LBRACKET;
	case ']': return TOK_RBRACKET;
	case '(': return TOK_LPAREN;
	case ')': return TOK_RPAREN;
	case ';': return TOK_SEMICOLON;
	case ',': return TOK_COMMA;
	case '.': return TOK_DOT;
	case ':': return TOK_COLON;
	case '?': return TOK_QUESTION;
	case '#': return TOK_HASH;   /* raw lexer may still tag bol_hash separately */
	default:  return TOK_INVALID;
	}
}

//uint64_t octalToInteger(const char* src)
//{
//	uint64_t res = 0;
//	if (!src || *src == '\0') return 0; // Handle null or empty string
//	while (*src) {
//		if (isOctalDigit(*src)) {
//			res = (res << 3) | (*src - '0');
//		}
//		else {
//			//// Invalid character found
//			//printf("Error: Invalid binary digit '%c'\n", *bin_str);
//			return 0;
//		}
//		src++;
//	}
//	return res;
//}
//
//uint64_t binaryToInteger(const char* src)
//{
//	uint64_t res = 0;
//	if (!src || *src == '\0') return 0; // Handle null or empty string
//	while (*src) {
//		if (*src == '0' || *src == '1') {
//			res = (res << 1) | (*src - '0');
//		}
//		else {
//			//// Invalid character found
//			//printf("Error: Invalid binary digit '%c'\n", *bin_str);
//			return 0;
//		}
//		src++;
//	}
//	return res;
//}
//
//uint64_t hexadecimalToInteger(const char* src)
//{
//	uint64_t res = 0;
//	if (!src || *src == '\0') return 0; // Handle null or empty string
//
//	// Optional 0x or 0X prefix
//	if (src[0] == '0' && (tolower(src[1]) == 'x')) {
//		src += 2;
//	}
//
//
//	while (*src) {
//		if (isHexadecimalDigit(*src)) {
//			uint8_t digit = (isdigit(*src)) ? (*src - '0') : (10 + (tolower(*src) - 'a'));
//			res = (res << 4) | digit;
//		}
//		else {
//			//// Invalid character found
//			//printf("Error: Invalid binary digit '%c'\n", *bin_str);
//			return 0;
//		}
//		src++;
//	}
//	return res;
//}
//
//uint64_t decimalToInteger(const char* src)
//{
//	uint64_t res = 0;
//	if (!src || *src == '\0') return 0; // Handle null or empty string
//	while (*src) {
//		if (isdigit(*src)) {
//			res = res * 10 + (*src - '0');
//		}
//		else {
//			//// Invalid character found
//			//printf("Error: Invalid binary digit '%c'\n", *bin_str);
//			return 0;
//		}
//		src++;
//	}
//	return res;
//}

//------------------------------------------------------------------------------