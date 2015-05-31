#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "utils.h"

#define CREATE 1
#define CHANGE 2

char* pp_sc = ".pipe_sc";
char* pp_cs = ".pipe_cs";

char* db = ".userdb";


typedef struct user {

    char* username;
    char* password;
    double balance;
    int n_pids;
    int pids[1024];

} user;

void init_db();

void write_to_client(char* message);

int write_user(user* usr, int action);

user* get_user(char* username);

int check_password(char* username, char* password);

void monitor_pipe();

void register_new_user(char* username, char* password);

void run_command(char* username, char* command);

void deposit(char* username, double value);

/* int main(void); */
