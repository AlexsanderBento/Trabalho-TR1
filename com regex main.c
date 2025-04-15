#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#define MAX_TOKENS 300

typedef enum {
    TOK_RESERVADA,
    TOK_IDENTIFICADOR,
    TOK_INTEIRO,
    TOK_FLOAT,
    TOK_COMENTARIO,
    TOK_PONTUACAO
} TokenType;

const char* tipoToString(TokenType tipo) {
    switch (tipo) {
        case TOK_RESERVADA: return "reservada";
        case TOK_IDENTIFICADOR: return "identificador";
        case TOK_INTEIRO: return "inteiro";
        case TOK_FLOAT: return "float";
        case TOK_COMENTARIO: return "comentário";
        case TOK_PONTUACAO: return "pontuação";
        default: return "desconhecido";
    }
}

const char* palavrasReservadas[] = {
    "int", "double", "char", "float", "if", "while", "for"
};

const char* operadoresCompostos[] = {
    "==", "!=", "<=", ">=", "++", "--", "+=", "-=", "*=", "/="
};

typedef struct {
    char valor[100];
    TokenType tipo;
} Token;

int ehPalavraReservada(const char* palavra) {
    for (int i = 0; i < sizeof(palavrasReservadas) / sizeof(palavrasReservadas[0]); i++) {
        if (strcmp(palavra, palavrasReservadas[i]) == 0)
            return 1;
    }
    return 0;
}

int ehOperadorComposto(const char* s) {
    for (int i = 0; i < sizeof(operadoresCompostos) / sizeof(operadoresCompostos[0]); i++) {
        if (strcmp(s, operadoresCompostos[i]) == 0)
            return 1;
    }
    return 0;
}

int casaRegex(const char* palavra, const char* padrao) {
    regex_t regex;
    int resultado;

    regcomp(&regex, padrao, REG_EXTENDED);
    resultado = regexec(&regex, palavra, 0, NULL, 0);
    regfree(&regex);

    return resultado == 0;
}

int tokenJaExiste(Token* tokens, int count, const char* valor) {
    for (int i = 0; i < count; i++) {
        if (strcmp(tokens[i].valor, valor) == 0)
            return 1;
    }
    return 0;
}

TokenType classificar(const char* palavra) {
    if (ehPalavraReservada(palavra)) return TOK_RESERVADA;
    if (casaRegex(palavra, "^[A-Z]+$")) return TOK_IDENTIFICADOR;
    if (casaRegex(palavra, "^[0-9]+$")) return TOK_INTEIRO;
    if (casaRegex(palavra, "^[0-9]+,[0-9]+$")) return TOK_FLOAT;
    if (ehOperadorComposto(palavra)) return TOK_PONTUACAO;
    if (strlen(palavra) == 1 && strchr("();=<>+-*/{}", palavra[0])) return TOK_PONTUACAO;

    return -1;
}

void quebrarTokens(char* linha, Token* tokens, int* count) {
    char* comentarioInicio = strchr(linha, '#');
    char linhaSemComentario[256] = "";

    if (comentarioInicio) {
        int pos = comentarioInicio - linha;
        strncpy(linhaSemComentario, linha, pos);
        linhaSemComentario[pos] = '\0';

        char comentario[256];
        strcpy(comentario, comentarioInicio);
        if (!tokenJaExiste(tokens, *count, comentario)) {
            strcpy(tokens[*count].valor, comentario);
            tokens[*count].tipo = TOK_COMENTARIO;
            (*count)++;
        }
    } else {
        strcpy(linhaSemComentario, linha);
    }

    int i = 0;
    while (linhaSemComentario[i]) {
        if (linhaSemComentario[i + 1]) {
            char dois[3] = {linhaSemComentario[i], linhaSemComentario[i + 1], '\0'};
            if (ehOperadorComposto(dois)) {
                if (!tokenJaExiste(tokens, *count, dois)) {
                    strcpy(tokens[*count].valor, dois);
                    tokens[*count].tipo = TOK_PONTUACAO;
                    (*count)++;
                }
                i += 2;
                continue;
            }
        }

        if (strchr("();=<>+-*/{}", linhaSemComentario[i])) {
            char pont[2] = {linhaSemComentario[i], '\0'};
            if (!tokenJaExiste(tokens, *count, pont)) {
                strcpy(tokens[*count].valor, pont);
                tokens[*count].tipo = TOK_PONTUACAO;
                (*count)++;
            }
            i++;
            continue;
        }

        char temp[100] = "";
        int j = 0;
        while (linhaSemComentario[i] && !strchr(" ();=<>+-*/{}\n\t", linhaSemComentario[i])) {
            temp[j++] = linhaSemComentario[i++];
        }
        temp[j] = '\0';

        if (strlen(temp) > 0) {
            TokenType tipo = classificar(temp);
            if (tipo != -1 && !tokenJaExiste(tokens, *count, temp)) {
                strcpy(tokens[*count].valor, temp);
                tokens[*count].tipo = tipo;
                (*count)++;
            }
        }

        if (linhaSemComentario[i] && strchr(" \t\n", linhaSemComentario[i])) i++;
    }
}

int main() {
    char linha[256];
    Token tokens[MAX_TOKENS];
    int tokenCount = 0;

    printf("Digite o código (Ctrl+D para encerrar):\n");

    while (fgets(linha, sizeof(linha), stdin)) {
        quebrarTokens(linha, tokens, &tokenCount);
    }

    printf("\nentrada\t\tvalor\t\t\ttipo\n");
    for (int i = 0; i < tokenCount; i++) {
        printf("%d\t\t%-20s\t%s\n", i + 1, tokens[i].valor, tipoToString(tokens[i].tipo));
    }

    return 0;
}
