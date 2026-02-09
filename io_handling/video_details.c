#include "video_details.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


static void centroid_write_to_fp(FILE *fp, struct centroid c)
{
	// successful?
	fwrite(&c.successful, sizeof(bool), 1, fp);
	
	// attempted?
	fwrite(&c.attempted, sizeof(bool), 1, fp);

	// Centroid x (col) then y (row)
	fwrite(&c.centroid.x, sizeof(int), 1, fp);
	fwrite(&c.centroid.y, sizeof(int), 1, fp);

	// Technique
	fwrite(&c.technique, sizeof(int), 1, fp);

	return;
}


static struct centroid centroid_read_from_fp(FILE *fp)
{
	struct centroid out;
	//
	// successful?
	fread(&out.successful, sizeof(bool), 1, fp);
	
	// attempted?
	fread(&out.attempted, sizeof(bool), 1, fp);

	// Centroid x (col) then y (row)
	fread(&out.centroid.x, sizeof(int), 1, fp);
	fread(&out.centroid.y, sizeof(int), 1, fp);

	// Technique
	fread(&out.technique, sizeof(int), 1, fp);

	return out;
}


static void centroid_results_write_to_fp(FILE *fp, struct centroid_results cr)
{
	// target number of blocks
	fwrite(&cr.goal_centroid_n, sizeof(int), 1, fp);

	// found number of centroids
	fwrite(&cr.centroids_length, sizeof(int), 1, fp);

	// Now paste in all the centroids.
	for (int i=0; i<cr.centroids_length; ++i)
	{
		centroid_write_to_fp(fp, cr.centroids[i]);
	}
	return;
}


static struct centroid_results centroid_results_read_from_fp(FILE *fp)
{
	struct centroid_results out;

	// target number of blocks.
	fread(&out.goal_centroid_n, sizeof(int), 1, fp);
	
	// found number of centroids
	fread(&out.centroids_length, sizeof(int), 1, fp);

	// And then we have to loop through and actually find the centroids.
	out.centroids = (struct centroid *) malloc(out.centroids_length * sizeof(struct centroid));
	
	for(int i=0; i<out.centroids_length; ++i)
	{
		out.centroids[i] = centroid_read_from_fp(fp);
	}

	return out;
}



void VIDEO_DETAILS_write_to_file(const char *fn, struct VideoDetails vd)
{
	FILE *fp = fopen(fn, "wb");


	// write our version number:
	fwrite(&vd.version, sizeof(int), 1, fp);


	// write the filename length
	fwrite(&vd.video_fn.len, sizeof(int), 1, fp);

	// write the video filename
	fwrite(vd.video_fn.s, sizeof(char), vd.video_fn.len, fp);
	
	// print whether split to frames or not
	fwrite(&vd.split_to_frames, sizeof(bool), 1, fp);

	// frames length
	if (!vd.split_to_frames)
	{
		// We're done.
		fclose(fp);
		return;
	}
	fwrite(&vd.frames_fn_length, sizeof(int), 1, fp);

	for(int i=0; i<vd.frames_fn_length; ++i)
	{
		// write the filename len
		fwrite(&vd.frames_fn[i].len, sizeof(int), 1, fp);

		// write the filename
		fwrite(vd.frames_fn[i].s, sizeof(char), vd.frames_fn[i].len, fp);
	}
	
	for (int i=0; i<vd.frames_fn_length; ++i)
	{
		centroid_results_write_to_fp(fp, vd.centroids[i]);
	}

	fclose(fp);
}

struct VideoDetails VIDEO_DETAILS_read_from_file(const char *fn)
{
	struct VideoDetails out;

	FILE *fp = fopen(fn, "rb");

	// version
	fread(&out.version, sizeof(int), 1, fp);

	// filename length
	fread(&out.video_fn.len, sizeof(int), 1, fp);

	// then the video filename
	out.video_fn.s = (char *) malloc(sizeof(char) * out.video_fn.len);
	fread(out.video_fn.s, sizeof(char), out.video_fn.len, fp);

	// Have we already split it?
	fread(&out.split_to_frames, sizeof(bool), 1, fp);

	if (!out.split_to_frames)
	{
		// we're done.
		out.frames_fn_length = 0;
		out.centroids = NULL;
		out.frames_fn = NULL;
		fclose(fp);
		return out;
	}
	
	// frames length
	fread(&out.frames_fn_length, sizeof(int), 1, fp);

	// get the filenames
	out.frames_fn = (struct string *) malloc(sizeof(struct string)*out.frames_fn_length);
	for(int i=0; i<out.frames_fn_length; ++i)
	{
		// read the length
		fread(&out.frames_fn[i].len, sizeof(int), 1, fp);
		out.frames_fn[i].s = (char *) malloc(sizeof(char)*out.frames_fn[i].len);
		fread(out.frames_fn[i].s, sizeof(char), out.frames_fn[i].len, fp);
	}

	out.centroids = (struct centroid_results *) malloc(sizeof(struct centroid_results) * out.frames_fn_length);
	for(int i=0; i<out.frames_fn_length; ++i)
	{
		out.centroids[i] = centroid_results_read_from_fp(fp);
	}




	fclose(fp);

	return out;
}
