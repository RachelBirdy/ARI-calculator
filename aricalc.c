#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int errormsg(int errno) {
    printf("Error: ");
    switch(errno) {
        case 1 :
            printf("Too few arguments. Use '-h' to see help.\n");
            break;
        case 2 :
            printf("Filename not provided. Use '-h' to see help.\n");
        break;
        case 3 :
            printf("Error opening file. Check the file exists and that you have access to it. Use '-h' to see help.\n");
        default :
            printf("Undefined error.\n");
    }
    return errno;
}

int help() {
    printf("This program is a reading age calculator. It calculates the appropriate age for a certain text to be read by based on a short sample, using the number of characters, words, and sentences contained.\n\n");
    printf("The first argument provided should either be 'OPEN', to analyze a new sample, or 'FIND', to search the database for an existing sample, or '-h' to view this help screen.\n\n");
    printf("After this, the available options are:\n");
    printf("\t-n (name)\t\tThe name of the text. Any spaces should be preceded with a backslash. I.e. 'This\\ is\\ a\\ Name'\n");
    printf("\t-r (age)\t\tThe intended reading age of the text\n");
    printf("\t-c (Calculated age)\t(FIND ONLY) The calculated reading age to search for.\n");
    printf("\t-d (Database location)\tThe location of the database of sample text to search for. The default location if this isn't provided is './database'\n");
    printf("\t-k (keyword)\t\tA keyword associated with the text. This argument must be entered multiple times for multiple keywords.\n");
    printf("\t-f (filename)\t\t(OPEN ONLY)The filename for the sample of text to be analyzed.\n");
    printf("\nWhen using 'OPEN', the arguments '-n', '-r', '-k', and '-f' are mandatory.\n");
    printf("When using 'FIND', results matching all supplied arguments will be presented.\n");
    return 0;
}

int parse(int argc, char **argv, char ***keyword, int *keycount, char **name, char **filename, int *age, int *calcage, char** database) {
    int i = 0, j = 0, k = 0, l = 0, m = 0, len = 0, wordlen;
    char endchar;

    // Iterate through supplied arguments to find options
    for(i=1;i<argc-1;i++) {
        if(strncmp(argv[i],"-n",2) == 0) {
                    // Names can be several words long, separated by backslashes. Because C reads each word as a separate argument, we need
                    // to iterate through the list of arguments starting from '-n' twice. Once to determine how many arguments we need to read
                    // and how long the name is, then we assign enough memory to the name variable to store the name, then we iterate through
                    // it again to read the arguments into the name variable, replacing the backslash with a space in each one
            j=i;
            do {
                j++;
                len += strlen(argv[j]);
                endchar = argv[j][strlen(argv[j])-1];
            } while(endchar == 92);
            *name = (char*)calloc((size_t)len, sizeof(char));
            for(k = i+1;k<=j;k++) {
                wordlen = (int)strlen(argv[k]);
                for(l=0;l<wordlen;l++) {
                    name[0][m] = argv[k][l];
                    m++;
                }
                if(k<j) {
                    name[0][strlen(name[0])-1] = ' ';
                }
            }
            i=j;
        }

                // Read the target into 'age' if -r is found
        if(strncmp(argv[i],"-r",2) == 0) {
            *age = atoi(argv[i+1]);
            i++;
        }
                // Read the calculated reading age into 'calcage' if -c is found. This value is only used with 'FIND'
        if(strncmp(argv[i],"-c",2) == 0) {
            *calcage = atoi(argv[i+1]);
            i++;
        }

                // Read the filename into 'filename' when -f is found
        if(strncmp(argv[i],"-f",2) == 0) {
            *filename = (char*)calloc(strlen(argv[i+1]),sizeof(char));
            strcat(*filename,argv[i+1]);
            i++;
        }

                // Read the database location into 'database' when -d is found
        if(strncmp(argv[i],"-d",2) == 0) {
            *database = (char*)calloc(strlen(argv[i+1]),sizeof(char));
            strcat(*database,argv[i+1]);
            i++;
        }

                // There can be several keywords, and each is preceded by a '-k' option. So, here we just count the number of kewords 
                // that there are.
        if(strncmp(argv[i],"-k",2) == 0) {
            *keycount += 1;
            i++;
        }
    }


                // Next, an array of strings is created to store the keywords themselves. The length of the strings themselves are 
                // undefined at this point, but we have the correct number of them. We then iterate through the input list again, and 
                // each time we encounter a '-k' option, one of the strings is assigned a block of memory the size of the following 
                // word, which is then read into that block.
    keyword[0] = (char**)calloc((size_t)*keycount,sizeof(char *));
    *keycount = 0;
    for(i=1;i<argc-1;i++) {
        if(strncmp(argv[i],"-k",2)==0) {
            *keycount += 1;
            i++;
            keyword[0][*keycount - 1] = (char*)calloc(strlen(argv[i]),sizeof(char));
            strcat(keyword[0][*keycount - 1],argv[i]);
        }
    }
    return 0;
}

int agecalc(FILE *sample, char **text) {
    int filelen = 0;
    int spaces = 1, characters = 0, sentences = 0;
    int currentchar, textchar = 0;

    fseek(sample, 0, SEEK_END);
    filelen = ftell(sample);

    *text = (char*)calloc((size_t)filelen,sizeof(char));
    fseek(sample, 0, SEEK_SET);
    currentchar = fgetc(sample);

		// Set the file pointer to the start of the file, and read through it one character at a time,
		// incrementing variables depending what character is read until the end of the file is reached.
		// It is important that the file doesn't have unnecessary whitespace at the end, as each space
		// will be counted as a word in the calculation.
    while(currentchar != EOF) {
        switch(currentchar) {
            case 32 :
                spaces++;
                text[0][textchar] = (char)currentchar;
                textchar++;
                break;
            case 65 ... 90 :
                characters++;
                text[0][textchar] = (char)currentchar;
                textchar++;
                break;
            case 97 ... 122 :
                characters++;
                text[0][textchar] = (char)currentchar;
                textchar++;
                break;
            case 46 :
                sentences++;
                text[0][textchar] = (char)currentchar;
                textchar++;
                break;
        }
    currentchar = fgetc(sample);
    }

    printf("The sample contains %d words, %d characters and %d sentences\n",spaces,characters,sentences);

    float calcage;
    int outage;

		// Perform the reading age calculation and output the results. All of the variables in the calculation must
		// be cast as floats, or the intermediate results will be rounded to the nearest whole number and the result
		// will be wrong.
    calcage = (float)4.71*((float)characters / (float)spaces) + (float)0.5*((float)spaces / (float)sentences) - (float)21.43;
    outage = (int)ceilf(calcage);

    return outage;
}

char *readstrng(FILE* dbase) {
    // Function to read a field of a record and store it in a string. It stores the current location in the file passed to it, and declares 
    // a pointer. It then uses the variable len to count the number of characters from there to the next null character, and assigns that 
    // much memory to a pointer. Then it returns to the original starting place, and reads len characters into the memory it's just 
    // assigned, and returns the address of that memory to the function calling it.
    int currentchar,len = 0, at = 0, i;
    char * output;
    at = ftell(dbase);
    currentchar = fgetc(dbase);
    while(currentchar != 0) {
        len++;
        currentchar = fgetc(dbase);
    }
    output = (char*)calloc((size_t)len,sizeof(char));
    fseek(dbase,at,SEEK_SET);
    for(i = 0;i<len;i++) {
        output[i] = (char)fgetc(dbase);
    }
    fgetc(dbase);
    return output;
}


int readrec(FILE *dbase, char *name, char **keywords, int *age, int *keycount, int *calcage) {
    int spaces = 0,len,j=0, i, k, l;
    char **reckeyword;
    // Function to read a record from the database
    char *recname = readstrng(dbase); // The function readstrng() reads a field from the database file, stores it in memory and 
                                      // returns the address of that memory as its output, so we can assign that directly to a pointer.

    char *temp = readstrng(dbase); // The keyword field of the file is further split up by spaces, so after it's been read into memory it 
                                   // needs to be further divided into individual words. The following code counts the number of spaces 
                                   // in the field, creates an array of pointers of that size, and then counts the length of each individual 
                                   // word, assigns a string of that size to each member of the array and then reads that many characters 
                                   // into the string.

    len = (int)strlen(temp);
    for(i = 0;i < len;i++) {
        if(temp[i]==' ') {
            spaces++;
        }
    }
    reckeyword = (char**)calloc((size_t)spaces, sizeof(char*));
    for(i=0;i<spaces;i++) {
        k = j;
        len = 0;
        while(temp[j] != ' ') {
            len++;
            j++;
        }
        j++;
        reckeyword[i] = (char*)calloc((size_t)len + 1,sizeof(char)); // The memory allocated must be the size of the keyword, plus one byte
                                                                     // for a null character to signal the end of the string.
        for(l=0;l<len;l++) {
            reckeyword[i][l] = temp[k + l];
        }
    }
    char *tempage = readstrng(dbase);
    int recage = atoi(tempage);
    free(tempage);
    char *tempcalc = readstrng(dbase);
    int recoutage = atoi(tempcalc); // The intended reading age and calculated reading age are numbers, so atoi() converts the strings into 
                                    // integers. The strings are read into temporary locations rather than directly into atoi() so that we can 
                                    // then free them and avoid a memory leak. I don't know if that would actually be a memory leak. But 
                                    // the memory doesn't get freed at any point otherwise so I'd imagine it is? Anyway, these don't need to 
                                    // be numbers instead of strings for the program as it is now, but if you wanted to add the ability to 
                                    // search for values within a certain range rather than just exact matches, this would be useful.
    free(tempcalc);
    char *rectext = readstrng(dbase);
    char match = 0; // The following code compares the variables the user inputs to the variables read from the database. If any 
                    // of them match, it sets match to 1, and then if match is 1 at the end it prints the record to the console. If it's 
                    // 0, the function exits without displaying anything.
    if(strcmp(name,recname) == 0) {
        printf("Match found for name!\n");
        match = 1;
    }
    if(*age == recage) {
        printf("Match found for intended reading age!\n");
        match = 1;
    }
    if(*calcage == recoutage) {
        printf("Match found for calculated reading age!\n");
        match = 1;
    }

    for(i=0;i<*keycount;i++) {   // Every user supplied keyword needs to be compared with every keyword read from the record in the
        for(j=0;j<spaces;j++) {  // database. Hence, we need nested for loops.
            if(strcmp(keywords[i],reckeyword[j]) == 0) {
                printf("Match found for keyword!\n");
                match = 1;
            }
        }
    }

    if(match == 1) {
        printf("\nName: %s\n",recname);
        printf("Intended reading age: %d\n",recage);
        printf("Calculated reading age: %d\n",recoutage);
        printf("Keywords: ");
        for(i=0;i<spaces;i++) {
            printf("%s ",reckeyword[i]);
        }
        printf("\nSample text:\n%s\n\n",rectext);
    }

    // Free all of the locally declared pointers that store what was read from the file and exit the function.
    free(recname);
    free(temp);
    free(rectext);
    for(i=0;i<spaces;i++) {
        free(reckeyword[i]);
    }
    free(reckeyword);
    return 0;
}


int main(int argc, char * argv[]) {
    int i, age = 0, keycount = 0, calcage;
    char *name = NULL, **keyword = NULL, *filename = NULL, *text = NULL, *dataloc = NULL;
    FILE *sample = NULL;
    FILE *database = NULL;

    if(argc==1) { return errormsg(1); }

    //Help message
    if(argc ==2 && strncmp(argv[1],"-h",2) == 0) { return help(); }

    if(argc >= 2 && strncmp(argv[1],"OPEN",4) == 0) {

    //Parse input, print results
        parse(argc, argv, &keyword, &keycount, &name, &filename,&age, &calcage, &dataloc);
        printf("Name: %s\n",name);
        printf("Age: %d\n",age);
        printf("Filename: %s\n",filename);
        printf("Keywords: ");
        for(i=0;i<keycount;i++) { printf("%s ",keyword[i]); }
        printf("\n");


    // Check that a filename was found. Open the file to read. If there is an error, return a message and exit.
        if(filename == NULL) {return errormsg(2);}
        sample = fopen(filename, "r+");
        if(sample == NULL) {return errormsg(3);}


    // Calculate the sample's reading age
        int outage = agecalc(sample,&text);
        printf("The output age is %d\n",outage);
        printf("The sample reads:\n%s\n",text);
        fclose(sample);


    // Record the output in the order name, keywords(separated by spaces), intended age, calculated age, the text 
    // itself, in fields separated by null characters. There's no indexing in the files, but because the records are a fixed 
    // length they can just be read back in groups of 5. This isn't error tolerant but at this point I've spent enough time
    // working on this so we'll just call that a version 2.0 feature
        if(dataloc == NULL) {
            database = fopen("./database","a+");
        } else {
            database = fopen(dataloc,"a+");
        }

        fprintf(database,"%s%c",name,NULL);
        for(i=0;i<keycount;i++) { fprintf(database,"%s ",keyword[i]); }
        fprintf(database,"%c",NULL);
        fprintf(database,"%d%c",age,NULL);
        fprintf(database,"%d%c",outage,NULL);
        fprintf(database,"%s%c",text,NULL);
        fclose(database);

    } else if(argc >= 2 && strncmp(argv[1],"FIND",4) == 0) {
        parse(argc,argv,&keyword,&keycount,&name,&filename,&age,&calcage,&dataloc);
        if(dataloc == NULL) {
            printf("Filename not supplied. Opening default location.\n");
            database = fopen("./database","r");
        } else {
            database = fopen(dataloc,"r");
        }
        if(database == NULL) { return errormsg(3);}
        
        // If name and keyword are null, strcmp() hangs when we feed it to them so we need to set them to something if they aren't provided
        if(name == NULL) {
            name = (char*)calloc(1,sizeof(char));
            *name = '0';
        }
        if(keyword == NULL) {
            keycount = 1;
            keyword = (char**)calloc(1,sizeof(char*));
            keyword[0] = (char*)calloc(1,sizeof(char));
            keyword[0][0] = 0;
        }

        // The following code reads the following character of the file. If the fgetc() doesn't return -1 (this is EOF, the signal for 
        // end of file, and the reason temp has to be an integer instead of a character) it rewinds by one character, calls the 
        // readrec() function to read and compare the next five fields from the database, and repeats until the next character check 
        // returns an EOF
        int temp;
        temp = fgetc(database);
        while(temp != EOF) {
            fseek(database,-1,SEEK_CUR);
            readrec(database,name,keyword,&age,&keycount,&calcage);
            temp = fgetc(database);
        }
        
        fclose(database);

    } else {        
        return errormsg(1);
    }

    // Free any pointers we've declared and exit
    free(name);
    free(filename);
    free(keyword);
    free(text);
    free(dataloc);    
    return 0;        
}
