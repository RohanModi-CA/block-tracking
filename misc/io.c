#include "io.h"
#include <dirent.h>
#include <stdlib.h>
#include "types.h"
#include <stdbool.h>
#include <string.h>

// thanks gemini
struct string_arr IO_get_directory_files(struct string path, bool just_filenames)
{
    // Validate path ends with '/'
    if (path.len == 0 || path.s[path.len - 1] != '/')
    {
        fprintf(stderr, "Error: Path must end with '/'\n");
        exit(1);
    }

    DIR *dir = opendir(path.s);
    if (!dir)
    {
        perror("opendir");
        exit(1);
    }

    struct string *files = NULL;
    int count = 0;
    struct dirent *ent;

    // First Pass: Collect all valid filenames
    while ((ent = readdir(dir)) != NULL)
    {
        // Skip "." and ".."
        if (ent->d_name[0] == '.')
        {
            if (ent->d_name[1] == '\0' || (ent->d_name[1] == '.' && ent->d_name[2] == '\0'))
            {
                continue;
            }
        }

        struct string *tmp = realloc(files, sizeof(struct string) * (count + 1));
        if (!tmp)
        {
            perror("realloc");
            exit(1);
        }
        files = tmp;

        struct string fname = new_string(ent->d_name);
        
        if (just_filenames)
        {
            files[count] = fname;
        }
        else
        {
            files[count] = string_concat(path, fname);
            string_free(fname); // Clean up the temporary filename string
        }
        
        count++;
    }

    closedir(dir);

    // Sort the resulting array alphabetically
    if (count > 0)
    {
        qsort(files, count, sizeof(struct string), string_compare);
    }

	struct string_arr out;
	out.n = count;
	out.arr = files;
	return out;
}

struct string_arr IO_filter_files_ext(struct string_arr files)
{
	struct string_arr out;

	// for now we'll just worst case malloc.
	out.arr = (struct string *) malloc(sizeof(struct string) * files.n);

	for(int i=0; i<files.n; ++i)
	{
		int ext_len = strlen(".vdts");
		if (files.arr[i].len < ext_len){continue;}

	}
}

