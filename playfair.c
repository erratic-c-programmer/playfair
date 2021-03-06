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

static short int table_w = 12, table_h = 8;

void pf_encrypt(char **result, char **table, struct pair firstletter, struct pair secondletter);
void pf_decrypt(char **result, char **table, struct pair firstletter, struct pair secondletter);

int main(int argc, char **argv)
{
	FILE *tablefile;
	char *tablefile_name = "";
	bool encode = true;
	char *msg;
	char *ans;
	char lowestchar = ' ';
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

	if (!tflag) {
		TOERR("[ERROR] No tablefile specified.  Bailing out!\n");
		exit(1);
	} else if (access(tablefile_name, F_OK) == -1) {
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
				table[i][j] = fgetc(tablefile);
			fgetc(tablefile); /* Skip newline */
		}
		fclose(tablefile);
	}

	if (optind >= argc) {
		TOERR("[ERROR] No message specified.  Bailing out!\n");
		exit(1);
	} else {
		msg = calloc(strlen(argv[optind])+2, sizeof(char));
		strcpy(msg, argv[optind]);
	}

	/* Done parsing options */
	ans = calloc(strlen(msg)+4, sizeof(char));

	if (strlen(msg) % 2) {
		TOERR("[WARN] Message length is odd.  Appending 'X'.\n");
		strcat(msg, "X");
	}

	reftable = malloc(table_h * table_w * sizeof(struct pair));
	for (int i = 0; i < table_h; i++) {
		for (int j = 0; j < table_w; j++)
			reftable[table[i][j]-lowestchar] = (struct pair){i, j};
	}

	for (size_t i = 0; i+1 < strlen(msg); i += 2) {
		if (encode)
			pf_encrypt(&ans, table, reftable[msg[i]-lowestchar], reftable[msg[i+1]-lowestchar]);
		else
			pf_decrypt(&ans, table, reftable[msg[i]-lowestchar], reftable[msg[i+1]-lowestchar]);
	}

	if (ans[strlen(ans)-1] == ' ' && encode) {
		TOERR("[WARN] last letter of encoded message is a space, appending XX.\n");
		strcat(ans, "XX");
	}

	printf("%s\n", ans);

	for (int i = 0; i < table_h; i++)
		free(table[i]);
	free(table);
	free(ans);
	free(msg);
	free(reftable);

	return 0;
}

void pf_encrypt(char **result, char **table, struct pair firstletter, struct pair secondletter)
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


void pf_decrypt(char **result, char **table, struct pair firstletter, struct pair secondletter)
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
