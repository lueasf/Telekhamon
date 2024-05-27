#pragma once
//this function is not from me but found on the internet


#include <stdio.h>
#include <stdlib.h>
 
// Fonction pour échanger deux nombres
void swap(char *x, char *y);
 
// Fonction pour inverser `buffer[i…j]`
char* reverse(char *buffer, int i, int j);
 
// Fonction itérative pour implémenter la fonction `itoa()` en C
char* itoa(int value, char* buffer, int base);
