#ifndef _H_VETORJA
#define _H_VETORJA

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

};

//essa funcao busca a coluna referente a variavel Xxixj, devendo xi < xj
int buscacol(int xi, int xj, vector<arestasteiner> vc){
    for(int i=0;i<vc.size();i++){
       if(vc[i].geti()==xi && vc[i].getj()==xj){
           return vc[i].getcol();
       }
   }
   return 0;
}

#endif
