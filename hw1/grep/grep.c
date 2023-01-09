#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int main(int argc, char *argv[])
{
    // make sure to consider -i as input
    //Make sure input is stored correctly and shit
    bool flag = false;
    if (argc == 3 && strcmp(argv[1], "-i") == 0)
    {
        flag = true;
    }
    else if (argc != 2)
    {
        printf("Error: Invalid Param(s): %s\n", argv[1]);
        return 1;
    }
    size_t fLength = 0;
    if (flag)
    {
        char temp[20];
        strcpy(temp, argv[2]);
        while (temp[fLength])
        {
            if (temp[fLength])
                fLength++;
            else
                break;
        }
    }
    else
    {
        char temp[20];
        strcpy(temp, argv[1]);
        while (temp[fLength])
        {
            if (temp[fLength])
                fLength++;
            else
                break;
        }
    }
    // printf("%ld",fLength);
    char *filter = malloc((fLength + 1) * sizeof(char));
    // char filter[30];
    strcpy(filter, flag ? argv[2] : argv[1]);
    filter[fLength] = '\0';
    // printf("%s, %ld", filter, strlen(filter));
    // make a 2d pointer array. Look to matMul.c.
    // buff can stay as a placeholder
    char buf[256];
    int tempL = 1;
    char **list = (char **)malloc(tempL * sizeof(char *));

    // this while loop scans for strings of 256 bytes in length
    size_t pos = 0;
    while ((fgets(buf, 256, stdin)))
    {
        int length = strlen(buf);
        char *tempElement = (char *)malloc(sizeof(char) * length);

        strncpy(tempElement, buf, length);
        tempElement[length - 1] = '\0';
        // list[pos++] = tempElement;
        // // basically makes list 1 element bigger than requires.
        // list = (char **)(realloc(list, (pos + 1) * sizeof(char *)));
        if (flag)
        {
            // for (size_t t = 0; t < pos; t++)
            // {
            bool print = false;
            size_t place = 0;
            size_t fPlace = 0;
            while (tempElement[place] && filter[fPlace])
            { // checks if current char is lower or upper
                char temp1, temp2;

                temp1 = tempElement[place] < 97 ? tempElement[place++] + 32 : tempElement[place++];
                temp2 = filter[fPlace] < 97 ? filter[fPlace] + 32 : filter[fPlace];
                if (temp1 == temp2)
                {
                    fPlace++;
                }
                if (fPlace == fLength)
                {
                    print = true;
                    break;
                }
                if (temp1 != temp2)
                {
                    fPlace = 0;
                    // break;
                }
            }
            if (print)
            {
                for (size_t x = 0; x < strlen(tempElement); x++)
                {
                    printf("%c", tempElement[x]);
                }
                printf("\n");
            }
            free(tempElement);
            // }
        }
        else
        {
            // for (size_t t = 0; t < pos; t++)
            // {
            bool print = false;
            size_t place = 0;
            size_t fPlace = 0;
            while (tempElement[place] && filter[fPlace])
            { 
                char temp1, temp2;

                temp1 = tempElement[place++];
                temp2 = filter[fPlace];
                if (temp1 == temp2)
                {
                    fPlace++;
                }
                if (fPlace == fLength)
                {
                    print = true;
                    break;
                }
                if (temp1 != temp2)
                {
                    fPlace = 0;
                }
            }

            if (print)
            {
                for (size_t x = 0; x < strlen(tempElement); x++)
                {
                    printf("%c", tempElement[x]);
                }
                printf("\n");
            }

            free(tempElement);
            // }
        }
    }

    for (unsigned char i = 0; i < pos; i++)
        free(list[i]);

    free(list);
    free(filter);
    // printf("\n\nHello WOrld");
}