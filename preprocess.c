#include "types.h"
#include "stat.h"
#include "user.h"
// preprocess data.txt -Dalongword=bird
// preprocess data.txt -Dpartialarg=fullarg

char buf[512];

/*
changes:
1. find #define & values
2. changed preprocess to command preprocess

yet to do:
0. add detected values to array
    >> var1 = val1 ; var1 = val2 ; var2 = val3 ; >> output val 3

2. join lines
3. remove comments
*/

// void joinLine(int fd)     // per file
// {
//     // temp : current input 
//     printf("input : %s\n", buf);

//     // per line >> start of line
//     int startofline = 0;
//     int endofline = 0;
//     int removedchar = 0;
//     char newbuf[512];
//     while ((n = read(fd, buf, sizeof(buf))) > 0) 
//     {
//         // per char >> end of line
//         do {
//             endofline
//         } while(buf[endofline] != '\n'); // endofline now at char \n

//         // if last char before \n is '\' join next line, remove '/'
//         if(buf[endofline-1] == '\\') 
//         {
//             //copy til before / >> copy buf[startofline ~ endofline - 2] to newbuf [startofline ~ endofline - 2]
//             for(int i = startofline; i <= endofline; i++)
//                 newbuf[i - removedchar] = buf[i]
//             removedchar++;
//         }
//         // move on
//         endofline++;
//         startofline = endofline;
//     }

//     // temp : current input 
//     printf("output : %s\n
//             startofline = %d\n
//             endofline = %d\n
//             removedchar = %d", buf, startofline, endofline, removedchar);
//     exit();
// }

void readToBuf(int fd, int *bufSize) {
    int n;
    while ((n = read(fd, buf, sizeof(buf))) > 0)
        *bufSize = n;
}

void preprocessDefine(int fd, int *varCount, int *varSize, char **vars, char **vals, int n) 
{
    char bufvar [512];
    int ibufvar = -1;
    char bufval [512];
    int ibufval = -1;

    // update vars and vals

    // read whole file
    int i = 0;
    // printf(1, "n = %d\n", n);

    while(i < n)
    {
    // skip beginning white space
        // printf(1, "now checking i = %d\n", i);
        while (buf[i] == ' ')
            i++;
    // found #
        if((i == 0 || buf[i - 1] == '\n') && buf[i] == '#') 
        {
            // printf(1, "found #\n");
    // skip white space after #
            while (buf[i] == ' ')
                i++;
    // found define
            if(buf[i+1] == 'd' && buf[i+2] == 'e' && buf[i+3] == 'f' && buf[i+4] == 'i' && buf[i+5] == 'n' && buf[i+6] == 'e' && buf[i+7] == ' ')
            {
                // printf(1, "found define\n");
                i = i + 8;
    // find 2 inputs
                while (buf[i] == ' ')
                    i++;
                //find var
                while ((buf[i] >= 48 && buf[i] <= 57) || (buf[i] >= 65 && buf[i] <= 90) || (buf[i] >= 91 && buf[i] <= 122))
                {
                    ibufvar++;
                    bufvar[ibufvar] = buf[i];
                    i++;
                }
                //skip white space
                while (buf[i] == ' ')
                    i++;
                
                // find val
                while ((buf[i] >= 48 && buf[i] <= 57) || (buf[i] >= 65 && buf[i] <= 90) || (buf[i] >= 91 && buf[i] <= 122))
                {
                    ibufval++;
                    bufval[ibufval] = buf[i];
                    i++;
                }
                // printf(1, "after finding val, i = %d  n = %d\n", i, n);
                // printf(1, "bufvar : %s\nbufval : %s\n", bufvar, bufval);

    // check for input #
                if(ibufval == -1 || ibufval == -1) 
                {
                    printf(1, "preprocess usage: #define <var> <val>");
                    exit();
                }

    // check nothing else on the line
                while (buf[i] != '\n' && i != n)
                {
                    if(buf[i] != ' ')
                    {
                        printf(1, "preprocess usage: extra char in line");
                        exit();
                    }
                    i++;
                }
                
                    // i at end of line
                    //printf(1, "after finding end of line, i = %d  n = %d\n", i, n);

    // update vars and vals, correct size variables
                char *var = malloc(sizeof(char) * (ibufvar + 2));
                char *val = malloc(sizeof(char) * (ibufval + 2));

                for(int j = 0; j <= ibufvar; j++) 
                {
                    var[j] = bufvar[j];
                    bufvar[j] = '\0';
                }
                for(int j = 0; j <= ibufval; j++)
                {
                    val[j] = bufval[j];
                    bufval[j] = '\0';
                }
                var[ibufvar + 1] = '\0';
                val[ibufval + 1] = '\0';

                // // adjust size of vals and vars
                if(*varSize < *varCount + 1)
                {
                    char **tempvars;
                    char **tempvals;

                    if(*varSize == 0 || *varCount == 0)
                    {
                        tempvars = malloc(sizeof(var) * 4);
                        tempvals = malloc(sizeof(val) * 4);
                        *varSize = 4;
                        *varCount += 1;
                    }
                    else
                    {
                        tempvars = malloc(sizeof(vars) * 2);
                        tempvals = malloc(sizeof(vals) * 2);

                        for(int i = 0; i < *varCount; i++)
                        {
                            tempvars[i] = vars[i];
                            tempvals[i] = vals[i];
                        }

                        char **delvars = vars;
                        char **delvals = vals;

                        vars = tempvars;
                        vals = tempvals;

                        free(delvars);
                        free(delvals);

                        *varSize = (*varSize) * 2;
                        (*varCount)++; 
                    }
                    
                }

                // copy var & val to arrays
                vars[(*varCount) - 1] = var;
                vals[(*varCount) - 1] = val;
                
                // printf(1, "pdefine : vals[(*varCount) - 1] = %s\n", (vals[1]));
                printf(1, "pdefine : var : %s\nval : %s\n", var, val);
                // printf(1, "pdefine : varCount : %d\nvarSize : %d\n", *varCount, *varSize);
                // free(var);
                // free(val);
            }
        }
        else i++;
    }
}


// save A=B into vars and vals
void organizeArgs(int argc, char *argv[], char **vars, char **vals)
{
    int i, j, k, inVar;
    for (i = 2; i < argc; i++)
    {
        // Verify that arg is valid
        if (argv[i][0] != '-' || argv[i][1] != 'D' ||
            ((65 > argv[i][2] || argv[i][2] > 90) && (97 > argv[i][2] || argv[i][2] > 122) && (argv[i][2] != '_')))
        {
            printf(1, "Invalid format for argument.\nArguments must have form -D<var>=<val>.\n");
            exit();
        }
        // Allocating memory for deep copies of arguments
        vars[i - 2] = malloc(strlen(argv[i]) * sizeof(char));
        vals[i - 2] = malloc(strlen(argv[i]) * sizeof(char));
        vars[i - 2][0] = argv[i][2];
        // 3 is first position of variable in -D<var>=<val>
        j = 3;
        k = 1;
        inVar = 1;
        while (argv[i][j] != '\0')
        {
            char c = argv[i][j++];
            if (inVar)
            {
                if ((48 <= c && c <= 57) || (65 <= c && c <= 90) || (97 <= c && c <= 122) || (c == '_'))
                    vars[i - 2][k++] = c;
                else if (c == '=')
                {
                    inVar = 0;
                    k = 0;
                }
                else
                {
                    printf(1, "Invalid format for argument.\nArguments must have form -D<var>=<val>.\n");
                    exit();
                }
            }
            else
                vals[i - 2][k++] = c;
        }
    }
}
int checkVar(int *pos, int *activevarCount, char c, char *var, char *val)
{
    int k;
    if (*pos + 1 == strlen(var))
    {
        // Check if c is not a character in a valid C identifier
        if ((48 > c || c > 57) && (65 > c || c > 90) && (97 > c || c > 122) && (c != '_'))
        {
            (*activevarCount)--;
            printf(1, "%s", val);
            printf(1, "%c", c);
        }
        else
        {
            (*activevarCount)--;
            if (*activevarCount == 0)
            {
                printf(1, "%s", var);
                printf(1, "%c", c);
            }
        }
        // inVar[j] = 0;
        *pos = 0;
    }
    else if (var[*pos + 1] == c)
    {
        // Positions (2 - varPositions[j]) have been verified in argv[j + 2]
        (*pos)++;
        return 1;
    }
    else
    {
        // inVar[j] = 0;
        (*activevarCount)--;
        if (*activevarCount == 0)
            for (k = 0; k <= *pos; k++)
                printf(1, "%c", var[k]);
        *pos = 0;
        printf(1, "%c", c);
    }
    return 0;
}
void preprocessCommand(int fd, int varCount, char **vars, char **vals, int n)
{
    printf(1, "pcommand : in preprocessCommand : vals[(*varCount) - 1] = %s\n", (vals[1]));
    int i, j;
    // bool value that tracks if function is passing through a non-arg string
    int inStr = 0;
    int activevarCount = 0;
    // Pointer to bool values that track if function is passing through 1 or more args. >> varCount
    int *inVars = malloc((varCount) * sizeof(int));
    // Pointer to int values that track current position in each arg. >> varCount
    int *varPositions = malloc((varCount) * sizeof(int));

    for (i = 0; i < n; i++) // for each char
    {
        char c = buf[i];
        //printf(1, "made it to line 299\n");
        if (!inStr && activevarCount == 0) // at index inStr of buf
        {
            // Check if c is a character in a valid C identifier
            if ((48 <= c && c <= 57) || (65 <= c && c <= 90) || (97 <= c && c <= 122) || (c == '_'))
            {
                for (j = 0; j < varCount; j++) // check first letter of each var entry
                {
                    // printf(1, "argv[j][2] = %c & c = %c\n", argv[j][2], c);
                    //  Check if c matches the first character in arg j
                    if (c == vars[j][0])
                    {
                        inVars[j] = 1;
                        // The first charcter in var[j] has been verified
                        varPositions[j] = 0;
                        activevarCount++;
                        // printf(1, "In arg %d\n", j);
                        // printf(1, "activevarCount = %d\n", activevarCount);
                    }
                }
                //printf(1, "made it to line 319\n");
                if (activevarCount == 0)
                {
                    inStr = 1;
                    printf(1, "%c", c);
                }
            }
            else
                printf(1, "%c", c);
        }
        else if (activevarCount > 0)
        {
            for (j = 0; j < varCount; j++)
                if (inVars[j])
                    inVars[j] = checkVar(&(varPositions[j]), &activevarCount, c, vars[j], vals[j]);
        }
        else
        {
            if ((48 > c || c > 57) && (65 > c || c > 90) && (97 > c || c > 122))
                inStr = 0;
            printf(1, "%c", c);
        }
    }
    printf(1, "\n");
    free(inVars);
    free(varPositions);
    
    // Check if partial variable was missed at the end.
    // for()
}
int main(int argc, char *argv[])
{
    int i, fd;
    int varCount = argc - 2;
    char **vars = malloc((argc - 2) * sizeof(char *));
    char **vals = malloc((argc - 2) * sizeof(char *));
    if (argc == 1)
    {
        printf(1, "No input provided for preprocessing.");
        exit();
    }
    if ((fd = open(argv[1], 0)) < 0)
    {
        printf(1, "preprocess: cannot open%s\n", argv[1]);
        exit();
    }
    if (argc > 2)
    {
        vars = malloc((argc - 2) * sizeof(char *));
        vals = malloc((argc - 2) * sizeof(char *));
        organizeArgs(argc, argv, vars, vals);
    }

    // CHANGE
    int varSize = argc - 2;
    int bufSize = 0;
    readToBuf(fd, &bufSize);
    preprocessDefine(fd, &varCount, &varSize, vars, vals, bufSize);
    // printf(1, "main : made it back to main\n");
    // printf(1, "main : %s, %s\n", vals[0], vals[1]);
    // printf(1, "main : varCount = %d\n", varCount);
    preprocessCommand(fd, varCount, vars, vals, bufSize);
    // printf(1, "main : made it past preprocess\n");
    

    close(fd);
    for (i = 0; i < varSize; i++)
    {
        free(vars[i]);
        free(vals[i]);
    }
    free(vars);
    free(vals);
    printf(1, "\n");
    exit();
}