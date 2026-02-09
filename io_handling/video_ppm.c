#include "video_ppm.h"
#include "video_details.h"
#include <stdio.h>
#include <stdlib.h>
#include "../misc/io.h"

// Okay, we're going to first check the output directory for a list of stuff. Okay, time to start to properly deal with misc.

// Takes the root_directory where we will be searching
// for our video_details files, which we will terminate in
// .vdts. ROOT DIR CONTAINS A TRAILING SLASH. 
static struct VideoDetails *vds(const struct string root_dir)
{
	if (root_dir.len <=0 ||  root_dir.s[root_dir.len-1]!='/')
	{
		printf("root dir must end in a /");
		exit(1);
	}

	// Okay now let's get all the files in this directory.
	int file_count;
	struct string *files = IO_get_directory_files(root_dir, false, &file_count);
	

}




