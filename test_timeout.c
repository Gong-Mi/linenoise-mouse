#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "linenoise.h"

int main(void) {
    const char *line;
    
    printf("Test 1: Timeout after 2 seconds.\n");
    linenoiseSetTimeout(2000);
    
    while (1) {
        printf("Entering linenoise... (Type something but don't hit enter)\n");
        line = linenoise("timeout> ");
        
        if (line == NULL) {
            if (errno == EAGAIN) {
                printf("\n[TIMEOUT] linenoise returned NULL with EAGAIN. State should be saved.\n");
                printf("Doing some 'game logic' work for 1 second...\n");
                sleep(1);
                printf("Resuming linenoise...\n");
            } else {
                printf("\n[EOF/Error] linenoise returned NULL. Exiting.\n");
                break;
            }
        } else {
            printf("\n[INPUT] User typed: '%s'\n", line);
            free((void*)line);
            break;
        }
    }
    
    return 0;
}
