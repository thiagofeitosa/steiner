#include "vetorja.h"


int buscacol(int xi, int xj, const vector<arestasteiner>& vc){
    for(int i=0;i<vc.size();i++){
       if(vc[i].geti()==xi && vc[i].getj()==xj){
           return vc[i].getcol();
       }
   }
   return 0;
}
