#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "../include/list.h"
#include "../include/general.h"
#include <errno.h>
#include <sys/stat.h>
#include <signal.h>

int main(int argc, char* argv[]) {
    char* inputFile = argv[1];						// Το αρχείο που διαβάζουμε
    int NumOfBuilders = atoi(argv[2]);				// Ο αριθμός των builders
	int RootId = atoi(argv[5]);						// Το id του root process
	int fdx[NumOfBuilders];
	char NamedPipe[20];
	char num[10];
	// Κρατάμε σε έναν πίνακα όλα τα fd των named pipes που έχουμε δημιουργήσει στο root
	for (int i = 0; i < NumOfBuilders; i++) {
		strcpy(NamedPipe, "fifo");
		sprintf(num, "%d", i);
		strcat(NamedPipe, num);
		fdx[i] = open(NamedPipe, O_WRONLY);
	}
	int p = atoi(argv[3]);						// O fd του pipe που στέλνει πληροφορίες από τo root στους splitters
	char* ExclusionList = argv[4];				// Το αρχείο με τις λέξεις που δεν πρέπει να στείλουμε στους builders
    char buffer[1024];
    int nbytes;
	char inbuf[100];
    ssize_t bytesRead;
    int j = 0, fd3;
	int Start = 0, End = 0;
	int k = 0;
	char excl[20] = " ";
    // Δημιουργία του Exclusion List
	fd3 = open(ExclusionList, O_RDONLY);
	if (fd3 == -1) {
		printf("Error opening Exclusion List file!");
		return(1);
	}
	List ExcList = list_create();
	while ((bytesRead = read(fd3, buffer, 1024)) > 0) {
        for (ssize_t i = 0; i < bytesRead; i++) {
			if (buffer[i] != ' ' && buffer[i] != '\n') {
				if (k == 0) {
					strncpy(excl, &buffer[i], 1);	// Αντιγράφουμε το buffer[i] στην αρχή της λέξης
					k = 1;
				}
				else {
					strncat(excl, &buffer[i], 1);	// Προσθέτουμε έναν έναν τους χαρακτήρες μέχρι να ολοκληρωθεί η λέξη
				}
			}
			else {
				if (strcmp(excl, "")) {
					list_insert_first(ExcList, excl);	// Τοποθετούμε τη λέξη στη λίστα
					memset(excl,0,sizeof(excl));		// Σβήνουμε τους χαρακτήρες αφού ολοκληρώσουμε τη δουλειά με μια λέξη
					k = 0;
				}
			}
        }
    }
	if (strcmp(excl, "")) {
		// Τοποθετούμε την τελευταία λέξη στη λίστα
		list_insert_first(ExcList, excl);
		memset(excl,0,sizeof(excl));
	}
		// Οι Splitters διαβάζουν την πληροφορία που έχουμε στείλει μέσω των pipes
		nbytes = read(p, inbuf, 100);
		Start = atoi(inbuf);
		nbytes = read(p, inbuf, 100);
		End = atoi(inbuf);
		close(p);
		int fd;
		// Άνοιγμα του inputfile και έλεγχος λαθών
		fd = open(inputFile, O_RDONLY);
		if (fd == -1) {
			printf("Error opening source file in process %d\n", getpid());
			return(1);
		}
		int currentLine = 1;
    	int startReading = 0;
		char temp[20] = " ";
		k = 0;
		int HashNumber;
		// Διάβασμα του inputfile 
		while ((bytesRead = read(fd, buffer, 1024)) > 0) {
        	for (ssize_t i = 0; i < bytesRead; i++) {
            	if (currentLine >= Start) startReading = 1;		// Όταν φτάσουμε στη γραμμή Start ξεκινάμε να παίρνουμε τις λέξεις
				if (currentLine > End) {
                	close(fd);				// Κλείσιμο του αρχείου όταν φτάσουμε στην γραμμή End
                	break;
            	}
            	if (startReading) {
					// Ενώνουμε τους χαρακτήρες και δημιουργούμε λέξεις
					if (buffer[i] != ' ' && buffer[i] != '\n') {
						// Κρατάμε μόνο τους ascii χαρακτήρες που αποτελούν γράμματα
						if ((buffer[i] >= 65 && buffer[i] <= 90) || (buffer[i] >= 97 && buffer[i] <= 122)) {
							if (k == 0) {
								strncpy(temp, &buffer[i], 1);	// Αντιγράφουμε το buffer[i] στην αρχή της λέξης
								k = 1;
							}
							else {
								strncat(temp, &buffer[i], 1);	// Προσθέτουμε έναν έναν τους χαρακτήρες μέχρι να ολοκληρωθεί η λέξη
							}
						}
					}
					else {
						if (strcmp(temp, "")) {
							if (!list_find(ExcList, temp)) {
								// Στέλνουμε τη λέξη στον builder με νούμερο το HashNumber που προκύπτει αν την περάσουμε από μια hash function
								HashNumber = hash(temp) % (NumOfBuilders);
								write(fdx[HashNumber], temp, sizeof(temp));
							}

							memset(temp,0,sizeof(temp));		// Σβήνουμε τους χαρακτήρες αφού ολοκληρώσουμε τη δουλειά με μια λέξη
							k = 0;
						}
					}
				}
            	if (buffer[i] == '\n')  currentLine++;
        	}
    	}
        if (strcmp(temp, "")) {
			// Στέλνουμε την τελευταία λέξη στον builder με νούμερο το HashNumber που προκύπτει αν την περάσουμε από μια hash function
			if (!list_find(ExcList, temp)) {
				HashNumber = hash(temp) % (NumOfBuilders);
				write(fdx[HashNumber], temp, sizeof(temp));
			}
		}
	// Σβήνουμε τη λίστα
    list_destroy(ExcList);
	// Κλείνουμε όλα τα named pipes
	for (int i = 0; i < NumOfBuilders; i++) {
		close(fdx[i]);
	}
	// Στέλνουμε σήμα στο root ότι ο splitter τελείωσε τη δουλειά του
	kill(RootId, SIGUSR1);
}