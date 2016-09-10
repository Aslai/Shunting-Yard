#include <string.h>
#include <ctype.h>
#include <stdio.h>

typedef enum{
    SY_TYPE_NONE,
    SY_TYPE_NUM,
    SY_TYPE_STR,
    SY_TYPE_CHR,
    SY_TYPE_ID,
    SY_TYPE_OPER,
    SY_TYPE_FUNC,
} sy_type_t;

typedef enum {
    SY_OP_INC,
    SY_OP_DEC,
    SY_OP_CBRACE_OPEN,
    SY_OP_CBRACE_CLOSE,
    SY_OP_PAREN_OPEN,
    SY_OP_PAREN_CLOSE,
    SY_OP_SBRACE_OPEN,
    SY_OP_SBRACE_CLOSE,
    SY_OP_DOT,
    SY_OP_ARROW,
    SY_OP_PLUS,
    SY_OP_MINUS,
    SY_OP_BANG,
    SY_OP_TILDE,
    SY_OP_STAR,
    SY_OP_AMP,
    SY_OP_DIV,
    SY_OP_MOD,
    SY_OP_RSHIFT,
    SY_OP_LSHIFT,
    SY_OP_LT,
    SY_OP_LTE,
    SY_OP_GT,
    SY_OP_GTE,
    SY_OP_EQUALS,
    SY_OP_BANG_EQUALS,
    SY_OP_CARAT,
    SY_OP_PIPE,
    SY_OP_AND,
    SY_OP_OR,
    SY_OP_ASSIGN,
    SY_OP_PLUS_ASSIGN,
    SY_OP_MINUS_ASSIGN,
    SY_OP_STAR_ASSIGN,
    SY_OP_DIV_ASSIGN,
    SY_OP_MOD_ASSIGN,
    SY_OP_LSHIFT_ASSIGN,
    SY_OP_RSHIFT_ASSIGN,
    SY_OP_AMP_ASSIGN,
    SY_OP_CARAT_ASSIGN,
    SY_OP_PIPE_ASSIGN,
    SY_OP_COMMA,
    SY_OP_SEMICOLON,
    SY_OP_COLON,
    SY_OP_SIZEOF,
    SY_OP_NULL,
    SY_OP_INDEX,
} sy_op_t;

typedef enum {
    SY_ASSOC_RTL,
    SY_ASSOC_LTR,
    SY_ASSOC_NONE
} assoc_t;

struct tok_equiv{
    const char * name;
    sy_op_t op;
    size_t prec_prefix;
    size_t prec_suffix;
    assoc_t assoc_prefix;
    assoc_t assoc_suffix;
};

#define SY_OP2( ord, str, prec_pre, ass_pre, prec_suf, ass_suf ) [ord] = {str, ord, prec_pre, prec_suf, ass_pre, ass_suf }
#define SY_OP( ord, str, args ) SY_OP2(ord, str, args)
#define SY_TWO(prec_pre, ass_pre, prec_suf, ass_suf) prec_pre, ass_pre, prec_suf, ass_suf
#define SY_ONE(prec, ass) SY_TWO(prec, ass, prec, ass)
#define SY_NONE() SY_ONE(1000, SY_ASSOC_NONE)

const struct tok_equiv token_list[] = {
        SY_OP( SY_OP_INC,           "++",       SY_TWO( 2,  SY_ASSOC_RTL,   1, SY_ASSOC_LTR ) ),
        SY_OP( SY_OP_DEC,           "--",       SY_TWO( 2,  SY_ASSOC_RTL,   1, SY_ASSOC_LTR ) ),
        SY_OP( SY_OP_PLUS,          "+",        SY_TWO( 2,  SY_ASSOC_RTL,   4, SY_ASSOC_LTR ) ),
        SY_OP( SY_OP_MINUS,         "-",        SY_TWO( 2,  SY_ASSOC_RTL,   4, SY_ASSOC_LTR ) ),
        SY_OP( SY_OP_STAR,          "*",        SY_TWO( 2,  SY_ASSOC_RTL,   3, SY_ASSOC_LTR ) ),
        SY_OP( SY_OP_AMP,           "&",        SY_TWO( 2,  SY_ASSOC_RTL,   8, SY_ASSOC_LTR ) ),
        SY_OP( SY_OP_SIZEOF,        "sizeof",   SY_ONE( 2,  SY_ASSOC_RTL ) ),
        SY_OP( SY_OP_DOT,           ".",        SY_ONE( 1,  SY_ASSOC_LTR ) ),
        SY_OP( SY_OP_ARROW,         "->",       SY_ONE( 1,  SY_ASSOC_LTR ) ),
        SY_OP( SY_OP_BANG,          "!",        SY_ONE( 2,  SY_ASSOC_RTL ) ),
        SY_OP( SY_OP_TILDE,         "~",        SY_ONE( 2,  SY_ASSOC_RTL ) ),
        SY_OP( SY_OP_DIV,           "/",        SY_ONE( 3,  SY_ASSOC_LTR ) ),
        SY_OP( SY_OP_MOD,           "%",        SY_ONE( 3,  SY_ASSOC_LTR ) ),
        SY_OP( SY_OP_RSHIFT,        ">>",       SY_ONE( 5,  SY_ASSOC_LTR ) ),
        SY_OP( SY_OP_LSHIFT,        "<<",       SY_ONE( 5,  SY_ASSOC_LTR ) ),
        SY_OP( SY_OP_LT,            "<",        SY_ONE( 6,  SY_ASSOC_LTR ) ),
        SY_OP( SY_OP_LTE,           "<=",       SY_ONE( 6,  SY_ASSOC_LTR ) ),
        SY_OP( SY_OP_GT,            ">",        SY_ONE( 6,  SY_ASSOC_LTR ) ),
        SY_OP( SY_OP_GTE,           ">=",       SY_ONE( 6,  SY_ASSOC_LTR ) ),
        SY_OP( SY_OP_EQUALS,        "==",       SY_ONE( 7,  SY_ASSOC_LTR ) ),
        SY_OP( SY_OP_BANG_EQUALS,   "!=",       SY_ONE( 7,  SY_ASSOC_LTR ) ),
        SY_OP( SY_OP_CARAT,         "^",        SY_ONE( 9,  SY_ASSOC_LTR ) ),
        SY_OP( SY_OP_PIPE,          "|",        SY_ONE( 10, SY_ASSOC_LTR ) ),
        SY_OP( SY_OP_AND,           "&&",       SY_ONE( 11, SY_ASSOC_LTR ) ),
        SY_OP( SY_OP_OR,            "||",       SY_ONE( 12, SY_ASSOC_LTR ) ),
        SY_OP( SY_OP_ASSIGN,        "=",        SY_ONE( 13, SY_ASSOC_RTL ) ),
        SY_OP( SY_OP_PLUS_ASSIGN,   "+=",       SY_ONE( 13, SY_ASSOC_RTL ) ),
        SY_OP( SY_OP_MINUS_ASSIGN,  "-=",       SY_ONE( 13, SY_ASSOC_RTL ) ),
        SY_OP( SY_OP_STAR_ASSIGN,   "*=",       SY_ONE( 13, SY_ASSOC_RTL ) ),
        SY_OP( SY_OP_DIV_ASSIGN,    "/=",       SY_ONE( 13, SY_ASSOC_RTL ) ),
        SY_OP( SY_OP_MOD_ASSIGN,    "%=",       SY_ONE( 13, SY_ASSOC_RTL ) ),
        SY_OP( SY_OP_LSHIFT_ASSIGN, "<<=",      SY_ONE( 13, SY_ASSOC_RTL ) ),
        SY_OP( SY_OP_RSHIFT_ASSIGN, ">>=",      SY_ONE( 13, SY_ASSOC_RTL ) ),
        SY_OP( SY_OP_AMP_ASSIGN,    "&=",       SY_ONE( 13, SY_ASSOC_RTL ) ),
        SY_OP( SY_OP_CARAT_ASSIGN,  "^=",       SY_ONE( 13, SY_ASSOC_RTL ) ),
        SY_OP( SY_OP_PIPE_ASSIGN,   "|=",       SY_ONE( 13, SY_ASSOC_RTL ) ),
        SY_OP( SY_OP_COMMA,         ",",        SY_ONE( 13, SY_ASSOC_RTL ) ),
        SY_OP( SY_OP_SEMICOLON,     ";",        SY_NONE( ) ),
        SY_OP( SY_OP_COLON,         ":",        SY_NONE( ) ),
        SY_OP( SY_OP_CBRACE_OPEN,   "{",        SY_NONE( ) ),
        SY_OP( SY_OP_CBRACE_CLOSE,  "}",        SY_NONE( ) ),
        SY_OP( SY_OP_PAREN_OPEN,    "(",        SY_NONE( ) ),
        SY_OP( SY_OP_PAREN_CLOSE,   ")",        SY_NONE( ) ),
        SY_OP( SY_OP_SBRACE_OPEN,   "[",        SY_NONE( ) ),
        SY_OP( SY_OP_SBRACE_CLOSE,  "]",        SY_NONE( ) ),
        SY_OP( SY_OP_NULL,          NULL,       SY_NONE( ) ),
};

typedef struct token{
    const char * data;
    size_t length;
    size_t tok_len;
    int prefix;
    sy_type_t type;
    sy_op_t op_ord;
    size_t args;
} tok_t;

void print_tok( tok_t * tok ){
    const char * name;
    switch( tok->type ){
        case SY_TYPE_CHR: name = "CHAR"; break;
        case SY_TYPE_ID: name = "ID  "; break;
        case SY_TYPE_NUM: name = "NUM "; break;
        case SY_TYPE_OPER: name = "OPER"; break;
        case SY_TYPE_STR: name = "STR "; break;
        case SY_TYPE_FUNC: name = "FUNC"; break;

        default: name = "NONE"; break;
    }
    printf("%s ", name);
    size_t i;
    for( i = 0; i < tok->tok_len; ++i ){
        putchar(tok->data[i]);
    }
    if( tok->type == SY_TYPE_FUNC ){
        printf("(%zu)", tok->args);
    }
    putchar('\n');
}
void print_tok2( tok_t * tok ){
    size_t i;
    if( tok->type == SY_TYPE_OPER && tok->op_ord == SY_OP_INDEX){
        putchar('[');
        putchar(']');
    }
    else{
        for( i = 0; i < tok->tok_len; ++i ){
            putchar(tok->data[i]);
        }
    }
    putchar(' ');
}

int isidchar(int value){
    return isalnum(value) || value == '$' || value == '_';
}
int isnumchar(int value){
    return isalnum(value) || value == '.';
}

tok_t sy_prep_token( const char * data, size_t len ){
    tok_t ret;
    ret.data = data;
    ret.length = len;
    ret.tok_len = 0;
    ret.type = SY_TYPE_NONE;
    ret.op_ord = SY_OP_NULL;
    ret.prefix = 0;
    ret.args = 0;
    return ret;
}

int sy_read_token( tok_t * tok ){
    const struct tok_equiv * list = token_list;
    const char * match = NULL;

    tok->data += tok->tok_len;
    while( tok->length > 0 && isblank( tok->data[0] ) ){
        tok->data++;
        tok->length--;
    }
    if( tok->length == 0 ){
        return 0;
    }

    const char * string = tok->data;
    size_t length = tok->length;
    tok->type = SY_TYPE_NONE;
    size_t match_len = 0;

    while( list->op != SY_OP_NULL ){
        const char * tok_name = list->name;
        size_t cmp_len = 0;
        while( tok_name && cmp_len < length && tok_name[cmp_len] ){
            if( tok_name[cmp_len] != string[cmp_len] ){
                break;
            }
            cmp_len++;
        }
        if( tok_name && tok_name[cmp_len] == 0 && cmp_len > match_len ){
            match = tok_name;
            match_len = cmp_len;
            tok->type = SY_TYPE_OPER;
            tok->op_ord = list->op;
        }
        ++list;
    }
    if( match_len == 0 || (match[0] == '.' && isdigit(string[1])) ){
        int escaped = 0;
        if( isdigit(*string) || *string == '.' ){
            tok->type = SY_TYPE_NUM;
            while( match_len < length && isnumchar( string[match_len] ) ){
                match_len++;
            }
        }
        else if( *string == '"' ){
            tok->type = SY_TYPE_STR;
            while( match_len < length && (match_len == 0 || escaped || string[match_len] != '"') ){
                escaped = 0;
                if( string[match_len] == '\\' ){
                    escaped = 1;
                }
                match_len ++;
            }
            match_len ++;
        }
        else if( *string == '\'' ){
            tok->type = SY_TYPE_CHR;
            while( match_len < length && ( match_len == 0 || escaped || string[match_len] != '\'') ){
                escaped = 0;
                if( string[match_len] == '\\' ){
                    escaped = 1;
                }
                match_len ++;
            }
            match_len ++;
        }
        else{
            tok->type = SY_TYPE_ID;
            while( match_len < length && isidchar( string[match_len] ) ){
                match_len++;
            }
        }
    }
    tok->tok_len = match_len;
    return match_len > 0;
}

#include <stdlib.h>
#define PUSH(base,ptr) do{ size_t new_len = (ptr) - (base) + 1; (base) = realloc((base), (new_len + 1) * sizeof(*(base))); (ptr) = (base) + new_len; } while(0)
#define POP(base,ptr) ((ptr)--)



void pop_to( tok_t ** out_base, tok_t ** out, tok_t ** ops_base, tok_t ** ops, sy_op_t ord ){
    while( *ops != *ops_base && (*ops)[-1].op_ord != ord ){
        POP(*ops_base, *ops);
        **out = **ops;
        PUSH(*out_base, *out);
    }
}


//Precedence rules are yucky
void get_precedence( tok_t * val, int * precedence, assoc_t * assoc ){
    if( val->type != SY_TYPE_OPER ){
        *assoc = SY_ASSOC_NONE;
        return;
    }
    if( val->prefix ){
        *precedence = token_list[ val->op_ord ].prec_prefix;
        *assoc = token_list[ val->op_ord ].assoc_prefix;
    }
    else{
        *precedence = token_list[ val->op_ord ].prec_suffix;
        *assoc = token_list[ val->op_ord ].assoc_suffix;
    }
}

void push_op( tok_t ** out_base, tok_t ** out, tok_t ** ops_base, tok_t ** ops, tok_t * val, int prefix ){
    val->prefix = prefix;
    int p1 = 100000;
    assoc_t a1 = SY_ASSOC_LTR;
    get_precedence( val, &p1, &a1 );
    while( *ops > *ops_base ){
        int p2 = 100000;
        assoc_t a2 = SY_ASSOC_LTR;
        get_precedence( (*ops)-1, &p2, &a2 );
        if( a2 != SY_ASSOC_NONE && ((a1 == SY_ASSOC_LTR && p1 >= p2) || (a1 == SY_ASSOC_RTL && p1 > p2))){
            POP( *ops_base, *ops );
            **out = **ops;
            PUSH(*out_base, *out);
        }
        else{
            break;
        }
    }
    **ops = *val;
    PUSH(*ops_base, *ops);
}
void increment_args( tok_t ** ops_base, tok_t ** ops ){
    tok_t * i;
    for( i = *ops; i > *ops_base + 1; --i ){
        if( i[-1].type == SY_TYPE_OPER && i[-1].op_ord == SY_OP_PAREN_OPEN ){
            if( i[-2].type == SY_TYPE_FUNC ){
                i[-2].args++;
            }
            return;
        }
    }
}

tok_t * infix_to_postfix( const char * data ){
    size_t len = strlen(data);
    tok_t tok = sy_prep_token( data, len );
    tok_t * out_base, * out, * ops_base, * ops;
    out_base = malloc( sizeof( *out_base ) );
    ops_base = malloc( sizeof( *ops_base ) );
    out = out_base;
    ops = ops_base;
    int prefix = 1;
    int in_func = 0;

    while( sy_read_token( &tok ) ){
        if( in_func && (tok.type != SY_TYPE_OPER || tok.op_ord != SY_OP_PAREN_CLOSE )){
            increment_args( &ops_base, &ops );
            in_func = 0;
        }
        switch( tok.type ){
            case SY_TYPE_ID:
            case SY_TYPE_CHR:
            case SY_TYPE_STR:
            case SY_TYPE_NUM:
                prefix = 0;
                *out = tok;
                PUSH(out_base, out); break;
            case SY_TYPE_OPER:
                switch( tok.op_ord ){
                    case SY_OP_COMMA:
                        pop_to( &out_base, &out, &ops_base, &ops, SY_OP_PAREN_OPEN );
                        increment_args( &ops_base, &ops );
                        prefix = 1;
                        break;
                    case SY_OP_PAREN_OPEN:
                        if( out - out_base != 0 ){
                            if( out[-1].type == SY_TYPE_ID ){
                                POP(out_base, out);
                                *ops = *out;
                                ops->type = SY_TYPE_FUNC;
                                PUSH(ops_base, ops);
                                prefix = 0;
                                in_func = 1;
                            }
                        }
                    case SY_OP_SBRACE_OPEN:
                        *ops = tok;
                        ops->prefix = 0;
                        prefix = 1;
                        PUSH(ops_base, ops);
                        break;
                    case SY_OP_SBRACE_CLOSE:
                        prefix = 0;
                        pop_to( &out_base, &out, &ops_base, &ops, SY_OP_SBRACE_OPEN );
                        POP(ops_base, ops);
                        *out = tok;
                        out->op_ord = SY_OP_INDEX;
                        PUSH(out_base, out);
                        break;
                    case SY_OP_PAREN_CLOSE:
                        prefix = 0;
                        pop_to( &out_base, &out, &ops_base, &ops, SY_OP_PAREN_OPEN );
                        POP(ops_base, ops);
                        if( ops - ops_base != 0 ){
                            if( ops[-1].type == SY_TYPE_FUNC ){
                                POP(ops_base, ops);
                                *out = *ops;
                                PUSH(out_base, out);
                            }
                        }
                        break;

                    default:
                        push_op( &out_base, &out, &ops_base, &ops, &tok, prefix );
                        prefix = 1;
                    break;
                }
                break;
            default: break;
        }
    }
    while( ops - ops_base > 0 ){
        POP( ops_base, ops );
        *out = *ops;
        PUSH( out_base, out );
    }
    out->type = SY_TYPE_NONE;
    free( ops_base );
    return out_base;
}

int main(){
    const char * data = "average(foo.level(\"Isaac\", \"osrs\", 20), foo.level(\"Isaac\", \"osrs\", 0))";
    //const char * data = "sizeof(0)[\"123456789\"]";
    tok_t * postfix = infix_to_postfix( data );
    tok_t * iter = postfix;
    printf("Input: %s\nOutput: ", data);
    while( iter->type != SY_TYPE_NONE ){
        print_tok2( iter++ );
    }
    printf("\n\n");
    free( postfix );
    return 0;
}
