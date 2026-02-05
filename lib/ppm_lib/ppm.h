/* PPMLib is a ppm image reading and writing library written in C
 * Copyright (C) 2017  Jacob Massengill
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PPM_H
#define PPM_H
 
#include <stdio.h>
typedef unsigned char uchar;

typedef struct rgb {
   uchar R,G,B;
} RGB;

typedef struct ppm {
    int width, height;
    RGB *data;
}* PPM;

RGB new_RGB(uchar R, uchar G, uchar B);

//generates a new PPM object
PPM PPM_fnew(FILE *fp);
PPM PPM_snew(const char *fn);
PPM PPM_dnew(int w, int h);

void PPM_free(PPM image);

//ppm reading and writing
void PPM_fread(FILE *fp, PPM image);
void PPM_sread(const char *fn, PPM image);
void PPM_fwrite(FILE *fp, PPM image);
void PPM_swrite(const char *fn, PPM image);

//clears image to color
void PPM_apply_color(PPM image, RGB color);


unsigned char *PPM_ppm_to_array(PPM ppm);
PPM PPM_8bit_array_to_ppm(unsigned char *input_array, int height, int width);


#endif //PPM_H
