#ifndef MISC_TYPES_H
#define MISC_TYPES_H

#include <stdio.h>

struct string
{
	int len;
	char *s;
};


struct string_arr
{
	int n;
	struct string *arr;
};


struct string new_string(const char *arr);

struct string string_concat(struct string s1, struct string s2);

struct string string_substring(struct string s, int start, int end);

void string_free(struct string s);

void string_write_to_fp(FILE *fp, struct string s);

struct string string_read_from_fp(FILE *fp);

int string_compare(const void *a, const void *b);

void string_array_free(struct string_arr arr);

#endif
