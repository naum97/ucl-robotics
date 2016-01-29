/*
  Created by Matthew Bell and Wayne Tsui.
  This programme sorts a user input list of random numbers in ascending order using bubble sort algorithim.
*/

#include <stdio.h>
#include <stdlib.h>
#include "simpletools.h"

  int* getIntArr() {
    int i;
    printf("Enter number of elements\n");
    scanf("%i", &i);
    int *inputs = malloc(sizeof(int) * i);
    printf("Enter %i integers\n", i);

    for (int n = 0; n < i; n++) {
      scanf("%i", (inputs+n));
    }
    return inputs;
  }

  int* bubbleSort(int* g, int len) {
    int temp;
    int counter = 0;

    for (int j = len-1; j > 0; j--) {
      if (*(g+j) > *(g+j-1)) {
        counter++;
      }
    }

    if (counter == len-1)
      return g;
    else
      counter = 0;

    for (int n = 0; n < len; n++) {
      for (int i = 0; i < len-1; i++) {
        if (*(g+i) > *(g+i+1)) {
          temp = *(g+i+1);
          *(g+i+1) = *(g+i);
          *(g+i) = temp;
        }
      }

    }
    return g;
  }

  void results(int* g, int len) {
    printf("Sorted list in ascending order: \n");
    for (int n = 0; n < len; n++) {
      printf("%i ", *(g+n));
    }
    printf("\n");
  }

  int arrLen(int* g) {
    int length = 0;
    int* h = g;
    while(1) {
      if (*h != '\0') {
        length++;
      }
      else {
        break;
      }
      h++;
    }
    return length;
  }

int main() {

    int* a = getIntArr();
    int length = arrLen(a);
    results(bubbleSort(a, length), length);
    free(a);

}
