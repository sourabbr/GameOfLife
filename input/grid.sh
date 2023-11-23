#!/bin/bash

GRID_SIZE=$1

FILENAME="${GRID_SIZE}.txt"

touch $FILENAME

for ((i = 0; i < GRID_SIZE * GRID_SIZE; i++)); do
    cell=$((RANDOM % 2))
    printf "$cell" >> $FILENAME
done

printf "\n" >> $FILENAME