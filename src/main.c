
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
		printf("%s -n -f <database file> -a <new employee> -l", argv0);
		printf("\t -n: create new file");
		printf("\t -f: <required> path to database file");
		printf("\t -a: add employee: comma-separated, no trailing space (name,address,hours)");
		printf("\t -l: list employees");/*
		printf("");*/

		printed = true;
	}
}

void free_memory() {

	if (header != NULL) {
		free(header);
		header = NULL;
	}

	if (employees != NULL) {
		free(employees);
		employees = NULL;
	}

	if (filepath != NULL) {
		free(filepath);
		filepath = NULL;
	}

	if (addarg != NULL) {
		free(addarg);
		addarg = NULL;
	}
}

int main(int argc, char *argv[]) {

	int dbfd = -1;
	struct dbheader_t*header = NULL;
	struct employee_t*employees = NULL;
	
	int c = {0};
	bool newfile = false;
	char*filepath = NULL;
	char*addarg = NULL;
	bool list = false;

	while ((c = getopt(argc, argv, "nf:a:")) != -1) {
		switch (c) {
		case 'n':
			newfile = true;
			break;
		case 'f':
			filepath = optarg;
			break;
		case 'a':
			addarg = optarg;
			break;
		case 'l':
			list = true;
			break;
		case '?':
			printf("Unknown argument");
			print_usage(argv[0]);
			break;
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

	if (addarg) {
		if(add_employee(header, &employees, addarg) == STATUS_ERROR) {
			printf("Error: failed to add employee.\n");
		}
	}

	if (list) list_employees(header, employees);

	//INSERT CODE OVER THIS LINE

	output_file(dbfd, header, employees);

	close(dbfd);

	free_memory();
}
