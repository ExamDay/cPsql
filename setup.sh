#!/bin/bash
LIGHT_CYAN="\033[1;96m"
LIGHT_RED="\033[1;31m"
SUCCESS="\ncPsqlDemo.bin compiled\n\nsetup complete\n"
FAILURE="\nCompilation failed.\n\nMust be root.\n\nIf already root then dependencies are missing (gcc, lipq-dev, postgresql-devel)\n"
# check for dependencies and install them. System flavor is detected by duck-typing.
# (we know we are on a RedHat system if the first two commands fail).
sudo dpkg -s libpq-dev || apt-get install libpq-dev || rpm -q "postgresql-devel" || yum install postgresql-devel
# compile the code
gcc cPsqlDemo.c -o cPsqlDemo.bin -I$(pg_config --includedir) -lpq -std=c99 && echo -e "${LIGHT_CYAN}${SUCCESS}" || echo -e "${LIGHT_RED}${FAILURE}"
gcc autoPsql.c -o autoPsql.bin -I$(pg_config --includedir) -lpq -std=c99 && echo -e "${LIGHT_CYAN}${SUCCESS}" || echo -e "${LIGHT_RED}${FAILURE}"
