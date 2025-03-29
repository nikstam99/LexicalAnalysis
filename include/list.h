#include <stdio.h>

typedef struct list* List;
typedef struct transaction* Transaction;
typedef struct list_node* Listnode;

#define LIST_BOF (Listnode)0
#define LIST_EOF (Listnode)0

// Δομή της λίστας
struct list {
    Listnode last; 		// Τελευταίος κόμβος της λίστας
    int size; 			// Μέγεθος λίστας
    Listnode dummy; 	// Πρώτος (βοηθητικός) κόμβος της λίστας χωρίς στοιχεία 
    int total_words;
};

// Κόμβος της λίστας
struct list_node {
    Listnode next; 		// Δείκτης στον επόμενο κόμβο της λίστας
    char value[20];        // Το περιεχόμενο του κόμβου
    int number;         // Αριθμός των φορών που υπάρχει το στοιχείο
};

//Δημιουργία της λίστας
List list_create();

// Επιστρέφει το μέγεθος της λίστας
int list_size(List list);

// Εισάγει ένα node στην πρώτη θέση της λίστας
void list_insert_first(List list, char value[20]);

// Εισάγει ένα στοιχείο στη λίστα στη θέση μετά το node
void list_insert_next(List list, Listnode node, char value[20], int num);

// Επιστρέφει τον επόμενο κόμβο του node
Listnode node_next(Listnode node);

// Διαγράφει το πρώτο node της λίστας
void list_remove_first(List list);

// Επιστρέφει το πρώτο node της λίστας
Listnode list_first(List list);

// Επιστρέφει το τελευταίο node της λίστας
Listnode list_last(List list);

// Επιστρέφει το στοιχείο value αν υπάρχει στη λίστα
char* list_find(List list, char value[20]);

// Βρίσκει και επιστρέφει ένα node με βάση το value του
Listnode list_find_node(List list, char value[20]);

// Διαγραφή της λίστας 
void list_destroy(List list);

// Επιστρέφει τον προηγούμενο κόμβο του node
Listnode list_find_previous_node(List list, Listnode node);

// Εκτυπώνει το περιεχόμενο της λίστας
void print_list(List list, int k);

// Επιστρέφει τον αριθμό των επαναλήψεων της λέξης στο κείμενο
int node_num(Listnode node);