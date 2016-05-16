#include "config.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>

#ifndef BLOCK
#define BLOCK 32
#endif


typedef struct parser_t * Parser;
typedef struct state_t * State;
typedef struct arc_t * Arc;
typedef int (*actionFn)(FILE *, char *, Parser, Config);




static char *getParam(char *line);
static char * getValue(char *line);
static int isSectionTag(char *line);
static int isParam(char *line);
char * leadingTrim(char **str);
char * readLine(FILE *fp);
int resize(char **str, int newSize);



/****************************************/
/* Structures and structure's functions */
/****************************************/

struct config_t {

	struct addresses_t {
		char *server;
		char *listeningPort;
		char *database;
		char *logging;
	} addresses;
};

struct parser_t {

	State actualState;
	State *states;
	int statesQuantity;
};

struct state_t {

	char *name;
	Arc *arcs;
	int arcsQuantity;
	actionFn action;
};

struct arc_t {

	char *tag;
	State next;
};


static State createState(char *name, actionFn action) {
	State state = malloc(sizeof(*state));
	if (state == NULL) {
		return NULL;
	}
	state->name = name;
	state->arcs = NULL;
	state->arcsQuantity = 0;
	state->action = action;
	return state;
}

static Arc createArc(char *tag, State next) {
	Arc arc = malloc(sizeof(*arc));
	if (arc == NULL) {
		return NULL;
	}
	arc->tag = tag;
	arc->next = next;
	return arc;
}

static int addArcs(State state, int quantity, ...) {

	va_list ap;
	int i = state->arcsQuantity;
	Arc *aux = realloc(state->arcs, (state->arcsQuantity + quantity) * sizeof(Arc));
	if (aux == NULL) {
		return -1;
	}
	state->arcs = aux;
	va_start(ap, quantity);
	while (i < quantity) {
		state->arcs[i++] = va_arg(ap, Arc);
	}
	va_end(ap);
    state->arcsQuantity += quantity;
	return 0;
}

static int moveTo(Parser parser, char *name) {

	int i = 0;
	while (i < parser->statesQuantity) {
		State aux = parser->actualState->arcs[i++]->next;
		if (!strcmp(name, aux->name)) {
			parser->actualState = aux;
			return 0;
		}
	}
	return -1;
}


/****************************/
/* Automaton initialization */
/****************************/

int q0Action(FILE *fp, char *line, Parser parser, Config config) {

	if (feof(fp)) {
		return 0; /* final state */
	}

	line = readLine(fp);
	if (isSectionTag(line)) {
		int i = 0;
		while (i < parser->actualState->arcsQuantity) {
			if (!strcmp(line, parser->actualState->arcs[i]->tag)) {
				moveTo(parser, parser->actualState->arcs[i]->next->name);
				return parser->actualState->action(fp, NULL, parser, config);
			}
			i++;
		}
    } else if(!strcmp(line, "")) {
        return parser->actualState->action(fp, NULL, parser, config);
    }
	return -1;
}

int q1Action(FILE *fp, char *line, Parser parser, Config config) {

	if (feof(fp)) {
		return -1; /* Not final state */
	}

	line = readLine(fp);
	if (!strcmp(line, "end")) {
		moveTo(parser, "q0");
		return parser->actualState->action(fp, NULL, parser, config);
	} else if (isParam(line)) {
		char *param = getParam(line);
		int i = 0;
		while (i < parser->actualState->arcsQuantity) {
			if (!strcmp(param, parser->actualState->arcs[i]->tag)) {
				char *value = getValue(line);
				moveTo(parser, parser->actualState->arcs[i]->next->name);
				free(param);
				return parser->actualState->action(fp, value, parser, config);
			}
			i++;
		}
		free(param);
        return parser->actualState->action(fp, NULL, parser, config);
    } else if(!strcmp(line, "")) {
        return parser->actualState->action(fp, NULL, parser, config);
    }
	return -1;
}

int q2Action(FILE *fp, char *line, Parser parser, Config config) {
    
    line = leadingTrim(&line);
	config->addresses.server = line;
	moveTo(parser, "q1");
	return parser->actualState->action(fp, NULL, parser, config);
}

int q3Action(FILE *fp, char *line, Parser parser, Config config) {

    line = leadingTrim(&line);
	config->addresses.listeningPort = line;
	moveTo(parser, "q1");
	return parser->actualState->action(fp, NULL, parser, config);
}

int q4Action(FILE *fp, char *line, Parser parser, Config config) {
    
    line = leadingTrim(&line);
	config->addresses.database = line;
	moveTo(parser, "q1");
	return parser->actualState->action(fp, NULL, parser, config);
}

int q5Action(FILE *fp, char *line, Parser parser, Config config) {

    line = leadingTrim(&line);
	config->addresses.logging = line;
	moveTo(parser, "q1");
	return parser->actualState->action(fp, NULL, parser, config);
}


static Parser createAutomaton() {

	Parser parser = malloc(sizeof(*parser));
	/* Automaton states creation*/
	State q0 = createState("q0", &q0Action); /* Waits for a section tag */
	State q1 = createState("q1", &q1Action); /* Waits for one of the following keys: "server", "listining port", "database", or "logging" */
	State q2 = createState("q2", &q2Action); /* Copies "address" value into config_t structure */
	State q3 = createState("q3", &q3Action); /* Copies "listening port" value into config_t structure */
	State q4 = createState("q4", &q4Action); /* Copies "database" value into config_t structure */
	State q5 = createState("q5", &q5Action); /* Copies "logging" value into config_t structure */
	/* Automaton arcs creation*/
	Arc addresses = createArc("[addresses]", q1);
	Arc server = createArc("server", q2);
	Arc listeningPort = createArc("listening port", q3);
	Arc database = createArc("database", q4);
	Arc logging = createArc("logging", q5);
	Arc backToQ0 = createArc("end", q0);
	Arc lambdaToQ1 = createArc("", q1);
	/* Automaton arcs adding */
	addArcs(q0, 1, addresses);
	addArcs(q1, 5, server, listeningPort, database, logging, backToQ0);
	addArcs(q2, 1, lambdaToQ1);
	addArcs(q3, 1, lambdaToQ1);
	addArcs(q4, 1, lambdaToQ1);
	addArcs(q5, 1, lambdaToQ1);
	
	parser->actualState = q0;
    parser->states = malloc(6 * sizeof(State));
	parser->states[0] = q0;
	parser->states[1] = q1;
	parser->states[2] = q2;
	parser->states[3] = q3;
	parser->states[4] = q4;
	parser->states[5] = q5;
	parser->statesQuantity = 6;

	return parser;

}




/*********************************/
/* Setup and automaton execution */
/*********************************/

int parse(FILE *fp, Config config) {

	Parser parser = createAutomaton();
	return parser->actualState->action(fp, NULL, parser, config);

}

FILE * openConfigFile() {

	FILE *configFile = fopen(CONF_FILE_PATH, "r");
	if (configFile == NULL) {
		return NULL;
	}
	return configFile;
}

Config setup() {

	FILE *fp = openConfigFile();
	Config config = malloc(sizeof(*config));
    int flag = 0;

	if (fp == NULL || config == NULL) {
		return NULL;
	}
    flag = parse(fp, config);
    fclose(fp);
    return flag ? NULL : config;

}

char *getServerAddress(Config config) {
    return config->addresses.server;
}

char *getListeningPort(Config config) {
        return config->addresses.listeningPort;
}

char *getDatabaseAddress(Config config) {
        return config->addresses.database;
}

char *getLoggingAddress(Config config) {
    return config->addresses.logging;
}


/*********************/
/* Auxiliary Function */
/*********************/

int resize(char **str, int newSize) {

	char *aux = realloc(*str, newSize * sizeof(char));
	if (aux == NULL) {
		free(*str);
		return -1;
	}
	*str = aux;
	return 0;
}

char * leadingTrim(char **str) {
    
    int i = 0, j = 0, length = 0;
    char *result = NULL;
    while((*str)[i] != 0){
    	i++;
    }
    while ((*str)[j] != 0 && ((*str)[j] == ' ' || (*str)[j] == '\t')) {
        j++;
    }
    length = (i - j + 1) * sizeof(char);
    result = malloc(length);
    if (result == NULL) {
    	return NULL;
    }
    memcpy(result, *str + (j * sizeof(char)), length);
    return result;
}


char * readLine(FILE *fp) {
    
    char *string = NULL;
    int flag = 0, i = 0, j = 0;
    
    if (fp == NULL) {
        return NULL;
    }
    
    do {
        char c = getc(fp);
        flag = (c == '\n' || c == EOF);
        if ( ((i % BLOCK) == 0) && resize(&string, BLOCK + i)) {
            return NULL;
        }
        string[i++] = flag ? 0 : c;
    } while (!flag);
    
    string = leadingTrim(&string);
    
    /*if ( (i % BLOCK) != 0) {
        char *aux = realloc(string, (i - j + 1) * sizeof(char));
        string = aux;
    }*/
    
    return string;
}

static int isParam(char *line) {

	regex_t regex;
	char *expr = "^[a-zA-Z][a-zA-Z0-9 ]*=[^\n\t=]*$";
	regcomp(&regex, expr, REG_EXTENDED);
	return !regexec(&regex, line, 0, NULL, 0);

}

static int isSectionTag(char *line) {

	regex_t regex;
	char *expr = "^\\[[a-zA-Z][a-zA-Z0-9]*\\]$";
	regcomp(&regex, expr, REG_EXTENDED);
	return !regexec(&regex, line, 0, NULL, 0);

}

static char * getValue(char *line) {

	int i = 0;
	while (line[i++] != '=') {
		;
	}
	return (char *)( ((void *)line) + (i * sizeof(char)) );

}

static char *getParam(char *line) {
	
	int i = 0;
	char *result;
	while(line[i] != '=') {
		i++;
	}
	result = calloc(1, i * sizeof(char));
	memcpy(result, line, i * sizeof(char));
	return result;
}