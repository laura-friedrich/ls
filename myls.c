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
  /*Open current directory*/

  int opt;
  int argL = 0;
  int argA = 0;
  char *pathStringArray[argc];
  int argumentTypeArray[argc];


  /*Determine if -l, -a, -al, -la have been passed*/
  while ((opt = getopt(argc, argv, "la")) != -1) {
    switch (opt) {
      case 'l':
      argL = 1;
      break;
      case 'a':
      argA = 1;
      break;
      default: /* '?' */
      //printf("Unrecognized argument. USAGE: ");
      break;
    }
  }

  // Loop through argumens to find files and directories
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
      }else{
        // Check if argument is a file
        struct stat path_stat;
        stat(argv[i], &path_stat);
        if(S_ISREG(path_stat.st_mode)){
          // Argument is a file
          pathStringArray[i] = argv[i];
          argumentTypeArray[i] = 1;
        }else{
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

  int printCurrent = 0;

  for(int i = 1; i < argc; i++){
    //printf("Path String: %s\tArgument Type: %d\n", pathStringArray[i], argumentTypeArray[i]);
    // Print if argument is a directory
    if(argumentTypeArray[i] == 0){
      printDirectory(pathStringArray[i], argA, argL);
      printCurrent = 2; // Don't need to print current
    }else if(argumentTypeArray[i] == 1){
      printFile(pathStringArray[i], argA, argL);
      printCurrent += 1; // Don't need to print current
      //print file
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
    printf("%s\n", fileString);
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


    //return permissions;
  }
}

/************************/
/*******OLD CODE********/
/**********************/
// Check to see if parameter provided is directory
// int useDefaultDirectory = 0;
// char *defaultDirectory = ".";
// for(int i = 1; i < argc; i++){// Loop through parameters except for default
//   if(opendir(argv[i]) != NULL){
//     useDefaultDirectory = 1;
//     defaultDirectory = argv[i];
//   }
// }
// int i = useDefaultDirectory;
// while(i < argc){// Loop through parameters except for default
//   if(opendir(argv[i]) != NULL){ // Prevent duplicates
//     if(currentDirectory != opendir(argv[i])){
//       currentDirectory = opendir(argv[i]);
//     }
//   }else{
//     if(currentDirectory != opendir(defaultDirectory)){
//       currentDirectory = opendir(defaultDirectory);
//     }
//
//   }
//   // Finding if there exits an argument that's a valid directory
//   //printf("%s: \n", argv[i]);
//
//   /*Run main 'meat' of the program ie produce text to be printed */
//
//   /*Go through all files in current VALID directory*/
//   if ((strcmp(argv[i],"-al") != 0) &&
//   (strcmp(argv[i],"-l") != 0) &&
//   (strcmp(argv[i],"-a") != 0)
// ){
//   printDirectory(defaultDirectory, argA, argL);
// }
// /*clean up*/
//
// i++;
//
// }< argc; i++){// Loop through parameters except for default
//   if(opendir(argv[i]) != NULL){
//     useDefaultDirectory = 1;
//     defaultDirectory = argv[i];
//   }
// }
// int i = useDefaultDirectory;
// while(i < argc){// Loop through parameters except for default
//   if(opendir(argv[i]) != NULL){ // Prevent duplicates
//     if(currentDirectory != opendir(argv[i])){
//       currentDirectory = opendir(argv[i]);
//     }
//   }else{
//     if(currentDirectory != opendir(defaultDirectory)){
//       currentDirectory = opendir(defaultDirectory);
//     }
//
//   }
//   // Finding if there exits an argument that's a valid directory
//   //printf("%s: \n", argv[i]);
//
//   /*Run main 'meat' of the program ie produce text to be printed */
//
//   /*Go through all files in current VALID directory*/
//   if ((strcmp(argv[i],"-al") != 0) &&
//   (strcmp(argv[i],"-l") != 0) &&
//   (strcmp(argv[i],"-a") != 0)
// ){
//   printDirectory(defaultDirectory, argA, argL);
// }
// /*clean up*/
//
// i++;
//
// }
