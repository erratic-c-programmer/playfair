#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <errno.h>

#define TOERR(m) fprintf(stderr, m)

struct pair {
	int first;
	int second;
};

static bool odd = false;
static struct pair reftable[25];
#ifndef MAX_MSG_LEN
#error Please specify -DMAX_MSG_LEN=XXX
#endif

#if defined ENCRYPT
#define COMPUTE encrypt
#elif defined DECRYPT
#define COMPUTE decrypt
#else
#error Please specify -DENCYPT or -DDECRYPT
#endif

void encrypt(char **result, char **table, struct pair firstletter, struct pair secondletter);
void decrypt(char **result, char **table, struct pair firstletter, struct pair secondletter);

int main(int argc, char **argv)
{
	/* Init/error handling */
	/* Allocate memory */
	char *ans = calloc(MAX_MSG_LEN, sizeof(char));
	char **table = malloc(5 * sizeof(char*));
	for (int i = 0; i < 5; ++i)
		table[i] = malloc(5 * sizeof(char));

	switch (argc) {
		case 1:
			TOERR("[ERROR] No message specified.  Bailing out!\n"); goto free_mem;
			break;
		case 2:
			TOERR("[WARN] No tablefile specified.  Using default sequential.\n");
			for (int i = 0; i < 5; i++) {
				for (int j = 0; j < 5; j++)
					table[i][j] = 'A' + 5 * i + j;
			}
			break;
		case 3:
			;;
			FILE *tablefile = fopen(argv[2], "r");
			if (errno != 0) {
				TOERR("[ERROR] Keyfile not found.  Bailing out!\n");
				exit(2);
			}
			printf("[NOTE] Using tablefile %s\n",  argv[2]);
			for (int i = 0; i < 5; i++) {
				for (int j = 0; j < 5; j++)
					table[i][j] = toupper(fgetc(tablefile));
				fgetc(tablefile); /* Skip newline */
			}
			break;
		default:
			TOERR("Too many arguments.  Bailing out!\n");
			goto free_mem;
			break;
	}

	if (strlen(argv[1]) % 2) {
		TOERR("[WARN] Message length is odd.  Appending a random letter.\n");
		odd = true;
	}

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++)
			reftable[table[i][j]-'A'] = (struct pair){i, j}; /* to avoid needless looping
									    to search for a letter */
	}

	for (int i = 0; i < strlen(argv[1]); i += 2)
		COMPUTE(&ans, table, reftable[toupper(argv[1][i])-'A'], reftable[toupper(argv[1][i+1])-'A']);


	printf("%s\n", ans);

free_mem:
	for (int i = 0; i < 5; i++)
		free(table[i]);
	free(table);
	free(ans);
}

void encrypt(char **result, char **table, struct pair firstletter, struct pair secondletter)
{
	if (firstletter.first == secondletter.first) { /* Same row */
		if (firstletter.second == 5-1) /* Wrap */
			strcat(*result, (char[2]){table[firstletter.first][0], '\0'});
		else
			strcat(*result, (char[2]){table[firstletter.first]
					[firstletter.second+1], '\0'});
		if (secondletter.second == 5-1) /* Wrap */
			strcat(*result, (char[2]){table[secondletter.first][0], '\0'});
		else
			strcat(*result, (char[2]){table[secondletter.first]
					[secondletter.second+1], '\0'});
	} else if (firstletter.second == secondletter.second) { /* Same column */
		if (firstletter.first == 5-1) /* Wrap */
			strcat(*result, (char[2]){table[0][firstletter.second], '\0'});
		else
			strcat(*result, (char[2]){table[firstletter.first+1]
					[firstletter.second], '\0'});
		if (secondletter.first == 5-1) /* Wrap */
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
			strcat(*result, (char[2]){table[firstletter.first][5-1], '\0'});
		else
			strcat(*result, (char[2]){table[firstletter.first]
					[firstletter.second-1], '\0'});
		if (secondletter.second == 0) /* Wrap */
			strcat(*result, (char[2]){table[secondletter.first][5-1], '\0'});
		else
			strcat(*result, (char[2]){table [secondletter.first]
					[secondletter.second-1], '\0'});
	} else if (firstletter.second == secondletter.second) { /* Same column */
		if (firstletter.first == 0) /* Wrap */
			strcat(*result, (char[2]){table[5-1][firstletter.second], '\0'});
		else
			strcat(*result, (char[2]){table[firstletter.first-1]
					[firstletter.second], '\0'});
		if (secondletter.first == 5-1) /* Wrap */
			strcat(*result, (char[2]){table[5-1][secondletter.second], '\0'});
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
