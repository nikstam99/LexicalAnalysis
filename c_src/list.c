#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../include/list.h"

List list_create() {
    List list = malloc(sizeof(*list)); // Δέσμευση μνήμης για τη λίστα
    list->size = 0;	
    list->dummy = malloc(sizeof(*list->dummy)); // δέσμευση μνήμης για τον dummy κόμβο
	list->dummy->next = NULL;	
    list->last = list->dummy;
	list->total_words = 0;
    return list;
}

int list_size(List list) {
	return list->size;
}

Listnode node_next(Listnode node) {
	return node->next;
}

int node_num(Listnode node) {
	return node->number;
}

void list_insert_first(List list, char value[20]) {
	Listnode node = list_find_node(list, value);
	if (node != 0) {
		node->number++;
		return;
	}
	Listnode new = malloc(sizeof(*new));
	new->next = list_first(list);
	list->dummy->next = new;
	list->size++;
	strcpy(new->value, value);
	new->number = 1;
	if (list->last == list->dummy)
		list->last = new;
}

void list_insert_next(List list, Listnode node, char value[20], int num) {
	// Αν δεν δωθεί κόμβος εισάγουμε μετά τον dummy
	if (node == NULL)
		node = list->dummy;
	// Δημιουργία του νέου κόμβου
	Listnode new = malloc(sizeof(*new));
	new->number = num;
	strcpy(new->value, value);
	list->total_words += num;
	// Σύνδεση του new ανάμεσα στο node και το node->next
	new->next = node->next;
	node->next = new;
	// Ενημέρωση των size & last
	list->size++;
	if (list->last == node)
		list->last = new;
}

void list_remove_first(List list) {
	Listnode first = list_first(list);
	if (first == list->dummy) return;
	Listnode next = first->next;
	list->dummy->next = next;
	if (list->last == first) 
		list->last == next;
	free(first);
	list->size--;
}
void list_destroy(List list) {
	Listnode node = list->dummy;
	while (node != NULL) {
		Listnode next = node->next;		
		free(node);	// Απελευθερώνουμε τον ίδιο τον κόμβο
		node = next;
	}

	free(list);	// Απελευθερώνουμε τη λίστα
}

char* list_find(List list, char value[20]) {
	Listnode node = list_find_node(list, value);
	if (node == 0) return 0;
	else return node->value;
}

Listnode list_find_node(List list, char value[20]) {
	for (Listnode node = list->dummy->next; node != 0; node = node->next)	// Αν βρει το value επιστρέφει το node αλλιώς 0 
		if (!strcmp(value, node->value))
			return node;		

	return 0;	
}

Listnode list_find_previous_node(List list, Listnode node) {
	Listnode previous = list->dummy;
	for (Listnode n = list->dummy->next; n != NULL; n = n->next) {	// Αποθηκεύουμε έναν κόμβο στο previous και ξεκινάμε την αναζήτηση από τον επόμενο
		if (!strcmp(n->value, node->value)) return previous; // Όταν βρούμε το node έχουμε στο previous τον προηγούμενό του και τον επιστρεφουμε
		previous = previous->next;
	}
	return 0;
}

Listnode list_first(List list) {
	// Ο πρώτος κόμβος είναι ο επόμενος του dummy
	return list->dummy->next;
}

Listnode list_last(List list) {
	// Αν η λίστα είναι κενή ο last είναι ο dummy και επιστρέφουμε 0 αλλίως επιστρέφουμε το last της λίστας
	if (list->last == list->dummy)
		return LIST_EOF;		
	else
		return list->last;
}

void print_list(List list, int k) {
	int i = 0;
	for (Listnode node = list->dummy->next; node != NULL; node = node->next) {
		float f = (double)node->number / (double)list->total_words;
		printf("%d. %s : %d (f = %f)\n", i+1, node->value, node->number, f);
		i++;
		if (i == k) return;
	}
}