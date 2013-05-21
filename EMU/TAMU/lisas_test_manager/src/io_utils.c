#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <string.h>

int getint(int *n);
int gethex(int *n);
void wait_for_return(void);
int rand(void);

/* getint: read in an integer, with n as the default
 * (returns 0 if successful, -1 if integer conversion failed)
 */

int getint(int *n) {
  int default_n, i, j;
  char line[80];
  
  default_n = *n;
  i = scanf("%[^\n]", line);  /* Read up to the first "\n"=return */
  getchar();
  j = sscanf(line, "%d", n);
  if (!(i > 0 && j > 0)) {
    *n = default_n;
    if (i > 0) return -1;
  }
  return 0;
}

/* gethex: read in a hex integer, with n as the default
 * (returns 0 if successful, -1 if integer conversion failed)
 */

int gethex(int *n) {
    int default_n, i, j;
    char line[80];

    default_n = *n;
    i = scanf("%[^\n]", line);  /* Read up to the first "\n"=return */
    getchar();
    j = sscanf(line, "%x", n);
    if (!(i > 0 && j > 0)) {
        *n = default_n;
        if (i > 0) return -1;
    }
    return 0;
}


/* getreal: read in a real number, with x as the default
 * (returns 0 if successful, -1 if float conversion failed)
 */

int getreal(float *x) {
    int default_x, i, j;
    char line[80];

    default_x = *x;
    i = scanf("%[^\n]", line);  /* Read up to the first "\n"=return */
    getchar();
    j = sscanf(line, "%f", x);
    if (!(i > 0 && j > 0)) {
        *x = default_x;
        if (i > 0) return -1;
    }
    return 0;
}

/* getcha: read in a character, with c as the default
 * (returns 0 if successful, -1 if character conversion failed)
 */

int getcha(char *c) {
    int default_c, i, j;
    char line[80];

    default_c = *c;
    i = scanf("%[^\n]", line);  /* Read up to the first "\n"=return */
    getchar();
    j = sscanf(line, "%c", c);
    if (!(i > 0 && j > 0)) {
        *c = default_c;
        if (i > 0) return -1;
    }
    return 0;
}

/* getstr: read in a string, with s as the default
 * (returns 0 if successful, -1 if character conversion failed)
 */
 
int getstr(char *s) {
    int  i, j;
    char default_s[80];
    char line[80];
    
    strcpy(default_s, s);
    i = scanf("%[^\n]", line);  /* Read up to the first "\n"=return */
    getchar();
    j = sscanf(line, "%s", s);
    if (!(i > 0 && j > 0)) {
        strcpy(s, default_s);
        if (i > 0) return -1;
    }
    return 0;
}

/* asctos: a little function to convert our 4-character ascii constants to 
 * strings. (Maybe there is a C-library equivalent?)
 */

char *asctos(int j)
{
  int            i;
  char           *dummy;
  static char    name[5];
  dummy = (char *)&j;
  for (i = 0; i < 4; i++) name[i] = dummy[3-i];
  name[4] = '\0';
  return name;
}

/* Wait for the return key to be pressed */

void wait_for_return(void)
{
    char   ans;
    printf("\nEnter RETURN to continue : ");
    scanf("%c", &ans);
    return;
}

/* Fortran version of Wait for the return key to be pressed */

void wait_for_return_(void)
{
    wait_for_return();
    return;
}

void ran_(float *x) 
{
  *x = rand() / (RAND_MAX + 1.0);
  return;
}
