#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int count1;
int count2;

unsigned long hash(unsigned char *str) {
    unsigned long hash = 17;
    int c; 
    while (c = *str++)
        hash = ((hash << 5) + hash) + c; 
    return hash;
} 

void usr1(int SigNum) {
    if (SigNum == SIGUSR1) {
        count1++;
    }
}

void usr2(int SigNum) {
    if (SigNum == SIGUSR2) {
        count2++;
    }
}