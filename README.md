# Fireland

## Architecture
* we are using unity builds (i know skelington handles everything correctly the first time, this is an easy-of-work optimization)
* i am not exactly sattisfied with my progress, but it was nice fucking finally coding again mostly by hand

| File                 | Description |
| :------------------: | :---------: |
| main.cpp             | main, duh |
| raylib-extra.hpp     | the minimal of what i needed, feel free to copy over the whole thing if you wish |
| draw.hpp             | drawing decoupled; due to the scope of the project, admittedly it doesnt matter that much |
| Board.hpp            | my majestic board, if you dont like it, you can go fuck yourself; please read the comments |
| board_generation.hpp | random map generation completely divined by the machinegod; you may want to play around with the values in GenerationParams |

## Notes
* please emil, im begging you, please do not over complicate things
* flexibility >>> performance, because we have one day and 4 ghz * 8
* please dont make me edit your GNU/pain style, i will literally die
* if i were you, i would fine tune the map sizing (roads, buildings, number of tiles) / window size (hard code le BIG if it takes more than 3 femto seconds) and introduce the actual driving
* also, drawmaxx because Lord knows i cant
* i should be using libchads random, but im not currently; i know, im an evil and twisted person
* somehow the build is so fucked that gdb will kill itself
