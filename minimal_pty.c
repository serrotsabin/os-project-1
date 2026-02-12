// Getting Issues Like All Key Press Not Getting Captured.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <util.h>
#include <sys/select.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>

double now_ms()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
}

int main()
{
  int master;
  pid_t pid = forkpty(&master, NULL, NULL, NULL);

  if (pid < 0)
  {
    perror("forkpty");
    exit(1);
  }

  if (pid == 0)
  {
    // Child: become bash
    execl("/bin/bash", "bash", NULL);
    exit(1);
  }

  // Parent: relay I/O
  fd_set readfds;
  char buf[4096];

  printf("Minimal PTY started. Type commands (Ctrl+D to exit)\n");

  FILE *log = fopen("appendix/appendix_latency_raw.txt", "w");
  fprintf(log, "keystroke_ms,char,echo_ms,latency_ms\n");
  fflush(log);

  double last_key_time = 0;
  char last_char = 0;

  while (1)
  {
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    FD_SET(master, &readfds);

    int ret = select(master + 1, &readfds, NULL, NULL, NULL);
    if (ret < 0)
    {
      perror("select");
      break;
    }

    // User input -> shell
    if (FD_ISSET(STDIN_FILENO, &readfds))
    {
      int n = read(STDIN_FILENO, buf, 1);
      if (n <= 0)
        break;

      last_key_time = now_ms();
      last_char = buf[0];

      write(master, buf, 1);
    }

    // Shell output -> user
    if (FD_ISSET(master, &readfds))
{
    int n = read(master, buf, sizeof(buf));
    if (n <= 0) break;

    double t_echo = now_ms();
    write(STDOUT_FILENO, buf, n);

    if (last_key_time > 0)
    {
        double latency = t_echo - last_key_time;
        fprintf(log, "%.3f,%c,%.3f,%.3f\n",
                last_key_time, last_char, t_echo, latency);
        fflush(log);

        last_key_time = 0;  // reset
    }
}

  }

  return 0;
}