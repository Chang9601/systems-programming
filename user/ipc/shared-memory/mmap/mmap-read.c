#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <memory.h>

struct person {
  char name[20];
  int age;
  char phone[20];
};

int 
main(int argc, char *argv[]){
  int fd;
  struct person p;
  struct stat sb;
  void *addr;

  if (argc != 2) {
    fputs("사용법: <파일 경로>\n", stderr);
    exit(EXIT_FAILURE);
  }
   
  if ((fd = open(argv[1], O_RDONLY)) == -1) {
    fputs("open()\n", stderr);
    exit(EXIT_FAILURE);
  }

  if (fstat(fd, &sb) == -1) {
    fputs("fstat()\n", stderr);
    exit(EXIT_FAILURE);
  }

  addr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0); 
  if (addr == MAP_FAILED) {
    fputs("mmap()\n", stderr);
    exit(EXIT_FAILURE);
  }
  
  if (close(fd) == -1) {
    fputs("close()\n", stderr);
    exit(EXIT_FAILURE);
  };

  // TODO: 파일 -> 구조체
  memcpy(&p, addr, sizeof(p));

  printf ("이름: %s\n", p.name);
  printf ("나이: %d\n", p.age);
  printf ("전화번호: %s\n", p.phone);

  if (munmap(addr, sb.st_size) == -1){
    fputs("munmap()\n", stderr);
    exit(EXIT_FAILURE);
  }
  
  exit(EXIT_SUCCESS);
}