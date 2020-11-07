#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "negative.c"

#define MINE_ARG "--mine"
#define THEIRS_ARG "--theirs"

#define PALETTE_BYTES 4

enum ERRORS
{
    NO_ERROR,
    FORMAT_ERROR,
    SIZE_ERROR,
    RESERVED_ERROR,
    VERSION_ERROR,
    WIDTH_ERROR,
    HEIGHT_ERROR,
    PLANES_ERROR,
    BITS_PER_PIXEL_ERROR,
    COMPRESSION_ERROR,
    PIXELS_SIZE_ERROR,
    TABLE_COLOR_ERROR1,
    TABLE_COLOR_ERROR2,
    TABLE_COLOR_ERROR3,
    TABLE_COLOR_ERROR4,
};

typedef struct image_bitmap_info_header
{
    short format;
    int file_size;
    short reserved1;
    short reserved2;
    int pixel_array_off_set;
    int version_header;
    int width;
    int height;
    short planes;
    short bits_per_pixel;
    int compression;
    int size_pixels_file;
    int hor_res_image;
    int ver_res_image;
    int number_of_colors;
    int number_of_important_colors;
} IMAGE_BITMAP_INFO_HEADER;

typedef struct image
{
    IMAGE_BITMAP_INFO_HEADER *meta_data_header;
    int *palette;
    int *array_of_pixels;
} IMAGE;

int convert_with_my_method(char *input_name, char *output_name, enum ERRORS *error_type);

long long int calc_file_size(FILE *file)
{
    long long int file_size = 0;

    fseek(file, 0, SEEK_END);

    file_size = ftell(file);

    fseek(file, 0, SEEK_SET);
    
    return file_size;
}

IMAGE_BITMAP_INFO_HEADER *tinker_image_bitmapinfoheader(FILE *file, enum ERRORS *error_type)
{
    long long int file_size = calc_file_size(file);

    IMAGE_BITMAP_INFO_HEADER *image_bitmap_info_header = malloc(sizeof(IMAGE_BITMAP_INFO_HEADER));

    if (!image_bitmap_info_header)
    {
        fprintf(stdout, "Couldn't allocate memory\n");
        return NULL;
    }

    image_bitmap_info_header->format = 0;
    image_bitmap_info_header->file_size = 0;
    image_bitmap_info_header->reserved1 = 0;
    image_bitmap_info_header->reserved2 = 0;
    image_bitmap_info_header->pixel_array_off_set = 0;
    image_bitmap_info_header->version_header = 0;
    image_bitmap_info_header->width = 0;
    image_bitmap_info_header->height = 0;
    image_bitmap_info_header->planes = 0;
    image_bitmap_info_header->bits_per_pixel = 0;
    image_bitmap_info_header->compression = 0;
    image_bitmap_info_header->size_pixels_file = 0;

    fread(&image_bitmap_info_header->format, sizeof(image_bitmap_info_header->format), 1, file);

    if (image_bitmap_info_header->format != 0x4D42)
    {
        *error_type = FORMAT_ERROR;
        free(image_bitmap_info_header);
        return NULL;
    }

    fread(&image_bitmap_info_header->file_size, sizeof(image_bitmap_info_header->file_size), 1, file);

    if (file_size != image_bitmap_info_header->file_size)
    {
        *error_type = SIZE_ERROR;
        free(image_bitmap_info_header);
        return NULL;
    }

    fread(&image_bitmap_info_header->reserved1, sizeof(image_bitmap_info_header->reserved1), 1, file);
    fread(&image_bitmap_info_header->reserved2, sizeof(image_bitmap_info_header->reserved2), 1, file);

    if (image_bitmap_info_header->reserved1 != 0 || image_bitmap_info_header->reserved2 != 0)
    {
        *error_type = RESERVED_ERROR;
        free(image_bitmap_info_header);
        return NULL;
    }

    fread(&image_bitmap_info_header->pixel_array_off_set, sizeof(image_bitmap_info_header->pixel_array_off_set), 1, file);

    fread(&image_bitmap_info_header->version_header, sizeof(image_bitmap_info_header->version_header), 1, file);

    if (image_bitmap_info_header->version_header != 40)
    {
        *error_type = VERSION_ERROR;
        free(image_bitmap_info_header);
        return NULL;
    }

    fread(&image_bitmap_info_header->width, sizeof(image_bitmap_info_header->width), 1, file);

    if (image_bitmap_info_header->width <= 0)
    {
        *error_type = WIDTH_ERROR;
        free(image_bitmap_info_header);
        return NULL;
    }

    fread(&image_bitmap_info_header->height, sizeof(image_bitmap_info_header->height), 1, file);

    if (image_bitmap_info_header->height == 0)
    {
        *error_type = HEIGHT_ERROR;
        free(image_bitmap_info_header);
        return NULL;
    }

    fread(&image_bitmap_info_header->planes, sizeof(image_bitmap_info_header->planes), 1, file);

    if (image_bitmap_info_header->planes != 1)
    {
        *error_type = PLANES_ERROR;
        free(image_bitmap_info_header);
        return NULL;
    }

    fread(&image_bitmap_info_header->bits_per_pixel, sizeof(image_bitmap_info_header->bits_per_pixel), 1, file);

    if ((image_bitmap_info_header->bits_per_pixel != 8) && (image_bitmap_info_header->bits_per_pixel != 24))
    {
        *error_type = BITS_PER_PIXEL_ERROR;
        free(image_bitmap_info_header);
        return NULL;
    }

    fread(&image_bitmap_info_header->compression, sizeof(image_bitmap_info_header->compression), 1, file);

    if (image_bitmap_info_header->compression != 0)
    {
        *error_type = COMPRESSION_ERROR;
        free(image_bitmap_info_header);
        return NULL;
    }

    fread(&image_bitmap_info_header->size_pixels_file, sizeof(image_bitmap_info_header->size_pixels_file), 1, file);

    if (image_bitmap_info_header->size_pixels_file != 0)
    {
        int size_only_pixels = image_bitmap_info_header->file_size - image_bitmap_info_header->pixel_array_off_set;

        if (image_bitmap_info_header->size_pixels_file != size_only_pixels)
        {
            *error_type = PIXELS_SIZE_ERROR;
            free(image_bitmap_info_header);
            return NULL;
        }
    }

    fread(&image_bitmap_info_header->hor_res_image, sizeof(image_bitmap_info_header->hor_res_image), 1, file);
    fread(&image_bitmap_info_header->ver_res_image, sizeof(image_bitmap_info_header->ver_res_image), 1, file);
    fread(&image_bitmap_info_header->number_of_colors, sizeof(image_bitmap_info_header->number_of_colors), 1, file);

    if (image_bitmap_info_header->number_of_colors == 0 && image_bitmap_info_header->bits_per_pixel == 8)
    {
        *error_type = TABLE_COLOR_ERROR1;
        free(image_bitmap_info_header);
        return NULL;
    }

    if (image_bitmap_info_header->number_of_colors > 256 && image_bitmap_info_header->bits_per_pixel == 8)
    {
        *error_type = TABLE_COLOR_ERROR2;
        free(image_bitmap_info_header);
        return NULL;
    }

    if (image_bitmap_info_header->number_of_colors != 0 && image_bitmap_info_header->bits_per_pixel == 24)
    {
        *error_type = TABLE_COLOR_ERROR3;
        free(image_bitmap_info_header);
        return NULL;
    }

    fread(&image_bitmap_info_header->number_of_important_colors,
          sizeof(image_bitmap_info_header->number_of_important_colors), 1, file);

    if (image_bitmap_info_header->number_of_important_colors > 256 && image_bitmap_info_header->bits_per_pixel == 8)
    {
        *error_type = TABLE_COLOR_ERROR4;
        free(image_bitmap_info_header);
        return NULL;
    }

    return image_bitmap_info_header;
}

int *tinker_image_pixels(FILE *file, int width, int height, int bits_per_pixel)
{
    int bytes_per_pixel = bits_per_pixel / 8;

    height = abs(height);

    int *array_image_pixels = malloc(sizeof(int) * width * height * bytes_per_pixel);

    if (!array_image_pixels)
    {
        fprintf(stdout, "Couldn't allocate memory\n");
        return 0;
    }

    for (long long int i = 0; i < width * height * bytes_per_pixel; i++)
    {
        array_image_pixels[i] = 0;
        fread(&array_image_pixels[i], 1, 1, file);
    }

    return array_image_pixels;
}

int *tinker_image_palette(FILE *file, int number_of_colors)
{
    int size_table_colors = number_of_colors * PALETTE_BYTES;
    int *array_table_colors = malloc(sizeof(int) * size_table_colors);

    if (!array_table_colors)
    {
        fprintf(stdout, "Couldn't allocate memory\n");
        return 0;
    }

    for (long long int i = 0; i < size_table_colors; i++)
    {
        array_table_colors[i] = 0;
        fread(&array_table_colors[i], 1, 1, file);
    }

    return array_table_colors;
}

IMAGE *tinker_image(FILE *file, enum ERRORS *error_type)
{
    IMAGE *image = calloc(1, sizeof(IMAGE));

    if (!image)
    {
        fprintf(stdout, "Couldn't allocate memory\n");
        return NULL;
    }

    image->meta_data_header = tinker_image_bitmapinfoheader(file, error_type);

    if (!image->meta_data_header)
    {
        free(image);
        return NULL;
    }

    if (image->meta_data_header->number_of_colors != 0 && image->meta_data_header->bits_per_pixel == 8)
    {
        image->palette = tinker_image_palette(file, image->meta_data_header->number_of_colors);

        if (!image->palette)
        {
            free(image);
            return NULL;
        }
    }

    image->array_of_pixels = tinker_image_pixels(file, image->meta_data_header->width,
                                                       image->meta_data_header->height,
                                                       image->meta_data_header->bits_per_pixel);

    if (!image->array_of_pixels)
    {
        free(image);
        return NULL;
    }

    return image;
}

void convert(IMAGE *image_file)
{
    for (long long int i = 0;
         i < image_file->meta_data_header->width * abs(image_file->meta_data_header->height) *
             (image_file->meta_data_header->bits_per_pixel / 8); i++)
    {
        image_file->array_of_pixels[i] = ~image_file->array_of_pixels[i];
    }
}

void convert_palette(IMAGE *image_file)
{
    for (long long int i = 0; i < image_file->meta_data_header->number_of_colors * PALETTE_BYTES; i++)
    {
        if ((i + 1) % 4 != 0)
        {
            image_file->palette[i] = ~image_file->palette[i];
        }
    }
}

void create_new_file(IMAGE *image_file, char *output_name)
{
    FILE *new_file = fopen(output_name, "wb");
    fwrite(&image_file->meta_data_header->format, sizeof(image_file->meta_data_header->format), 1, new_file);
    fwrite(&image_file->meta_data_header->file_size, sizeof(image_file->meta_data_header->file_size), 1, new_file);
    fwrite(&image_file->meta_data_header->reserved1, sizeof(image_file->meta_data_header->reserved1), 1, new_file);
    fwrite(&image_file->meta_data_header->reserved2, sizeof(image_file->meta_data_header->reserved2), 1, new_file);
    fwrite(&image_file->meta_data_header->pixel_array_off_set,
           sizeof(image_file->meta_data_header->pixel_array_off_set), 1, new_file);
    fwrite(&image_file->meta_data_header->version_header, sizeof(image_file->meta_data_header->version_header), 1,
           new_file);
    fwrite(&image_file->meta_data_header->width, sizeof(image_file->meta_data_header->width), 1, new_file);
    fwrite(&image_file->meta_data_header->height, sizeof(image_file->meta_data_header->height), 1, new_file);
    fwrite(&image_file->meta_data_header->planes, sizeof(image_file->meta_data_header->planes), 1, new_file);
    fwrite(&image_file->meta_data_header->bits_per_pixel, sizeof(image_file->meta_data_header->bits_per_pixel), 1,
           new_file);
    fwrite(&image_file->meta_data_header->compression, sizeof(image_file->meta_data_header->compression), 1, new_file);
    fwrite(&image_file->meta_data_header->size_pixels_file, sizeof(image_file->meta_data_header->size_pixels_file), 1,
           new_file);
    fwrite(&image_file->meta_data_header->hor_res_image, sizeof(image_file->meta_data_header->hor_res_image), 1,
           new_file);
    fwrite(&image_file->meta_data_header->ver_res_image, sizeof(image_file->meta_data_header->ver_res_image), 1,
           new_file);
    fwrite(&image_file->meta_data_header->number_of_colors, sizeof(image_file->meta_data_header->number_of_colors), 1,
           new_file);
    fwrite(&image_file->meta_data_header->number_of_important_colors,
           sizeof(image_file->meta_data_header->number_of_important_colors), 1, new_file);

    if (image_file->meta_data_header->number_of_colors != 0 && image_file->meta_data_header->bits_per_pixel == 8)
    {
        for (long long int i = 0; i < image_file->meta_data_header->number_of_colors * PALETTE_BYTES; i++)
        {
            fwrite(&image_file->palette[i], 1, 1, new_file);
        }
    }

    for (long long int i = 0;
         i < image_file->meta_data_header->width * abs(image_file->meta_data_header->height) *
             (image_file->meta_data_header->bits_per_pixel / 8); i++)
    {
        fwrite(&image_file->array_of_pixels[i], 1, 1, new_file);
    }

    fclose(new_file);
}

void free_image(IMAGE *image)
{
    free(image->meta_data_header);

    free(image->palette);

    free(image->array_of_pixels);

    free(image);

    image = NULL;
}

int convert_with_my_method(char *input_name, char *output_name, enum ERRORS *error_type)
{
    FILE *file;

    if ((file = fopen(input_name, "rb")) == NULL)
    {
        fprintf(stdout, "Cannot open file. There is no such a file with the name %s", input_name);
        return -1;
    }

    IMAGE *image = tinker_image(file, error_type);

    if (!image)
    {
        fclose(file);
        return -1;
    }

    if (image->meta_data_header->number_of_colors != 0 && image->meta_data_header->bits_per_pixel == 8)
    {
        convert_palette(image);
    }

    else
    {
        convert(image);
    }

    create_new_file(image, output_name);

    free_image(image);

    fclose(file);
    return 0;
}

int main(int argc, char **argv)
{
    enum ERRORS error_type = NO_ERROR;

    int mine_flag = 0;
    int theirs_flag = 0;

    if (argc != 4)
    {
        fprintf(stdout, "Tip: Use %s --mine/--theirs <input file>.bmp <output file>.bmp\n", argv[0]);
        return -1;
    }

    if (strncmp(argv[1], MINE_ARG, strlen(MINE_ARG)) == 0)
    {
        mine_flag = 1;
    }

    else if (strncmp(argv[1], THEIRS_ARG, strlen(THEIRS_ARG)) == 0)
    {
        theirs_flag = 1;
    }

    if (mine_flag != 1 && theirs_flag != 1)
    {
        fprintf(stdout, "Tip: Use %s --mine/--theirs <input file>.bmp <output file>.bmp\n", argv[0]);
        return -1;
    }

    if (mine_flag == 1)
    {
        if (convert_with_my_method(argv[2], argv[3], &error_type))
        {
            if (error_type == FORMAT_ERROR)
            {
                fprintf(stdout, "BMP file format is supported only");
                return -1;
            }

            else if (error_type == SIZE_ERROR)
            {
                fprintf(stdout, "Size in the metadata does not match the actual one");
                return -2;
            }

            else if (error_type == RESERVED_ERROR)
            {
                fprintf(stdout, "Reserved bytes are not 0");
                return -2;
            }

            else if (error_type == VERSION_ERROR)
            {
                fprintf(stdout, "BMP v3 is supported only");
                return -2;
            }

            else if (error_type == WIDTH_ERROR)
            {
                fprintf(stdout, "Width should be a positive number");
                return -2;
            }

            else if (error_type == HEIGHT_ERROR)
            {
                fprintf(stdout, "Height shouldn't be 0");
                return -2;
            }

            else if (error_type == PLANES_ERROR)
            {
                fprintf(stdout, "The number of planes must be 1");
                return -2;
            }

            else if (error_type == BITS_PER_PIXEL_ERROR)
            {
                fprintf(stdout,
                        "8 and 24 bits per pixel are supported only");
                return -2;
            }

            else if (error_type == COMPRESSION_ERROR)
            {
                fprintf(stdout, "Compression isn't supported");
                return -2;
            }

            else if (error_type == PIXELS_SIZE_ERROR)
            {
                fprintf(stdout,
                        "The size of the pixel data from metadata does not match the actual size");
                return -2;
            }

            else if (error_type == TABLE_COLOR_ERROR1)
            {
                fprintf(stdout,
                        "Images with =< 8 bits always have a color table");
                return -2;
            }

            else if (error_type == TABLE_COLOR_ERROR2)
            {
                fprintf(stdout,
                        "Images with =< 8 bits always use no more then 256 colors in palette");
                return -2;
            }

            else if (error_type == TABLE_COLOR_ERROR3)
            {
                fprintf(stdout,
                        "Images with 24 bits shouldn't have a palette");
                return -2;
            }

            else if (error_type == TABLE_COLOR_ERROR4)
            {
                fprintf(stdout,
                        "Images with =< 8 bits always use no more then 256 important colors in palette");
                return -2;
            }

            else return -1;
        }
    }

    else if (theirs_flag == 1)
    {
        if (convert_with_qdbmp_method(argv) != 0) return -3;
    }
    
    return 0;
}