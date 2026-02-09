#ifndef VIDEO_DETAILS_H
#define VIDEO_DETAILS_H
#include <stdbool.h>
#include "../image_processing/IP_types.h"


struct string
{
	int len;
	char *s;
};



struct centroid
{
	bool attempted;
	bool successful;
	struct int_xy centroid;
	int technique;
};


struct centroid_results
{
	int goal_centroid_n;
	int centroids_length;
	struct centroid *centroids;
};

struct VideoDetails
{
	// version number, why not.
	int version;
	// should we store the relative or absolute link?
	// Not sure. Maybe both? absolute is useless
	// for shared, but makes things simpler... or we
	// could hash the videos? but that will take a while, 
	// which could be annoying, but maybe better than ffmpeg
	// or filename compare. For now we'll just store
	// a filename, of any type.
	//
	// please null terminate... please...
	struct string video_fn;
	// One of the first operations we'll do is 
	// split the video file into its constituent
	// ppms, which is a costly operation,
	// so we'd definitely like to cache the outputs
	// so maybe we'll keep this all local based.
	// So we will store absolute links to the
	bool split_to_frames;
	int frames_fn_length;
	struct string *frames_fn;

	// Then the next thing is the actual centroid results. Each of these should probably be in their own struct?
	// We will pre-allocate this for all frames and then within the struct
	// we determine whether we've attempted it yet
	struct centroid_results *centroids;
};



#endif


