#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_utils.h"

#define MAX_LINE_LENGTH 256

void load_env_from_file(const char *filepath) 
{
    FILE *file = fopen(filepath, "r");
    if (!file) 
    {
        perror("Error opening file");
        return;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) 
    {
        // Remove trailing newline character
        line[strcspn(line, "\n")] = '\0';

        // Skip empty lines or comments
        if (line[0] == '\0' || line[0] == '#') 
        {
            continue;
        }

        if (line[0] == ' ') 
        {
            perror("Leading whitespace is not supported");
        }

        trim_trailing(line);

        // Split line into key and value
        char *key = strtok(line, "=");
        char *value = strtok(NULL, "=");

        if (key && value) 
        {
            // Set the environment variable
            if (setenv(key, value, 1) != 0) 
            {
                perror("Error setting environment variable");
            }
        }
    }

    fclose(file);
}
