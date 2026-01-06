#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "common.h"
#include "parse.h"

#define VERSION 0x1
/*
void list_employees(struct dbheader_t *dbhdr, struct employee_t *employees) {
	//
}

int add_employee(struct dbheader_t *dbhdr, struct employee_t *employees, char *addstring) {
	//
}

int read_employees(int fd, struct dbheader_t *dbhdr, struct employee_t **employeesOut) {
	//
}
*/
int output_file(int fd, struct dbheader_t *dbhdr/*, struct employee_t *employees*/) {

	if (fd < 0) {
		printf("FATAL ERROR: Invalid File descriptor.");
		return STATUS_ERROR;
	}

	header->magic = htonl(header->magic);
	header->version = htons(header->version);
	header->count = htons(header->count);
	header->filesize = htonl(header->filesize);

	lseek(fd, 0, SEEK_SET);

	write(fd, dbhdr, sizeof(struct dbheader_t));

	return STATUS_SUCCESS;
}	

int validate_db_header(int fd, struct dbheader_t **headerOut) {

	if (fd < 0) {
		printf("FATAL ERROR: Invalid File descriptor.");
		return STATUS_ERROR;
	}

	struct dbheader_t*header = calloc(1, sizeof(struct dbheader_t));
	if (header == NULL) {
		printf("FATAL ERROR: Calloc failed to create the header");
		return STATUS_ERROR;
	}

	if (read(fd, header, sizeof(struct dbheader_t)) != sizeof(struct dbheader_t)) {
		perror("read");
		free(header);
		return STATUS_ERROR;
	}

	header->magic = ntohl(header->magic);
	header->version = ntohs(header->version);
	header->count = ntohs(header->count);
	header->filesize = ntohl(header->filesize);

	if (header->magic != HEADER_MAGIC) {
		printf("FATAL ERROR: Database invalid (magic).\n");
		return STATUS_ERROR;
	}

	if (header->version != VERSION) {
		printf("FATAL ERROR: Version unsupported.\n");
		return STATUS_ERROR;
	}

	struct stat dbstat = {0};
	fstat (fd, &dbstat);
	if (header->filesize != dbstat.st_size) {
		printf("Incorrect filesize, expect database corruption.\n");
		return STATUS_ERROR;
	}

	*headerOut = header;
}

int create_db_header(int fd, struct dbheader_t **headerOut) {

	struct dbheader_t*header = calloc (1, sizeof(struct dbheader_t));
	if (header == NULL) {
		printf("FATAL ERROR: Calloc failed to create the header");
		return STATUS_ERROR;
	}

	header->magic = HEADER_MAGIC;
	header->version = 0x1;
	header->count = 0;
	header->filesize = sizeof(struct dbheader_t);

	*headerOut = header;

	return STATUS_SUCCESS;
}


