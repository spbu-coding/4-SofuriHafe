/**************************************************************

	QDBMP - Quick n' Dirty BMP

	v1.0.0 - 2007-04-07
	http://qdbmp.sourceforge.net


	The library supports the following BMP variants:
	1. Uncompressed 32 BPP (alpha values are ignored)
	2. Uncompressed 24 BPP
	3. Uncompressed 8 BPP (indexed color)

	QDBMP is free and open source software, distributed
	under the MIT licence.

	Copyright (c) 2007 Chai Braudo (braudo@users.sourceforge.net)

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.

**************************************************************/

/**************************************************************

	The program reads an BMP image file and creates a new
	image that is the negative of the input file.

**************************************************************/

#include "qdbmp.h"
#include <stdio.h>

/* Creates a negative image of the input bitmap file */
int convert_with_qdbmp_method(char *argv[])
{
    UCHAR r, g, b;
    UINT width, height;
    UINT x, y;
    BMP *bmp;

    /* Read an image file */
    bmp = BMP_ReadFile(argv[2]);
    BMP_CHECK_ERROR(stdout, -1);

    /* Get image's dimensions */
    width = BMP_GetWidth(bmp);
    height = BMP_GetHeight(bmp);

    /* Iterate through all the image's pixels */
    if (bmp->Header.BitsPerPixel == 24)
    {
        for (x = 0; x < width; ++x)
        {
            for (y = 0; y < height; ++y)
            {
                /* Get pixel's RGB values */
                BMP_GetPixelRGB(bmp, x, y, &r, &g, &b);

                /* Invert RGB values */
                BMP_SetPixelRGB(bmp, x, y, ~r, ~g, ~b);
            }
        }
    }

    else if (bmp->Header.BitsPerPixel == 8)
    {
        for (long long int i = 0; i < BMP_PALETTE_SIZE_8bpp; i++)
        {
            if ((i + 1) % 4 != 0)
            {
                bmp->Palette[i] = ~bmp->Palette[i];
            }
        }
    }

    /* Save result */
    BMP_WriteFile(bmp, argv[3]);
    BMP_CHECK_ERROR(stdout, -2);

    /* Free all memory allocated for the image */
    BMP_Free(bmp);

    return 0;
}

