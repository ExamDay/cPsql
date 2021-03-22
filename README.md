<h1 align="center">cPsql:<br>PostgreSQL Automation Utilities in C</h1>

This is a set of programs that use C to interact with a PostgreSQL database. While useful in it's
unmodified state for automating table updates, this also serves as an example of how to use the
otherwise opaque Postgres C libraries for controlling PostgreSQL servers
and their databases.

## Why cPsql?
There are many closed-source database management programs available for PostgreSQL that do almost
everything for you. If you, dear reader, are anything like myself, then standalone
proprietary software that "Does Almost Everything!"&trade; smells of bloat and engineered
dependance. The vast majority of use-cases for such programs can and should be
handled by a single C function, especially whenever a particular operation needs to be
integrated into another piece of software, or to run with maximum time/space efficiency for
deployment at scale. This software demonstrates how such a thing can be done, and will soon include
a set of convenient C/C++ headers for the minimalist developer.
## How To Compile
#### PSA
The provided installation script `setup.sh` should run fine on any Debian or RedHat Linux
system. I <strong>strongly</strong> recommend you read it before you run it! This script does
require sudo privileges and confident as I am in my own code, you will know your computer
better than I do.
<br><br>
Only YOU can prevent wildfires.
#### Debian and RedHat Linux
- Clone this repository.
- Read `setup.sh`.
- Then compile the with:
```bash
cd /path/to/this/repository
sudo bash setup.sh
```
- That's it!
- Optionally: follow the testing instructions described below.
#### Windows
- Coming Soon.
#### Mac
- Coming Soon.
## Testing:
It's a good idea to check your installation by running `cPsqlDemo.bin` (compiled in the step above).
This is a C script that attempts to connect to a ProstgreSQL
server and perform some operations on whatever database you point it at.

Specifically, it does this list of things, in order:

1. Prompts the user for a host IP address, port number, role/username, database name,
and password with which to forge a connection to a running PostgreSQL server.
(To get past this step, you will need to provide connection info to an actual database,
but I trust you can manage that on your own, if you don't have one laying
around already.)

2. Attempts to forge a connection with the provided credentials and prints out the server version as a proof of life. Otherwise prints an error message.

3. Drops any tables in the database named "cPsql_demo_users" or "cPsql_demo_cars" and
recreates them with certain constraints on their columns.
In particular, the car_id column of cPsql_demo_users is referenced to the id column of
cPsql_demo_cars so that each user can be assigned at most one car from the cars table, and only
those cars are allowed.

    If by some cosmic coincidence you
    already have tables with these names in your database:

    1. Buy a lottery ticket and PayPal
    half your winnings to watch.closely.0@gmail.com (Unintuitive, certainly, but rest
    assured; this step is crucial to assuage the wrath of the Karmic Random
    Number Gods).
    2. Make a new db before running this program.

5. Populates these tables with a load of mock data from the CSV files in the folder labeled
accordingly. This program buffers files in small sets of lines at
once, dynamically allocating only as much memory as needed for each chunk. This means it is able to
transmit tables of any size with minimum memory usage and a maximum that can be specified. (so long as your device has the memory to
store any single line of the table at once).


6. After data transfer, adds a couple more constraints to these tables,
requiring each user have a unique email address, and a unique car_id, or none at all.

7. Assigns some random people random cars.

8. Cleans up.

9. Prints "Success!" and exits.

All this is done with a ton of error checking along the way to ensure everything goes as intended
and any errors are reported if they exist.
##

## Contributing
For contributors to the project; do this before making your first commit:

- Install pre-commit
```bash
cd /path/to/this/repository/
sudo apt install pre-commit
pre-commit install
```
(we do all of our development on linux)

- To test updates to the readme and other GitHub flavored markdown, simply install Grip
and feed it your desired file.
```bash
pip3 install grip
grip README.md
```
- Then follow the link provided by the Grip sever for a live preview of your work.

- When satisfied with your changes you can compile to an html file with:
```bash
grip README.md --export README.html
```
## Authors

* **Gabe M. LaFond** - *Initial work* - [ExamDay](https://github.com/ExamDay)

See also the list of [contributors](https://github.com/ExamDay/cPsql/contributors) who participated in this project.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
