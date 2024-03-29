#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  if(argc < 3 || argc > 4){
    printf(2, "Usage: ln old new\n");
    exit();
  }
  if ( argc == 4 && strcmp(argv[1], "-s") == 0) {
    if( link(1, argv[2], argv[3]) < 0 )
      printf(2, "link soft %s %s: failed\n", argv[2], argv[3]);
  }
  else if(link(0 ,argv[1], argv[2]) < 0)
    printf(2, "link %s %s: failed\n", argv[1], argv[2]);
  exit();
}
