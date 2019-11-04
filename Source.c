#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include<locale.h>

#define TAM 101
#define TOKN_Menor 1
#define TOKN_Maior 2
#define TOKN_MenorIgual 3
#define TOKN_MaiorIgual 4
#define TOKN_Igualdade 5 //comparação
#define TOKN_Diferente 6
#define TOKN_Adicao 7
#define TOKN_Subtracao 8
#define TOKN_Multiplicacao 9
#define TOKN_Divisao 10
#define TOKN_Igual 11 //atribuição
#define TOKN_FechaPar 12
#define TOKN_AbrePar 13
#define TOKN_AbreChav 14
#define TOKN_FechaChav 15
#define TOKN_Virgula 16
#define TOKN_PontoeVirgula 17
#define TOKN_Main 18
#define TOKN_if 19
#define TOKN_else 20
#define TOKN_while 21
#define TOKN_do 22 
#define TOKN_for 23
#define TOKN_int 24 //valor inteiro
#define TOKN_Tipoint 25 //Palavra reservada
#define TOKN_float 26 //valor real
#define TOKN_Tipofloat 27 //Palavra reservada
#define TOKN_char 28 //valor caractere
#define TOKN_Tipochar 29 //Palavra reservada
#define TOKN_ID 30
#define TOKNFim_de_arq 31

typedef struct Lex{
	char name[TAM];
	int id;
}TLex;

typedef struct simbolo{
	char name[TAM];
	int id;
	int escopo;
	struct simbolo *prox;
}TSimbolo;

typedef struct simboloAuxiliar{
	TSimbolo s;
}TSimboloAux; //SIMBOLO RETORNA PONTEIRO SIMBOLOAUXILIAR RETORNA A STRUCT

TLex Scan(FILE *arq);
void parser();
void bloco(FILE *arq);
void StackVariavel(char name[], int tipo, int escop);
int CheckVariavelMesmoEscopo(char name[], int tipo, int escop); //na exp_reg usada para checar se uma variável já foi declarada no mesmo escopo;
TSimboloAux CheckTodasVariaveis(TLex l, FILE *arq); //usada para checar se a variável foi declarada independente do escopo;
TSimboloAux CheckCompatibilidade(TSimboloAux aux1, TSimboloAux aux2, TSimboloAux op, FILE *arq);
void UnstackVariavel(int escopo);
void decl_var(FILE *arq);
void comandoIf(FILE *arq);
void comando(FILE *arq);
void comandoBasico(FILE *arq);
void atribuicao(FILE *arq);
TSimboloAux expr_arit(FILE *arq);
TSimboloAux termo(FILE *arq);
TSimboloAux fator(FILE *arq);
void iteracao(FILE *arq);
TSimboloAux exp_relac(FILE *arq);
TSimboloAux op_relac(FILE *arq);

TLex lexema;
int linha = 1;
int coluna = 0;
int escopo = 0;
TSimbolo *tabela = NULL;
int label = 0;
int t = 0;

int main(int argc, char *argv[]) {
	FILE *arq;
	arq = fopen(argv[0], "r");
	if (arq == NULL){
		printf("Erro na leitura do arquivo \n");
	}
	else{
		parser(arq);
	}
	return 0;
}

TLex Scan(FILE *arq) {
	TLex lex;
	static char c = ' ';
	int i;
	int flag_comentario = 1;
	int flag_aspS = 1;
	setlocale(LC_ALL, "portuguese");
	inicio:
	while (isspace(c)){
		if (c == '\n') {
			coluna = 1;
			linha++;
			c = fgetc(arq);
		}
		else if (c == '\t') {
			coluna = coluna + 4;
			c = fgetc(arq);
		}
		else if (c == ' ') {
			coluna++;
			c = fgetc(arq);
		}
	}

	i = 0;
	if (feof(arq)) {
		lex.id = TOKNFim_de_arq;
		lex.name[i] = '\0';
		return lex;
	}
	else if (isalpha(c) || c == '_') {
		lex.name[i] = c;
		i++;
		coluna++;
		c = fgetc(arq);
		while (isalpha(c) || c == '_') {
			lex.name[i] = c;
			i++;
			coluna++;
			c = fgetc(arq);
		}
		lex.name[i] = '\0';
		if (strcmp(lex.name, "if") == 0){
			lex.id = TOKN_if;
		}
		else if (strcmp(lex.name, "else") == 0){
			lex.id = TOKN_else;
		}
		else if (strcmp(lex.name, "for") == 0){
			lex.id = TOKN_for;
		}
		else if (strcmp(lex.name, "while") == 0){
			lex.id = TOKN_while;
		}
		else if (strcmp(lex.name, "do") == 0){
			lex.id = TOKN_do;
		}
		else if (strcmp(lex.name, "main") == 0){
			lex.id = TOKN_Main;
		}
		else if (strcmp(lex.name, "int") == 0){
			lex.id = TOKN_Tipoint;
		}
		else if (strcmp(lex.name, "float") == 0){
			lex.id = TOKN_Tipofloat;
		}
		else if (strcmp(lex.name, "char") == 0){
			lex.id = TOKN_Tipochar;
		}
		else{
			lex.id = TOKN_ID;
		}
		return lex;
	}
	else if (isdigit(c)){
		lex.name[i] = c;
		i++;
		coluna++;
		c = fgetc(arq);
		while (isdigit(c)) {
			lex.name[i] = c;
			i++;
			coluna++;
			c = fgetc(arq);
		}
		if (c == '.') {
			lex.id = TOKN_float;
			lex.name[i] = c;
			i++;
			c = fgetc(arq);
			coluna++;
			if (isdigit(c)) {
				lex.name[i] = c;
				i++;
				c = fgetc(arq);
				coluna++;
				while (isdigit(c)) {
					lex.name[i] = c;
					i++;
					c = fgetc(arq);
					coluna++;
				}
				lex.name[i] = '\0';
				return lex;
			}
			else if (feof(arq)){
				lex.name[i] = '\0';
				printf("ERRO na linha %i, coluna %i, ultimo token lido %s: Erro no float. Obrigatório apenas número após o ponto\n", linha, coluna, lex.name);
				fclose(arq);
				exit(0);
			}
			else {
				lex.name[i] = c;
				lex.name[i + 1] = '\0';
				printf("ERRO na linha %i, coluna %i, ultimo token lido %s: Erro no float. Obrigatório apenas número após o ponto\n", linha, coluna, lex.name);
				fclose(arq);
				exit(0);
			}
		}
		else if (isalnum(c) && c != '.' && c != ';'){
			printf("ERRO na linha %i, coluna %i, ultimo token lido %s: Caractere alfanumérico posto numa sequencia de numeros\n", linha, coluna, lex.name);
			fclose(arq);
			exit(0);
		}
		else {
			lex.id = TOKN_int;
			lex.name[i] = '\0';
			return lex;
		}
	}
	else if (c == '.') {
		lex.name[i] = c;
		i++;
		c = fgetc(arq);
		lex.id = TOKN_float;
		coluna++;
		if (isdigit(c)) {
			lex.name[i] = c;
			i++;
			c = fgetc(arq);
			coluna++;
			while (isdigit(c)) {
				lex.name[i] = c;
				i++;
				c = fgetc(arq);
				coluna++;
			}
			lex.name[i] = '\0';
			return lex;
		}
		else if (feof(arq)){
			lex.name[i] = '\0';
			printf("ERRO na linha %i, coluna %i, ultimo token lido %s: Erro no float. Obrigatório apenas número após o ponto isolado\n", linha, coluna, lex.name);
			fclose(arq);
			exit(0);
		}
		else {
			lex.name[i] = c;
			lex.name[i + 1] = '\0';
			printf("ERRO na linha %i, coluna %i, ultimo token lido %s: Erro no float. Obrigatório apenas número após o ponto\n", linha, coluna, lex.name);
			fclose(arq);
			exit(0);
		}
	}
	else if (c == '<') {
		lex.name[i] = c;
		i++;
		coluna++;
		c = fgetc(arq);
		lex.id = TOKN_Menor;
		if (c == '=') {
			lex.name[i] = c;
			i++;
			coluna++;
			c = fgetc(arq);
			lex.id = TOKN_MenorIgual;
		}
		lex.name[i] = '\0';
		return lex;
	}
	else if (c == '>') {
		lex.name[i] = c;
		i++;
		coluna++;
		c = fgetc(arq);
		lex.id = TOKN_Maior;
		if (c == '=') {
			lex.name[i] = c;
			i++;
			coluna++;
			c = fgetc(arq);
			lex.id = TOKN_MaiorIgual;
		}
		lex.name[i] = '\0';
		return lex;
	}

	else if (c == '=') {
		lex.name[i] = c;
		i++;
		coluna++;
		c = fgetc(arq);
		lex.id = TOKN_Igual;
		if (c == '='){
			lex.name[i] = c;
			i++;
			coluna++;
			c = fgetc(arq);
			lex.id = TOKN_Igualdade;
		}
		lex.name[i] = '\0';
		return lex;
	}
	else if (c == '!') {
		lex.name[i] = c;
		i++;
		coluna++;
		c = fgetc(arq);
		lex.id = TOKN_Diferente;
		if (c == '=') {
			lex.name[i] = c;
			i++;
			coluna++;
			c = fgetc(arq);
			lex.name[i] = '\0';
			return lex;
		}
		else {
			lex.name[i] = c;
			lex.name[i + 1] = '\0';
			printf("ERRO na linha %i, coluna %i, ultimo token lido %s: Erro na exclamação, após ela é necessário o símbolo da igualdade \n", linha, coluna, lex.name);
			fclose(arq);
			exit(0);
		}
	}
	else if (c == '+') {
		lex.name[i] = c;
		i++;
		coluna++;
		lex.name[i] = '\0';
		c = fgetc(arq);
		lex.id = TOKN_Adicao;
		return lex;
	}
	else if (c == '-') {
		lex.name[i] = c;
		i++;
		coluna++;
		lex.name[i] = '\0';
		c = fgetc(arq);
		lex.id = TOKN_Subtracao;
		return lex;
	}
	else if (c == '*') {
		lex.name[i] = c;
		i++;
		coluna++;
		lex.name[i] = '\0';
		c = fgetc(arq);
		lex.id = TOKN_Multiplicacao;
		return lex;
	}
	else if (c == '(') {
		lex.name[i] = c;
		i++;
		coluna++;
		lex.name[i] = '\0';
		c = fgetc(arq);
		lex.id = TOKN_AbrePar;
		return lex;
	}
	else if (c == ')') {
		lex.name[i] = c;
		i++;
		coluna++;
		lex.name[i] = '\0';
		c = fgetc(arq);
		lex.id = TOKN_FechaPar;
		return lex;
	}
	else if (c == '{') {
		lex.name[i] = c;
		i++;
		coluna++;
		lex.name[i] = '\0';
		c = fgetc(arq);
		lex.id = TOKN_AbreChav;
		return lex;
	}
	else if (c == '}') {
		lex.name[i] = c;
		i++;
		coluna++;
		lex.name[i] = '\0';
		c = fgetc(arq);
		lex.id = TOKN_FechaChav;
		return lex;
	}
	else if (c == ',') {
		lex.name[i] = c;
		i++;
		coluna++;
		lex.name[i] = '\0';
		c = fgetc(arq);
		lex.id = TOKN_Virgula;
		return lex;
	}
	else if (c == ';') {
		lex.name[i] = c;
		i++;
		coluna++;
		lex.name[i] = '\0';
		c = fgetc(arq);
		lex.id = TOKN_PontoeVirgula;
		return lex;
	}
	else if (c == 39) { //39 tabela ASCII equivale a Aspas Simples
		while (flag_aspS == 1){
			c = fgetc(arq);
			coluna++;
			if (isalnum(c)) {
				lex.name[i] = c;
				i++;
				c = fgetc(arq);
				coluna++;
				if (c == 39) { //FECHAMENTO DA ASPAS SIMPLES
					lex.name[i] = '\0';
					lex.id = TOKN_char;
					i++;
					coluna++;
					c = fgetc(arq);
					return lex;
				}
				else {
					lex.name[i] = c;
					i++;
				}
			}
			else if (feof(arq)){
				lex.name[i] = '\0';
				printf("ERRO na linha %i, coluna %i, ultimo token lido %s: tipo Char com mal formato. Fim de arquivo sem fechar aspas\n", linha, coluna, lex.name); //CASO O CARACTERE NÃO FOR ALFA NUMÉRICO
				flag_aspS = 0;
				fclose(arq);
				exit(0);
			}
			else{
				lex.name[i] = '\0';
				printf("ERRO na linha %i, coluna %i, ultimo token lido %s: tipo Char com mal formato. Deve vir apenas 1 constante ou apenas 1 dígito após as aspas!\n", linha, coluna, lex.name); //CASO O CARACTERE NÃO FOR ALFA NUMÉRICO
				flag_aspS = 0;
				fclose(arq);
				exit(0);
			}
		}
	}
	else if (c == '/') {
		lex.name[i] = c;
		i++;
		coluna++;
		c = fgetc(arq);
		if (c == '/') { //próximo caractere foi a barra
			lex.name[i - 1] = '\0'; // i-1 como se eu excluisse o fato de existir o '//' e acabasse por ai
			coluna++;
			c = fgetc(arq);
			while (c != '\n' && !feof(arq)) { //programa percorre todos os strings até encontrar o '\n' e não for fim de arquivo
				if (c == '\t'){
					coluna = coluna + 4;
					c = fgetc(arq);
				}
				else{
					coluna++;
					c = fgetc(arq);
				}
			}
			if (feof(arq)) { //fim do arquivo 
				printf("ERRO na linha %i, coluna %i, ultimo token lido: %c Fim de arquivo no meio do comentário Simples\n", linha, coluna, c);
				fclose(arq);
				exit(0);
			}
			if (c == '\n') {
				i = 0;
				coluna = 1;
				linha++;
				c = fgetc(arq);
				goto inicio;
			}
			if (c == '\t') { //ja incrementei a coluna previamente
				coluna = coluna + 3;
				c = fgetc(arq);
				goto inicio;
			}
		}
		else if (c == '*'){ //início de um comentário de multiplas linhas
			c = fgetc(arq);
			coluna++;
			while (flag_comentario == 1){
				while (c != '*' && !feof(arq)){
					coluna++;
					c = fgetc(arq);
					if (c == '\n') {
						linha++;
						coluna = 0;
						c = fgetc(arq);
					}
					if (c == '\t') {
						coluna = coluna + 3;
						c = fgetc(arq);
						if (c == '\n') {
							linha++;
							coluna = 0;
							c = fgetc(arq);
						}
					}
				}
				if (feof(arq)) {
					printf("ERRO na linha %i, coluna %i, ultimo token lido: %c Fim de arquivo no meio do comentário Multiplas Linhas\n", linha, coluna, c);
					fclose(arq);
					exit(0);
				}
				else if (c == '*') { //encontro do asteristico que fecha o comentário
					coluna++;
					c = fgetc(arq);
					if (c == '/' && !feof(arq)) { //encontro da barra que fecha o comentário
						c = fgetc(arq);
						coluna++;
						lex.name[i-1] = '\0';
						goto inicio;
					}
					else if (feof(arq) != 0) {
						printf("ERRO na linha %i, coluna %i, ultimo token lido: %c Fim de arquivo com mau fechamento do comentário de Multiplas Linhas \n", linha, coluna, c);
						fclose(arq);
						exit(0);
					}
					else if (c == '\n'){
						linha++;
						coluna = -1;
					}
					else { //não encontrou a barra
						c = fgetc(arq);
						coluna++;
					}
				}
			}
		}
		else { // barra simples = divisão
			lex.name[i] = '\0';
			i++;
			lex.id = TOKN_Divisao;
			return lex;
		}
	}
	else { //caso não entre em nenhuma condição dessas, eh pq não eh um caractere válido
		coluna++;
		printf("ERRO na linha %i, coluna %i, ultimo token lido %c: Caracter inválido\n\n", linha, coluna, c);
		fclose(arq);
		exit(0);
	}
}

void parser(FILE *arq){ //int main"("")" <bloco>
	lexema = Scan(arq);
	if (lexema.id == TOKN_Tipoint){
		lexema = Scan(arq);
		if (lexema.id == TOKN_Main){ 
			lexema = Scan(arq);
			if (lexema.id == TOKN_AbrePar){
				lexema = Scan(arq);
				if (lexema.id == TOKN_FechaPar){	
					lexema = Scan(arq);
					bloco(arq);
					if (lexema.id == TOKNFim_de_arq){
						fclose(arq);
						exit(0);
					}
					else{
						printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Após o fechamento do programa principal não deve ter código(s)!!\n\n", linha, coluna, lexema.name);
						fclose(arq);
						exit(0);
					}
				}
				else{
					printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Fecha Parenteses ) não preenchido!!\n\n", linha, coluna, lexema.name);
					fclose(arq);
					exit(0);
				}
			}
			else{
				printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Abre Parenteses ( não preenchido!!\n\n", linha, coluna, lexema.name);
				fclose(arq);
				exit(0);
			}
		}
		else{
			printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Programa principal (Main) não preenchido!!\n\n", linha, coluna, lexema.name);
			fclose(arq);
			exit(0);
		}
	}
	else{
		printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Início do programa com Int não preenchido!!\n\n", linha, coluna, lexema.name);
		fclose(arq);
		exit(0);
	}
}

void bloco(FILE *arq) { //“{“ {<decl_var>}* {<comando>}* “}”
	escopo++;
	if (lexema.id == TOKN_AbreChav){
		lexema = Scan(arq);
		if (lexema.id == TOKN_Tipoint || lexema.id == TOKN_Tipofloat || lexema.id == TOKN_Tipochar){
			while (lexema.id == TOKN_Tipoint || lexema.id == TOKN_Tipofloat || lexema.id == TOKN_Tipochar){
				decl_var(arq);
			}
		}
		if (lexema.id == TOKN_if || lexema.id == TOKN_ID || lexema.id == TOKN_AbreChav || lexema.id == TOKN_do || lexema.id == TOKN_while){
			while (lexema.id == TOKN_if || lexema.id == TOKN_ID || lexema.id == TOKN_AbreChav || lexema.id == TOKN_do || lexema.id == TOKN_while){
				comando(arq);
			}
		}
		if (lexema.id == TOKN_FechaChav){
			UnstackVariavel(escopo);
			escopo--;
			lexema = Scan(arq);
		}
		else{
			printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Ausência de '}', declaração variáveis ou IF, DO, WHILE!!\n\n", linha, coluna, lexema.name);
			fclose(arq);
			exit(0);
		}
	}
	else{
		printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Ausência de '{'!!\n\n", linha, coluna, lexema.name);
		fclose(arq);
		exit(0);
	}
}

void StackVariavel(char name[], int tipo, int escop){
	TSimbolo *aux;
	aux = (TSimbolo*)malloc(sizeof(TSimbolo));
	strcpy(aux->name, name);
	aux->id = tipo;
	aux->escopo = escop;
	aux->prox = tabela;
	tabela = aux;
}

int CheckVariavelMesmoEscopo(char name[], int tipo, int escop){ //O mesmo nome de variável não pode estar no mesmo escopo independentemente do tipo
	int flag = 0;
	TSimbolo *aux;
	aux = tabela;
	while (aux != NULL){
		if (aux->escopo == escopo){
			if (strcmp(aux->name, name) == 0){
				flag = 1; //false
				break;
			}
		}
		aux = aux->prox;
	}
	return flag;
}

TSimboloAux CheckTodasVariaveis(TLex l, FILE *arq){
	TSimbolo *aux;
	TSimboloAux simAux;
	aux = tabela;
	if (aux == NULL){
		printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Variável não declarada!!\n\n", linha, coluna, lexema.name);
		fclose(arq);
		exit(0);
	}
	else{
		while (aux != NULL){
			if (strcmp(aux->name, l.name) == 0){
				simAux.s.escopo = escopo;
				simAux.s.id = aux->id;
				strcpy(simAux.s.name, l.name);
				simAux.s.prox = NULL;
				return simAux;
			}
			aux = aux->prox;
			if (aux == NULL){
				printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Variável não declarada!!\n\n", linha, coluna, lexema.name);
				fclose(arq);
				exit(0);
			}
		}
	}
}

void UnstackVariavel(int escopo){
	TSimbolo *aux;
	while (tabela != NULL){
		aux = tabela;
		tabela = tabela->prox;
		if (aux->escopo == escopo){
			free(aux);
		}
		else{
			tabela = aux;
			break;
		}
	}
}

TSimboloAux CheckCompatibilidade(TSimboloAux aux1, TSimboloAux aux2, TSimboloAux op, FILE *arq){
	if (op.s.id == TOKN_Diferente || op.s.id == TOKN_Igualdade || op.s.id == TOKN_MenorIgual || op.s.id == TOKN_Menor || op.s.id == TOKN_MaiorIgual || op.s.id == TOKN_Maior){
		if (aux1.s.id == TOKN_int){
			if (aux2.s.id == TOKN_int || aux2.s.id == TOKN_float){
				return aux2;
			}
			else{
				printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Expressão relacional incompatível com Int %s Char!!\n\n", linha, coluna, lexema.name, op.s.name);
				fclose(arq);
				exit(0);
			}
		}
		else if (aux1.s.id == TOKN_float){
			if (aux2.s.id == TOKN_int || aux2.s.id == TOKN_float){
				return aux1;
			}
			else{
				printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Expressão relacional incompatível com Float %s Char!!\n\n", linha, coluna, lexema.name, op.s.name);
				fclose(arq);
				exit(0);
			}
		}
		else if (aux1.s.id == TOKN_char){
			if (aux2.s.id == TOKN_int){
				printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Expressão relacional incompatível com Char %s Int!!\n\n", linha, coluna, lexema.name, op.s.name);
				fclose(arq);
				exit(0);
			}
			else if (aux2.s.id == TOKN_float){
				printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Expressão relacional incompatível com Char %s float!!\n\n", linha, coluna, lexema.name, op.s.name);
				fclose(arq);
				exit(0);
			}
			else{
				return aux1;
			}
		}
	}
	else if (op.s.id == TOKN_Multiplicacao){
		if (aux1.s.id == TOKN_int){
			if (aux2.s.id == TOKN_int || aux2.s.id == TOKN_float){
				return aux2;
			}
			else{
				printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Multiplicação incompatível com Int %s Char!!\n\n", linha, coluna, lexema.name, op.s.name);
				fclose(arq);
				exit(0);
			}
		}
		else if (aux1.s.id == TOKN_float){
			if (aux2.s.id == TOKN_float || aux2.s.id == TOKN_int){
				return aux1;
			}
			else if (aux2.s.id == TOKN_char){
				printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Multiplicação incompatível com Float %s Char!!\n\n", linha, coluna, lexema.name, op.s.name);
				fclose(arq);
				exit(0);
			}
		}
		else if (aux1.s.id == TOKN_char){
			if (aux2.s.id == TOKN_int){
				printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Multiplicação incompatível com Char %s Int!!\n\n", linha, coluna, lexema.name, op.s.name);
				fclose(arq);
				exit(0);
			}
			else if (aux2.s.id == TOKN_float){
				printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Multiplicação incompatível com Char %s Float!!\n\n", linha, coluna, lexema.name, op.s.name);
				fclose(arq);
				exit(0);
			}
			else if (aux2.s.id == TOKN_char){
				return aux1;
			}
		}
	}
	else if (op.s.id == TOKN_Divisao){
		if (aux1.s.id == TOKN_int){
			if (aux2.s.id == TOKN_int){
				aux2.s.id = TOKN_float;
				return aux2;
			}
			else if (aux2.s.id == TOKN_float){
				aux1.s.id = TOKN_float;
				return aux1;
			}
			else if (aux2.s.id == TOKN_char){
				printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Divisão incompatível com Int %s Char!!\n\n", linha, coluna, lexema.name, op.s.name);
				fclose(arq);
				exit(0);
			}
		}
		else if (aux1.s.id == TOKN_float){
			if (aux2.s.id == TOKN_int || aux2.s.id == TOKN_float){
				return aux1;
			}
			else if (aux2.s.id == TOKN_char){
				printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Divisão incompatível com Float %s Char!!\n\n", linha, coluna, lexema.name, op.s.name);
				fclose(arq);
				exit(0);
			}
		}
		else if (aux1.s.id == TOKN_char){
			if (aux2.s.id == TOKN_int){
				printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Divisão incompatível com Char %s Int!!\n\n", linha, coluna, lexema.name, op.s.name);
				fclose(arq);
				exit(0);
			}
			else if (aux2.s.id == TOKN_float){
				printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Divisão incompatível com Char %s Float!!\n\n", linha, coluna, lexema.name, op.s.name);
				fclose(arq);
				exit(0);
			}
			else if (aux2.s.id == TOKN_char){
				return aux1;
			}
		}
	}
	else if (op.s.id == TOKN_Adicao || op.s.id == TOKN_Subtracao){
		if (aux1.s.id == TOKN_int){
			if (aux2.s.id == TOKN_int || aux2.s.id == TOKN_float){
				return aux2;
			}
			else if (aux2.s.id == TOKN_char){
				printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Adição/Subtração incompatível com Int %s Char!!\n\n", linha, coluna, lexema.name, op.s.name);
				fclose(arq);
				exit(0);
			}
		}
		else if (aux1.s.id == TOKN_float){
			if (aux2.s.id == TOKN_int || aux2.s.id == TOKN_float){
				return aux1;
			}
			else if (aux2.s.id == TOKN_char){
				printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Adição/Subtração incompatível com Float %s Char!!\n\n", linha, coluna, lexema.name, op.s.name);
				fclose(arq);
				exit(0);
			}
		}
		else if (aux1.s.id == TOKN_char){
			if (aux2.s.id == TOKN_int){
				printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Adição/Subtração incompatível com Char %s Int!!\n\n", linha, coluna, lexema.name, op.s.name);
				fclose(arq);
				exit(0);
			}
			else if (aux2.s.id == TOKN_float){
				printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Adição/Subtração incompatível com Char %s float!!\n\n", linha, coluna, lexema.name, op.s.name);
				fclose(arq);
				exit(0);
			}
			else if (aux2.s.id == TOKN_char){
				return aux1;
			}
		}
	}
	else if (op.s.id == TOKN_Igual){
		if (aux1.s.id == TOKN_int){
			if (aux2.s.id == TOKN_int){
				return aux1;
			}
			else if (aux2.s.id == TOKN_float){
				printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Atribuição incompatível para Int = Float!!\n\n", linha, coluna, lexema.name);
				fclose(arq);
				exit(0);
			}
			else if (aux2.s.id == TOKN_char){
				printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Atribuição incompatível para Int = Char!!\n\n", linha, coluna, lexema.name);
				fclose(arq);
				exit(0);
			}
		}
		else if (aux1.s.id == TOKN_float){
			if (aux2.s.id == TOKN_int || aux2.s.id == TOKN_float){
				return aux1;
			}
			else if (aux2.s.id == TOKN_char){
				printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Atribuição incompatível para Float = Char!!\n\n", linha, coluna, lexema.name);
				fclose(arq);
				exit(0);
			}
		}
		else if (aux1.s.id == TOKN_char){
			if (aux2.s.id == TOKN_int){
				printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Atribuição incompatível para Char = Int!!\n\n", linha, coluna, lexema.name);
				fclose(arq);
				exit(0);
			}
			else if (aux2.s.id == TOKN_float){
				printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Atribuição incompatível para Char = Float!!\n\n", linha, coluna, lexema.name);
				fclose(arq);
				exit(0);
			}
			else if (aux2.s.id == TOKN_char){
				printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Atribuição incompatível para Char = Char!!\n\n", linha, coluna, lexema.name);
				fclose(arq);
				exit(0);
			}
		}
	}
}

void decl_var(FILE *arq){ //<tipo> <id> {,<id>}* ";"
	int tipo_variavel, flag;
	if (lexema.id == TOKN_Tipoint){
		tipo_variavel = TOKN_int;
		lexema = Scan(arq);
		if (lexema.id == TOKN_ID){
			flag = CheckVariavelMesmoEscopo(lexema.name, tipo_variavel, escopo);
			if (flag == 1){
				printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Variável já declarada com esse nome!!\n\n", linha, coluna, lexema.name);
				fclose(arq);
				exit(0);
			}
			StackVariavel(lexema.name, tipo_variavel, escopo);
			lexema = Scan(arq);
			while (lexema.id == TOKN_Virgula){
				lexema = Scan(arq);
				if (lexema.id == TOKN_ID){
					flag = CheckVariavelMesmoEscopo(lexema.name, tipo_variavel, escopo);
					if (flag == 1){
						printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Variável já declarada com esse nome!!\n\n", linha, coluna, lexema.name);
						fclose(arq);
						exit(0);
					}
					StackVariavel(lexema.name, tipo_variavel, escopo);
					lexema = Scan(arq);
				}
				else{
					printf("ERRO na linha %d, coluna %d, ultimo token lido %s: É obrigatório alguma variável após a vírgula!!\n\n", linha, coluna, lexema.name);
					fclose(arq);
					exit(0);
				}
			}
			if (lexema.id == TOKN_PontoeVirgula){
				lexema = Scan(arq);
			}
			else{
				printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Ausência do ponto e vírgula na declaração de variáveis!!\n\n", linha, coluna, lexema.name);
				fclose(arq);
				exit(0);
			}
		}
		else{
			printf("ERRO na linha %d, coluna %d, ultimo token lido %s:Declaração de variáveis do tipo 'Int' inválida!!\n\n", linha, coluna, lexema.name);
			fclose(arq);
			exit(0);
		}
	}
	else if (lexema.id == TOKN_Tipofloat){
		tipo_variavel = TOKN_float;
		lexema = Scan(arq);
		if (lexema.id == TOKN_ID){
			flag = CheckVariavelMesmoEscopo(lexema.name, tipo_variavel, escopo);
			if (flag == 1){
				printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Variável já declarada com esse nome!!\n\n", linha, coluna, lexema.name);
				fclose(arq);
				exit(0);
			}
			StackVariavel(lexema.name, tipo_variavel, escopo);
			lexema = Scan(arq);
			while (lexema.id == TOKN_Virgula){
				lexema = Scan(arq);
				if (lexema.id == TOKN_ID){
					flag = CheckVariavelMesmoEscopo(lexema.name, tipo_variavel, escopo);
					if (flag == 1){
						printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Variável já declarada com esse nome!!\n\n", linha, coluna, lexema.name);
						fclose(arq);
						exit(0);
					}
					StackVariavel(lexema.name, tipo_variavel, escopo);
					lexema = Scan(arq);
				}
				else{
					printf("ERRO na linha %d, coluna %d, ultimo token lido %s: É obrigatório alguma variável após a vírgula!!\n\n", linha, coluna, lexema.name);
					fclose(arq);
					exit(0);
				}
			}
			if (lexema.id == TOKN_PontoeVirgula){
				lexema = Scan(arq);
			}
			else{
				printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Ausência do ponto e vírgula na declaração de variáveis!!\n\n", linha, coluna, lexema.name);
				fclose(arq);
				exit(0);
			}
		}
		else{
			printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Declaração de variáveis do tipo 'Float' inválida!!\n\n", linha, coluna, lexema.name);
			fclose(arq);
			exit(0);
		}
	}
	else if (lexema.id == TOKN_Tipochar){
		tipo_variavel = TOKN_char;
		lexema = Scan(arq);
		if (lexema.id == TOKN_ID){
			flag = CheckVariavelMesmoEscopo(lexema.name, tipo_variavel, escopo);
			if (flag == 1){
				printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Variável já declarada com esse nome!!\n\n", linha, coluna, lexema.name);
				fclose(arq);
				exit(0);
			}
			StackVariavel(lexema.name, tipo_variavel, escopo);
			lexema = Scan(arq);
			while (lexema.id == TOKN_Virgula){
				lexema = Scan(arq);
				if (lexema.id == TOKN_ID){
					flag = CheckVariavelMesmoEscopo(lexema.name, tipo_variavel, escopo);
					if (flag == 1){
						printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Variável já declarada com esse nome!!\n\n", linha, coluna, lexema.name);
						fclose(arq);
						exit(0);
					}
					StackVariavel(lexema.name, tipo_variavel, escopo);
					lexema = Scan(arq);
				}
				else{
					printf("ERRO na linha %d, coluna %d, ultimo token lido %s: É obrigatório alguma variável após a vírgula!!\n\n", linha, coluna, lexema.name);
					fclose(arq);
					exit(0);
				}
			}
			if (lexema.id == TOKN_PontoeVirgula){
				lexema = Scan(arq);
			}
			else{
				printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Ausência do ponto e vírgula na declaração de variáveis!!\n\n", linha, coluna, lexema.name);
				fclose(arq);
				exit(0);
			}
		}
		else{
			printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Declaração de variáveis do tipo 'Char' inválida!!\n\n", linha, coluna, lexema.name);
			fclose(arq);
			exit(0);
		}
	}
	else{
		printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Ausência do Float Int e Char na declaração de Variáveis!!\n\n", linha, coluna, lexema.name);
		fclose(arq);
		exit(0);
	}
}

void comandoIF(FILE *arq){
	TSimboloAux aux;
	char nome[TAM];
	int Else, acabou, IF;
	if (lexema.id == TOKN_AbrePar){
		lexema = Scan(arq);
		if (lexema.id != TOKN_FechaPar){
			Else = label + 1;
			acabou = label + 2;
			IF = label;
			printf("L%d:\n\n", IF);
			aux = exp_relac(arq);
			strcpy(nome, aux.s.name);
			sprintf(nome, "T%d", t-1);
			printf("IF (%s == 0) goto L%d\n\n", nome, Else);
		}
		else{
			printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Erro no IF. Expressão Relacional ausente ou incompleta!!\n\n", linha, coluna, lexema.name);
			fclose(arq);
			exit(0);
		}
		if (lexema.id == TOKN_FechaPar){
			lexema = Scan(arq);
			label = label + 3;
			comando(arq);
			//label = label - 3;
			printf("goto L%d\n\n", acabou);
			if (lexema.id == TOKN_else){
				printf("L%d:\n\n", Else);
				lexema = Scan(arq);
				label = label + 2;
				comando(arq);
				//label = label - 2;
			}
			else{
				printf("L%d:\n\n", Else);
			}
			printf("L%d:\n\n", acabou);
		}
		else{
			printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Erro no IF. Ausência do fechamento de parenteses!!\n\n", linha, coluna, lexema.name);
			fclose(arq);
			exit(0);
		}
	}
	else{
		printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Erro no IF. Ausência da abertura de parenteses!!\n\n", linha, coluna, lexema.name);
		fclose(arq);
		exit(0);
	}
}

void comando(FILE *arq){ //<comando> ::= <comando_básico> | <iteração> | if "("<expr_relacional>")" <comando> {else <comando>}?
	if (lexema.id == TOKN_ID || lexema.id == TOKN_AbreChav){
		comandoBasico(arq);
	}
	else if (lexema.id == TOKN_while || lexema.id == TOKN_do){
		iteracao(arq);
	}
	else if (lexema.id == TOKN_if){
		lexema = Scan(arq);
		comandoIF(arq);
	}
	else{
		printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Erro no comando espera-se alguma função de looping, condição, variavel ou '}'!!\n\n", linha, coluna, lexema.name);
		fclose(arq);
		exit(0);
	}
}

void comandoBasico(FILE *arq){ //<comando_básico> ::= <atribuição> | <bloco>
	if (lexema.id == TOKN_ID){
		atribuicao(arq);
	}
	else if (lexema.id == TOKN_AbreChav){
		bloco(arq);
	}
	else{
		printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Erro no comando Básico espera-se alguma variável ou '}'!!\n\n", linha, coluna, lexema.name);
		fclose(arq);
		exit(0);
	}
}

void atribuicao(FILE *arq){ //<id> "=" <expr_arit> ";"
	TSimboloAux simAux1, simAux2, aux, op;
	if (lexema.id == TOKN_ID){
		simAux1 = CheckTodasVariaveis(lexema, arq);
		lexema = Scan(arq);
		if (lexema.id == TOKN_Igual){
			op.s.id = lexema.id;
			strcpy(op.s.name, lexema.name);
			lexema = Scan(arq);
			simAux2 = expr_arit(arq);
			aux = CheckCompatibilidade(simAux1, simAux2, op, arq);
			printf("%s = T%d\n\n", simAux1.s.name, t - 1);
			if (lexema.id == TOKN_PontoeVirgula){
				lexema = Scan(arq);
			}
			else{
				printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Erro na atribuição. Após uma expressão aritimética obrigatório ponto e vírgula sucedendo-a!!\n\n", linha, coluna, lexema.name);
				fclose(arq);
				exit(0);
			}
		}
		else{
			printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Erro na atribuição. Ausência do '='!!\n\n", linha, coluna, lexema.name);
			fclose(arq);
			exit(0);
		}
	}
}

TSimboloAux expr_arit(FILE *arq){ //<expr_arit> -> <expr_arit> "+" <termo>   | <expr_arit> "-" <termo> | <termo> 
	TSimboloAux simAux1, simAux2, r1, op, aux;
	simAux1 = termo(arq);
	r1 = simAux1;
	while (lexema.id == TOKN_Adicao || lexema.id == TOKN_Subtracao){
		aux = r1;
		op.s.id = lexema.id;
		strcpy(op.s.name, lexema.name);
		lexema = Scan(arq);
		simAux2 = termo(arq);
		r1 = CheckCompatibilidade(r1, simAux2, op, arq);
		if (aux.s.id == TOKN_int && simAux2.s.id == TOKN_float){
			printf("T%d = (float)%s\n", t, aux.s.name);
			sprintf(aux.s.name, "T%d", t);
			t++;
			printf("T%d = %s %s %s\n", t, aux.s.name, op.s.name, simAux2.s.name);
			sprintf(r1.s.name, "T%d", t);
			t++;
		}
		else if (aux.s.id == TOKN_float && simAux2.s.id == TOKN_int){
			printf("T%d = (float)%s\n", t, simAux2.s.name);
			sprintf(simAux2.s.name, "T%d", t);
			t++;
			printf("T%d = %s %s %s\n", t, aux.s.name, op.s.name, simAux2.s.name);
			sprintf(r1.s.name, "T%d", t);
			t++;
		}
		else if (aux.s.id == simAux2.s.id){
			printf("T%d = %s %s %s\n", t, aux.s.name, op.s.name, simAux2.s.name);
			sprintf(r1.s.name, "T%d", t);
			t++;
		}
	}
	return r1;
}

TSimboloAux termo(FILE *arq){ //<termo> ::= <termo> "*" <fator> | <termo> “/” <fator> | <fator>
	TSimboloAux simAux1, simAux2, r1, op, aux;
	simAux1 = fator(arq);
	r1 = simAux1;
	while (lexema.id == TOKN_Multiplicacao || lexema.id == TOKN_Divisao){
		aux = r1;
		op.s.id = lexema.id;
		strcpy(op.s.name, lexema.name);
		lexema = Scan(arq);
		simAux2 = fator(arq);
		r1 = CheckCompatibilidade(simAux1, simAux2, op, arq);
		if (aux.s.id == TOKN_int && simAux2.s.id == TOKN_float){
			printf("T%d = (float)%s\n", t, aux.s.name);
			sprintf(aux.s.name, "T%d", t);
			t++;
			printf("T%d = %s %s %s\n", t, aux.s.name, op.s.name, simAux2.s.name);
			sprintf(r1.s.name, "T%d", t);
			t++;
		}
		else if (aux.s.id == TOKN_float && simAux2.s.id == TOKN_int){
			printf("T%d = (float)%s\n", t, simAux2.s.name);
			sprintf(simAux2.s.name, "T%d", t);
			t++;
			printf("T%d = %s %s %s\n", t, aux.s.name, op.s.name, simAux2.s.name);
			sprintf(r1.s.name, "T%d", t);
			t++;
		}
		else if (aux.s.id == simAux2.s.id){
			printf("T%d = %s %s %s\n", t, aux.s.name, op.s.name, simAux2.s.name);
			sprintf(r1.s.name, "T%d", t);
			t++;
		}
	}
	return r1;
}

TSimboloAux fator(FILE *arq){ //“(“ <expr_arit> “)” | <id> | <real> | <inteiro> | <char>
	TSimbolo *aux;
	TSimboloAux simAux;
	int flag;
	if (lexema.id == TOKN_AbrePar){
		lexema = Scan(arq);
		simAux = expr_arit(arq);
		if (lexema.id == TOKN_FechaPar){
			lexema = Scan(arq);
			return simAux;
		}
		else{
			printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Erro no fator. Ausência do ')'!!\n\n", linha, coluna, lexema.name);
			fclose(arq);
			exit(0);
		}
	}
	else if (lexema.id == TOKN_ID){
		simAux = CheckTodasVariaveis(lexema, arq);
		lexema = Scan(arq);
		return simAux;
	}
	else if (lexema.id == TOKN_float){
		simAux.s.id == TOKN_float;
		strcpy(simAux.s.name, lexema.name);
		lexema = Scan(arq);
		return simAux;
	}
	else if (lexema.id == TOKN_int){
		strcpy(simAux.s.name, lexema.name);
		simAux.s.id = 24;
		lexema = Scan(arq);
		return simAux;
	}
	else if (lexema.id == TOKN_char){
		simAux.s.id = TOKN_char;
		strcpy(simAux.s.name, lexema.name);
		lexema = Scan(arq);
		return simAux;
	}
	else{
		printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Erro no fator. Expressão irregular!!\n\n", linha, coluna, lexema.name);
		fclose(arq);
		exit(0);
	}
}

void iteracao(FILE *arq){ //while "("<expr_relacional>")" <comando> | do <comando> while "("<expr_relacional>")"";"
	TSimboloAux aux;
	char nome[TAM];
	int fim, inicio;
	if (lexema.id == TOKN_while){
		lexema = Scan(arq);
		if (lexema.id == TOKN_AbrePar){
			inicio = label;
			fim = label + 1;
			printf("L%d:\n\n", inicio);
			lexema = Scan(arq);
			aux = exp_relac(arq);
			strcpy(nome, aux.s.name);
			sprintf(nome, "T%d", t - 1);
			printf("IF(%s == 0) goto L%d\n\n", nome, fim);
			if (lexema.id == TOKN_FechaPar){
				lexema = Scan(arq);
				label = label + 2;
				comando(arq);
				//label = label - 2;
				printf("goto L%d\n\n", inicio);
				printf("L%d:\n\n", fim);
			}
			else{
				printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Erro na iteração. Ausência do fechamento do parenteses!!\n\n", linha, coluna, lexema.name);
				fclose(arq);
				exit(0);
			}
		}
		else{
			printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Erro na iteração. Ausência da abertura de parenteses(while)!!\n\n", linha, coluna, lexema.name);
			fclose(arq);
			exit(0);
		}
	}
	else if (lexema.id == TOKN_do){
		int ini;
		ini = label;
		printf("L%d:\n\n", ini);
		lexema = Scan(arq);
		label = label + 1;
		comando(arq);
		//label = label - 1;
		if (lexema.id == TOKN_while){
			lexema = Scan(arq);
			if (lexema.id == TOKN_AbrePar){
				lexema = Scan(arq);
				aux = exp_relac(arq);
				strcpy(nome, aux.s.name);
				sprintf(nome, "T%d", t - 1);
				printf("IF (%s == 1) goto L%d\n\n", nome, ini);
				if (lexema.id == TOKN_FechaPar){
					lexema = Scan(arq);
					if (lexema.id == TOKN_PontoeVirgula){
						lexema = Scan(arq);
					}
					else{
						printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Erro na iteração. Ausência do ponto e virgula!!\n\n", linha, coluna, lexema.name);
						fclose(arq);
						exit(0);
					}
				}
				else{
					printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Erro na iteração. Ausência do fechamento de parenteses!!\n\n", linha, coluna, lexema.name);
					fclose(arq);
					exit(0);
				}
			}
			else{
				printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Erro na iteração. Ausência da abertura de parenteses!!\n\n", linha, coluna, lexema.name);
				fclose(arq);
				exit(0);
			}
		}
		else{
			printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Erro na iteração. Ausência do WHILE!!\n\n", linha, coluna, lexema.name);
			fclose(arq);
			exit(0);
		}
	}
	else{
		printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Erro na iteração. Ausência 'DO' nem 'WHILE'!!\n\n", linha, coluna, lexema.name);
		fclose(arq);
		exit(0);
	}
}

TSimboloAux exp_relac(FILE *arq){ //<expr_arit> <op_relacional> <expr_arit>
	TSimboloAux r1, r2, aux, op;
	r1 = expr_arit(arq);
	aux = r1;
	op = op_relac(arq);
	r2 = expr_arit(arq);
	r1 = CheckCompatibilidade(r1, r2, op, arq);
	if (aux.s.id == TOKN_int && r2.s.id == TOKN_float){
		printf("T%d = (float)%s\n", t, aux.s.name);
		sprintf(aux.s.name, "T%d", t);
		t++;
		printf("T%d = %s %s %s\n", t, aux.s.name, op.s.name, r2.s.name);
		sprintf(r1.s.name, "T%d", t);
		t++;
	}
	else if (aux.s.id == TOKN_float && r2.s.id == TOKN_int){
		printf("T%d = (float)%s\n", t, r2.s.name);
		sprintf(r2.s.name, "T%d", t);
		t++;
		printf("T%d = %s %s %s\n", t, aux.s.name, op.s.name, r2.s.name);
		sprintf(r1.s.name, "T%d", t);
		t++;
	}
	else if (aux.s.id == r2.s.id){
		printf("T%d = %s %s %s\n", t, aux.s.name, op.s.name, r2.s.name);
		sprintf(r1.s.name, "T%d", t);
		t++;
	}
	return aux;
}

TSimboloAux op_relac(FILE *arq){
	TSimboloAux simboloOp;
	if (lexema.id != TOKN_Menor && lexema.id != TOKN_Maior && lexema.id != TOKN_MenorIgual && lexema.id != TOKN_MaiorIgual && lexema.id != TOKN_Igualdade && lexema.id != TOKN_Diferente){
		printf("ERRO na linha %d, coluna %d, ultimo token lido %s: Erro na Operação Relacional. Ausência de operadores de comparação!!\n\n", linha, coluna, lexema.name);
		fclose(arq);
		exit(0);
	}
	else{
		simboloOp.s.id = lexema.id;
		strcpy(simboloOp.s.name, lexema.name);
		lexema = Scan(arq);
		return simboloOp;
	}
}
