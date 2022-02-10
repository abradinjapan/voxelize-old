#pragma once

#include <stdio.h>
#include <string.h>

namespace abradinjapan {
    char* concatenate(char* s1, char* s2) {
        long long l1 = strlen(s1);
        long long l2 = strlen(s2);
        long long lt = l1 + l2;
        char* output = new char[lt + 1];

        // fill in data
        for (long long i = 0; i < l1; i++) {
            output[i] = s1[i];
        }
        for (long long i = l1; i < lt; i++) {
            output[i] = s2[i - l1];
        }

        // null terminate
        output[lt] = 0;

        // return new string
        return output;
    }

    char* load_file(char* file_path) {
        char* output = 0;
        long long length = 0;
        FILE* f;

        // open the file
        f = fopen(file_path, "rb");

        // get file length
        if (f)
        {
            fseek(f, 0, SEEK_END);
            length = ftell(f);
            rewind(f);
        }
        else
        {
            return 0;
        }

        // check if file is too large
        if (length > 2147483647) // subtracted 1 for null termination
        {
            fclose(f);
            return 0;
        }

        // create output
        output = new char[length + 1];

        // read file into memory
        length = fread(output, 1, length, f);

        // close file
        fclose(f);

        // write the null termination
        output[length] = 0;

        // return the binary buffer
        return output;
    }
}