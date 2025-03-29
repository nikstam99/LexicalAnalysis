extern int count1;
extern int count2;

// Hash συνάρτηση djb2 για strings
unsigned long hash(unsigned char *str);

// Μετράει τα USR1 σήματα που στέλνουν οι splitters
void usr1(int SigNum);

// Μετράει τα USR2 σήματα που στέλνουν οι builders
void usr2(int SigNum);