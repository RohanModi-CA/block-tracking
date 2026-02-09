#include "types.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


// if index is positive, returns index, if negative, does standard wraparound.
static int index_wraparound(int index, int arr_len)	
{
	if(index>=0){return index;}

	// It's negative:
	if(index <= )

}


struct string new_string(const char *arr)
{
	struct string out;
	out.len = strlen(arr);
	out.s = (char *) malloc((1+out.len) * sizeof(char));
	memcpy(out.s, arr, sizeof(char)*out.len);
	out.s[out.len] = '\0';

	return out;
}

struct string string_concat(struct string s1, struct string s2)
{
	struct string out;
	out.len = s1.len + s2.len;

	out.s = (char *) malloc(sizeof(char)*(out.len + 1));

	memcpy(out.s, s1.s, s1.len * sizeof(char));
	memcpy(out.s + s1.len*sizeof(char), s2.s, s2.len *sizeof(char));
	out.s[out.len] = '\0';
	return out;
}

void string_free(struct string s)
{
	// Just free it.
	free(s.s);
}


void string_write_to_fp(FILE *fp, struct string s)
{
	// The pattern for this is write the length INCLUDING 
	// null terminator (so s.len+1) for compatibility,
	// and then write the string.
	
	int len_with_null_terminator = s.len + 1;

	fwrite(&len_with_null_terminator, sizeof(int), 1, fp);

	// Then write the char array, noting s.s includes null terminator. at s.s[s.len]
	fwrite(s.s, sizeof(char), len_with_null_terminator, fp);

	return;
}

struct string string_read_from_fp(FILE *fp)
{
	// The inverse of the above;
	struct string out;
	int len_with_null_terminator;

	fwrite(&len_with_null_terminator, sizeof(int), 1, fp);
	out.len = len_with_null_terminator - 1;

	out.s = (char *) malloc(sizeof(char)*len_with_null_terminator);

	fwrite(out.s, sizeof(char), len_with_null_terminator, fp);

	return out;
}


int string_compare(const void *a, const void *b)
{
    const struct string *strA = (const struct string *)a;
    const struct string *strB = (const struct string *)b;
    return strcmp(strA->s, strB->s);
}

void string_array_free(struct string_arr arr)
{
	for(int i=0; i<arr.n; ++i)
	{
		string_free(arr.arr[i]);
	}
	return;
}

struct string string_substring(struct string s, int start, int end)
{
	struct string out;

	// prevent going past the end
	end = (end > s.len) ? s.len-1 : end;



}

