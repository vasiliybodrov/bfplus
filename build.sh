#! /bin/sh

rm -f bf

gcc -std=c89 -Wall -Wextra -pedantic -gdwarf-4 bf+.c -o bf+ && echo "OK" || echo "ERROR"
