#include "lib.c"

#define PALETTE_SIZE 256

int main(int argc, char* argv[])
{
    FILE *file1, *file2;

    file1 = fopen(argv[1], "rb");
    file2 = fopen(argv[2], "rb");

    struct BMP_FILE bmp_info1, bmp_info2;
    int return_value = get_bmp_info(file1, &bmp_info1);

    if(return_value)
    {
        printf("\nUnable to open the first file\n");

        return -1;
    }

    return_value = get_bmp_info(file2, &bmp_info2);

    if(return_value)
    {
        printf("\nUnable to open the second file\n");

        return -1;
    }

    return_value = compare_info(&bmp_info1, &bmp_info2);

    if(return_value) return -1;

    if(!bmp_info1.number_of_colors) bmp_info1.number_of_colors = PALETTE_SIZE;

    unsigned int palette_size = bmp_info1.number_of_colors;

    return_value = compare_pixels(file1, file2, &bmp_info1, &palette_size);

    if(return_value) return -1;

    return 0;
}
