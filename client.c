#include "client.h"

void create_pipes(){

    int err = mkfifo(pp_cs, 0666); 

    if (err != 0 && errno != EEXIST){
        print_error_and_exit();
    }

    err = mkfifo(pp_sc, 0666); 

    if (err != 0 && errno != EEXIST){
        print_error_and_exit();
    }
}

void write_to_server(char* message){

    int open_pp;
    int ret;
    open_pp = open(pp_cs, O_WRONLY);

    if (open_pp <= 0){
        printf("Couldn't open server-to-client pipe!\n");
    }

    ret = write(open_pp, message, strlen(message));
    close(open_pp);

    if (ret == -1) print_error_and_exit();
}

char* wait_for_server(){

    int size = MAX_SIZE;
    char* buffer = malloc(sizeof(char) * size);
    int open_pp;
    open_pp = open(pp_sc, O_RDONLY);
    read(open_pp, buffer, size);
    close(open_pp);

    return buffer;
}

void print_help(){
    printf("Usage:\n");
    printf("\tcloudsh register <username>\n");
    printf("\tcloudsh deposit <saldo> <username>\n");
    printf("\tcloudsh balance <username>\n");
    printf("\tcloudsh shell <username>\n");

    printf("\n\tsaldo is the value in €(integer only)\n");
    exit(EXIT_FAILURE);
}


void registration(char* args[]){

    char message[MAX_SIZE];

    char* username = strdup(args[0]);
    char password[20];

    char* server_answer;

    strncpy(password, getpass("Password: "), 20);

    sprintf(message, "%s\n%s\n%s\n", username, password, "register");
    write_to_server(message);

    server_answer = wait_for_server();
    printf("%s\n", server_answer);
}

void login(char* args[]){

    char* server_answer;
    char message[MAX_SIZE];

    char* username = strdup(args[0]);
    char password[20];
    strncpy(password, getpass("Password: "), 20);

    sprintf(message, "%s\n%s\n%s\n", username, password, "login");

    write_to_server(message);

    server_answer = wait_for_server();
    printf("%s\n", server_answer);

    if (server_answer[0] == 'L'){
        shell(username, password);
    }
}

void shell(char* username, char* password){

    char command[MAX_SIZE]; 
    char message[MAX_SIZE]; 
    char* server_answer;

    while (1){

        printf("> ");
        if (fgets(command, MAX_SIZE, stdin) == NULL){
            exit(0);
        }
        command[strlen(command) - 1] = '\0';

        if (strcmp(command, "exit") == 0){
            break;
        }

        sprintf(message, "%s\n%s\n%s\n%s\n", username, password, "command", command);
        write_to_server(message);
        server_answer = wait_for_server();
        printf("%s\n", server_answer);
    }

    puts("Exiting...");
}

void deposit(char* args[]){

    char message[MAX_SIZE];

    char* value_str = strdup(args[0]);
    double value = atof(value_str);
    char* username = strdup(args[1]);
    char password[20];

    char* server_answer;

    strncpy(password, getpass("Password: "), 20);

    sprintf(message, "%s\n%s\n%s\n%f\n", username, password, "deposit", value);
    write_to_server(message);

    server_answer = wait_for_server();
    printf("%s\n", server_answer);
}

void balance(char* args[]){

    char message[MAX_SIZE];

    char* username = strdup(args[0]);
    char password[20];

    char* server_answer;

    strncpy(password, getpass("Password: "), 20);

    sprintf(message, "%s\n%s\n%s\n", username, password, "balance");
    write_to_server(message);

    server_answer = wait_for_server();
    printf("%s\n", server_answer);
}




int main(int argc, char* argv[]){

    create_pipes();

    if (argc <= 1){
        print_help();
    } else if (strcmp(argv[1], "register") == 0){
        if (argc != 3) print_help();
        registration(argv + 2);
    } else if (strcmp(argv[1], "balance") == 0){
        if (argc != 3) print_help();
        balance(argv + 2);
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
