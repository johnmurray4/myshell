#include <stdio.h>
#include <stdlib.h>
#include <string.h> //for functs strtok, strcmp, strstr...
#include <unistd.h> //for fflush, getcwd
#include <ctype.h> //for isspace
#include <sys/types.h> 
#include <sys/wait.h>
#include <dirent.h>     //for opendir, readdir, closedir
#include <fcntl.h>

//builtins
void shCd(char *argv[]);
void shClr();
void shDir(char *argv[]);
void shEnviron(char *argv[]);
void shEcho(char *argv[]);
void shHelp(char *argv[]);
void shPause();
void shQuit();
void shPath();
void (*builtins[9])(char *argv[]);      //array of builtin function pointers
static char *PATH;     //absolute path of shell executable
void errorMessage();
char *findFile(char *argv[]);
char p[200];
void shParse(char *input, char **argv);
int isBuiltin(char *input);
//static int argc;
static char **env;
void shBackground(char *input);
void shParallel(char *input);
void shRedirect(char *input);
void shSimple(char *input);
void shPipe(char *input);
void shLaunch(char *input);
int isOperator(char c);