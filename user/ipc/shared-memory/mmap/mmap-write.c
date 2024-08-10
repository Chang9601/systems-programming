#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <memory.h>

#define MEM_SZ 100

struct person {
  char name[20];
  int age;
  char phone[20];
};

int 
main(int argc, char *argv[]){
  int fd;
  struct person p;
  void *addr;

  if (argc != 2) {
    fputs("사용법: <파일 경로>\n", stderr);
    exit(EXIT_FAILURE);
  }
   
  if ((fd = open(argv[1], O_RDWR)) == -1) {
    fputs("open()\n", stderr);
    exit(EXIT_FAILURE);
  }

  addr = mmap(NULL, MEM_SZ, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); 
  if (addr == MAP_FAILED) {
    fputs("mmap()\n", stderr);
    exit(EXIT_FAILURE);
  }
  
  if (close(fd) == -1) {
    fputs("close()\n", stderr);
    exit(EXIT_FAILURE);
  };

  strncpy(p.name, "가나다", sizeof(p.name));
  p.age = 21;
  strncpy(p.phone, "010-1234-5678", sizeof(p.phone));

  memcpy(addr, &p, sizeof(p));
  msync(addr, sizeof(p), MS_SYNC);

  if (munmap(addr, MEM_SZ) == -1){
    fputs("munmap()\n", stderr);
    exit(EXIT_FAILURE);
  }
  
  exit(EXIT_SUCCESS);
}