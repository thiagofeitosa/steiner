#include <stdio.h>
#include "steiner.h"

int main(void) {
  load(); //le os dados dos pontos fixos e inicializa as principais variaveis do programa

  //inicializa as variaveis coordSteiner, vetLB e vetUB - deve ser executado sempre ap�s o procedimento load()
  initvariables();

  loadvetLBUB(&vetLB,&vetUB); //carrega LB e UB de cada dimens�o

  mostravetLBUB(vetLB, vetUB); //mostra os dados de vetLB e vetUB - no BRKGA n�o precisa

  loadSteiner(&coordSteiner, 1); //isso � pra carregar os pontos de Steiner de um arquivo - no BRKGA n�o precisa

  mostraSteiner(coordSteiner); //isso � pra mostrar se o vetor est� carregado direito - no BRKGA n�o precisa
  //steiner � o c�digo que executa a formula��o para as coordenadas dos pontos de Steiner(coordSteiner) e retorna o tamanho da �rvore
  double tamTree=steiner(coordSteiner, 1);
  printf("\n\nTamanho da árvore=%lf",tamTree);
  return 0;
}
