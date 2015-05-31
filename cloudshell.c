#include "cloudshell.h"

void init_db(){
    int ret = mkdir(db, 0744);
    if (!(ret == 0 || errno == EEXIST)) print_error_and_exit();
}

void write_to_client(char* message){

    int open_pp;
    int ret;
    open_pp = open(pp_sc, O_WRONLY);

    if (open_pp <= 0){
        printf("Couldn't open server-to-client pipe!\n");
    }

    ret = write(open_pp, message, strlen(message));
    close(open_pp);

    if (ret == -1) 
        printf("Couldn't write to client!\n");
}

pid_t popen_to_pipe(char* command){

    int in_pp;
    int out_pp;
    char* cmd[4] = { "/bin/sh", "-c", command, 0};
    pid_t pid = fork();

    if (pid == -1){
        printf("Couldn't fork to run command!\n");
    } else if (pid == 0){

        out_pp = open(pp_sc, O_WRONLY);
        in_pp = open(pp_cs, O_RDONLY);

        if (out_pp <= 0){
            printf("Couldn't open server-to-client pipe!\n");
        }
        if (in_pp  <= 0){
            printf("Couldn't open client-to-server pipe!\n");
        }

        dup2(out_pp, 1);
        dup2(in_pp, 0);
        close(out_pp);
        close(in_pp);
        execv(cmd[0], cmd);

        write_to_client("Error running command!");
        printf("execv failed!");
          
    } else {
        //parent
    }

    return pid;
}

int write_user(user* usr, int action){

    FILE* f;
    char path[1024];
    sprintf(path, "%s/%s", db, usr -> username);

    if (get_user(usr -> username) != NULL){
        if (action == CREATE){
            write_to_client("User already exists!\n");
            return 0;
        }
    }

    f = fopen(path, "w");

    if (f == NULL){
        write_to_client("Couldn't open user!\n");
        return 0;
    }

    fprintf(f, "%s\n%s\n%f\n", usr -> username, 
                                usr -> password,
                                usr -> balance);
    for(int i = 0; i < (usr -> n_pids); ++i){
        fprintf(f, "%d ", usr -> pids[i]);    
    }
    fprintf(f, "\n");    
    fclose(f);
    free(usr);

    return 1;
}

user* get_user(char* username){

    user* usr = malloc(sizeof(user));
    char path[1024];

    FILE* f; 
    char line[1024];

    sprintf(path, "%s/%s", db, username);
    
    f = fopen(path, "r");
    if (f == NULL){
        /* Couldn't open file. */
        /*check errno */
        return NULL; 
    }

    /* Following for loops for the 3 lines the files must have. */
    for(int i = 0; i < 3; ++i){
        if (fgets(line, sizeof(line), f) == NULL){
            write_to_client("User record is malformed! Please contact admin!\n");
            return NULL;
        }
        line[strlen(line) -1] = '\0';

        switch (i) {
            case 0: usr -> username = strdup(line); break;
            case 1: usr -> password = strdup(line); break;
            case 2: usr -> balance  =   atof(line); break;
        }
    }

    /* Checking for pids on the 4th line and parsing them. */
    if (fgets(line, sizeof(line), f) != NULL){

        char* pid_tok = strtok(line, " ");
        do {
            int pid = atoi(pid_tok);
            if (pid != 0) {
                usr -> pids[usr -> n_pids] = pid;
                usr -> n_pids += 1;
            }
            pid_tok = strtok(NULL, " ");
        } while (pid_tok != NULL);
    }
    
    fclose(f);
    return usr;
}

int check_password(char* username, char* password){

    int valid;
    user* usr = get_user(username);
    if (usr == NULL){
        write_to_client("User does not exist or is corrupted!");
        return 0; 
    }
    valid = strcmp(usr -> password, password) == 0;

    if (!valid){
        write_to_client("Incorrect password!\n");
    }

    return valid;
}

void register_new_user(char* username, char* password){

    user* usr = malloc(sizeof(user));
    usr -> username = username;
    usr -> password = password;
    usr -> balance  = 0;
    usr -> n_pids   = 0;

    if (write_user(usr, CREATE)){
        write_to_client("User created successfully!\n");
    } else {
        write_to_client("Error creating user!\n");
    }
}

void get_balance(char* username){

    char return_message[100];
    user* usr = get_user(username);
    if (usr == NULL) return ;
    sprintf(return_message, "Current balance: %f", usr -> balance);

    write_to_client(return_message);
}

void deposit(char* username, double balance){

    char return_message[100];
    double bal;
    user* usr = get_user(username);

    if (usr == NULL) return ;

    usr -> balance += balance;
    bal = usr -> balance;

    if (write_user(usr, CHANGE)){
        sprintf(return_message, "Deposit successful! Current balance: %f", bal);
    } else {
        sprintf(return_message, "Couldn't make deposit!");
    }

    write_to_client(return_message);
}

int add_pid_to_user(char* username, int pid){

    user* usr = get_user(username);
    if (usr == NULL) return 0;

    usr -> pids[usr -> n_pids] = pid;
    usr -> n_pids += 1;

    if (!write_user(usr, CHANGE)) return 0;

    return 1;

}

void run_command(char* username, char* command){

    int pid = popen_to_pipe(command);
    add_pid_to_user(username, pid);
}

void monitor_pipe(){

    char* username;
    char* password;
    char* action;
    char* command;
    double deposit_value;

    char buffer[1024];
    int open_pp;

    char* pch;

    while (1){

        open_pp = open(pp_cs, O_RDONLY);
        read(open_pp, buffer, sizeof(buffer));

        pch = strtok(buffer, "\n");
        if (pch == NULL){
            //error 
            continue;
            close(open_pp);
        }
        username = strdup(pch);

        pch = strtok(NULL, "\n");
        if (pch == NULL){
            //error 
            continue;
            close(open_pp);
        }
        password = strdup(pch);

        pch = strtok(NULL, "\n");
        if (pch == NULL){
            //error 
            continue;
            close(open_pp);
        }
        action = strdup(pch);

        if (strcmp(action, "deposit") == 0) {
            pch = strtok(NULL, "\n");
            if (pch == NULL){
                //error 
                continue;
                close(open_pp);
            }
            deposit_value = atof(pch);
        } else if (strcmp(action, "command") == 0){
            pch = strtok(NULL, "\n");
            if (pch == NULL){
                //error 
                continue;
                close(open_pp);
            }
            command = strdup(pch);
        }

        close(open_pp);

        if (strcmp(action, "register") == 0){
            register_new_user(username, password);
        }
        if (strcmp(action, "login") == 0){
            if (check_password(username, password) != 0){
                write_to_client("Login successeful!");
            }
        }
        if (strcmp(action, "deposit") == 0){
            if (check_password(username, password) != 0){
                deposit(username, deposit_value);
            }
        }
        if (strcmp(action, "balance") == 0){
            if (check_password(username, password) != 0){
                get_balance(username);
            }
        }
        if (strcmp(action, "command") == 0){
            if (check_password(username, password) != 0){
                run_command(username, command);
            }
        }
    }
}


int main(void){

    init_db();
    monitor_pipe();
    
}
