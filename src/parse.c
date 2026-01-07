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

void list_employees(struct dbheader_t *dbhdr, struct employee_t *employees) {
	printf("Employees List:\n\n");
	for (int i = 0; i < dbhdr->count; i++) {
		printf("Employee #%d\n", i);
		printf("\tName: %s\n", employees[i].name);
		printf("\tAddress: %s\n", employees[i].address);
		printf("\tHours: %d\n\n", employees[i].hours);
	}
	printf("---\n");
}

int add_employee(struct dbheader_t *dbhdr, struct employee_t **employees, char *addstring) {

	if (dbhdr == NULL) return STATUS_ERROR;
	if (employees == NULL) return STATUS_ERROR;
	if (*employees == NULL) return STATUS_ERROR;
	if (addstring == NULL) return STATUS_ERROR;
	
	char*name = strtok(addstring, ",");
	if (name == NULL) return STATUS_ERROR;
	char*address = strtok(NULL, ",");
	if (address == NULL) return STATUS_ERROR;
	char*hours = strtok(NULL, ",");
	if (hours == NULL) return STATUS_ERROR;

	int index = dbhdr->count;

	struct employee_t*e = *employees;
	e = realloc(e, sizeof(struct employee_t)*index+1);
	if (e == NULL) {
		printf("Realloc failed to assign new memory for the employee addition.\n");
		return STATUS_ERROR;
	}

	strncpy(e[index].name, name, sizeof(e[index].name)-1);
	strncpy(e[index].address, address, sizeof(e[index].address)-1);
	e[index].hours = atoi(hours);

	dbhdr->count++;

	*employees = e;

	return STATUS_SUCCESS;
}

int read_employees(int fd, struct dbheader_t *dbhdr, struct employee_t **employeesOut) {

	if (fd < 0) {
		printf("FATAL ERROR: Invalid File descriptor.");
		return STATUS_ERROR;
	}

	int count = dbhdr->count;

	struct employee_t*employees = calloc(1, sizeof(struct employee_t));
	if (employees == NULL) {
		printf("FATAL ERROR: Calloc failed to read the employees");
		return STATUS_ERROR;
	}

	int i = 0;
	for (;i < count; i++) employees[i].hours = ntohl(employees[i].hours);

	*employeesOut = employees;
	return STATUS_SUCCESS;
}

int output_file(int fd, struct dbheader_t *dbhdr, struct employee_t *employees) {

	if (fd < 0) {
		printf("FATAL ERROR: Invalid File descriptor.");
		return STATUS_ERROR;
	}

	int realcount = dbhdr->count;

	dbhdr->magic = htonl(dbhdr->magic);
	dbhdr->version = htons(dbhdr->version);
	dbhdr->count = htons(dbhdr->count);
	dbhdr->filesize = htonl(sizeof(struct dbheader_t) + sizeof(struct employee_t) * realcount);

	lseek(fd, 0, SEEK_SET);

	write(fd, dbhdr, sizeof(struct dbheader_t));

	int i = 0;
	for (; i < realcount; i++) {
		employees[i].hours = htonl(employees[i].hours);
		write(fd, &employees[i], sizeof(struct employee_t));
	}

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

int create_db_header(struct dbheader_t **headerOut) {

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
