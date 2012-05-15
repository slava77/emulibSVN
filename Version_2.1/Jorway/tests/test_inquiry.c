#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ieee_fun_types.h"

int main( int argc, char** argv ){

  int branch = 0;
  if ( argc == 2 ){
    branch = atoi( argv[1] );
  }
  printf( "Inquiring ID %d, BUS %d\n", ID(branch), BUS(branch) );

  cdchn( branch, 1, 0 );

  unsigned char *buf;
  buf = sjy_inquiry (branch);
  printf("Buffer is at %p\n", buf );
  printf("Jorway Hardware Version is \"%s\"\n",&buf[8]);
  
  cdchn( branch, 0, 0 );

  return 0;
}
