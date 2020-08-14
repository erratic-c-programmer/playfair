#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>

#define TOERR(m) fprintf(stderr, m)

struct pair {
	int first;
	int second;
};

static bool odd = false;
static short int table_w = 5, table_h = 5;

void encrypt(char **result, char **table, struct pair firstletter, struct pair secondletter);
void decrypt(char **result, char **table, struct pair firstletter, struct pair secondletter);

int main(int argc, char **argv)
{
	FILE *tablefile;
	char *tablefile_name = "";
	bool encode = true;
	char *msg;
	char lowestchar = 'A';
	struct pair *reftable;

	char **table;

	bool tflag = false;
	bool edflag = false;
	bool Wflag = false;
	bool Hflag = false;
	bool lflag = false;

	extern int optind;
	extern char *optarg;
	char c;

	/* Parsing options */
	while ((c = getopt(argc, argv, "t:edl:W:H:h")) != -1) {
		switch (c) {
		case 't':
			if (tflag)
				TOERR("[WARN] -t flag specified more than once.\n");
			tflag = true;
			tablefile_name = optarg;
			break;

		case 'e':
			if (edflag)
				TOERR("[WARN] -e or -d flag(s) specified more than once.\n");
			edflag = true;
			encode = true;
			break;

		case 'd':
			if (edflag)
				TOERR("[WARN] -e or -d flag(s) specified more than once.\n");
			edflag = true;
			encode = false;
			break;

		case 'W':
			if (Wflag)
				TOERR("[WARN] -W flag specified more than once.\n");
			Wflag = true;
			table_w = atoi(optarg);
			break;

		case 'H':
			if (Hflag)
				TOERR("[WARN] -H flag specified more than once.\n");
			Hflag = true;
			table_h = atoi(optarg);
			break;

		case 'l':
			if (lflag)
				TOERR("[WARN] -l flag specified more than once.\n");
			lflag = true;
			lowestchar = optarg[0];
			break;

		case 'h':
			printf(
					"Usage: %s -t <tablefile> {-d|-e} [-l <lowest character>] "
					"[-W <tablewidth>] [-H <tableheight>] <message>\n",
					argv[0]);
			exit(0);
		}
	}

	if (!edflag) {
		TOERR("[ERROR] At least one of -e (encode) or -d (decode) flags must be specified.  Bailing out!\n");
		exit(1);
	}

	if (optind >= argc) {
		TOERR("[ERROR] No message specified.  Bailing out!\n");
		exit(1);
	} else {
		msg = argv[optind];
	}


	if (!tflag) {
		TOERR("[ERROR] No tablefile specified.  Bailing out!\n");
		exit(1);
	}
	if (access(tablefile_name, F_OK) == -1) {
		TOERR("[ERROR] File not found.  Bailing out!\n");
		exit(2);
	} else {
		table = malloc(table_h * sizeof(char*));
		for (int i = 0; i < table_h; ++i)
		table[i] = malloc(table_w * sizeof(char));

		printf("[NOTE] Using tablefile %s\n",  tablefile_name);
		tablefile = fopen(tablefile_name, "r");
		for (int i = 0; i < table_h; i++) {
			for (int j = 0; j < table_w; j++)
				table[i][j] = toupper(fgetc(tablefile));
			fgetc(tablefile); /* Skip newline */
		}
		fclose(tablefile);
	}
	/* Done parsing options */
	char *ans = calloc(strlen(msg)+1, sizeof(char));

	if (strlen(msg) % 2) {
		TOERR("[WARN] Message length is odd.  Appending a random letter.\n");
		odd = true;
	}

	reftable = malloc(table_h * table_w * sizeof(struct pair));
	for (int i = 0; i < table_h; i++) {
		for (int j = 0; j < table_w; j++)
			reftable[table[i][j]-lowestchar] = (struct pair){i, j};
	}

	for (size_t i = 0; i < strlen(msg); i += 2) {
		if (encode)
			encrypt(&ans, table, reftable[toupper(msg[i])-lowestchar], reftable[toupper(msg[i+1])-lowestchar]);
		else
			decrypt(&ans, table, reftable[toupper(msg[i])-lowestchar], reftable[toupper(msg[i+1])-lowestchar]);
	}

	printf("%s\n", ans);

	for (int i = 0; i < table_h; i++)
		free(table[i]);
	free(table);
	free(ans);
	free(reftable);

	return 0;
}

void encrypt(char **result, char **table, struct pair firstletter, struct pair secondletter)
{
	if (firstletter.first == secondletter.first) { /* Same row */
		if (firstletter.second == table_w-1) /* Wrap */
			strcat(*result, (char[2]){table[firstletter.first][0], '\0'});
		else
			strcat(*result, (char[2]){table[firstletter.first]
					[firstletter.second+1], '\0'});
		if (secondletter.second == table_w-1) /* Wrap */
			strcat(*result, (char[2]){table[secondletter.first][0], '\0'});
		else
			strcat(*result, (char[2]){table[secondletter.first]
					[secondletter.second+1], '\0'});
	} else if (firstletter.second == secondletter.second) { /* Same column */
		if (firstletter.first == table_h-1) /* Wrap */
			strcat(*result, (char[2]){table[0][firstletter.second], '\0'});
		else
			strcat(*result, (char[2]){table[firstletter.first+1]
					[firstletter.second], '\0'});
		if (secondletter.first == table_h-1) /* Wrap */
			strcat(*result, (char[2]){table[0][secondletter.second], '\0'});
		else
			strcat(*result, (char[2]){table[secondletter.first+1]
					[secondletter.second], '\0'});
	} else { /* Normal case */
		strcat(*result, (char[2]){table [firstletter.first]
				[secondletter.second], '\0'});
		strcat(*result, (char[2]){table [secondletter.first]
				[firstletter.second], '\0'});
	}
}


void decrypt(char **result, char **table, struct pair firstletter, struct pair secondletter)
{
	if (firstletter.first == secondletter.first) { /* Same row */
		if (firstletter.second == 0) /* Wrap */
			strcat(*result, (char[2]){table[firstletter.first][table_w-1], '\0'});
		else
			strcat(*result, (char[2]){table[firstletter.first]
					[firstletter.second-1], '\0'});
		if (secondletter.second == 0) /* Wrap */
			strcat(*result, (char[2]){table[secondletter.first][table_w-1], '\0'});
		else
			strcat(*result, (char[2]){table [secondletter.first]
					[secondletter.second-1], '\0'});
	} else if (firstletter.second == secondletter.second) { /* Same column */
		if (firstletter.first == 0) /* Wrap */
			strcat(*result, (char[2]){table[table_h-1][firstletter.second], '\0'});
		else
			strcat(*result, (char[2]){table[firstletter.first-1]
					[firstletter.second], '\0'});
		if (secondletter.first == 0) /* Wrap */
			strcat(*result, (char[2]){table[table_h-1][secondletter.second], '\0'});
		else
			strcat(*result, (char[2]){table[secondletter.first-1]
					[secondletter.second], '\0'});
	} else { /* Normal case */
		strcat(*result, (char[2]){table [firstletter.first]
				[secondletter.second], '\0'});
		strcat(*result, (char[2]){table [secondletter.first]
				[firstletter.second], '\0'});
	}
}
