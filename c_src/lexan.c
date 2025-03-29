#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "../include/list.h"
#include "../include/general.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#include <sys/times.h>

// int count1;		// Συνολικά USR1 σήματα από τους splitters
// int count2;		// Συνολικά USR2 σήματα από τους builders

int main(int argc, char *argv[]) {
	char *inputFile = NULL;
	char *outputFile = NULL;
	char *ExculsionList = NULL;
	long int n1;
	int NumOfSplitters, NumOfBuilders, TopPopular;
	if (argc != 13) {
		printf("Error!\n");
		return(1);
	}
	for (int i = 1; i < 12; i++) {			// Ελέγχουμε αν τα δεδομένα από τη γραμή εντολών είναι σωστά
		if (!strcmp(argv[i], "-i")) 		
			inputFile = argv[i+1];			// Αρχείο εισόδου
		else if (!strcmp(argv[i], "-l"))
			NumOfSplitters = atoi(argv[i+1]);		// Αριθμός των splitters
		else if (!strcmp(argv[i], "-m"))
			NumOfBuilders = atoi(argv[i+1]);		// Αριθμός των builders
		else if (!strcmp(argv[i], "-t"))
			TopPopular = atoi(argv[i+1]);			// Αριθμός των πιο σημαντικών λέξεων που θα εμφανιστούν
		else if (!strcmp(argv[i], "-e"))
			ExculsionList = argv[i+1];		// Αρχείο με λέξεις που θα εξαιρεθούν
		else if (!strcmp(argv[i], "-o"))
			outputFile = argv[i+1];			//Αρχείο εξόδου
		i++;
	}
	count1 = 0;
	count2 = 0;
	// Τροποποιούμε τα SIGUSR1 και SIGUSR2 για να στένουμε σήματα εξόδου από τους builders, splitters στο root
	signal(SIGUSR1, usr1);
	signal(SIGUSR2, usr2);
	// Ανοίγουμε το inputfile και ελέγχουμε για λάθη
	int fd1 = open(inputFile, O_RDONLY);
	if (fd1 == -1) {
		printf("Error opening source file!");
		return(1);
	}
    char buffer[1024];
    ssize_t bytesRead;
    int lines = 0;
	// Μετράμε τις γραμμές του αρχείου
    while ((bytesRead = read(fd1, buffer, 1024)) > 0) {
        for (ssize_t i = 0; i < bytesRead; i++) {
            if (buffer[i] == '\n') {
                lines++;
            }
        }
    }
	lines++;
	close(fd1);
	pid_t pid = 1; 
	pid_t wpid;
	int status = 0;
	pid_t RootId = 1;
	int p[2];
	char num[100] ="";
	char NamedPipe[20];
	char Root[20];
	// Δημιουργούμε τα Named Pipes από τα οποία θα στέλνουν λέξεις οι builders στους splitters
	// Τα pipes αυτά θα είναι σε αριθμό όσοι και οι builders
	for (int i = 0; i < NumOfBuilders; i++) {
		strcpy(NamedPipe, "fifo");
		sprintf(num, "%d", i);
		strcat(NamedPipe, num);
		if (mkfifo(NamedPipe, 0666) == -1) {
			if (errno != EEXIST) {
				printf("Error in named pipe\n");
				return 0;
			}
		}
	}
	// Δημιουργία των Splitters
	for (int i = 0; i < NumOfSplitters; i++) {
		if (pid != 0) {
			if (pipe(p) < 0) 		// Δημιουργία των pipes για να στείλουμε πληροφορία στους Splitters
        		exit(1);
			sprintf(num, "%d", lines/NumOfSplitters*i + 1);
			// Στέλνουμε τον αριθμό της γραμμής που πρέπει να ξεκινήσουν να διαβάζουν οι Splitters
			write(p[1], num, 100);
			sprintf(num, "%d", lines/NumOfSplitters*i+ + lines/NumOfSplitters);
			if (i == NumOfSplitters - 1) 
				sprintf(num, "%d", lines/NumOfSplitters*i+ + lines/NumOfSplitters + lines%NumOfSplitters);
			// Στέλνουμε τον αριθμό της γραμμής που πρέπει να τελειώσουν το διάβασμα οι Splitters
			write(p[1], num, 100);
			close(p[1]);
			// Δημιουργία του clone process
			pid = fork();
		}
			
		if (pid != 0 && i == 0) {
			RootId = getpid();		// Αποθηκεύουμε το id του root
		}
	}
	// Το κομμάτι που θα εκτελέσει το root
	if (getpid() == RootId) {
		pid = 1;
		int p2[2];
		int p3[2];
		int rd[NumOfBuilders];
		int rd2[NumOfBuilders];
		int wr1[NumOfBuilders];
		int wr2[NumOfBuilders];
		char pipe2[10];
		char pipe3[10];
		// Δημουργία των Builders
		for (int i = 0; i < NumOfBuilders; i++) {
			if (pid != 0) {
				// Δημιουργούμε τα pipes μέσω των οποίων θα στέλνουμε τα αποτελέσματα από τους builders στο root 
				if (pipe(p2) < 0) {
					exit(1);
				}
				if (pipe(p3) < 0) {
					exit(1);
				}
				// Κρατάμε τα fd των pipes σε έναν πίνακα
				rd[i] = p2[0];
				rd2[i] = p3[0];
				wr1[i] = p2[1];
				wr2[i] = p3[1];
				// Δημιουργία του clone process
				pid = fork();
			}
			if (!pid) {
				sprintf(pipe2, "%d", p2[1]);
				sprintf(pipe3, "%d", p3[1]);
				sprintf(num, "%d", NumOfBuilders);
				sprintf(Root, "%d", RootId);
				// Οι builders εκτελούν το excecutable του builders.c
				execlp("./builders", "builders", num, pipe2, Root, pipe3, NULL);
				perror("Builder exec failure.. ");
				exit(1);
			}
		}
		int counter = 0;
		char buff[20];
		char buff2[20];
		char word[20];
		int number;
		int n1b, n2b;
		// Δημιουργούμε τη λίστα που θα περιέχει τα αποτελέσματα
		List Results = list_create();
		// Κλείνουμε το write end των pipe που στέλνουν τις λέξεις από τον builder στο root
		for (int i = 0; i < NumOfBuilders; i++) {
			close(wr1[i]);	
			close(wr2[i]);
		}
		for (int i = 0; i < NumOfBuilders; i++) {
			// Διαβάζουμε ότι έχει στείλει ο κάθε builder μέσω του pipe του στο root και περνάμε όλες τις λέξεις στη λίστα με τα αποτελέσματα
			while(1){
				n1b = read(rd[i], buff, sizeof(buff));
				n2b = read(rd2[i], buff2, sizeof(buff2));
				if (n1b == 0 || n2b == 0) break;
				strcpy(word, buff);
				number = atoi(buff2);
				if (list_size(Results) == 0) {
					list_insert_next(Results, NULL, word, number);
					continue;
				}
				for (Listnode node = list_first(Results); node != NULL; node = node_next(node)) {
					if (node->number <= number) {
						Listnode node2 = list_find_previous_node(Results, node);
						list_insert_next(Results, node2, word, number);
						break;
					}
				}
			}
			float f1, f2;
			read(rd2[i], buff, sizeof(buff));
			f1 = atof(buff);
			read(rd2[i], buff2, sizeof(buff2));
			f2 = atof(buff2);		
			printf ("Run time was %lf sec ( REAL time ) although we used the CPU for %lf sec ( CPU time ).\n", f2 , f1);
		}
		// Εκτύπωση των ζητουμένων στο tty
		printf("\nTop %d Popular Words: \n", TopPopular);
		print_list(Results, TopPopular);
		printf("\n");
		printf("Signals caught from Splitters: %d\n", count1);
		printf("Signals caught from Builders: %d\n", count2);
		
		FILE* fd2;
		fd2 = fopen(outputFile, "w");
		if (fd2 == NULL) {
			printf("Error opening output file in process %d\n", getpid());
			return(1);
		}
		// Περνάμε τη λίστα με τα αποτελέσματα στο output file
		for (Listnode node = list_first(Results); node != NULL; node = node_next(node)) {
			fprintf(fd2,"%s: %d (f = %f)\n", node->value, node->number, (double)node->number / (double)Results->total_words);
		}
		fclose(fd2);
		char NamedPipe[20];
		char num1[10];
		// Κάνουμε unlink όλα τα named pipes που φτιάξαμε
		for (int i = 0; i < NumOfBuilders; i++) {
			strcpy(NamedPipe, "fifo");
			sprintf(num1, "%d", i);
			strcat(NamedPipe, num1);
			unlink(NamedPipe);
		}
		list_destroy(Results);
		// Μετράμε πόση ώρα χρειάστηκε το root
		double t1 , t2 , cpu_time ;
		struct tms tb1 , tb2 ;
		double ticspersec ;
		int ii, sum = 0;
		ticspersec = ( double ) sysconf ( _SC_CLK_TCK );
		t1 = ( double ) times (& tb1) ;
		for (ii = 0; ii < 100000000; ii ++)
		sum += ii;
		t2 = ( double ) times (& tb2) ;
		cpu_time = ( double ) (( tb2 . tms_utime + tb2 . tms_stime ) -
		( tb1 . tms_utime + tb1 . tms_stime ));
		printf ("Root with process id %d: Run time was %lf sec ( REAL time ) although we used the CPU for %lf sec ( CPU time ).\n", getpid() ,(t2 - t1) / ticspersec , cpu_time / ticspersec );
	}
	// Το κομμάτι που θα εκτελέσουν οι Splitters
	if (getpid()!= RootId) {
		char P[100];
		sprintf(Root, "%d", RootId);
		sprintf(P, "%d", p[0]);
		sprintf(num, "%d", NumOfBuilders);
		// Οι splitters θα εκτελέσουν το excecutable του splitters.c
		execlp("./splitters", "splitters", inputFile, num, P, ExculsionList, Root, NULL);
		perror("Splitter exec failure.. ");
		exit(1);
	}
}