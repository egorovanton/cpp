#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"
#include "ctype.h"
#include "signal.h"
#include "unistd.h"

#define max(x, y) (((x)>(y))?(x):(y))

typedef struct Contact {
    bool exist;
    int id;
    char *number;
    char *name;
} Contact;

// helper function for bsearch
int compareContactWithKey(const void *key, const void *pelem) {
    return *(int *)key - (*(Contact *)pelem).id;
}

int compareContacts(const void *a, const void *b) {
    return (*(Contact *)a).id - (*(Contact *)b).id;
}


//works as kinda vector
typedef struct Book {       
    int length;   //length of the array "contacts"
    int size;     //number of actual elements
    int quantity; //number of existing elements
    int lastId;   
    Contact *contacts;
} Book;

const int startSize = 2;

Book *createBookSize(const int n) {
    Book *book = (Book *) malloc(sizeof(Book));
    book->length = n;
    book->size = 0;
    book->quantity = 0;
    book->lastId = -1;
    book->contacts = (Contact *) malloc(sizeof(Contact)*n);
    for (int i = 0; i < n; ++i) {
        book->contacts[i].exist = false;
        book->contacts[i].number = NULL;
        book->contacts[i].name = NULL;
    }
    return book;
}


Book *createBook() {
    return createBookSize(startSize);
}

void deleteBook(Book *book) {
    for(int i = 0; i < book->size; ++i) {
        free(book->contacts[i].name);
        free(book->contacts[i].number);
    }
    free(book->contacts);
}

void enlargeBook(Book *book) {
    book->length *= 2;
    book->contacts = (Contact *) realloc(book->contacts, sizeof(Contact)*book->length); //TODO
    for (int i = book->size; i < book->length; ++i) {
        book->contacts[i].exist = false;
        book->contacts[i].number = NULL;
        book->contacts[i].name = NULL;
    }
}

void reorganizeBook(Book *book) {
    if (book->quantity == 0)
        return;
    Contact *newContacts = (Contact *) malloc(sizeof(Contact)*book->quantity*2);
    Contact *oldContacts = book->contacts;
    for (int i = 0, j = 0; i < book->size; ++i) {
        if (!oldContacts[i].exist) {
            free(oldContacts[i].name);
            free(oldContacts[i].number);
            continue;
        }
        newContacts[j].name   = oldContacts[i].name;
        newContacts[j].number = oldContacts[i].number;
        newContacts[j].id     = oldContacts[i].id;
        newContacts[j].exist  = true;
        ++j;
    }
    free(oldContacts);
    book->contacts = newContacts;
    book->size = book->quantity;
    book->length = book->quantity*2;
}

void writeBookIndex(Book *book, const char *name, const char *number, const int i) {
    if (book->quantity <= book->length / 4) {
        reorganizeBook(book);
    }
    if (book->size == book->length)
        enlargeBook(book);
    free(book->contacts[i].name);
    free(book->contacts[i].number);
    book->contacts[i].name = (char *) malloc(sizeof(char)*(strlen(name) + 1));
    book->contacts[i].number = (char *) malloc(sizeof(char)*(strlen(number) + 1));
    strcpy(book->contacts[i].name, name);
    strcpy(book->contacts[i].number, number);
    book->contacts[i].exist = true;
    book->contacts[i].id = book->lastId + 1;
    book->lastId++;
    book->quantity++;
}

void appendBook(Book *book, const char *name, const char *number) {
    writeBookIndex(book, name, number, book->size);
    book->size++;
}

void appendBookID(Book *book, const char *name, const char *number, const int id) {
    writeBookIndex(book, name, number, book->size);
    book->contacts[book->size].id = id;
    book->lastId = max(id, book->lastId);
    book->size++; 
}

Contact *findContactById(Book *book, int id) {
    return (Contact *) bsearch(&id, book->contacts, book->size, sizeof(Contact), compareContactWithKey);
}

void removeContact(Book *book, const int id) {
    Contact *contact = findContactById(book, id);
    if (contact == NULL || contact->exist == false) {
        printf("ERROR: contact with id = %d does not exist\n", id);
        return;
    }
    contact->exist = false;
    book->quantity--;
    if (book->quantity <= book->size/4) {
        reorganizeBook(book);
    }
}

void changeName(Book *book, const int id, const char *name) {
    Contact *contact = findContactById(book, id);
    if (contact == NULL || contact->exist == false) {
        printf("ERROR: contact with id = %d does not exist\n", id);
        return;
    }
    free(contact->name);
    contact->name = (char *) malloc(strlen(name)+1);
    strcpy(contact->name, name);
}

void changeNumber(Book *book, const int id, const char *number) {
    Contact *contact = findContactById(book, id);
    if (contact == NULL || contact->exist == false) {
        printf("ERROR: contact with id = %d does not exist\n", id);
        return;
    }
    free(contact->number);
    contact->number = (char *) malloc(strlen(number)+1);
    strcpy(contact->number, number);
}

void printContact(FILE *output, const Book *book, const int index) {
    fprintf(output, "%d %s %s\n", book->contacts[index].id, book->contacts[index].name, book->contacts[index].number);
}

void printFound(FILE *output, const Book *book, const char *str) {
    for (int i = 0; i < book->size; ++i) {
        if (book->contacts[i].exist) {
            if (strstr(book->contacts[i].name, str) || strstr(book->contacts[i].number, str)) 
                printContact(output, book, i);
        }
    }
}

void printBook(FILE *output, const Book *book) {
    for (int i = 0; i < book->size; ++i) {
        if (book->contacts[i].exist) {
            printContact(output, book, i);
        }
    }
}

//debug print
void fullPrint(const Book *book) {
    printf("length = %d\nsize = %d\nquantity = %d\nlastId = %d\n", book->length, book->size, book->quantity, book->lastId);
    Contact *contacts = book->contacts;
    for (int i = 0; i < book->size; ++i) {
        printf("%d. %d %s %s %s\n", i, contacts[i].id, contacts[i].name, contacts[i].number, contacts[i].exist ? "true" : "false");
    }
}

char *getStringFromFile(FILE* input) {
    int length = 16;
    int size = 0;
    char *str = (char *) malloc(sizeof(char)*length);
    char c;
    do {
        c = fgetc(input);
    } while (isspace(c) && !feof(input)); 

    while(!(isspace(c)) && !feof(input)) {
        if (length == size) {
            length *= 2;
            str = (char *) realloc(str, length);
        }
        str[size] = c;
        size++;
        c = fgetc(input);
    }

    if (length == size) {
        length++;
        str = (char *) realloc(str, length);
    }

    str[size] = '\0';
    return str;
}

//deletes spaces from string
void concatenate(char *string) {
    int length = strlen(string)+1;
    int size = 0;
    for (int i = 0, j = 0; i < length; ++i) {
        if ((string[i] != ' ') && (i != j)) {
            string[j] = string[i];
            string[i] = ' ';
        }
        if (string[j] != ' ') {
            ++j;
            ++size;            
        }
    }
    string[size] = '\0';
}

bool correctName(char *name) {
    int length = strlen(name);
    for(int i = 0; i < length; ++i) {
        if(!((name[i] >= 'a' && name[i] <= 'z') || (name[i] >='A' && name[i] <= 'Z'))) {
            return false;
        }
        name[i] = (char) tolower(name[i]);
    }
    return true;
}


bool afterDefis(char c) {
    return (c == '(') || isdigit(c);
}

// deletes all non-digit characters from the number
// returns true if the given string is a correct number
// otherwise, returns false
bool correctNumber(char *number) {
    int length = strlen(number);
    bool wasOpen  = false; //Have we ever seen opening bracket?
    bool wasClose = false; //Have we ever seen closing bracket?
    
    int i = 0;
    if (number[0] == '+') {
        number[0] = ' ';
        ++i;
    }

    for (; i < length; ++i) {
        if (number[i] == '-') {
            if (wasOpen && !wasClose)
                return false;
            if (afterDefis(number[i+1]))
                number[i] = ' ';
            else 
                return false;
        }
        else if (number[i] == '(')
            if (wasOpen)
                return false;
            else {
                wasOpen = true;
                number[i] = ' ';
            } 
        else if (number[i] == ')') {
            if(wasClose || !wasOpen)
                return false;
            wasClose = true;
            number[i] = ' ';
        } else if (!isdigit(number[i]))
            return false;
    }
    concatenate(number);
    return true;
}

void readBookFromFile(FILE *input, Book *book) {
    int id;
    int maxID = 0;
    bool unsorted = false;
    char *name, *number;
    while (!feof(input)) {
        fscanf(input, "%d", &id);
        name = getStringFromFile(input);
        number = getStringFromFile(input);
        if (name[0] == 0 || number[0] == 0) {
            free(name);
            free(number);
            continue;
        }
        if (!correctName(name) || !correctNumber(number)) {
            printf("ERROR: cannot getStringFromFile %d %s %s\n", id, name, number);
            free(name);
            free(number);
            return;
        }
        if (id < maxID)
            unsorted = true;
        appendBookID(book, name, number, id);
        maxID = max(id, maxID);
        free(name);
        free(number);
    }
    if (unsorted)
        qsort(book->contacts, book->size, sizeof(Contact), compareContacts);
    book->lastId = maxID;
}

char *sh_fileName;
Book *sh_book;
void signal_handler(int sig) {
    FILE* output = fopen(sh_fileName, "w");
    printBook(output, sh_book);
    fclose(output);
    deleteBook(sh_book);
    exit(EXIT_SUCCESS);
    return;
}

// void (*tricky_signal_handler(const char *fileName, Book *book)) (int) {
//     FILE* output = fopen(fileName, "w");
//     printBook(output, book);
//     fclose(output);
//     deleteBook(book);

//     return signal_handler;
// }

int main(int argc, char const *argv[]) {
    if (argc == 1) {
        printf("ERROR: the program takes one argument\n");
        return 0;
    }
    argv[1];

    Book *book = createBook();
    if( access( argv[1], R_OK ) != -1 ) {
        FILE *input = fopen(argv[1], "r");
        readBookFromFile(input, book);
        fclose(input);
    }

    sh_fileName = (char *) malloc(strlen(argv[1] + 1));
    strcpy(sh_fileName, argv[1]);
    sh_book = book;
    signal(SIGINT, signal_handler); 

    char command[10];
    char *name = NULL;   //(char *) malloc(sizeof(char)*80);
    char *number = NULL; // (char *) malloc(sizeof(char)*80);
    char *str = NULL;    //(char *) malloc(80);

    bool corName;
    bool corNumber;
    
    while (true) {
        corName = true;
        corNumber = true;
        scanf("%s", command);
        if(!strcmp(command, "debprint")) {
            fullPrint(book);
        } else if(!strcmp(command, "print")) {
            printBook(stdout, book);
        } else if(!strcmp(command, "find")) {
            str = getStringFromFile(stdin);
            if (correctName(str) || correctNumber(str))
                printFound(stdout, book, str);
            free(str);
        } else if(!strcmp(command, "create")) {
            name = getStringFromFile(stdin); number = getStringFromFile(stdin);
            corName = correctName(name);
            corNumber = correctNumber(number);
            if (correctName && correctNumber) {
                appendBook(book, name, number);
            }
            free(name); free(number);
        } else if(!strcmp(command, "delete")) {
            int id;
            scanf("%d", &id);
            removeContact(book, id);
        } else if(!strcmp(command, "change")) {
            int id;
            scanf("%d%s", &id, command);
            str = getStringFromFile(stdin);
            if(!strcmp(command, "number")) {
                corName = correctName(str);
                if (corName)
                    changeNumber(book, id, str);
            } else if (!strcmp(command, "name")) {
                corNumber = correctNumber(str);
                if (corNumber)
                    changeName(book, id, str);
            } else {
                printf("ERROR: Expected \"name\" or \"number\" after id in the command change\n");
            }
            free(str);
        } else if(!strcmp(command, "exit")) {
            FILE* output = fopen(argv[1], "w");
            printBook(output, book);
            fclose(output);
            deleteBook(book);
            break;
        } else {
            printf("ERROR: Unknown command %s\n", command);
        }
        if (!corName) {
            printf("ERROR: Invalid name\n");
        }
        if (!corNumber) {
            printf("ERROR: Invalid number\n");
        }
        fflush(stdout);
    }

    return 0;
}