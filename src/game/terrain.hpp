#pragma once

#include "types.hpp"

namespace abradinjapan::voxelize {
    chunk_888* generate_chunk(long long x, long long y, long long z) {
        chunk_888* output = new chunk_888();
        unsigned short height;
        float value_1, value_2;

        for (unsigned int i = 0; i < 8; i++) {
            for (unsigned int j = 0; j < 8; j++) {
                value_1 = (4.0f * sin(2 * 3.14159 + i) + 4.0f);

                //printf("%f / %f\n", value_1, value_2);

                height = (unsigned short)value_1;

                for (unsigned short k = 0; k < height; k++) {
                    output->set_block_at(i, j, k, 1);
                }

                for (unsigned short k = height; k < 8; k++) {
                    output->set_block_at(i, j, k, 0);
                }
            }
        }

        return output;
    }
}