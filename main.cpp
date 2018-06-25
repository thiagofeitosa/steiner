#include "glpk.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "steiner.h"
//#include "load.h"

double *coordSteiner;

int main(void) {
  load();
  coordSteiner = new double[numSteiner*numDim];
  loadSteiner(&coordSteiner, 1);
  //coordSteiner[0]=5;coordSteiner[1]=3;coordSteiner[2]=5;coordSteiner[3]=6;
  mostraSteiner(coordSteiner);
  double tamTree=steiner(coordSteiner);
  printf("\n\nTamanho da Arvore de Steiner %lf\n\n",tamTree);
  return 0;
}
