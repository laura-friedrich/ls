/*
* myls.c
*
* usage: mimics the behhivior of the ls command in terminal
*
* Laura Friedrich and Henry Mound CS315 HW1
*/

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>

void printPermissions(int permissions);
void printDirectory(char *directoryString, int argA, int argL);
void printFile(char *fileString, int argA, int argL);

int main(int argc, char *argv[])
{
  // Declaring local variables
  int opt;
  int argL = 0;
  int argA = 0;
  char *pathStringArray[argc];
  int argumentTypeArray[argc];
  int directoryCounter = 0;

  /*Determine if -l, -a, -al, -la have been passed*/
  while ((opt = getopt(argc, argv, "la")) != -1) {
    switch (opt) {
      case 'l':
        argL = 1;
      break;
      case 'a':
        argA = 1;
      break;
      default: // Do nothing
      break;
    }
  }

  // Loop through arguments to find files and directories
  for(int i = 0; i < argc; i++){
    // Check the argument is not a flag
    if ((strcmp(argv[i],"-al") != 0) &&
    (strcmp(argv[i],"-la") != 0) &&
    (strcmp(argv[i],"-l") != 0) &&
    (strcmp(argv[i],"-a") != 0)){
      // Check if argument is a directory
      if(opendir(argv[i]) != NULL){
        // Argument is a directory that we need to print
        pathStringArray[i] = argv[i];
        argumentTypeArray[i] = 0;
        directoryCounter += 1;
      }else{
        // Check if argument is a file
        struct stat path_stat;
        if(stat(argv[i], &path_stat) == 0){
          // Argument is a file
          pathStringArray[i] = argv[i];
          argumentTypeArray[i] = 1;
        }else{
          // Argument should not be printed
          pathStringArray[i] = "";
          argumentTypeArray[i] = -1;
        }
      }
    }else{
      // Argument doesn't need to be printed
      pathStringArray[i] = "";
      argumentTypeArray[i] = -1;
    }
  }

  int printCurrent = 0; // Flag to print current directory if no other args given
  for(int i = 1; i < argc; i++){
    if(argumentTypeArray[i] == 0){ // Print if argument is a directory
      if(directoryCounter >= 2){
        printf("%s:\n", pathStringArray[i]);
      }
      printDirectory(pathStringArray[i], argA, argL);
      printf("\n");
      printCurrent = 2; // Don't need to print current
    }else if(argumentTypeArray[i] == 1){ // Print if argument is a file
      printFile(pathStringArray[i], argA, argL);
      printCurrent += 1; // Don't need to print current
    }
  }

  if(printCurrent < 1){
    printDirectory(".", argA, argL);
  }
}

void printFile(char *fileString, int argA, int argL){
  // Declare local variables
  errno = 0;
  struct stat buffer[sizeof(struct stat)];
  // Loop through currentDirectory
  /*if -l is an arg, print long*/
  if (argL == 1){
    stat(fileString, buffer);
    /*if -a is an arg, show hidden files*/
    if(fileString[0] != '.' || argA == 1){
      int size = buffer->st_size;
      int userID = buffer->st_uid;

      // Get username
      struct passwd *pws;
      pws = getpwuid(userID);
      char *username = pws->pw_name;

      //Get group name
      int groupID = buffer->st_gid;
      struct group *grid;
      grid = getgrgid(groupID);
      char *groupName = grid->gr_name;

      // Get time
      time_t t = buffer->st_mtime; /*st_mtime is type time_t */
      struct tm *info;
      info = localtime(&t);
      char *timeFormatted = asctime(info);
      timeFormatted[strlen(timeFormatted) - 1] = 0;// Remove '\n'
      int permissions = buffer->st_mode;
      printPermissions(permissions);
      printf("\t %s \t %s \t %d \t %s \t %s \n", username, groupName, size, timeFormatted, fileString);
    }
  }else{
    /*if no args are given, just print file name*/
    if(fileString[0] != '.' || argA == 1){
      printf("%s\n", fileString);
    }
  }
}

void printDirectory(char *directoryString, int argA, int argL){
  // Declare local variables
  errno = 0;
  struct dirent *dp;
  DIR *currentDirectory = opendir(directoryString);
  // Loop through currentDirectory
  while ((dp=readdir(currentDirectory)) != NULL){
    char *fileName = dp->d_name;
    printFile(fileName, argA, argL);
  }
  closedir(currentDirectory);
}

void printPermissions(int permissions)
{
  char* permissionsArray[10];
  for(int i=0; i<10; i++){
    permissionsArray[i]="-";
  }
  if (S_ISDIR(permissions)){
    permissionsArray[9] = "d";
  }
  if (S_IRUSR&permissions){
    permissionsArray[8] = "r";
  }
  if (S_IWUSR&permissions){
    permissionsArray[7] = "w";
  }
  if (S_IXUSR&permissions){
    permissionsArray[6] = "x";
  }
  if (S_IRGRP&permissions){
    permissionsArray[5] = "r";
  }
  if (S_IWGRP&permissions){
    permissionsArray[4] = "w";
  }
  if (S_IXGRP&permissions){
    permissionsArray[3] = "x";
  }
  if (S_IROTH&permissions){
    permissionsArray[2] = "r";
  }
  if (S_IWOTH&permissions){
    permissionsArray[1] = "w";
  }
  if (S_IXGRP&permissions){
    permissionsArray[0] = "x";
  }
  for(int i=9; i>=0; i--){
    printf("%s", permissionsArray[i]);
  }
}
