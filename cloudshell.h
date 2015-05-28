#include <errno.h>
#include <sys/stat.h>
#include <string.h>

#include "utils.h"

char* pp = ".pipe";
char* db = ".userdb";

typedef struct user {

    char* username;
    char* password;
    double balance;

} user;

void init_cloud();

user* get_user(char* username);

int check_password(char* username, char* password);

/* void monitor_pipe(); */

void register_new_user(char* username, char* password);

/* void run_command(char* username, char* command); */

void deposit(char* username, double value);

/* void server(); */

/* int main(void); */
