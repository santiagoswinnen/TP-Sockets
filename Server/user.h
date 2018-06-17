typedef struct userCDT {
    int id;
    char * name;
} user;

typedef user * User;

User newUser(int id, char * name); /* Crea un nuevo usuario */
void deleteUser(User user);
