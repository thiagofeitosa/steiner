#ifndef _H_STEINER
#define _H_STEINER

#include "glpk.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "vetorja.h"

#include <exception>
#include <iostream>

//comparar float
#define EPS 1e-4
#define COMP(x, y) (fabs(x - y) <= EPS ? 0 : x < y ? -1 : 1)
// COMP(7.001, 7) == 1
// COMP(7.000001, 7) == 0
// COMP(7, 8) == -1

//esse � o vetor de solu��es, cada ponto cont�m numDim coordenadas - por exemplo com 2 pontos de steiner com as coordenadas (5,3) e (5,6) ficaria
//coordSteiner[0]=5;coordSteiner[1]=3;coordSteiner[2]=5;coordSteiner[3]=6;
double *coordSteiner;

//vetor com os LB e UB de cada ponto de steiner
double *vetLB, *vetUB;


int numPontos, numDim, numSteiner;
FILE *entradafixo, *saidafixo;

//criação de matriz das coordenadas dos pontos fixos a partir de arquivo
float **coordenadaFixo;

void initvariables()
{
	coordSteiner = new double[numSteiner*numDim]; //esse � o vetor de solucao que vai determinar os pontos de Steiner
    vetLB = new double[numSteiner*numDim];
    vetUB = new double[numSteiner*numDim];
}

void load(void){
   entradafixo = fopen("fixos.txt","r");


   saidafixo = fopen("gnuplot/ptfixos.dat","w");

   //carrega o numero de pontos fixos e dimensões
   fscanf(entradafixo, "%d %d", &numPontos, &numDim);
   numSteiner=numPontos-2;

   coordenadaFixo = new float*[numPontos+1];
   for(int i = 0; i <= numPontos; i++) {
      coordenadaFixo[i] = new float[numDim+1];
   }

   for ( int i=1; i<=numPontos; i++ ){
    for ( int j=1; j<=numDim; j++ )
    {
      fscanf (entradafixo, "%f", &coordenadaFixo[i][j]);
      fprintf(saidafixo, "%f ", coordenadaFixo[i][j]);
    }
    fprintf(saidafixo, "\n");
  }
    fclose(entradafixo);
    fclose(saidafixo);

    //impressão de matriz das coordenadas dos pontos fixos
    printf("\nespaço pertencente ao R%d\n\n", numDim);

    printf("matriz de coordenadas dos pontos fixos no R%d\n", numDim);
    for (int i=1; i<=numPontos; i++) {
      for (int j=1; j<=numDim; j++) {
        printf("%f ", coordenadaFixo[i][j]);
      }
      printf("\n");
    }
    printf("\n");




}

void loadvetLBUB(double **vetLB, double **vetUB){
		
	
	for(int i=1; i<=numDim; i++){
	   int min,max; 	
	   for(int j=1; j<=numPontos; j++){
	       if(j==1){
      	   max=min=coordenadaFixo[j][i];	 
		 }
		 else{
		 	if(coordenadaFixo[j][i]>max){
		 	  max=coordenadaFixo[j][i]; 	
			}
			if(coordenadaFixo[j][i]<min){
		 	  max=coordenadaFixo[j][i]; 	
			}
		 }	
	   }
	   (*vetLB)[i-1]=min;
	   (*vetUB)[i-1]=max;	
	}
	
	for(int i=numDim;i<numSteiner*numDim;i++){
		(*vetLB)[i]=(*vetLB)[i%numDim];
		(*vetUB)[i]=(*vetUB)[i%numDim];
	}
	
}

void mostravetLBUB(double *vetLB, double *vetUB){
	printf("\n\nVETOR LB\n");
	for(int i=0;i<numSteiner*numDim; i++){
	   printf(" %lf",vetLB[i]);	
	}
	printf("\n\nVETOR UB\n");
	for(int i=0;i<numSteiner*numDim; i++){
	   printf(" %lf",vetUB[i]);	
	}
	
	printf("\n***************\n");
}

void loadSteiner(double **coordSteiner, int carregaSteiner){


    if(carregaSteiner==1){



    	FILE *entradasteiner, *saidasteiner;

        entradasteiner= fopen("steiner.txt","r");
        saidasteiner = fopen("gnuplot/ptsteiner.dat","w");



        int cont=0;
        for (int i=0; i<numSteiner; i++ ){
          for (int j=0; j<numDim; j++ )
          {
          	 double dado;
             fscanf (entradasteiner, "%lf", &dado);
             (*coordSteiner)[cont]=dado;


             fprintf(saidasteiner, "%lf ", (*coordSteiner)[cont]);
             cont++;
          }
          fprintf(saidasteiner, "\n");


       }
       fclose(entradasteiner);
       fclose(saidasteiner);


   }

}

void mostraSteiner(double *coordSteiner){
	int c=0;
	for (int i=0; i<numSteiner; i++ ){
		  printf("\n");
          for (int j=0; j<numDim; j++ ){
          	 printf(" %lf",coordSteiner[c+j]);

		}
		c=c+numDim;
   }

   printf("\n\n");
}

double steiner(double *coordSteiner){
  //declaração de variáveis utilizadas em todas as partes do problema
  int i,j,k, escolha=0, cont; //, numPontos, numDim, numSteiner,numRestricoes, numBinarias,numBinariaSteiner;
  int numRestricoes, numBinarias,numBinariaSteiner;
  float distancia;
  char var[100]; char lixo[50];
  double Z;

  //abre arquivo para leitura
  //FILE *entradafixo, *entradasteiner, *saidafixo, *saidasteiner, *edges;

  FILE *edges;
  edges = fopen("gnuplot/edges.dat","w");

  /***********
  FILE *entradasteiner, *saidasteiner, *edges;

  entradasteiner= fopen("steiner.txt","r");
  saidasteiner = fopen("gnuplot/ptsteiner.dat","w");
  edges = fopen("gnuplot/edges.dat","w");
  ***********/

  float distancias[numPontos*numSteiner*2];

  //leitura de coordenadas dos pontos de steiner
  // printf("digite 1 para ler coordena dos Pontos de Steiner do arquivo e 2 para inserir manualmente:\n" );
  // while (escolha!=1 and escolha!=2) {
  //   scanf("%d", &escolha);
  // }




  /**********CODIGO NOVO****************/
  //calculo das distancias euclideanas de todos os pontos fixos para steiner
  float maiordfixos = 0;
  cont=0;

  for (int i=1; i<=numPontos; i++) {
  	int c=0;
    for (int j=0; j<numSteiner; j++) {
      distancia=0;
      for (int k=0; k<numDim; k++) {
      	 distancia=distancia+pow(coordenadaFixo[i][k+1]-coordSteiner[c+k],2);
      }
      c=c+numDim;
      distancia=sqrt(distancia);
      printf("distancia euclideana de pt fixo %i para pt Steiner %i: %f\n", i,j+1+numPontos,distancia);
      distancias[cont]=distancia;
      cont++;
      if (COMP(distancia, maiordfixos) == 1) {
        maiordfixos=distancia;
      }
    }
  }
  printf("maiordfixos = %f\n", maiordfixos);

  //calculo das distancias euclideanas de todos os pontos de steiner entre si
  float maiordsteiner=0;
  int c=0;
  for(int i=0;i<numSteiner-1;i++){
  	int d=c+numDim;
  	for(int j=i+1;j<numSteiner;j++){
  	  distancia=0;
  	  for(int k=0;k<numDim;k++){
  	  	  distancia=distancia+pow(coordSteiner[c+k]-coordSteiner[d+k],2);
	  }
	  d=d+numDim;
	  distancia=sqrt(distancia);
      printf("distancia euclideana de pt steiner %i para pt Steiner %i: %f\n", i+1+numPontos,j+1+numPontos,distancia);
      distancias[cont]=distancia;
      cont++;
      if (COMP(distancia, maiordsteiner) == 1) {
        maiordsteiner=distancia;
      }
	}
	c=c+numDim;
  }
  printf("maiordsteiner = %f\n", maiordsteiner);
  /******FIM CODIGO NOVO****************/


//---------------------OTIMIZAÇÃO DA ÁRVORE----------------------------------

  //criaçãp do problema, nome e tipo
  glp_prob *lpst;
  lpst = glp_create_prob();

  glp_set_prob_name(lpst, "steiner");

  glp_set_obj_dir(lpst, GLP_MIN);



  //----------------------------------------------------------------------------//

  //criação das variáveis referentes as ligações possíveis
  glp_add_cols(lpst, (numPontos*numSteiner+(numSteiner*(numPontos-3)/2)));

  //criações das variáveis das ligações de pontos fixos para Steiner com distancias
  printf("\nFuncao Objetivo: ");
  cont=1;
  numBinarias=0;
  for (i = 1; i <= numPontos; i++) {
    for (j = 1; j <= numSteiner; j++) {

      strcpy(var,"x");
      gcvt(i,10,lixo);
      strcat(var,lixo);
      strcat(var,"i");
      gcvt(j+numPontos,10,lixo);
      strcat(var,lixo);

        glp_set_col_kind(lpst, cont, GLP_BV);
        glp_set_col_name(lpst, cont, var);
        if(COMP(distancias[cont-1],maiordfixos*0.87)==-1){
           glp_set_obj_coef(lpst, cont, distancias[cont-1]);
        }else{
           glp_set_obj_coef(lpst, cont, 99999);
           glp_set_col_bnds(lpst, cont, GLP_FX, 0.0, 0.0);

        }

        //printf("%lf%s ",distancias[cont-1],var);
        numBinarias++;
        cont++;
    }
  }



  //criações das variáveis das ligações de pontos Steiner para Steiner com distancia
  numBinariaSteiner=0;
  for (i = 1; i <= numSteiner-1; i++) {
    for (j = i+1; j <= numSteiner; j++) {
      strcpy(var,"x");
      gcvt((i+numPontos),10,lixo);
      strcat(var,lixo);
      strcat(var,"i");
      gcvt((j+numPontos),10,lixo);
      strcat(var,lixo);

      glp_set_col_kind(lpst, cont, GLP_BV);
      glp_set_col_name(lpst, cont, var);
      if(COMP(distancias[cont-1],maiordsteiner*0.87)==-1 || numSteiner == 2){
         glp_set_obj_coef(lpst, cont, distancias[cont-1]);
      }
      else{
         glp_set_obj_coef(lpst, cont, 99999);
         glp_set_col_bnds(lpst, cont, GLP_FX, 0.0, 0.0);

      }
      //printf("%lf%s ",distancias[cont-1],var);
      numBinarias++;
      numBinariaSteiner++;
      cont++;
    }
  }

  printf("***************\n\n");

  //-----------------------------------------------------------------------------

    //criação das linhas (restrições)
    numRestricoes = numPontos+numSteiner;
    glp_add_rows(lpst, numRestricoes);

    //linhas referentes as restrições de pontos fixos
    for (i = 1; i <= numPontos; i++) {
      strcpy(var,"p");
      gcvt(i,10,lixo);
      strcat(var,lixo);
      glp_set_row_name(lpst, i, var);
      glp_set_row_bnds(lpst, i, GLP_FX, 1.0, 1.0);

    }
    //linhas referentes as restrições de pontos de Steiner
    for (i = 1; i <= numSteiner; i++) {
      strcpy(var,"q");
      gcvt(i+numPontos,10,lixo);
      strcat(var,lixo);
      glp_set_row_name(lpst, i+numPontos, var);
      glp_set_row_bnds(lpst, i+numPontos, GLP_FX, 3.0, 3.0);

    }
  //---------------------------------------------------------------------------

//-----------------------------------------------------------------------------
  //tamanho do vetor da matriz a seguir
  int tamVetor = ((numBinarias*2)+1);
  //printf("\n\ntamVetor é %d\n\n", tamVetor);

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
  // for( i = 0 ; i < tamVetor; i++)
  // {
  //    printf("ia[%d] = %d\n",i, ia[i]);
  // }
  // printf("\n");

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

  //terceira regra
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
          pos=pos+1;

        }
        if(i>j){
          pi=j+numPontos;pj=i+numPontos;
          //aqui pega a coluna desejada a ser inserida no vetor ja
          col=buscacol(pi,pj,vc);
          ja[pos]=col;
          pos=pos+1;
        }
      }
    }
    pos=pos+numPontos;
  }

  //impressão para testes
  // printf("\n");
  // for( i = 0 ; i < tamVetor; i++)
  // {
  //    printf("ja[%d] = %d\n",i, ja[i]);
  // }

  //vetor ar que representa o valor dos itens da matriz delimitados acima inserindo valores
  double ar[tamVetor];
  for (i = 0; i < tamVetor; i++) {
    if (i==0){
      ar[i]=i;
    }else{
      ar[i]=1;
    }
  }

try{

  /**********/
  glp_iocp parm;
  glp_init_iocp(&parm);
  parm.mir_cuts=GLP_ON;
  parm.gmi_cuts=GLP_ON;
  parm.cov_cuts=GLP_ON;
  parm.clq_cuts=GLP_ON;
  parm.presolve=GLP_ON;

  cout << "passou0" << endl;
  /**********/
  glp_write_lp(lpst,NULL,"teste-steiner.lp");
  glp_load_matrix(lpst, tamVetor-1, ia, ja, ar);
  glp_simplex(lpst,NULL);
  //glp_intopt(lpst,NULL);
  glp_intopt(lpst,&parm);

   cout << "passou1" << endl;

  //tamanho da árvore
  Z = glp_mip_obj_val(lpst);

  cout << "passou2" << endl;

  //imprime o tamanho da árvore e os valores das variáveis binarias correspondentes as ligações

  for (i = 1; i <= numBinarias; i++) {
    //cout << glp_get_col_name(lpst,i) << "=" << glp_mip_col_val(lpst,i) <<"\n";
    if (glp_mip_col_val(lpst,i)==1) {
      fprintf(edges, "%s\n",glp_get_col_name(lpst,i));
    }
  }
  fclose(edges);
  printf( "\nZ = %g;\n", Z);

  printf("\n\n");

  //finaliza o problema
  glp_delete_prob(lpst);
}
//catch(...){
catch(exception& e){

	printf("\nErro!!!\n\n");
	cout << "erro=" << e.what() << endl;
}
  //system("gnuplot plota.gnu");

  return Z;
}

#endif
