#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char *argv[])
{
    int input = atoi(argv[1]);
    // repeat until n is odd
    // while (input % 2)
    // {
    //     printf("%d ", 2);
    //     input /= 2;
    // }
    // // n is now odd, so can skip even numbers
    // for (int i = 3; i <= sqrt(input); i += 2)
    // {
    //     while (input % i)
    //     {
    //         printf("%d ", i);
    //         input /= i;
    //     }
    // }
    // // when n is a prime number  larger than 2
    // if (input > 2)
    //     printf("%d ", input);

    int i = 2;
    while (i * i <= input)
    {
        if (input % i)
            i += 1;
        else
        {
            input /= i;
            printf("%d ", i);
        }
    }
    if (input > 1)
        printf("%d", input);
    //  return factors
}