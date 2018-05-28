#include <stdio.h>
#include <stdlib.h>
#include <time.h>

FILE *entradafixo, *entradasteiner;

int main()
{
srand( time(0) );
entradafixo = fopen("fixos.txt","w+");
entradasteiner = fopen("steiner.txt","w+");

int i, n=0, m;

printf("digite a quantidade de Pontos Fixos\n");
scanf("%d", &n);
m=n*3;

fprintf(entradafixo, "%d 2\n", n);
for (i = 0; i < n; i++)
  {
    fprintf(entradafixo, "%d %d\n", rand() % m, rand() % m);
  }

for (i = 2; i < n; i++)
  {
    fprintf(entradasteiner, "%d %d\n", 1+rand()%m-1, 1+rand()%m-1);
  }

fclose(entradafixo);
fclose(entradasteiner);
     return 0;
}
