#define _GNU_SOURCE
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include <unistd.h>
#include <ctype.h>

// global variables //
PGconn *CONN;
PGresult *RESULT;

// prototypes //
void graceful_exit();
void check_result();
void sql(char SQLcomand[]);
int psql_copy(char tabelName[], char fileName[], int chunkSize);

// functions //
void main(int argc, char *argv[]) {
    printf("so far");
    char host[20];
    int port;
    char user[40];
    char db[20];
    char password[1024] = "";
    if (argc >= 2 ) {  // get connection information from command line arguments:
        if (argc < 6) {  // need all parameters or none of them
            printf("\nhost, port, role, database, and password required.\n(-h -p -r -d and -a respectively)\n");
            graceful_exit();
        }
        int option;
        printf("this far");
        while ((option = getopt(argc, argv, ":h:p:r:d:a:")) != -1) {
            switch (option) {
            case 'h':
                printf("%s", optarg);
                sprintf(host, "%s", optarg);
            case 'p':
                port = atoi(optarg);
            case 'r':
                printf("%s", optarg);
                sprintf(user, "%s", optarg);
            case 'd':
                printf("%s", optarg);
                sprintf(db, "%s", optarg);
            case 'a':
                printf("%s", optarg);
                sprintf(password, "%s", optarg);
            }
        }
    } else {
        // request connection information from user
        printf("host:");
        scanf("%s", host);
        printf("port:");
        scanf("%i", &port);
        printf("role:");
        scanf("%s", user);
        printf("database:");
        scanf("%s", db);
        // request password for user/role
        char *getpass(const char *prompt);
        sprintf(password, "%s", getpass("password:"));
    }
    printf("got it");
    // create arg-string for PQconnectdb function
    char pgArg[256];
    sprintf(pgArg, "host=%s port=%i user=%s dbname=%s", host, port, user, db);
    if (strlen(password) > 0) { // append password to arg string if given
        char passArg[265];
        sprintf(passArg, " password=%s", password);
        strcat(pgArg, passArg);
    }
    CONN = PQconnectdb(pgArg); // connect to database with arg-string like: (host=XXX port=XXX user=XXX database=XXX [password=XXX])
    if (PQstatus(CONN) == CONNECTION_BAD) {
        printf("Connection to database failed: %s\n", PQerrorMessage(CONN));
        graceful_exit();
    }
    printf("\nCONNECTION ESTABLISHED\n\n");
    // get and return connection information
    int ver = PQserverVersion(CONN);
    printf("\nserver version: %d\n", ver);
    sql("DROP TABLE IF EXISTS cPsql_demo_users;");
    sql("DROP TABLE IF EXISTS cPsql_demo_cars;");
    sql("CREATE TABLE cPsql_demo_cars("
        "id UUID NOT NULL PRIMARY KEY,"
        "make VARCHAR(50) NOT NULL,"
        "model VARCHAR(50) NOT NULL,"
        "model_year VARCHAR(50) NOT NULL,"
        "country_of_manufacture VARCHAR(50),"
        "price VARCHAR(50) NOT NULL"
        ");");
    sql("CREATE TABLE cPsql_demo_users ("
        "id UUID NOT NULL PRIMARY KEY,"
        "first_name VARCHAR(50) NOT NULL,"
        "last_name VARCHAR(50) NOT NULL,"
        "gender VARCHAR(50) NOT NULL,"
        "birthday DATE NOT NULL,"
        "email VARCHAR(50),"
        "country_of_origin VARCHAR(50) NOT NULL,"
        "car_id UUID REFERENCES cars(id)"
        ");");
    if (psql_copy("cPsql_demo_cars", "csv/cars.csv", 100) != 1) {
        printf("%s\n", "psql_copy failed");
        graceful_exit();
    }
    printf("\nSENT %s TO TABLE %s\n", "cars.csv", "cPsql_demo_cars");

    if (psql_copy("cPsql_demo_users", "csv/users.csv", 100) != 1) {
        printf("%s\n", "psql_copy failed");
        graceful_exit();
    }
    printf("\nSENT %s TO TABLE %s\n", "users.csv", "cPsql_demo_users");

    sql("ALTER TABLE cPsql_demo_users ADD CONSTRAINT unique_emails UNIQUE(email);");
    sql("ALTER TABLE cPsql_demo_users ADD CONSTRAINT unique_cars UNIQUE(car_id);");
    sql("UPDATE cPsql_demo_users SET car_id = '921577ef-22ee-4db8-9dbb-9656b8be1621' WHERE id = 'c9b5c30e-bbeb-480c-9ed0-81a612012e4a'");
    sql("UPDATE cPsql_demo_users SET car_id = '8a85c592-ee25-4315-b60c-4faa0b410f20' WHERE first_name = 'Gustavo' AND last_name = 'Antoniottii' AND email = 'gantoniottiir@stumbleupon.com';");
    sql("UPDATE cPsql_demo_users SET car_id = '1312448e-9550-4118-929a-c3d58e3e7161' WHERE id = '80e1e7ce-86a3-4a81-841a-5e1a6a2da191';");
    graceful_exit();
};

void graceful_exit() { // cleans up before exiting
    printf("%s\n", PQerrorMessage(CONN));
    PQclear(RESULT);
    PQfinish(CONN);
    exit(1);
}

void check_result() {
    if (PQresultStatus(RESULT) != PGRES_COMMAND_OK) {
        graceful_exit();
    } else {
        PQclear(RESULT);
    }
}

void sql(char SQLcommand[]) {
    RESULT = PQexec(CONN, SQLcommand);
    check_result();
}

int psql_send_chunk(char copyCommandStr[], char buffer[]) {
    RESULT = PQexec(CONN, copyCommandStr); // open copy stream to postgres
    if (PQresultStatus(RESULT) == PGRES_COPY_IN) { // check to make sure it's open
        PQclear(RESULT);
    } else {
        printf("\nERROR: could not open copy stream\n");
        return 0;
    }
    if (PQputCopyData(CONN, buffer, strlen(buffer)) == 0) { // send buffer to postgres to be copied and check it worked
        printf("\nERROR: could not putCopyData\n");
        return 0;
    }
    if (PQputCopyEnd(CONN, NULL) != 1) { // close copy stream to commit changes and check it worked
        printf("\nERROR: problem with PQputCopyEnd\n");
        return 0;
    }
    // issue success code if successful
    return 1;
}

int psql_copy(char tableName[], char fileName[], int chunkSize) {
    // -- open file, read it's contents, and load into buffer -- //
    FILE *fpointer = fopen(fileName, "rb");
    // check if file exists
    if (fpointer == NULL) {
        printf("\nERROR: could not open file\n");
        // fclose(fpointer);
        return 0;
    }
    // get file size by cursor position at end of file
    fseek(fpointer, 0L, SEEK_END);
    int file_size = ftell(fpointer);
    fseek(fpointer, 0L, SEEK_SET); // reset cursor to beginning of file
    // construct command for opening  copy stream like: "COPY table_name FROM STDIN DELIMITER "," ;"
    int command_str_length = 32 + strlen(tableName); // 32 for characters of command string wihout table_name.
    char copy_command_str[command_str_length];
    sprintf(copy_command_str, "COPY %s FROM STDIN DELIMITER ',' CSV HEADER;", tableName);
    // set up for getting header
    int capacity = 1;
    char *buffer = (char *)malloc(capacity);
    buffer[0] = 0;
    char *temp_buffer = (char *)malloc(capacity);
    temp_buffer[0] = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t characters_read = 0;
    int lines = 1; // initial at 1 for the header
    // -- get and send header -- //
    getline(&line, &len, fpointer);
    capacity += strlen(line);
    temp_buffer = realloc(buffer, capacity);
    strcat(temp_buffer, line);
    buffer = temp_buffer;
    // send header
    if (psql_send_chunk(copy_command_str, buffer) != 1) {
        printf("\nERROR: could not send header\npsql_send_chunk failed\n");
        return 0;
    }
    // reset
    capacity = 1;
    lines = 0;
    strcpy(buffer, "");
    buffer = realloc(buffer, capacity);
    strcpy(temp_buffer, "");
    temp_buffer = realloc(temp_buffer, capacity);
    // -- get and send chunks -- //
    sprintf(copy_command_str, "COPY %s FROM STDIN DELIMITER ',';", tableName); // remove CSV HEADER from copy comand
    while (ftell(fpointer) < file_size) {
        while ((characters_read = getline(&line, &len, fpointer)) != -1) {
            lines++;
            capacity += strlen(line);
            temp_buffer = realloc(buffer, capacity);
            strcat(temp_buffer, line);
            buffer = temp_buffer;
            if (lines >= chunkSize) {
                break;
            }
        }
        // send chunks
        if (psql_send_chunk(copy_command_str, buffer) != 1) {
            printf("\nERROR: could not send chunk\npsql_send_chunk failed\n");
            return 0;
        }
        // reset
        capacity = 1;
        lines = 0;
        // printf("\nBUFFER:\n%s\n", buffer);
        strcpy(buffer, "");
        buffer = realloc(buffer, capacity);
        strcpy(temp_buffer, "");
        temp_buffer = realloc(temp_buffer, capacity);
    }
    buffer = NULL;
    free(buffer);
    temp_buffer = NULL;
    free(temp_buffer);
    fclose(fpointer);
    // return succes signal if successful
    return 1;
}
