#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    TABLE_COLOR_ERROR5
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
    int **array_of_pixels;
} IMAGE;

int compare(char *file_name1, char *file_name2, enum ERRORS *error_type);

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

int **tinker_image_pixels(FILE *file, int width, int height, int bits_per_pixel)
{
    int bytes_per_pixel = bits_per_pixel / 8;

    height = abs(height);

    int **array_image_pixels = malloc(sizeof(int *) * height);

    if (!array_image_pixels)
    {
        fprintf(stdout, "Couldn't allocate memory\n");
        return 0;
    }

    for (long long int i = 0; i < height; i++)
    {
        array_image_pixels[i] = malloc(sizeof(int) * width);

        if (!array_image_pixels[i])
        {
            for (int j = 0; j < i; j++)
            {
                free(array_image_pixels[i]);
            }

            free(array_image_pixels);
            return 0;
        }
    }

    for (long long int i = 0; i < height; i++)
    {
        for (long long int j = 0; j < width; j++)
        {
            array_image_pixels[i][j] = 0;
            fread(&array_image_pixels[i][j], bytes_per_pixel, 1, file);
        }
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

    for (long long int i = 0; i < number_of_colors; i++)
    {
        array_table_colors[i] = 0;
        fread(&array_table_colors[i], PALETTE_BYTES, 1, file);
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

void free_image(IMAGE *image)
{
    for (long long int i = 0; i < abs(image->meta_data_header->height); i++)
    {
        free(image->array_of_pixels[i]);
    }

    free(image->array_of_pixels);

    free(image->meta_data_header);

    free(image->palette);

    free(image);

    image = NULL;
}

void compare_pixels(IMAGE *image, IMAGE *image2)
{
    int counter = 0;

    if (image->meta_data_header->height > 0 && image2->meta_data_header->height > 0)
    {
        for (long long int i = 0; i < image->meta_data_header->height; i++)
        {
            for (long long int j = 0; j < image->meta_data_header->width; j++)
            {
                if (counter < 100 && image->array_of_pixels[i][j] != image2->array_of_pixels[i][j])
                {
                    if (image->meta_data_header->height > 0 && image2->meta_data_header->height > 0)
                    {
                        fprintf(stderr, "X: %lld Y: %lld \n", j, i);
                    }

                    counter++;
                }
            }
        }
    }

    else if (image->meta_data_header->height < 0 && image2->meta_data_header->height < 0)
    {
        for (long long int i = 0; i < abs(image->meta_data_header->height); i++)
        {
            for (long long int j = 0; j < image->meta_data_header->width; j++)
            {
                if (counter < 100 && image->array_of_pixels[abs(image->meta_data_header->height) - i - 1][j] !=
                                   image2->array_of_pixels[abs(image2->meta_data_header->height) - i - 1][j])
                {
                    fprintf(stderr, "X: %lld Y: %lld \n", j, i);
                    counter++;
                }
            }
        }
    }

    else if (image->meta_data_header->height > 0 && image2->meta_data_header->height < 0)
    {
        for (long long int i = 0; i < image->meta_data_header->height; i++)
        {
            for (long long int j = 0; j < image->meta_data_header->width; j++)
            {
                if (counter < 100 && image->array_of_pixels[i][j] !=
                                   image2->array_of_pixels[abs(image2->meta_data_header->height) - i - 1][j])
                {
                    fprintf(stderr, "X: %lld Y: %lld \n", j, i);
                    counter++;
                }
            }
        }
    }

    else if (image->meta_data_header->height < 0 && image2->meta_data_header->height > 0)
    {
        for (long long int i = 0; i < image2->meta_data_header->height; i++)
        {
            for (long long int j = 0; j < image->meta_data_header->width; j++)
            {
                if (counter < 100 && image->array_of_pixels[abs(image->meta_data_header->height) - i - 1][j] !=
                                   image2->array_of_pixels[i][j])
                {
                    fprintf(stderr, "X: %lld Y: %lld \n", j, i);
                    counter++;
                }
            }
        }
    }
}

int compare_tables(IMAGE *image, IMAGE *image2, enum ERRORS *error_type)
{
    for (long long int i = 0; i < image->meta_data_header->number_of_colors; i++) {
        if (image->palette[i] != image2->palette[i]) {
            *error_type = TABLE_COLOR_ERROR5;
            return -1;
        }
    }

    return 0;
}

int compare(char *file_name1, char *file_name2, enum ERRORS *error_type)
{
    FILE *file;
    FILE *file2;

    if ((file = fopen(file_name1, "rb")) == NULL)
    {
        fprintf(stdout, "Cannot open file. No file with name %s exists. ", file_name1);
        return -1;
    }

    IMAGE *image = tinker_image(file, error_type);

    if (!image)
    {
        fprintf(stdout, "Error for a file named %s. ", file_name1);
        fclose(file);
        return -1;
    }

    if ((file2 = fopen(file_name2, "rb")) == NULL) {
        fprintf(stdout, "Cannot open file. No file with name %s exists. ", file_name2);
        return -1;
    }

    IMAGE *image2 = tinker_image(file2, error_type);
    if (!image2) {
        fprintf(stdout, "Error for a file named %s. ", file_name2);
        fclose(file2);
        return -1;
    }

    if ((image->meta_data_header->width != image2->meta_data_header->width) ||
        (abs(image->meta_data_header->height) != abs(image2->meta_data_header->height))) {
        fprintf(stdout, "Images have different values of width or height. ");
        free_image(image);
        free_image(image2);
        fclose(file);
        fclose(file2);
        return -1;
    }

    else if ((image->meta_data_header->file_size != image2->meta_data_header->file_size)) {
        fprintf(stdout, "Images have different values of sizes. ");
        free_image(image);
        fclose(file);
        free_image(image2);
        fclose(file2);
        return -1;
    }

    else if ((image->meta_data_header->size_pixels_file != image2->meta_data_header->size_pixels_file)) {
        fprintf(stdout, "Images have different values of pixels data size. ");
        free_image(image);
        fclose(file);
        free_image(image2);
        fclose(file2);
        return -1;
    }

    else if ((image->meta_data_header->bits_per_pixel != image2->meta_data_header->bits_per_pixel)) {
        fprintf(stdout, "Images have different values of bits_per_pixel. ");
        free_image(image);
        fclose(file);
        free_image(image2);
        fclose(file2);
        return -1;
    }

    else if ((image->meta_data_header->number_of_colors != image2->meta_data_header->number_of_colors)) {
        fprintf(stdout, "Images have different number of colors in the table. ");
        free_image(image);
        fclose(file);
        free_image(image2);
        fclose(file2);
        return -1;
    }

    else
    {
        if (!compare_tables(image, image2, error_type))
        {
            compare_pixels(image, image2);
        }

        else
        {
            free_image(image);
            fclose(file);
            free_image(image2);
            fclose(file2);
            return -1;
        }
    }
    free_image(image);
    fclose(file);
    free_image(image2);
    fclose(file2);

    return 0;
}

int main(int argc, char **argv)
{
    enum ERRORS error_type = NO_ERROR;

    if (argc != 3)
    {
        fprintf(stdout, "Tip: Use %s <file1_name>.bmp <file2_name>.bmp\n", argv[0]);
        return -1;
    }

    if (compare(argv[1], argv[2], &error_type))
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

        else if (error_type == TABLE_COLOR_ERROR5)
        {
            fprintf(stdout, "Palettes don't match");
            return -2;
        }

        else return -1;
    }

    return 0;
}