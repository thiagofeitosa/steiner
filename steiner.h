#ifndef _H_STEINER
#define _H_STEINER

#include <glpk.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

int steiner(void){
  //declaração de variáveis utilizadas em todas as partes do problema
  int i,j,k, cont, numPontos, numDim, numSteiner,numRestricoes, numBinarias;
  float distancia, distancias[100], limiteMax, limiteMin;
  char var[100]; char lixo[50];

  double Z, x1i5, x1i6, x2i5, x2i6, x3i5, x3i6, x4i5, x4i6, x5i6;
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
    printf("espaço pertencente ao R%d\n\n", numDim);

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
    printf("\n\n");
  }



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

  //criação das colunas (restrições)
  numRestricoes = numPontos+numSteiner;
  glp_add_rows(lpst, numRestricoes);

  //colunas referentes as restrições de pontos fixos
  for (i = 1; i <= numPontos; i++) {
    strcpy(var,"p"); gcvt(i,0,lixo); strcat(var,lixo);
    glp_set_row_name(lpst, i, var);
    glp_set_row_bnds(lpst, i, GLP_FX, 1.0, 1.0);
  }
  //colunas referentes as restrições de pontos de Steiner
  for (i = 1; i <= numSteiner; i++) {
    strcpy(var,"q"); gcvt(i+numPontos,0,lixo); strcat(var,lixo);
    glp_set_row_name(lpst, i+numPontos, var);
    glp_set_row_bnds(lpst, i+numPontos, GLP_FX, 3.0, 3.0);
  }


//-----------------------------------------------------------------------------

  //criação das variáveis referentes as ligações possíveis
  glp_add_cols(lpst, (numPontos*numSteiner+(numSteiner*(numPontos-3)/2)));

  //criações das variáveis das ligações de pontos fixos para Steiner com distancias
  printf("\nFuncao Objetivo: ");
  cont=0;
  numBinarias=0;
  for (i = 1; i <= numPontos; i++) {
    for (j = 1; j <= numSteiner; j++) {
      strcpy(var,"x");
      gcvt(i,0,lixo);
      strcat(var,lixo);
      strcat(var,"i");
      gcvt(j+numPontos,0,lixo);
      strcat(var,lixo);

      glp_set_col_kind(lpst, i,GLP_BV);
      glp_set_col_name(lpst, i, var);
      glp_set_obj_coef(lpst, i, distancias[cont]);
      printf("%s%lf ",var,distancias[cont]);
      cont++;
      numBinarias++;
    }
  }

  //criações das variáveis das ligações de pontos Steiner para Steiner com distancia
  for (i = 1; i <= numSteiner-1; i++) {
    for (j = i+1; j <= numSteiner; j++) {
      strcpy(var,"x");
      gcvt(i+numPontos,0,lixo);
      strcat(var,lixo);
      strcat(var,"i");
      gcvt(j+numPontos,0,lixo);
      strcat(var,lixo);

      glp_set_col_kind(lpst, i,GLP_BV);
      glp_set_col_name(lpst, i, var);
      glp_set_obj_coef(lpst, i, distancias[cont]);
      printf("%s%lf ",var,distancias[cont]);
      cont++;
      numBinarias++;
    }
  }

//-----------------------------------------------------------------------------
  // int ia[19]={0,1,1,2,2,3,3,4,4,5,5,5,5,5,6,6,6,6,6};
  // int ja[19]={0,1,2,3,4,5,6,7,8,1,3,5,7,9,2,4,6,8,9};
  // double ar[19]={0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

  //tamanho do vetor da matriz a seguir
  int tamVetor = ((numBinarias*2)+1);
  printf("\n\ntamVetor é %d\n\n", tamVetor);

  //vetor ia que representa a linha da matriz quantidade de restrições >>valores de teste
  int ia[tamVetor];
  for (i = 0; i < tamVetor; i++) {
    if (i==0){
      ia[i]=i;
    }else{
      ia[i]=1+(rand() % numRestricoes);
    }
  }

  //vetor ja que representa a coluna da matriz são as variáveis binarias das ligações >>valores de teste
  int ja[tamVetor];
  for (i = 0; i < tamVetor; i++) {
    if (i==0){
      ja[i]=i;
    }else{
      ja[i]=1+(rand() % numBinarias);
    }
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

  glp_load_matrix(lpst, tamVetor-1, ia, ja, ar);
  glp_simplex(lpst,NULL);
  glp_intopt(lpst,NULL);

  //tamanho da árvore
  Z = glp_mip_obj_val(lpst);

  x1i5 = glp_mip_col_val(lpst, 1);
  x1i6 = glp_mip_col_val(lpst, 2);
  x2i5 = glp_mip_col_val(lpst, 3);
  x2i6 = glp_mip_col_val(lpst, 4);
  x3i5 = glp_mip_col_val(lpst, 5);
  x3i6 = glp_mip_col_val(lpst, 6);
  x4i5 = glp_mip_col_val(lpst, 7);
  x4i6 = glp_mip_col_val(lpst, 8);
  x5i6 = glp_mip_col_val(lpst, 9);

  //mostra valores de tamanho da árvore e quais ligações foram utilizadas na árvore otimizada
  //ainda ver como fica essas variáveis das ligações
  printf( "\nZ = %g; x1i5 = %g; x1i6 = %g; x2i5 = %g; x2i6 = %g; x3i5 = %g; x3i6 = %g; x4i5 = %g; x4i6 = %g; x5i6 = %g\n", Z, x1i5, x1i6, x2i5, x2i6, x3i5, x3i6, x4i5, x4i6, x5i6);

  //finaliza o problema
  glp_delete_prob(lpst);

  return 0;
}


//void steiner();

#endif
