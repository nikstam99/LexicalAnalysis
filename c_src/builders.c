#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "../include/list.h"
#include "../include/general.h"
#include <errno.h>
#include <signal.h>
#include <sys/times.h>

int main(int argc, char* argv[]) {
    int NumOfBuilders = atoi(argv[1]);                      // Ο αριθμός των builders
    int pipe = atoi(argv[2]);                               // Ο fd του pipe που στέλνει ο builder τις λέξεις στο root
    int RootId = atoi(argv[3]);                             // To id του root process
    int pipe2 = atoi(argv[4]);
    char NamedPipe[20];
	char num[100];
	strcpy(NamedPipe, "fifo");
	sprintf(num, "%d", (getpid() % (NumOfBuilders)));
	strcat(NamedPipe, num);
    int fd = open(NamedPipe, O_RDONLY);                     // Ανοίγουμε το Named Pipe που αντιστοιχεί στον builder
    if (fd == -1) {
        printf("Error opening fifo\n");
        return 0;
    }
    char buffer[20];
    // Δημιουργούμε τη λίστα στην οποία αποθηκεύουμε τις λέξεις
    List list = list_create();
    ssize_t nbytes;
    int i = 0;
    int HashNumber;
    // Διαβάζουμε ότι έχουν στείλει οι splitters στο named pipe
    while(nbytes = read(fd, buffer, 20) > 0) {
        list_insert_first(list, buffer);
    }
    char word[20];
    char number[20];
    // Στέλνουμε στο root όλες τις λέξεις της λίστας που μάζεψε ο builder μαζί με τον αριθμό των φορών που εμγανίστηκε στο κείμενο
    for (Listnode node = list_first(list); node != NULL; node = node_next(node)) {
        strcpy(word, node->value);
        write(pipe, word, sizeof(word));    
        sprintf(number, "%d", node->number);
        write(pipe2, number, sizeof(number));
    }
    close(pipe);
    close(fd);
    list_destroy(list);             // Σβήνουμε τη λίστα
    kill(RootId, SIGUSR2);          // Στέλνουμε σήμα στο root ότι ο builder τελείωσε τη δουλειά του
    // Μετράμε πόση ώρα χρειάστηκε ο κάθε builder
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
    sprintf(number, "%lf", (t2 - t1) / ticspersec);
    write(pipe2, number, sizeof(number));
    sprintf(number, "%lf", cpu_time / ticspersec);
    write(pipe2, number, sizeof(number));
    close(pipe2);
	//printf ("Builder with process id %d: Run time was %lf sec ( REAL time ) although we used the CPU for %lf sec ( CPU time ).\n", getpid() ,(t2 - t1) / ticspersec , cpu_time / ticspersec );
}