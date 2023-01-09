#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[])
{
    int numeric = 0;

    if(argc == 2 && strcmp(argv[1],"-n") == 0){
        numeric = 1;
    }else if(argc != 1){
        printf("Error: Invalid Param(s): %s\n", argv[1]);
        return 1;
    }

    int c = 20; //initial array size
    int numInputs = 0;


    char **elements; //elements array
    char input[100];

    fflush(NULL);

    elements = (char**) malloc(sizeof(char*) * c);

    while(fgets(input,100, stdin) != NULL){
            int length = strlen(input);
            char* tempElement = (char*) malloc(sizeof(char) * length);
            
            strncpy(tempElement, input, length);
            tempElement[length-1] = '\0';
            elements[numInputs] = tempElement;
            
            numInputs++;
            if(numInputs >= c)
            {
                c += 20;
                elements = (char**) realloc(elements, c * sizeof(char*));
            }
    }    

    int currpos = 0;
    char* holder;
    int max = numInputs;

    while(max > 1){ //sort array of numbers
        currpos = 0;
        do{
            holder = 0;
            if((numeric && atoi(elements[currpos]) > atoi(elements[currpos + 1])) || (!numeric && (strcasecmp(elements[currpos], elements[currpos+1])>0)))
                {
                    holder = elements[currpos + 1];
                    elements[currpos + 1] = elements[currpos];
                    elements[currpos] = holder;
                }
                currpos++;
        } while((currpos + 1) < max);
        max--;
    }

    for (int i = 0; i < numInputs; i++){
    printf("%s", elements[i]);
    printf("\n");
    free(elements[i]);
    }

    free(elements);     //freeing the number array that was allocated
    
    return 0;

}
