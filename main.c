#include "image_processing/frame_main.h"
#include "image_processing/IP_types.h"


/* thanks chat */
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int cmp_str(const void *a, const void *b)
{
    const char *sa = *(const char **)a;
    const char *sb = *(const char **)b;
    return strcmp(sa, sb);
}

/*
 * Returns number of files returned (<= max_n).
 * On success, *out_names points to an array of malloc'd strings.
 * Caller must free each string and the array.
 */
size_t get_first_n_files_sorted(
    const char *path,
    size_t max_n,
    char ***out_names
)
{
    DIR *dir = opendir(path);
    if (!dir) return 0;

    char **names = NULL;
    size_t count = 0;

    struct dirent *ent;
    while ((ent = readdir(dir))) {
        if (ent->d_name[0] == '.')
            continue;

        char **tmp = realloc(names, (count + 1) * sizeof *names);
        if (!tmp) break;
        names = tmp;

        names[count++] = strdup(ent->d_name);
    }

    closedir(dir);

    qsort(names, count, sizeof *names, cmp_str);

    if (count > max_n)
        count = max_n;

    *out_names = names;
    return count;
}
// ---



int main() 
{ 
	
	//struct int_xy centroid = frame_to_centroid("/home/gram/Documents/FileFolder/Projects/blocks/resources/vid/frames/000000121.ppm");
	// printf("X:%d, Y: %d\n",centroid.x, centroid.y);
	//
	//

	char **files = NULL;
	size_t n = get_first_n_files_sorted("resources/vid/frames", 500, &files);


	//struct int_xy centroid = frame_to_centroid("resources/vid/frames/000000001.ppm");


	for(int i=0; i<n; ++i)
	{
		printf("%s\n", files[i]);
		
		char buf[100] = "resources/vid/frames/";
		strcat(buf, files[i]);
		struct int_xy centroid = frame_to_centroid(buf);
		printf("X:%d, Y: %d\n",centroid.x, centroid.y);
	}

}
