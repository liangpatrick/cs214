#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[])
{

    if(argc > 1){   //if any params are included program will not run
        printf("Error: Invalid Param(s): %s\n", argv[1]);
        return 1;
    }

    int c = 20; //initial array size
    int numInputs = 0;


    char **elements; //elements array
    char input[100];

    fflush(NULL);

    elements = (char**) malloc(sizeof(char*) * c);

    while(fgets(input,100, stdin) != NULL){     //read input until ctrl-D
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

    int uniqElements = 0;       //total number of uniq elements (matches)
    char **uniq = (char**) malloc(sizeof(char*) * numInputs); //array of uniq elements (matches)
    char *curUniq;      //current element being matched
    int curUniqCount = 1;   //occurances of element being matched

    if(numInputs > 0){      //compares curUniq with current str and changes it if not matched
        curUniq = (char*) malloc(sizeof(char) * ((int)strlen(elements[0]) + 1));
        curUniq = strcpy(curUniq,elements[0]);

        for(int i = 1; i < numInputs; i++){
            if(strcmp(elements[i], curUniq) == 0){
                curUniqCount++;
            }else{
                uniq[uniqElements] = (char*) malloc(sizeof(char) * (strlen(curUniq) + 12));
                sprintf(uniq[uniqElements],"%d %s",curUniqCount,curUniq);
                uniqElements++;

                curUniqCount = 1;
                curUniq = (char*) realloc(curUniq, sizeof(char) * (strlen(elements[i])+1));
                curUniq = strcpy(curUniq,elements[i]);
                }
        }
        uniq[uniqElements] = (char*) malloc(sizeof(char) * (strlen(curUniq) + 12));
        sprintf(uniq[uniqElements],"%d %s",curUniqCount,curUniq);
        uniqElements++;
    }

    for (int i = 0; i < numInputs; i++){
        free(elements[i]);
        if(i < uniqElements){
            printf("%s", uniq[i]);
            printf("\n");
            free(uniq[i]);
        }
    }

    free(curUniq);
    free(elements);     //freeing the arrays allocated
    free(uniq);
    
    return 0;

}