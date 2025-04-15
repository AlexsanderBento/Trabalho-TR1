#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOKENS 1000
#define MAX_LENGTH 100

typedef enum {
    TOK_RESERVADA,
    TOK_IDENTIFICADOR,
    TOK_NUMERO_INT,
    TOK_NUMERO_FLOAT,
    TOK_COMENTARIO,
    TOK_PONTUACAO,
    TOK_DESCONHECIDO
} TokenType;

typedef struct {
    char valor[MAX_LENGTH];
    TokenType tipo;
} Token;

const char* palavrasReservadas[] = {
    "int", "double", "char", "float", "if", "while", "for"
};

int isReservada(const char* palavra) {
    for (int i = 0; i < 7; i++) {
        if (strcmp(palavra, palavrasReservadas[i]) == 0)
            return 1;
    }
    return 0;
}

int isLetra(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

int isLetraMaiuscula(char c) {
    return (c >= 'A' && c <= 'Z');
}

int isDigito(char c) {
    return c >= '0' && c <= '9';
}

int isEspaco(char c) {
    return c == ' ' || c == '\n' || c == '\t';
}

int isPontuacao(char c) {
    return c == ';' || c == ',' || c == '(' || c == ')' || c == '<' || c == '>' || c == '=' || c == '{' || c == '}';
}

void imprimirTipo(TokenType tipo) {
    switch (tipo) {
        case TOK_RESERVADA: printf("reservada"); break;
        case TOK_IDENTIFICADOR: printf("identificador"); break;
        case TOK_NUMERO_INT: printf("inteiro"); break;
        case TOK_NUMERO_FLOAT: printf("float"); break;
        case TOK_COMENTARIO: printf("comentário"); break;
        case TOK_PONTUACAO: printf("pontuação"); break;
        default: printf("desconhecido");
    }
}

int tokenJaExiste(Token* tokens, int total, const char* valor, TokenType tipo) {
    for (int i = 0; i < total; i++) {
        if (strcmp(tokens[i].valor, valor) == 0 && tokens[i].tipo == tipo) {
            return 1;
        }
    }
    return 0;
}

int main() {
    Token tokens[MAX_TOKENS];
    int tokenCount = 0;
    char c, buffer[MAX_LENGTH];
    int index = 0;

    while ((c = getchar()) != EOF) {
        if (isEspaco(c)) {
            continue;
        }

        if (c == '#') {
            buffer[0] = c;
            index = 1;
            while ((c = getchar()) != '\n' && c != EOF) {
                buffer[index++] = c;
            }
            buffer[index] = '\0';
            if (!tokenJaExiste(tokens, tokenCount, buffer, TOK_COMENTARIO)) {
                strcpy(tokens[tokenCount].valor, buffer);
                tokens[tokenCount++].tipo = TOK_COMENTARIO;
            }
            continue;
        }

        if (isLetra(c)) {
            buffer[0] = c;
            index = 1;
            while ((c = getchar()) != EOF && isLetra(c)) {
                buffer[index++] = c;
            }
            buffer[index] = '\0';

            if (!isEspaco(c) && c != EOF)
                ungetc(c, stdin);

            TokenType tipo;
            if (isReservada(buffer)) {
                tipo = TOK_RESERVADA;
            } else if (isLetraMaiuscula(buffer[0])) {
                tipo = TOK_IDENTIFICADOR;
            } else {
                tipo = TOK_DESCONHECIDO;
            }

            if (!tokenJaExiste(tokens, tokenCount, buffer, tipo)) {
                strcpy(tokens[tokenCount].valor, buffer);
                tokens[tokenCount++].tipo = tipo;
            }
            continue;
        }

        if (isDigito(c)) {
            buffer[0] = c;
            index = 1;
            int isFloat = 0;
            while ((c = getchar()) != EOF && (isDigito(c) || c == ',')) {
                if (c == ',') {
                    if (isFloat) break;
                    isFloat = 1;
                }
                buffer[index++] = c;
            }
            buffer[index] = '\0';

            if (!isEspaco(c) && c != EOF)
                ungetc(c, stdin);

            TokenType tipo = isFloat ? TOK_NUMERO_FLOAT : TOK_NUMERO_INT;

            if (!tokenJaExiste(tokens, tokenCount, buffer, tipo)) {
                strcpy(tokens[tokenCount].valor, buffer);
                tokens[tokenCount++].tipo = tipo;
            }
            continue;
        }

        if (isPontuacao(c)) {
            buffer[0] = c;
            buffer[1] = '\0';
            if (!tokenJaExiste(tokens, tokenCount, buffer, TOK_PONTUACAO)) {
                strcpy(tokens[tokenCount].valor, buffer);
                tokens[tokenCount++].tipo = TOK_PONTUACAO;
            }
            continue;
        }

        buffer[0] = c;
        buffer[1] = '\0';
        if (!tokenJaExiste(tokens, tokenCount, buffer, TOK_DESCONHECIDO)) {
            strcpy(tokens[tokenCount].valor, buffer);
            tokens[tokenCount++].tipo = TOK_DESCONHECIDO;
        }
    }

    printf("entrada\t\tvalor\t\ttipo\n");
    for (int i = 0; i < tokenCount; i++) {
        printf("%d\t\t%s\t\t", i + 1, tokens[i].valor);
        imprimirTipo(tokens[i].tipo);
        printf("\n");
    }

    return 0;
}
