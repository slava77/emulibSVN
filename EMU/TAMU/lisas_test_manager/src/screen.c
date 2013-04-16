
/******************************************************************************
 *  This is the SCREEN subroutine for use by the run status display, etc
 *  (converted from Fortran to c on 21/1/99)
 *****************************************************************************/
#include <stdio.h>
#include <string.h>

int screen(char *mode)
{
    int    status;
    char   csi[2] = {(char)0x1b, '['};

    status = 0;

/* strcasecmp is a case-independent comparison of two strings, which yields
 * zero if the strings match
 */
    if (!strcasecmp(mode, "clear"))
        printf("%c%c%c%c%c%c", csi[0], csi[1],'H', csi[0], csi[1], 'J');
    else if (!strcasecmp(mode, "upperleft")) 
        printf("%c%c%c%c%c%c", csi[0], csi[1], '1', ';', '1', 'f');
    else if (!strcasecmp(mode, "lowerleft")) 
        printf("%c%c%c%c%c%c%c", csi[0], csi[1], '2', '1', ';', '1', 'f');
    else if (!strcasecmp(mode, "revvid"))
        printf("%c%c%c%c", csi[0], csi[1], '7', 'm');
    else if (!strcasecmp(mode, "normvid"))
        printf("%c%c%c", csi[0], csi[1], 'm');
    else if (!strcasecmp(mode, "toggle"))
        printf("%c%c%c%c%c%c", csi[0], csi[1], '?', '3', '8', 'l');
    else if (!strcasecmp(mode, "clr4010"))
        printf("%c%c%c%c%c%c", csi[0], csi[1], '?', '3', '8', 'h');
    else if (!strcasecmp(mode, "upone"))
        printf("%c%c%c%c", csi[0], csi[1], '2', 'A');
    else
        printf("SCREEN: unknown command \"%s\"\n", mode);
        status = -1;
    return(status);
}

/******************************************************************************
 *  This is wrapper for the SCREEN subroutine for calls from Fortran
 *****************************************************************************/

void screen_(char *mode) {
    char command[10];
    sscanf(mode, "%s", command);
    screen(command);
}
