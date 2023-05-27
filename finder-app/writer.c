#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

int main(int argc, char *argv[]) {

    // if no parameters are supplied, exit with error 1
    if (argc < 3) {
        syslog(LOG_ERR, "no parameters supplied");
        // print statement to console
        printf("no parameters supplied\n");
        exit(1);
    }

    FILE *file = fopen(argv[1], "w");
    char *str = argv[2];
    openlog("writer.c", LOG_PID, LOG_USER);
    syslog(LOG_DEBUG, "writing %s to %s", str, argv[1]);
    // print statement to console
    printf("writing %s to %s\n", str, argv[1]);
    fprintf(file, "%s", str);
    fclose(file);
    
    // if file is null or str is null, exit with error 1
    if (file == NULL || str == NULL) {
        syslog(LOG_ERR, "error opening %s", argv[1]);
        // print statement to console
        printf("error opening %s\n", argv[1]);
        exit(1);
    }

    closelog();

    return 0;
}