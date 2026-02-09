#ifndef MISC_IO_H
#define MISC_IO_H

#include <stddef.h>
#include "types.h"
#include <stdbool.h>


struct string_arr IO_get_directory_files(struct string path, bool just_filenames);

struct string_arr IO_filter_files_ext(struct string_arr files);


#endif
