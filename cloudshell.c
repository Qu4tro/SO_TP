#include "cloudshell.h"

void init_cloud(){
    int ret = mkdir(db, 0744);
    if (!(ret == 0 || errno == EEXIST)) print_error_and_exit();
}

void write_user(user* usr){

    FILE* f;
    char path[1024];
    sprintf(path, "%s/%s", db, usr -> username);

    if (get_user(usr -> username) != NULL){
        //user already exists 
    }

    f = fopen(path, "w");
    fprintf(f, "%s\n%s\n%f\n", usr -> username, 
                                usr -> password,
                                usr -> balance);

}

user* get_user(char* username){

    user* usr = malloc(sizeof(user));
    char path[1024];

    FILE* f; 
    int i;
    char line[1024];

    sprintf(path, "%s/%s", db, username);
    
    f = fopen(path, "r");
    if (f == NULL){
        return NULL; //No such file
    }

    for(i = 0; i < 3; ++i){
        if (fgets(line, sizeof(line), f) == NULL) //File malformed
            return NULL;
        line[strlen(line) -1] = '\0';

        switch (i) {
            case 0: usr -> username = strdup(line); break;
            case 1: usr -> password = strdup(line); break;
            case 2: usr -> balance  =   atof(line); break;
        }
    }
    return usr;
}

int check_password(char* username, char* password){

    user* usr = get_user(username);
    if (usr == NULL){
        return 0; //Username does not exist
    }
    return strcmp(usr -> password, password) == 0;

}

void register_new_user(char* username, char* password){

    user* usr = malloc(sizeof(user));
    usr -> username = username;
    usr -> password = password;
    usr -> balance  = 0;

    write_user(usr);
}

void deposit(char* username, double balance){

    user* usr = get_user(username);
    if (usr == NULL){
        return ; //Username does not exist
    }
    usr -> balance += balance;
    write_user(usr);
}

int main(void){

    deposit("12", -5);

    
}
