#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#if defined(__unix__) || defined (__APPLE__)
  #include <sys/stat.h>
#endif

#include "WebServer/include/ws.h"

FILE *input_file;

void onopen(int fd)
{
    printf("Connection opened\n");
}

void onclose(int fd)
{
    printf("Connection closed\n");
}

void onmessage(int fd, const unsigned char *msg, uint64_t size, int type)
{

    printf("%s\n", msg);
    fseek(input_file, 0, SEEK_SET);
    fprintf(input_file, "%s\n", msg);
}

int main()
{
    #if defined(__unix__) || defined (__APPLE__)
    mkfifo("input.pipe", 0666);
    input_file = fopen("input.pipe", "w");
    #else
    input_file = fopen("input.txt", "w");
    #endif
    struct ws_events evs;
    evs.onopen    = &onopen;
    evs.onclose   = &onclose;
    evs.onmessage = &onmessage;

    ws_socket(&evs, 8080, 0);

    return (0);
}
