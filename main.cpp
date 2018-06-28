#include <stdio.h>
#include "steiner.h"

int main(void) {
  load(); //le os dados dos pontos fixos e inicializa as principais variaveis do programa
    
  //inicializa as variaveis coordSteiner, vetLB e vetUB - deve ser executado sempre após o procedimento load()
  initvariables(); 
  
  loadvetLBUB(&vetLB,&vetUB); //carrega LB e UB de cada dimensão
  
  mostravetLBUB(vetLB, vetUB); //mostra os dados de vetLB e vetUB - no BRKGA não precisa
  
  loadSteiner(&coordSteiner, 1); //isso é pra carregar os pontos de Steiner de um arquivo - no BRKGA não precisa
  
  mostraSteiner(coordSteiner); //isso é pra mostrar se o vetor está carregado direito - no BRKGA não precisa
  //steiner é o código que executa a formulação para as coordenadas dos pontos de Steiner(coordSteiner) e retorna o tamanho da árvore
  double tamTree=steiner(coordSteiner); 
  printf("\n\nTamanho da árvore=%lf",tamTree); 
  return 0;
}
