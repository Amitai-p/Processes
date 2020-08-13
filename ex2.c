// Amitai Popovsky
// 312326218

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <errno.h>
#define MAX_SIZE_INPUT 100

struct history {
  char commands[MAX_SIZE_INPUT];
  int isParallel;
  int status;
  pid_t pid;
} typedef history;

struct jobs {
  char commands[MAX_SIZE_INPUT];
  int isParallel;
  int status;
  pid_t pid;
} typedef jobs;

int getNumCase(char *token);
int checkBackground(char **token, int index);
int doOp(char **argv, char *token, int isParllel);
void process(char **argv, int num);
int checkHistory(int i);
int checkJobs(int i);
int isEchoProb(char *str, char *token, char *token2);
int cdRegular(char **argv, int isParallel);
void increaseCounters();
int getCdType(char **token);
int cd(char **argv, int isParallel, int typeCd);
int cdAlone(char **argv, int isParallel);
int sizeOfArgv(char **argv);
int cdPoints(char **argv, int isParallel);
int cdMinus(char **argv, int isParallel);
int checkEnter(char *str);

struct history arrayHistory[MAX_SIZE_INPUT];
int sizeOfHistory = 0;
struct jobs arrayJobs[MAX_SIZE_INPUT];
int sizeOfJobs = 0;
char paths[MAX_SIZE_INPUT][MAX_SIZE_INPUT];
int sizeOfPaths = 0;
char firstPath[MAX_SIZE_INPUT];
char lastPath[MAX_SIZE_INPUT];
char *homedirFirst;

int main() {
  char str[MAX_SIZE_INPUT];
  int i = 0;
  int j = 0;
  int flag = 1;
  int placeStrEcho = 0;
  int isProbEchoDone = 0;
  int flagQuot = 0;
  int isParallelTemp = 0;
  char buffer[MAX_SIZE_INPUT];
  int isEcho = 0;
  // To know the first path.
  getcwd(buffer, MAX_SIZE_INPUT);
  strcpy(firstPath, buffer);
  // For cd ~.
  homedirFirst = getenv("HOME");
  while (1) {
    isParallelTemp = 0;
    flagQuot = 0;
    char *array[MAX_SIZE_INPUT];
    i = 0;
    printf("> ");
    // Get the string.
    fgets(str, MAX_SIZE_INPUT, stdin);
    if (checkEnter(str)) {
      continue;
    }
    if (str[strlen(str) - 1] == '\n') {
      str[strlen(str) - 1] = '\0';
    }
    // For echo " text     text".
    char *str2 = strstr(str, "\"");
    char str3[MAX_SIZE_INPUT];
    j = 0;
    int placeQ = 0;
    int k = 0;
    if (str2 != NULL) {
      isProbEchoDone = 1;
      flagQuot = 1;
      // Parser the string.
      while (j < strlen(str2)) {
        if (str2[j] != '\"') {
          str3[k] = str2[j];
          k++;
        } else {
          placeQ = k;
        }
        j++;
      }
      str3[placeQ] = '\0';
    } else {
      str2 = str;
    }
    // End of the string.
    str3[k] = '\0';
    str2 = str3;
    // save to the history and jobs.
    strcpy(arrayHistory[sizeOfHistory].commands, str);
    strcpy(arrayJobs[sizeOfJobs].commands, str);
    array[i] = strtok(str, " ");
    while (array[i] != NULL) {
      array[++i] = strtok(NULL, " ");
    }
    arrayHistory[sizeOfHistory].isParallel = checkBackground(array, i);
    if (arrayHistory[sizeOfHistory].isParallel == 1) {
      array[i - 1] = NULL;
      i--;
    }
    // Take care of echo with "".
    if (isEchoProb(str2, array[0], array[1]) || flagQuot) {
      array[1] = str2;
      flag = 0;
    }
    // Take care of end of strings.
    if (!arrayHistory[sizeOfHistory].isParallel && flag) {
      if (array[i - 1][strlen(array[i - 1]) - 1] == '\n') {
        array[i - 1][strlen(array[i - 1]) - 1] = '\0';
      }
    }
    // In case of &.
    for (j = 0; j < i; j++) {
      if (array[j] != NULL) {
        char *chs1 = "&\n";
        char *chs2 = "&";
        char *str = array[j];
        if (!strcmp(str, chs1)) {
          array[j] = NULL;
        }
        if (!strcmp(str, chs2)) {
          array[j] = NULL;
        }
      }
    }
    j = 0;
    // In case of that was echo " test "
    if (isProbEchoDone) {
      while (array[j] != NULL) {
        if (!strcmp(str2, array[j])) {
          placeStrEcho = 1;
        } else if (placeStrEcho) {
          array[j] = NULL;
        }
        j++;
      }
    }
    if (arrayHistory[sizeOfHistory].isParallel == 2) {
      arrayHistory[sizeOfHistory].isParallel = 0;
    }
    // Call the function that will do the operation.
    if (doOp(array, array[0], arrayHistory[sizeOfHistory].isParallel) == 2) {
      // In case of faild at chdir.
      return 0;
    }
    // Reset flags.
    placeStrEcho = 0;
    isProbEchoDone = 0;
  }
  return 0;
}

// Get the type of case to deal with him.
int getNumCase(char *token) {
  if (!strcmp(token, "exit")) {
    return 1;
  }
  if (!strcmp(token, "cd")) {
    return 2;
  }
  if (!strcmp(token, "jobs")) {
    return 3;
  }
  if (!strcmp(token, "history")) {
    return 4;
  }
  // The default.
  return 5;
}

// The function will call to the function acording to the case type.
int doOp(char **argv, char *token, int isParallel) {
  int i = 0;
  int caseCd;
  pid_t pid = getpid();
  switch (getNumCase(token)) {
    // Exit.
    case 1:printf("%d \n", pid);
      increaseCounters();
      exit(0);
      break;
      // Cd  .
    case 2:caseCd = getCdType(argv);
      if (cd(argv, isParallel, caseCd) == 2) {
        return 2;
      }
      increaseCounters();
      break;
      // Jobs.
    case 3:arrayHistory[sizeOfHistory].pid = pid;
      arrayJobs[sizeOfJobs].pid = pid;
      increaseCounters();
      for (i = 0; i < sizeOfJobs - 1; i++) {
        if (!checkJobs(i)) {
          printf("%d %s\n", arrayJobs[i].pid, arrayJobs[i].commands);
        }
      }
      break;
      // History.
    case 4:arrayHistory[sizeOfHistory].pid = pid;
      arrayJobs[sizeOfJobs].pid = pid;
      increaseCounters();
      for (i = 0; i < sizeOfHistory; i++) {
        if (checkHistory(i) && i < sizeOfHistory - 1) {
          printf("%d %s DONE\n", arrayHistory[i].pid, arrayHistory[i].commands);
        } else if (i == sizeOfHistory - 1) {
          printf("%d %s RUNNING\n", getpid(), arrayHistory[i].commands);
        } else {
          printf("%d %s RUNNING\n", arrayHistory[i].pid, arrayHistory[i].commands);
        }
      }
      break;
      // Other commands.
    case 5:process(argv, isParallel);
      break;
    default:break;
  }
  return 0;
}

// The default case.
void process(char **argv, int isParallel) {
  int status = 0;
  increaseCounters();
  // Make a new process.
  pid_t pid = fork();
  // Save the pid's.
  arrayHistory[sizeOfHistory - 1].pid = pid;
  arrayJobs[sizeOfJobs - 1].pid = pid;
  // In case of problem.
  if (pid < 0) {
    fprintf(stderr, "Error in system call\n");
    exit(1);
  } else if (pid == 0) {
    // Call the execvp.
    if (execvp(*argv, argv) < 0) {
      fprintf(stderr, "Error in system call\n");
      exit(1);
    }
  } else if (pid > 0) {
    printf("%d \n", pid);
    // In case of problem or & with char uncompletable.
    if (isParallel == 0) {
      int wait = waitpid(pid, &status, 0);
      if (wait < 0) {
        fprintf(stderr, "Error in system call\n");
      }
    }
  }
}

// Check if there is '&' at the end of the input.
int checkBackground(char **token, int index) {
  char *chs1 = "&\n";
  char *chs2 = "&\0";
  char chs3 = '&';
  char *str = token[index - 1];
  if (str != NULL) {
    if (!strcmp(str, chs1)) {
      return 1;
    }
    if (!strcmp(str, chs2)) {
      return 1;
    }
    // In case that there is more charachters after the '&'.
    if (*str == chs3) {
      return 2;
    }
  }
  return 0;
}

// Check if the process is still in job.
int checkHistory(int i) {
  int size = 0;
  if (waitpid(arrayHistory[i].pid, &size, WNOHANG)) {
    arrayHistory[i].status = 1;
  } else {
    arrayHistory[i].status = 0;
  }
  return arrayHistory[i].status;
}

// Check if the process is still in job.
int checkJobs(int i) {
  int size = 0;
  if (waitpid(arrayJobs[i].pid, &size, WNOHANG)) {
    arrayJobs[i].status = 1;
    return 1;
  } else {
    arrayJobs[i].status = 0;
  }
  return 0;
}
// Just check if there is a " for echo.
int isEchoProb(char *str, char *token, char *token2) {
  if (!strcmp(token, "echo")) {
    //if ((str + 5)[0] == '"') {
    if (token2 != NULL) {
      if (*token2 == '"') {
        return 1;
      }
    }
  }
  return 0;
}

// The regular case to git in folder.
int cdRegular(char **argv, int isParallel) {
  int size = 0;
  int lengthOfBuffer;
  pid_t pid = getpid();
  arrayHistory[sizeOfHistory].pid = pid;
  arrayJobs[sizeOfJobs].pid = pid;
  printf("%d \n", pid);
  // The first path.
  while (*argv != NULL) {
    argv++;
    size++;
  }
  // because of the &.
  if (isParallel) {
    argv--;
  }
  argv--;
  char buffer[MAX_SIZE_INPUT];
  // The first time.
  if (sizeOfPaths == 0) {
    getcwd(buffer, MAX_SIZE_INPUT);
    strcpy(paths[0], buffer);
  } else {
    getcwd(buffer, MAX_SIZE_INPUT);
    strcpy(paths[sizeOfPaths], buffer);
  }
  lengthOfBuffer = strlen(paths[sizeOfPaths]);
  int j = 0;
  while (paths[sizeOfPaths][j] != '\0') {
    j++;
  }
  // Parsering.
  paths[sizeOfPaths][j] = '/';
  paths[sizeOfPaths][lengthOfBuffer + 1] = '\0';
  char *getin = strcat(paths[sizeOfPaths], *argv);
  strcpy(paths[sizeOfPaths], getin);
  // For cd minus.
  getcwd(lastPath, MAX_SIZE_INPUT);
  if (chdir(getin)) {
    // In case of problem.
    if (errno != 0) {
      if (errno == ENOENT) {
        fprintf(stderr, "Error: No such file or directory\n");
        paths[sizeOfPaths][0] = '\0';
        errno = 0;
        return 2;
      } else {
        paths[sizeOfPaths][0] = '\0';
        if (errno == 10) {
          errno = 0;
          sizeOfPaths++;
          return 0;
        }
        errno = 0;
      }
      return 2;
    }
    return 2;
  }
  // Save for cd -.
  strcpy(lastPath, buffer);
  sizeOfPaths++;
  return 0;
}

// Just inscrease the counters.
void increaseCounters() {
  sizeOfHistory++;
  sizeOfJobs++;
}

// To know which type of cd.
int getCdType(char **token) {
  char *chs1 = "~\n";
  char *chs2 = "~";
  char *chs3 = "..\n";
  char *chs4 = "..";
  char *chs5 = "-\n";
  char *chs6 = "-";
  char *str = token[1];
  if (str != NULL) {
    if (!strcmp(str, chs1)) {
      return 1;
    }
    if (!strcmp(str, chs2)) {
      return 1;
    }
  }
  if (sizeOfArgv(token) == 1) {
    return 1;
  }
  if (str != NULL) {
    if (!strcmp(str, chs3)) {
      return 2;
    }
    if (!strcmp(str, chs4)) {
      return 2;
    }
  }
  if (str != NULL) {
    if (!strcmp(str, chs5)) {
      return 3;
    }
    if (!strcmp(str, chs6)) {
      return 3;
    }
  }
  return 0;
}

// Call the function that belong to the case of cd.
int cd(char **argv, int isParallel, int typeCd) {
  switch (typeCd) {
    case 0:
      if (sizeOfArgv(argv) > 2) {
        fprintf(stderr, "Error: Too many arguments\n");
        pid_t pid = getpid();
        printf("%d \n", pid);
        return 2;
      }
      if (cdRegular(argv, isParallel) == 2) {
        return 2;
      }
      break;
    case 1:
      if (cdAlone(argv, isParallel) == 2) {
        return 2;
      }
      break;
    case 2:
      if (sizeOfArgv(argv) > 2) {
        fprintf(stderr, "Error: Too many arguments\n");
        pid_t pid = getpid();
        printf("%d \n", pid);
        return 2;
      }
      if (cdPoints(argv, isParallel) == 2) {
        return 2;
      }
      break;
    case 3:
      if (sizeOfArgv(argv) > 2) {
        fprintf(stderr, "Error: Too many arguments\n");
        pid_t pid = getpid();
        printf("%d \n", pid);
        return 2;
      }
      if (cdMinus(argv, isParallel) == 2) {
        return 2;
      }
      break;
    default:break;
  }
  return 0;
}

// In case of cd alone or cd ~
int cdAlone(char **argv, int isParallel) {
  char buffer[MAX_SIZE_INPUT];
  pid_t pid = getpid();
  arrayHistory[sizeOfHistory].pid = pid;
  arrayJobs[sizeOfJobs].pid = pid;
  printf("%d \n", pid);
  char homedir[MAX_SIZE_INPUT];
  strcpy(homedir, homedirFirst);
  strcpy(paths[sizeOfPaths], homedir);
  // For cd miuns.
  getcwd(lastPath, MAX_SIZE_INPUT);
  // In case that there is folder to get in after the ~.
  if (sizeOfArgv(argv) > 2) {
    strcpy(buffer, argv[2]);
    strcat(homedir, "/");
    strcat(homedir, buffer);
    strcpy(paths[sizeOfPaths], homedir);
  }
  if (chdir(homedir)) {
    // In case of problem.
    if (errno != 0) {
      if (errno == ENOENT) {
        fprintf(stderr, "Error: No such file or directory\n");
        paths[sizeOfPaths][0] = '\0';
        errno = 0;
        return 2;
      } else {
        paths[sizeOfPaths][0] = '\0';
        if (errno == 10) {
          errno = 0;
          sizeOfPaths++;
          return 0;
        }
        errno = 0;
      }
      return 2;
    }
    return 2;
  }
  sizeOfPaths++;
  return 0;
}

// To know how much arguments we get.
int sizeOfArgv(char **argv) {
  int size = 0;
  while (*argv != NULL) {
    argv++;
    size++;
  }
  return size;
}

// In case of cd ..
int cdPoints(char **argv, int isParallel) {
  char buffer[MAX_SIZE_INPUT];
  pid_t pid = getpid();
  // Save the pid's.
  arrayHistory[sizeOfHistory].pid = pid;
  arrayJobs[sizeOfJobs].pid = pid;
  printf("%d \n", pid);
  if (sizeOfPaths == 0) {
    getcwd(buffer, MAX_SIZE_INPUT);
    strcpy(paths[0], buffer);
  } else {
    strcpy(paths[sizeOfPaths], paths[sizeOfPaths - 1]);
  }
  int placeQ = 0;
  int j = 0;
  // Parsering.
  if (paths[sizeOfPaths] != NULL) {
    while (j < strlen(paths[sizeOfPaths])) {
      if (paths[sizeOfPaths][j] == '/') {
        placeQ = j;
      }
      j++;
    }
    paths[sizeOfPaths][placeQ] = '\0';
  }
  // For cd minus.
  getcwd(lastPath, MAX_SIZE_INPUT);
  if (chdir(paths[sizeOfPaths])) {
    if (errno != 0) {
      if (errno == ENOENT) {
        fprintf(stderr, "Error: No such file or directory\n");
        paths[sizeOfPaths][0] = '\0';
        errno = 0;
      } else {
        paths[sizeOfPaths][0] = '\0';
        // In case that after history.
        if (errno == 10) {
          errno = 0;
          sizeOfPaths++;
          return 0;
        }
        errno = 0;
      }
      return 2;
    }
    return 2;
  }
  sizeOfPaths++;
  return 0;
}

// In case of cd -.
int cdMinus(char **argv, int isParallel) {
  int flagCh = 0;
  char buffer[MAX_SIZE_INPUT];
  pid_t pid = getpid();
  // Save the pid's.
  arrayHistory[sizeOfHistory].pid = pid;
  arrayJobs[sizeOfJobs].pid = pid;
  printf("%d \n", pid);
  // In the first time of cd, do nothing.
  if (sizeOfPaths == 0) {
    return 0;
  } else if (sizeOfPaths == 1) {
    getcwd(lastPath, MAX_SIZE_INPUT);
    strcpy(paths[sizeOfPaths], firstPath);
    if (chdir(firstPath)) {
      flagCh = 1;
    }
  } else {
    strcpy(paths[sizeOfPaths], paths[sizeOfPaths - 2]);
    strcpy(buffer, lastPath);
    // For cd minus.
    getcwd(lastPath, MAX_SIZE_INPUT);
    strcpy(paths[sizeOfPaths], buffer);
    if (chdir(buffer)) {
      flagCh = 1;
    }
  }
  if (errno != 0 && flagCh) {
    if (errno == ENOENT) {
      fprintf(stderr, "Error: No such file or directory\n");
      paths[sizeOfPaths][0] = '\0';
      errno = 0;
    } else {
      paths[sizeOfPaths][0] = '\0';
      if (errno == 10) {
        errno = 0;
        sizeOfPaths++;
        return 0;
      }
      errno = 0;
    }
    return 2;
  }
  // In case of problem at chdir.
  if (flagCh) {
    return 2;
  }
  sizeOfPaths++;
  return 0;
}

int checkEnter(char *str) {
  char strTemp[MAX_SIZE_INPUT];
  strcpy(strTemp, str);
  char *array[MAX_SIZE_INPUT];
  int i = 0;
  array[i] = strtok(strTemp, " ");
  while (array[i] != NULL) {
    array[++i] = strtok(NULL, " ");
  }
  if (sizeOfArgv(array) == 1) {
    if (!strcmp(array[0], "\n")) {
      return 1;
    }
  }
  return 0;
}