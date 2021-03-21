#!/bin/bash
LIGHT_CYAN="\033[1;96m"
LIGHT_RED="\033[1;31m"
# check for dependencies and install them. System flavor is detected by duck-typing.
# (we know we are on a RedHat system if the first two commands fail).
sudo dpkg -s libpq-dev || apt-get install libpq-dev || rpm -q "postgresql-devel" || yum install postgresql-devel 
# compile the code
gcc cPsqlDemo.c -o cPsqlDemo.bin -I$(pg_config --includedir) -lpq -std=c99 && echo -e "${LIGHT_CYAN}\ncPsqlDemo.bin compiled\n\nsetup complete\n" || echo -e "${LIGHT_RED}\ncompilation failed\n\nmust be root.\n\nif already root then dependencies are missing (gcc, lipq-dev, postgresql-devel)\n"
