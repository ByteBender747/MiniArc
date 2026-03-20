#!/bin/sh
FORMAT=16
FREQ=22050
sonar -i alien-shot.xml -b${FORMAT} -s${FREQ} output:../Assets/alien-shot.wav
sonar -i laser-shot.xml -b${FORMAT} -s${FREQ} output:../Assets/laser-shot.wav
sonar -i explosion.xml -b${FORMAT} -s${FREQ} output:../Assets/explosion.wav
sonar -i spawn-effect.xml -b${FORMAT} -s${FREQ} output:../Assets/spawn-effect.wav
sonar -i hit-effect.xml -b${FORMAT} -s${FREQ} output:../Assets/hit-effect.wav
sonar -i game-over.xml -b${FORMAT} -s${FREQ} output:../Assets/game-over.wav