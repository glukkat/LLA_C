#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv0) {
	static bool printed = false;
	if (!printed) {
		printf("Standard usage:\n");
		printf("%s -n -f <database file>", argv0);
		printf("\t -n: create new file");
		printf("\t -f: <required> path to database file");/*
		printf("");
		printf("");
		printf("");*/

		printed = true;
	}
}

int main(int argc, char *argv[]) {

	int dbfd = -1;
	struct dbheader_t*header = NULL;
	
	int c = {0};
	bool newfile = false;
	char*filepath = NULL;

	while ((c = getopt(argc, argv, "nf:")) != -1) {
		switch (c) {
		case 'n':
			newfile = true;
			break;
		case 'f':
			filepath = optarg;
			break;
		case '?':
			printf("Unknown argument");
			print_usage(argv[0]);
		default:
			printf("FATAL ERROR: Unhandled character exception in getopt loop.\nThe program will now quit.\n");
			return -1;
		}
	}
	
	if (filepath == NULL) {
		printf("FATAL ERROR: No filepath found.");
		print_usage(argv[0]);
		return -1;
	}

	if (newfile) {
		dbfd = create_db_file(filepath);
		if (dbfd == STATUS_ERROR) {
			printf("The program will now quit.\n");
			return -1;
		}

		if (create_db_header(header) == STATUS_ERROR) {
			printf("The program will now quit.\n");
			return -1
		}
	} else {
		dbfd = open_db_file(filepath);
		if (dbfd == STATUS_ERROR) {
			printf("The program will now quit.\n");
			return -1;
		}

		if (validate_db_header(dbfd, header) == STATUS_ERROR) {
			printf("The program will now quit.\n");
			return -1
		}
	}

	//INSERT CODE OVER THIS LINE

	output_file(dbfd, header, NULL);
}
