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
void printFile(char *fileString, char* directoryString, int argA, int argL);
int isDirectory(char *directoryString);
int isFile(char *fileString);

int main(int argc, char *argv[]){
  // Declaring local variable(s)
  int opt;
  int argL = 0;
  int argA = 0;

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

  for(int i = optind; i < argc; i++) {
    if(isDirectory(argv[i])) { // Print if argument is a directory
      if(argc - optind >= 2) {
        printf("%s:\n", argv[i]);
      }
      printDirectory(argv[i], argA, argL);
      printf("\n");
    }else if(isFile(argv[i]) == 1){ // Print if argument is a file
      printFile(argv[i], "", argA, argL);
    }
  }

  // Print current directory if no file or directory args
  if(argc - optind == 0) {
    printDirectory(".", argA, argL);
  }
}

void printFile(char *fileString, char* directoryString, int argA, int argL){
  if (argL == 1){
    char fullFileString [1024]; // Set aside 1024 characters for full filestring
    snprintf(fullFileString, 1024, "%s/%s", directoryString, fileString);

    struct stat buffer[sizeof(struct stat)]; // Create buffer for file
    stat(fullFileString, buffer);
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
  }else {
    /*if no args are given, just print file name*/
    if(fileString[0] != '.' || argA == 1) {
      printf("%s\n", fileString);
    }
  }
}

void printDirectory(char *directoryString, int argA, int argL) {
  // Declare local variables
  struct dirent *dp;
  // Check if argument is a directory
  if(isDirectory(directoryString) == 1){
    DIR *currentDirectory = opendir(directoryString);
    while ((dp=readdir(currentDirectory)) != NULL) {
      // Loop through currentDirectory
      char *fileName = dp->d_name;
      printFile(fileName, directoryString, argA, argL);
    }
    closedir(currentDirectory);
  }
}

void printPermissions(int permissions)
{
  char permissionsArray[11];
  for(int i=0; i<10; i++){
    permissionsArray[i]='-';
  }
  if (S_ISDIR(permissions)){
    permissionsArray[0] = 'd';
  }
  if (S_IRUSR&permissions){
    permissionsArray[1] = 'r';
  }
  if (S_IWUSR&permissions){
    permissionsArray[2] = 'w';
  }
  if (S_IXUSR&permissions){
    permissionsArray[3] = 'x';
  }
  if (S_IRGRP&permissions){
    permissionsArray[4] = 'r';
  }
  if (S_IWGRP&permissions){
    permissionsArray[5] = 'w';
  }
  if (S_IXGRP&permissions){
    permissionsArray[6] = 'x';
  }
  if (S_IROTH&permissions){
    permissionsArray[7] = 'r';
  }
  if (S_IWOTH&permissions){
    permissionsArray[8] = 'w';
  }
  if (S_IXGRP&permissions){
    permissionsArray[9] = 'x';
  }
  printf("%s", permissionsArray);
}

// Check if argument is a directory by calling
//  stat and then calling isdir
int isDirectory(char *directoryString){
  struct stat directory_stat;
  stat(directoryString, &directory_stat);
  return S_ISDIR(directory_stat.st_mode);
}

// Check if argument is a file by calling
//  stat and then calling isreg
int isFile(char *fileString){
  struct stat file_stat;
  stat(fileString, &file_stat);
  return S_ISREG(file_stat.st_mode);
}
