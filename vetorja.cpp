#include <iostream>
#include <vector>

using namespace std;

class arestasteiner{
  int i,j, col;
  public:
  arestasteiner(int a, int b, int c){
       i=a; j=b, col=c;
  }
  int geti(){ return i;}
  int getj(){ return j;}
  int getcol(){ return col;}
  void mostra(){
      cout <<"\nx"<<i<<"i"<<j<<" na coluna="<<col;
  }

}; 

vector<arestasteiner> vc;
int buscacol(int xi, int xj, vector<arestasteiner> vc);

main(){
   


   int numPontos=5; //vc nao precisa disso
   int numSteiner=numPontos-2; //vc nao precisa disso

   //botei +1 para não comecar de zero - vc nao precisa disso
   //int mat[numPontos+numSteiner+1][(numPontos*numSteiner)+(numSteiner*(numSteiner-1)/2)+1];
   int l=(numPontos+numSteiner)+1;
   int c=((numPontos*numSteiner)+(numSteiner*(numSteiner-1)/2))+1;
   int** mat = new int*[l];
   for(int i=0;i<l;i++){
       mat[i]=new int[c];
   }

   int col=1;
   for(int i=1;i<=numSteiner-1;i++){
     for(int j=i+1;j<=numSteiner;j++){
        arestasteiner as(i+numPontos,j+numPontos,col+(numPontos*numSteiner));
        vc.push_back(as);
        col++;
     }
   }
   for(int i=0;i<vc.size();i++){
       vc[i].mostra();
   }
   
   /*
   int xi=6, xj=8;
   int coluna=buscacol(xi,xj, vc);

   cout << "\ncoluna de x"<<xi<<"i"<<xj<<"="<<coluna;
   */

   //zera toda a matriz - vc nao precissa disso
   for(int i=1;i<=numPontos+numSteiner;i++){
     for(int j=1;j<=(numPontos*numSteiner)+(numSteiner*(numSteiner-1)/2);j++){
         mat[i][j]=0;
     }
     cout <<"\n\n";
   }  

   //pega a coluna correta na matriz a ser inserida no vetor ja
   //coloca 1 na posicao correta da matriz
   int pi,pj;
   for(int i=1;i<=numSteiner;i++){
     for(int j=1;j<=numSteiner;j++){
         if(i!=j){
             if(i<j){
                 pi=i+numPontos;pj=j+numPontos;
                 //aqui pega a coluna desejada a ser inserida no vetor ja
                 col=buscacol(pi,pj,vc); 
                 mat[pi][col]=1;// vc nao precisa dessa linha
                cout << "\ncoluna de x"<<pi<<"i"<<pj<<"="<<col;
             }
             if(i>j){
                 pi=j+numPontos;pj=i+numPontos;
                 //aqui pega a coluna desejada a ser inserida no vetor ja
                 col=buscacol(pi,pj,vc);
                 mat[pj][col]=1; // vc nao precisa dessa linha
                 cout << "\ncoluna de x"<<pj<<"i"<<pi<<"="<<col;
             }
         }
     }
   }

   //mostra se a matriz foi preenchida corretamente - vc nao precisa disso
   cout <<"\n\n";
   for(int i=1;i<=numPontos+numSteiner;i++){
     for(int j=1;j<=(numPontos*numSteiner)+(numSteiner*(numSteiner-1)/2);j++){
        cout << " mat["<<i<<"]["<<j<<"]="<<mat[i][j];
     }
     cout <<"\n\n";
   }            

} //fim do main


//essa funcao busca a coluna referente a variavel Xxixj, devendo xi < xj
int buscacol(int xi, int xj, vector<arestasteiner> vc){
    for(int i=0;i<vc.size();i++){
       if(vc[i].geti()==xi && vc[i].getj()==xj){
           return vc[i].getcol();
       }
   }
   return 0;
}
