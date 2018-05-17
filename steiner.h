#ifndef _H_STEINER
#define _H_STEINER

#include <glpk.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <vector>
#include "vetorja.h"

// #define EPS 1e-4
// #define COMP(x, y) (absf(x - y) <= EPS ? 0 : x < y ? -1 : 1)
// COMP(7.001, 7) == 1
// COMP(7.000001, 7) == 0
// COMP(7, 8) == -1

int steiner(void){
  //declaração de variáveis utilizadas em todas as partes do problema
  int i,j,k, cont, numPontos, numDim, numSteiner,numRestricoes, numBinarias,numBinariaSteiner;
  float distancia, distancias[100];// limiteMax, limiteMin;
  char var[100]; char lixo[50];
  double Z;

  //abre arquivo para leitura
  FILE *file;
  file = fopen("fixos.txt","r");

  //carrega o numero de pontos fixos e dimensões
  fscanf(file, "%d %d", &numPontos, &numDim);
  numSteiner=numPontos-2;
  //criação de matriz das coordenadas dos pontos fixos a partir de arquivo
  float coordenadaFixo[numPontos][numDim];

  for ( i=1; i<=numPontos; i++ )
    for ( j=1; j<=numDim; j++ )
    {
      fscanf (file, "%f", &coordenadaFixo[i][j]);

      //pra pegar os valores randomicos depois
      // if ((float)coordenadaFixo<(float)limiteMin)
      // {
      //   limiteMin=coordenadaFixo;
      // }else if ((float)coordenadaFixo>(float)limiteMax)
      // {
      //   limiteMax=coordenadaFixo;
      // }
    }
    fclose(file);

    //impressão de matriz das coordenadas dos pontos fixos
    printf("\nespaço pertencente ao R%d\n\n", numDim);

    printf("matriz de coordenadas dos pontos fixos no R%d\n", numDim);
    for (i=1; i<=numPontos; i++) {
      for (j=1; j<=numDim; j++) {
        printf("%f ", coordenadaFixo[i][j]);
      }
      printf("\n");
    }
    printf("\n");

  //leitura de coordenadas dos pontos de steiner
  float coordenadaSteiner[numSteiner][numDim];
  printf("digite as %d coordenadas dos pontos de steiner:\n", numDim);

  for ( i=1; i<=numSteiner; i++ ){
    printf("coordenadas do %dº ponto de steiner:\n",i);
    for ( j=1; j<=numDim; j++ )
    {
      if (j==1) {
        printf("coordenada x: ");
      }else if (j==2){
        printf("coordenada y: ");
      }else if(j==3){
        printf("coordenada z: ");
      }
      scanf ("%f", &coordenadaSteiner[i][j]);

      //pra pegar os valores randomicos depois
      // coordenadaSteiner[i][j]=limiteMin + rand() % limiteMax;
      // printf("%f\n", coordenadaSteiner[i][j]);
    }
  }

  printf("\n\n");

  //impressão de matriz das coordenadas dos pontos steiner
  printf("matriz das coordenadas dos pontos de Steiner\n" );
  for (i=1; i<=numSteiner; i++) {
    for (j=1; j<=numDim; j++) {
      printf("%f ", coordenadaSteiner[i][j]);
    }
    printf("\n");
  }
  printf("\n");

  //calculo das distancias euclideanas de todos os pontos fixos para steiner
  cont=0;
  for (i=1; i<=numPontos; i++) {
    for (j=1; j<=numSteiner; j++) {
      distancia=0;
      for (k=1; k<=numDim; k++) {
         distancia=distancia+pow(coordenadaFixo[i][k]-coordenadaSteiner[j][k],2);
      }
      distancia=sqrt(distancia);
      printf("distancia euclideana de pt fixo %i para pt Steiner %i: %f\n", i,j+numPontos,distancia);
      distancias[cont]=distancia;
      cont++;
    }
  }

  //calculo das distancias euclideanas de todos os pontos de steiner entre si
  for (i=1; i<numSteiner; i++) {
    for (j=i+1; j<=numSteiner; j++) {
      distancia=0;
      for (k=1; k<=numDim; k++) {
         distancia=distancia+pow(coordenadaSteiner[i][k]-coordenadaSteiner[j][k],2);
      }
      distancia=sqrt(distancia);
      printf("distancia euclideana de pt steiner %i para pt Steiner %i: %f\n", i+numPontos,j+numPontos,distancia);
      distancias[cont]=distancia;
      cont++;
    }
  }

//---------------------OTIMIZAÇÃO DA ÁRVORE----------------------------------

  //criaçãp do problema, nome e tipo
  glp_prob *lpst;
  lpst = glp_create_prob();
  glp_set_prob_name(lpst, "steiner");
  glp_set_obj_dir(lpst, GLP_MIN);

//-----------------------------------------------------------------------------

  //criação das linhas (restrições)
  numRestricoes = numPontos+numSteiner;
  glp_add_rows(lpst, numRestricoes);

  //linhas referentes as restrições de pontos fixos
  for (i = 1; i <= numPontos; i++) {
    strcpy(var,"p");
    gcvt(i,0,lixo);
    strcat(var,lixo);
    glp_set_row_name(lpst, i, var);
    glp_set_row_bnds(lpst, i, GLP_FX, 1.0, 1.0);
  }
  //linhas referentes as restrições de pontos de Steiner
  for (i = 1; i <= numSteiner; i++) {
    strcpy(var,"q");
    gcvt(i+numPontos,0,lixo);
    strcat(var,lixo);
    glp_set_row_name(lpst, i+numPontos, var);
    glp_set_row_bnds(lpst, i+numPontos, GLP_FX, 3.0, 3.0);
  }
//-----------------------------------------------------------------------------

  //criação das variáveis referentes as ligações possíveis
  glp_add_cols(lpst, (numPontos*numSteiner+(numSteiner*(numPontos-3)/2)));

  //criações das variáveis das ligações de pontos fixos para Steiner com distancias
  printf("\nFuncao Objetivo: ");
  cont=1;
  numBinarias=0;
  for (i = 1; i <= numPontos; i++) {
    for (j = 1; j <= numSteiner; j++) {
      strcpy(var,"x");
      gcvt(i,0,lixo);
      strcat(var,lixo);
      strcat(var,"i");
      gcvt(j+numPontos,0,lixo);
      strcat(var,lixo);

      glp_set_col_kind(lpst, cont, GLP_BV);
      glp_set_col_name(lpst, cont, var);
      glp_set_obj_coef(lpst, cont, distancias[cont-1]); //***********************erro tava aqui, mostrando a distancia errada
      printf("%lf%s ",distancias[cont-1],var);

      cont++;
      numBinarias++;
    }
  }

  //criações das variáveis das ligações de pontos Steiner para Steiner com distancia
  numBinariaSteiner=0;
  for (i = 1; i <= numSteiner-1; i++) {
    for (j = i+1; j <= numSteiner; j++) {
      strcpy(var,"x");
      gcvt((i+numPontos),0,lixo);
      strcat(var,lixo);
      strcat(var,"i");
      gcvt((j+numPontos),0,lixo);
      strcat(var,lixo);

      glp_set_col_kind(lpst, cont, GLP_BV);
      glp_set_col_name(lpst, cont, var);
      glp_set_obj_coef(lpst, cont, distancias[cont-1]); //***********************erro tava aqui, mostrando a distancia errada
      printf("%lf%s ",distancias[cont-1],var);

      cont++;
      numBinarias++;
      numBinariaSteiner++;
    }
  }

//-----------------------------------------------------------------------------
  //tamanho do vetor da matriz a seguir
  int tamVetor = ((numBinarias*2)+1);
  printf("\n\ntamVetor é %d\n\n", tamVetor);

  //vetor ia que representa a linha da matriz quantidade de restrições
  int ia[tamVetor];
  ia[0]=0;
  //primeira regra
  cont=1;
  int aux=0;
  for (i = 1; i <= numPontos*numSteiner; i++) {
    if (aux<numSteiner) {
      ia[i]=cont;
    }else{
      cont++;
      ia[i]=cont;
      aux=0;
    }
    aux++;
  }
  //segunda regra
  cont=numPontos+1;
  aux=0;
  for (i = numPontos*numSteiner+1; i <tamVetor; i++) {
    if (aux<numPontos+numSteiner-1) {
      ia[i]=cont;
    }else{
      cont++;
      ia[i]=cont;
      aux=0;
    }
    aux++;
  }
  //impressão para testes
  for( i = 0 ; i < tamVetor; i++)
  {
     printf("ia[%d] = %d\n",i, ia[i]);
  }
  printf("\n");

  //vetor ja que representa a coluna da matriz são as variáveis binarias das ligações
  int ja[tamVetor];
  //primeira regra
  for (i = 0; i <= numPontos*numSteiner; i++) {
      ja[i]=i;
  }

  //segunda regra
  aux=0;
  int aux2=1;
  cont=1;

  for (i = 0; i < numSteiner; i++) {
    for (j = numPontos*numSteiner+aux+i+1; j <numPontos*numSteiner+numPontos+aux+i+1; j++) {
      ja[j+aux]=cont;
      cont=cont+numSteiner;
    }

    aux2++;
    cont=aux2;
    aux=aux+numSteiner;
  }

  /*********Adicionado Prof**************/
  vector<arestasteiner> vc;

  int col=1;
  for(int i=1;i<=numSteiner-1;i++){
    for(int j=i+1;j<=numSteiner;j++){
       arestasteiner as(i+numPontos,j+numPontos,col+(numPontos*numSteiner));
       vc.push_back(as);
       col++;
    }
  }

  //pega a coluna correta na matriz a ser inserida no vetor ja
  //coloca 1 na posicao correta da matriz
  int pi,pj;
  int pos=(numPontos*numSteiner)+numPontos+1;
  for(int i=1;i<=numSteiner;i++){
    for(int j=1;j<=numSteiner;j++){
        if(i!=j){
            if(i<j){
                pi=i+numPontos;pj=j+numPontos;
                //aqui pega a coluna desejada a ser inserida no vetor ja
                col=buscacol(pi,pj,vc);
                ja[pos]=col;
                //cout << "\ncoluna de x"<<pi<<"i"<<pj<<"="<<col<<" na posicao="<<pos;
                pos=pos+1;

            }
            if(i>j){
                pi=j+numPontos;pj=i+numPontos;
                //aqui pega a coluna desejada a ser inserida no vetor ja
                col=buscacol(pi,pj,vc);
                ja[pos]=col;
                //cout << "\ncoluna de x"<<pj<<"i"<<pi<<"="<<col<<" na posicao="<<pos;
                pos=pos+1;

            }
        }
    }
    pos=pos+numPontos;
  }
  /***************************************/
  printf("\n");
  for( i = 0 ; i < tamVetor; i++)
  {
     printf("ja[%d] = %d\n",i, ja[i]);
  }

  //vetor ar que representa o valor dos itens da matriz delimitados acima inserindo valores
  double ar[tamVetor];
  for (i = 0; i < tamVetor; i++) {
    if (i==0){
      ar[i]=i;
    }else{
      ar[i]=1;
    }
  }

  glp_write_lp(lpst,NULL,"teste-steiner.lp");
  glp_load_matrix(lpst, tamVetor-1, ia, ja, ar);
  glp_simplex(lpst,NULL);
  glp_intopt(lpst,NULL);

  //tamanho da árvore
  Z = glp_mip_obj_val(lpst);

  //imprime o tamanho da árvore e os valores das variáveis binarias correspondentes as ligações
  printf( "\nZ = %g; ", Z);
  for (i = 1; i <= numBinarias; i++) {
    cout << glp_get_col_name(lpst,i) << "=" << glp_mip_col_val(lpst,i) <<"; ";
  }
  printf("\n\n");

  //finaliza o problema
  glp_delete_prob(lpst);

  return 0;
}

#endif
