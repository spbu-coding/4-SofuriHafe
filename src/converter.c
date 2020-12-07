#include "lib.c"

int tinker_arguments(int argc, char* argv[], int *is_mine_flag)
{
    if(argc != 4)
    {
        printf("\n3 arguments are required\n");

        return -1;
    }

    if(strcmp(argv[1], "--mine") == 0) *is_mine_flag = 1;

    else if(strcmp(argv[1], "--theirs") == 0) *is_mine_flag = 0;

    else
    {
        printf("\nWeird arguments\n");

        return -1;
    }

    for(int j = 2; j < 4; j++)
    {
        int exp_index = 0;

        for(int i = 0; i < strlen(argv[j]); i++) if(argv[j][i] == '.') exp_index = i + 1;

        if(exp_index != 0)
        {
            if(argv[j][exp_index] != 'b')
            {
                printf("\n.bmp only!\n");

                return -1;
            }

            if(argv[j][exp_index + 1] != 'm')
            {
                printf("\n.bmp only!\n");

                return -1;
            }

            if(argv[j][exp_index + 2] != 'p')
            {
                printf("\n.bmp only!\n");

                return -1;
            }
        }

        else
        {
            printf("\n.bmp only!\n");

            return -1;
        }
    }

    return 0;
}

int convert_with_my_method(char* argv[])
{
    struct BMP_FILE bmp_info;

    FILE* file;

    file = fopen(argv[2], "rb+");

    int return_value = get_bmp_info(file, &bmp_info);

    if(return_value != 0) return return_value;

    fseek(file, 0, SEEK_SET);

    FILE *write_file;

    write_file = fopen(argv[3], "wb+");

    return_value= convert_bmp(file, write_file, &bmp_info);

    if(return_value != 0) return return_value;

    return 0;
}

int main(int argc, char* argv[])
{
    int is_mine_flag;
    int return_value_my_method;
    int return_value_args = tinker_arguments(argc, argv, &is_mine_flag);

    if(return_value_args) return return_value_args;

    if(is_mine_flag)
    {
        return_value_my_method = convert_with_my_method(argv);

        if(return_value_my_method) return return_value_my_method;
    }

    else convert_with_qdbmp_method(argv);

    return 0;
}
