#include "myshell.h"
static int argc;

//traverse the string, break up by whitespace and pipe(|)
//store tokenized strings in argv, then return it
//TO TEST: 
//write code to print each string in argv after tokenizing
void shParse(char *input, char **argv){
    argc = 0;
    int i = 0;
    //use the strtok funct from string.h to easily break up by whitespace
    char *tok = strtok(input, " \t\n"); 
    while(tok){
        argv[i] = tok;
        argc++;
        tok = strtok(NULL, " \t\n");
        i++;
    }argv[i] = '\0';
    #if debug
        i = 0;
        while(argv[i]){
            printf("argv[%d]: %s\n", i, argv[i]);
            i++;
        }
    #endif
}

//this is to be called for any error in the program
void errorMessage(){
    char error_message[30] = "An error has occured\n";
    write(2, error_message, strlen(error_message));
}

//change directory
void shCd(char *argv[]){
    //no args entered -> print current directory
    //if arg is '.' -> stay in current directory
    //if arg is '..' -> move up one directory
    //if arg is string such as 'bin' -> look for that subdirectory
    //if arg is '-' -> prev directory
    if(argc > 1){
        //if(argv[1] == "." || *argv[1] == '\n'){   
            //do nothing, stay in same directory
          //  ;       
        //}
        //else{
            //cd can only take one arg, so ignore any extra args
            if(chdir(argv[1]) == 0){   // was successful
                //clear PATH variable, set it to new path, since we changed directory
                memset(p, '\0', sizeof(p));
                PATH = getcwd(p, sizeof(p));
                #if debug
                    printf("Changed directory to %s\n", PATH);
                #endif
            }else{
                errorMessage();
            }
        //}
    }else{
        PATH = getcwd(p, sizeof(p));
        printf("%s\n", PATH);
    }
}

//clears the screen
void shClr(){
    printf("\033[H\033[2J");
}

//display argv[1], or just a blank line if no args given
void shEcho(char *argv[]){
    if(argv[1]){
        printf("%s\n", argv[1]);
    }else{
        printf("\n");
    }
}

//pause process until user presses ENTER
void shPause(){
    printf("PAUSED. Press ENTER to resume: \n");
    while(getchar() != '\n');
}

//quit the shell
void shQuit(){
    exit(0);
}

void shPath(){
    //works only on Mac OS
    /*char path[1024];
    uint32_t size = sizeof(path);
    if(_NSGetExecutablePath(path, &size) == 0){
        printf("executable path is %s\n", path);
    } else{
        printf("buffer too small; need size %u\n", size);
    }*/
}

void shBackground(char *input){
    //parse the input
    char *argv[100];
    shParse(input, argv);
    char *params[25];
    int i = 0; int j = 0;
    while(i < argc){
        if(isOperator(argv[i][0])){

        }else{
            params[j] = argv[i];
            j++;
        }
        i++;
    }params[i] = '\0';
    char *scd, *sclr, *sclear, *sdir, *senv, *secho, *shelp, *spause, *squit, *spath;
    scd = "cd"; sclr = "clr"; sclear = "clear"; sdir = "dir"; senv = "environ";
    secho = "echo"; shelp = "help"; spause = "pause"; squit = "quit"; spath = "path";
    //check if command is built-in
    if(strncmp(argv[0], scd, 2) == 0){
        shCd(argv);
    }else if(strncmp(argv[0], sclr, 3) == 0 || strncmp(argv[0], sclear, 5) == 0){
        shClr();
    }
    else if(strncmp(argv[0], sdir, 3) == 0){
        shDir(argv);
    }else if(strncmp(argv[0], senv, 3) == 0){
        shEnviron(argv);
    }else if(strncmp(argv[0], secho, 4) == 0){
        shEcho(argv);
    }else if(strncmp(argv[0], shelp, 4) == 0){
        shHelp(argv);
    }else if(strncmp(argv[0], spause, 4) == 0){
        shPause();
    }else if(strncmp(argv[0], squit, 4) == 0){
        shQuit();
    }else if(strncmp(argv[0], spath, 4) == 0){
        shPath();
    }else{
        //find file to execute, pass args if any
        //argv[0] should be a file in current directory
        //create new child process to execute command(open file),
        //then return to parent process(prompt)
        if (fork() == 0){ 
            if(execvp(params[0], params)){
                errorMessage(); 
                #if debug
                    printf("execvp() failed.\n");
                #endif
                exit(1);
            }
        } 
        else{ 
            //do not wait for child to finish, return to prompt
            printf("Background operation started.\n");
        }
    }
}

void shParallel(char *input){
    //parse the input, except separate by '&' character
    char *argv[20];
    int c = 0;  //keeps track of number of commands to execute
    int i = 0;
    //use the strtok funct from string.h to easily break up by whitespace
    char *tok = strtok(input, "&\n"); 
    while(tok){
        argv[i] = tok;
        c++;
        tok = strtok(NULL, "&\n");
        i++;
    }argv[i] = '\0';
    #if debug
            printf("Commands to be run in parallel:\n");
            int j = 0;
            while(argv[j]){
                printf("argv[%d]: %s\n", j, argv[j]);
                j++;
            }
    #endif
    //now argv contains commands to be run in parallel
    //for each command to execute:
        //store argv[i] in a string
        //this string is a whole command(potentially with args)
        //parse string into array of strings 
        //call exec on that array like we would if it was a single command
    i = 0;
    char *command;
    char *cmds[100];
    while(i < c){
        command = argv[i];
        shParse(command, cmds);
        if (fork() == 0){ 
            if(execvp(cmds[0], cmds)){
                #if debug
                    printf("execvp() failed.\n");
                #endif
                errorMessage();
                exit(1);
            } 
        } 
        //Parent 
        else if(i >= 1){ 
            //prevents from creating run-away processes
            exit(0);
        }
        else{
            //wait for child processes to finish once all have been created
            int status = 0;
            wait(&status);
            printf("Parallel operation exited with status of %d\n", status);
        }
        i++;
    }
}

int isBuiltin(char *input){
    char *argv[100];
    shParse(input, argv);
    char *scd, *sclr, *sclear, *sdir, *senv, *secho, *shelp, *spause, *squit, *spath;
    scd = "cd"; sclr = "clr"; sclear = "clear"; sdir = "dir"; senv = "environ";
    secho = "echo"; shelp = "help"; spause = "pause"; squit = "quit"; spath = "path";
    if(strncmp(argv[0], scd, 2) == 0){
        return 1;
    }else if(strncmp(argv[0], sclr, 3) == 0 || strncmp(argv[0], sclear, 5) == 0){
        return 1;
    }
    else if(strncmp(argv[0], sdir, 3) == 0){
        return 1;
    }else if(strncmp(argv[0], senv, 3) == 0){
        return 1;
    }else if(strncmp(argv[0], secho, 4) == 0){
        return 1;
    }else if(strncmp(argv[0], shelp, 4) == 0){
        return 1;
    }else if(strncmp(argv[0], spause, 4) == 0){
        return 1;
    }else if(strncmp(argv[0], squit, 4) == 0){
        return 1;
    }else if(strncmp(argv[0], spath, 4) == 0){
        return 1;
    }else return 0;
}

void shRedirect(char *input){
    //parse the input by whitespace
    char *argv[100];
    shParse(input, argv);
    char *params[25];
    int i = 0; int j = 0;
    //args before first redirection operator are to be passed to execvp
    while(argv[i][0] != '>' && argv[i][0] != '<'){
        params[i] = argv[i];
        i++;
    }params[i] = '\0';
    //find the redirection operator indices
    //determine types of operators(input or output) and the given files/commands 
    //write code to handle '<', '>', and '>>' operators
    //fork and exec to create new processes for arguments
    //change I/O file descriptors for processes accordingly 
    //wait for child processes to finish 
    //parse separates command line so that the redirection operators will be at argv[i][0]
    //and the arg to be redirected will be argv[i+1]
    //MULTIPLE REDIRECTION OPERATORS OR MULTIPLE FILES TO THE RIGHT OF REDIRECT SIGN ARE ERRORS
    //redirection of input and output in same command line are supported
    i = 1;
    int ri = 0, ro = 0;
    //set to 1 if '>' is present, 2 if '>>' is present
    int apptrunc = 0;
    i = 0;
    while(argv[i]){
        //take argv[i] as an arg to command given
        //check that we don't have multiple of the same operator
        //create child process, redirect stdin to argv[i], execute
        if(argv[i][0] == '<'){
            if(ri){
                errorMessage();
                //exit(1);
            }
            ri = i+1;
        }else if(argv[i][0] == '>'){
            //checks if '>>' operator is present: append file (keep contents and add output to end)
            if(ro){
                errorMessage();
                //exit(1);
            }
            if(argv[i][1] == '>'){
                ro = i+1;
                apptrunc = 2;
            }
            //single '>' means truncate desired file (clear contents and write output to it)
            else{
                ro = i+1;
                apptrunc = 1;
            }
        }
        i++;
    }
    #if debug
        if(ro){
            printf("argv[ro]: %s\n", argv[ro]);
        }if(ri){
            printf("argv[ri]: %s\n", argv[ri]);
        }
    #endif
    //execute both input and output redirection
    if(ro && ri){
        if(apptrunc == 1){
            int pid = fork();
            if(pid == 0){
                int status = 0;
                wait(&status);
                printf("Parent exited with status of %d\n", status);
            }
            else{
                close(0);
                int newstdin = open(argv[ri], O_RDONLY);
                #if debug
                if(newstdin < 0)
                    printf("open() failed.\n");
                else
                    printf("open() succeeded; newstdin: %d\n", newstdin);
                #endif
                if(dup2(newstdin, 0)){
                    close(1);
                    int newstdout = open(argv[ro], O_RDWR | O_CREAT | O_TRUNC, 0666);
                    #if debug
                        if(newstdout < 0)
                            printf("open() failed.\n");
                        else
                            printf("open() succeeded; newstdin: %d\n", newstdin);
                    #endif
                    if(dup2(newstdout, 1)){
                        if(isBuiltin(params[0])){
                            //un-parse to accept any args there might be before redirection operator
                            i = 0;
                            char *unparsed;
                            while(params[i]){
                                strcat(unparsed, params[i]);
                                strcat(unparsed, " ");
                                i++;
                            }
                            shSimple(unparsed);
                        }else{
                            if(execvp(params[0], params)){
                                errorMessage();
                                #if debug
                                    printf("execvp() failed.\n");
                                #endif
                                exit(1);
                            }
                        }
                    } 
                }else{
                    errorMessage();
                    exit(1);
                }
            }
        }else{

        }
    }
    //execute only input redirection
    //usage: someprogram < somefile
    else if(ri){
        int pid = fork();
        if(pid == 0){
            close(0);
            int newstdin = open(argv[ri], O_RDONLY);
            #if debug
                if(newstdin < 0)
                    printf("open() failed.\n");
                else
                    printf("open() succeeded; newstdin: %d\n", newstdin);
            #endif
            /*if*/(dup(newstdin));//{
                if(isBuiltin(params[0])){
                    i = 0;
                    char *unparsed;
                    while(params[i]){
                        strcat(unparsed, params[i]);
                        strcat(unparsed, " ");
                        i++;
                    }
                    shSimple(unparsed);
                }
                //else{
                    if(execvp(params[0], params)){
                            errorMessage();
                            #if debug
                                printf("execvp() failed.\n");
                            #endif
                            exit(1);
                        }
                //}
            /*}else{
                errorMessage();
                #if debug
                    printf("dup2() failed.\n");
                #endif
                exit(1);
            }*/
        }else{
            int status = 0;
            wait(&status);
            printf("Child exited with status of %d\n", status);
            
        }
    }
    //execute only output redirection
    else if(ro){
        if(apptrunc == 1){
            int pid = fork();
            if(pid == 0){
                close(1);
                int newstdout = open(argv[ro], O_RDWR | O_CREAT | O_TRUNC, 0666);
                #if debug
                if(newstdout < 0)
                    printf("open() failed.\n");
                else
                    printf("open() succeeded; newstdout: %d\n", newstdout);
                #endif
                if(dup2(newstdout, 1)){
                    //might take this out
                    if(isBuiltin(params[0])){
                        i = 0;
                        char *unparsed;
                        while(params[i]){
                            strcat(unparsed, params[i]);
                            strcat(unparsed, " ");
                            i++;
                        }
                    shSimple(unparsed);
                    }else{
                        if(execvp(params[0], params)){
                            errorMessage();
                            #if debug
                                printf("execvp() failed.\n");
                            #endif
                            exit(1);
                        }
                    }
                }else{
                    errorMessage();
                    exit(1);
                }
            }else{
                int status = 0;
                wait(&status);
                printf("Child exited with status of %d\n", status);
            }
        }
        //same thing as if, except append to file if it exists
        else{
            int pid = fork();
            if(pid == 0){
                close(1);
                int newstdout = open(argv[ro], O_WRONLY | O_APPEND | O_CREAT, 0666);
                #if debug
                    if(newstdout < 0)
                        printf("open() returned 0.\n");
                    else
                        printf("open() succeeded; newstdout: %d\n", newstdout);
                #endif
                if(dup2(newstdout, 1)){
                    if(isBuiltin(argv[ro])){
                        shSimple(argv[ro]);
                    }else{
                        if(execvp(params[0], params)){
                            errorMessage();
                            #if debug
                                printf("execvp() failed.\n");
                            #endif
                            exit(1);
                        }
                    }
                }else{
                    errorMessage();
                    exit(1);
                }
            }else{
                int status = 0;
                wait(&status);
                printf("Child exited with status of %d\n", status);
            }
        }
    }
}

int isOperator(char c){
    if(c == '>' || c == '<' || c == '|' || c == '&'){
        return 1;
    }
    else return 0;
}

void shSimple(char *input){
    //parse the input
    char *argv[100];
    shParse(input, argv);
    char *params[25];
    int i = 0; int j = 0;
    #if debug
        printf("argc: %d\n", argc);
    #endif
    while(i < argc){
        if(isOperator(argv[i][0])){

        }else{
            params[j] = argv[i];
            j++;
        }
        i++;
    }params[j] = '\0';
    char *scd, *sclr, *sclear, *sdir, *senv, *secho, *shelp, *spause, *squit, *spath;
    scd = "cd"; sclr = "clr"; sclear = "clear"; sdir = "dir"; senv = "environ";
    secho = "echo"; shelp = "help"; spause = "pause"; squit = "quit"; spath = "path";
    //check if command is built-in
    if(strncmp(argv[0], scd, 2) == 0){
        shCd(argv);
    }else if(strncmp(argv[0], sclr, 3) == 0 || strncmp(argv[0], sclear, 5) == 0){
        shClr();
    }
    else if(strncmp(argv[0], sdir, 3) == 0){
        shDir(argv);
    }else if(strncmp(argv[0], senv, 3) == 0){
        shEnviron(argv);
    }else if(strncmp(argv[0], secho, 4) == 0){
        shEcho(argv);
    }else if(strncmp(argv[0], shelp, 4) == 0){
        shHelp(argv);
    }else if(strncmp(argv[0], spause, 4) == 0){
        shPause();
    }else if(strncmp(argv[0], squit, 4) == 0){
        shQuit();
    }else if(strncmp(argv[0], spath, 4) == 0){
        shPath();
    }else{
        //find file to execute, pass args if any
        //argv[0] should be a file in current directory
        //create new child process to execute command(open file),
        //then return to parent process(prompt)
        if (fork() == 0){ 
            if(execvp(params[0], params)){
                errorMessage(); 
                #if debug
                    printf("execvp() failed.\n");
                #endif
                exit(1);
            }
        } 
        else{ 
            int status = 0; 
            wait(&status); 
            printf("Child exited with status of %d\n", status);
        }
    }
}

//PROVE that this works
void shPipe(char *input){
    argc = 0;
    char *argv[100];
    shParse(input, argv);
    //args to be directed to pipe[0] (new stdin)
    char *inparams[25];
    //args to be directed to pipe[1] (new stdout)
    char *outparams[25];
    int i = 0; 
    while((argv[i][0]) != '|'){
        inparams[i] = argv[i];
        i++;
    }inparams[i] = '\0';
    i++;
    int j = 0;
    while(i < argc){
        outparams[j] = argv[i];
        i++; j++;
    }outparams[j] = '\0';
    #if debug
        int k = 0;
        while(inparams[k]){
            printf("inparams[%d]: %s\n", k, inparams[k]);
            k++;
        }
        k = 0;
        while(outparams[k]){
            printf("outparams[%d]: %s\n", k, outparams[k]);
            k++;
        }
    #endif
    //now inparams and outparams contain commands to be fed thru pipe
    //direct I/O to pipe appropriately
    //wait for child processes to finish
    //create pipe
    //pipe returns 0 on success
    int pfds[2];
    if(pipe(pfds) == 0){
        #if debug
        printf("pipe creation succesful.\n");
        #endif
        //create processes for each end 
            //need to fork twice so it returns to prompt after
        if(fork() == 0){
            close(1);
            dup2(pfds[1], 1);
            close(pfds[0]);
            if(execvp(inparams[0], inparams)){
                #if debug
                    printf("execvp() failed.\n");
                #endif
                errorMessage();
            }
            exit(0);
        }else{
            if(fork() == 0){
                close(0);
                dup2(pfds[0], 0);
                close(pfds[1]);
                if(execvp(outparams[0], outparams)){
                    #if debug
                        printf("execvp() failed.\n");
                    #endif
                    errorMessage();
                    exit(1);
                }
            }else{
                //parent, return to prompt
                int status = 0;
                wait(&status);
                printf("Pipe operation exited with status of %d\n", status);
            }
        }
    }else{
        #if debug
        printf("pipe failed.\n");
        #endif
        errorMessage();
    }
    fflush(stdin);
}