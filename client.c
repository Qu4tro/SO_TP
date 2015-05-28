#include "client.h"

void create_pipe(){

    int err = mkfifo(pp, 0666); 

    if (err == 0 || errno == EEXIST){
        return; 
    } else {
        print_error_and_exit();
    }
}

void print_help(){
    printf("Usage:\n");
    printf("\tcloudsh register <username>\n");
    printf("\tcloudsh deposit <saldo> <username>\n");
    printf("\tcloudsh shell <username>\n");

    printf("\n\tsaldo is the value in €(integer only)\n");
    exit(EXIT_FAILURE);
}


void registration(char* args[]){

    char message[MAX_SIZE];
    int open_pp;
    int ret;

    char* username = strdup(args[0]);
    char password[20];
    strncpy(password, getpass("Password: "), 20);

    open_pp = open(pp, O_WRONLY);
    sprintf(message, "%s\n%s\n%s\n", username, password, "register");
    ret = write(open_pp, message, strlen(message));
    close(open_pp);

    if (ret == -1) print_error_and_exit();

    //TODO sinais
    printf("Bem-vindo à CloudShell %s. Agora podes entrar quando quiseres!", username);
}

void login(char* args[]){

    /* char message[MAX_SIZE]; */
    /* int open_pp; */
    /* int ret; */

    char* username = strdup(args[0]);
    char password[20];
    strncpy(password, getpass("Password: "), 20);

    /* open_pp = open(pp, O_WRONLY); */
    /* sprintf(message, "%s\n%s\n%s\n", username, password, "shell"); */
    /* ret = write(open_pp, message, strlen(message)); */
    /* close(open_pp); */

    /* if (ret == -1) print_error_and_exit(); */

    shell(username, password);
}

void shell(char* username, char* password){

    char command[MAX_SIZE]; 
    char message[MAX_SIZE]; 
    int ret;

    int open_pp;

    while (1){

        open_pp = open(pp, O_WRONLY);

        printf("> ");
        if (fgets(command, MAX_SIZE, stdin) == NULL)
            print_error_and_exit();
        command[strlen(command) - 1] = '\0';

        if (strcmp(command, "exit") == 0){
            break;
        }

        sprintf(message, "%s\n%s\n%s\n%s\n", username, password, "command", command);
        ret = write(open_pp, message, strlen(message));
        if (ret == -1) print_error_and_exit();

        close(open_pp);
    }

    puts("Exiting...");
    close(open_pp);
}

void deposit(char* args[]){

    char message[MAX_SIZE];
    int open_pp;
    int ret;

    char* value_str = strdup(args[0]);
    double value = atof(value_str);
    char* username = strdup(args[1]);
    char password[20];
    strncpy(password, getpass("Password: "), 20);

    open_pp = open(pp, O_WRONLY);
    sprintf(message, "%s\n%s\n%s\n%f\n", username, password, "deposit", value);
    ret = write(open_pp, message, strlen(message));
    close(open_pp);

    if (ret == -1) print_error_and_exit();

    printf("Foram depositado %f€ para a conta %s.", value, username);
}


int main(int argc, char* argv[]){

    create_pipe();

    if (argc <= 1){
        print_help();
    } else if (strcmp(argv[1], "register") == 0){
        if (argc != 3) print_help();
        registration(argv + 2);
    } else if (strcmp(argv[1], "deposit") == 0){
        if (argc != 4) print_help();
        deposit(argv + 2);
    } else if (strcmp(argv[1], "shell") == 0){
        if (argc != 3) print_help();
        login(argv + 2);
    } else {
        print_help();
    }

    return 0;
}
