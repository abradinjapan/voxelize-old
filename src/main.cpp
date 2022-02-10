#include "game/game.hpp"

int main() {
    abradinjapan::voxelize::game g = abradinjapan::voxelize::game();
    abradinjapan::voxelize::et error;

    error = g.play();

    if (error != abradinjapan::voxelize::et::et_no_error) {
        printf("Voxelize Game Error Code: %i\n", error);
        fflush(stdout);
    }

    return 0;
}