#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;
  
  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;
  
  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

int getlink(char *path, struct linkblock *lb){
  int fd;
  if((fd = open(path, O_NOFOLLOW)) < 0) {
    return -1;
  }
  if(read(fd, &(lb->size), sizeof(lb->size)) != sizeof(lb->size)){
    close(fd);
    return -1;
  }
  if(read(fd, lb->path, lb->size+1) != lb->size+1){
    close(fd);
    return -1;
  }
  close(fd);
  return 0;
}

void
ls(char *path)
{
  char buf[512], *p;
  struct linkblock lb;
  int fd;
  struct dirent de;
  struct stat st;
  
  if((fd = open(path, O_NOFOLLOW)) < 0){
    printf(2, "ls: cannot open %s\n", path);
    return;
  }
  if(fstat(fd, &st) < 0){
    printf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }
  
  switch(st.type){
  case T_FILE:
    printf(1, "%s %d %d %d\n", fmtname(path), st.type, st.ino, st.size);
    break;

  case T_SYMLINK:
    if(getlink(path, &lb) < 0) {
      printf(1, "ls: cannot read link %s\n", path);
      break;
    }
    printf(1, "%s %d %d %d", fmtname(path), st.type, st.ino, st.size);
    printf(1, " -> %s\n", lb.path);
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf(1, "ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf(1, "ls: cannot stat %s\n", buf);
        continue;
      }
      printf(1, "%s %d %d %d", fmtname(buf), st.type, st.ino, st.size);
      if(st.type == T_SYMLINK){
        if(getlink(buf, &lb) < 0){
          printf(1, "ls: cannot read link %s\n", path);
        }
        printf(1," -> %s", lb.path);
      }
      printf(1,"\n");
    }
    break;

  close(fd);
  }
}

int
main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    ls(".");
    exit();
  }
  for(i=1; i<argc; i++)
    ls(argv[i]);
  exit();
}
