#include "myshell.h"

int main (int argsc, char *args[], char *envp[]){   
    //set PATH variable to current path
    //getcwd is from unistd.h and copies the path of current
        //working directory to the given string
    //PATH = getenv("PATH");
    #if debug
        printf("DEBUGGING: \n");
    #endif
    PATH = getcwd(p, sizeof(p));    
    env = envp;
    char *input; 
    if(argsc > 1){    
        //invoke shell with batchfile
        FILE *batch;
        char *line = NULL;
        size_t buf = 0;
        batch = fopen(args[1], "r");
        //check that file opened successfully
        if(batch){
            //read line by line and execute commands accordingly
            printf("Invoked shell with batchfile. \n");
            while(getline(&line, &buf, batch) != -1){
                shLaunch(line);
            }
        }else{
            errorMessage();
            exit(1);
        }
        fclose(batch);
        exit(0);
    }
    else{  
        size_t inputSize = 100;
        input = malloc(sizeof(char) * inputSize);
        int i = 0;
        //will loop endlessly until user/input file enters "exit" or "^d"
        while(1){  
            printf("%s/myshell> ", PATH);
            //while ((input[i] = (getchar())) != '\n');
            if(getline(&input, &inputSize, stdin) == -1){
                errorMessage();
            }
            else{
                #if debug
                    printf("Input: %s\n", input);
                #endif
                //if user types ENTER with no command, this will cause an error, so
                //this catches it before that happens
                if(*input == '\n' && *input == '\0'){
                    errorMessage();
                }
                else{
                    shLaunch(input);
                }
            } 
        }
    }
    free(input);
}

//function to find and execute command
void shLaunch(char *input){
    //if command ends with '&': run in background
        //check for '/' or '..' -> if no, file should be in current directory
        //find file
        //run in background, i.e. do not call wait() after forking
    //note: we do not need to perform linear search for this case
    if(input[strlen(input)-2] == '&'){
        #if debug
            printf("Input Case: Running command in background \n");
        #endif
        shBackground(input);
    }
    int i = 0;
    while(i < strlen(input)){
        //if command contains '&' anywhere else in input: run in parallel
            //find files
            //run multiple processes in parallel
        if(input[i] == '&' && input[i+1] != '\0'){
            #if debug
                printf("Input Case: Running commands in parallel\n");
            #endif
            shParallel(input);
        }
        //else if command contains redirect operators
            //change I/O file descriptors accordingly
            //run commands in order
        else if(input[i] == '<' || input[i] == '>'){
            #if debug
                printf("Input Case: Redirection\n");
            #endif
            shRedirect(input);
        }
        //else if command contains pipe operator
            //handle piping
        else if(input[i] == '|'){
            #if debug
                printf("Input Case: Pipe\n");
            #endif
            shPipe(input);
        }
        //else 
            //command is either builtin or can be found in files
            //find file and execute it
        else if(i == (strlen(input)-1) || input[i+1]=='\0'){
            #if debug
                printf("Input Case: Simple command\n");
            #endif
            shSimple(input);
        }
        i++;
    }
}

//list all the environment strings
void shEnviron(char *argv[]){
    //utilize the envp argument given to main function upon executing program
    //envp is an array of strings, terminated by 0
    //copy envp to new variable, char **env, for portability outside main funct
    //print dereferenced string and increment while there are more to read
    int i = 0;
    #if debug
            printf("Command: environ\n");
    #endif
    while(env[i]){
        printf("%s\n", env[i]);
        i++;
    }
}

//list the contents of directory at argv[1] or current directory
//if no args are given
void shDir(char *argv[]){
    //use functs from dirent.h and sys/types.h
    DIR *dir;
    if(argv[1]){
        //to open directory stream for given path
        char *arg;
        arg = strcpy(arg, argv[1]);
        dir = opendir(arg);
    }else{
        //to open directory stream for current path:
        PATH = getcwd(p, sizeof(p));
        dir = opendir(PATH); 
    }
    //to read each file in directory:
        //call readdir until it returns 0 (no more files to read)
        //readdir returnsa pointer to a dirent struct
        //a dirent struct has a string d_name which we want to print
        struct dirent *d;
        #if debug
            printf("Command: dir\n");
        #endif
        while(d = readdir(dir)){
            printf("%s\t", d->d_name);
        }
        //close dir for security reasons
        printf("\n");
        closedir(dir);
}

//display the user manual, stored in file named 'readme', using the more 
//filter, i.e. printing line-by-line when user types Enter
void shHelp(char *argv[]){
    FILE *manual;
    char *line = NULL;
    size_t buf = 0;
    manual = fopen("readme", "r");
    //check that file opened successfully
    if(manual){
        //read line by line and execute commands accordingly
        while(getline(&line, &buf, manual) != -1){
            printf("%s", line);
            while(getchar() != '\n');
        }
    }else{
        errorMessage();
        exit(1);
    }
}