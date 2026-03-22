#!/bin/sh
FORMAT=16
FREQ=22050
sonar -i ./sfx/alien-shot.xml -b${FORMAT} -s${FREQ} output:./Assets/alien-shot.wav
sonar -i ./sfx/laser-shot.xml -b${FORMAT} -s${FREQ} output:./Assets/laser-shot.wav
sonar -i ./sfx/explosion.xml -b${FORMAT} -s${FREQ} output:./Assets/explosion.wav
sonar -i ./sfx/spawn-effect.xml -b${FORMAT} -s${FREQ} output:./Assets/spawn-effect.wav
sonar -i ./sfx/hit-effect.xml -b${FORMAT} -s${FREQ} output:./Assets/hit-effect.wav
sonar -i ./sfx/game-over.xml -b${FORMAT} -s${FREQ} output:./Assets/game-over.wav
