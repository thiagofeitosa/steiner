#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "steiner.h"


/* Control Parameters of ABC algorithm*/
#define NP 50 /* The number of colony size (employed bees+onlooker bees)*/
#define FoodNumber NP/3 /*The number of food sources equals the half of the colony size*/
int limit=300;  /*A food source which could not be improved through "limit" trials is abandoned by its employed bee*/
#define maxCycle 8000 /*The number of cycles for foraging {a stopping criteria}*/

/* Problem specific variables*/
#define D 500 /*number of parameters of the problem to be optimized*/
#define lb 0 /*lower bound of the parameters. */
#define ub 20 /*upper bound of the parameters. lb and ub can be defined as arrays for
the problems of which parameters have different bounds*/

double Vlb[D]; /*Array Vlb that is the same use than lb variable above*/
double Vub[D]; /*Array Vub that is the same use than ub variable above*/

#define runtime 1  /*Algorithm can be run many times in order to see its robustness*/


double Foods[FoodNumber][D]; /*Foods is the population of food sources. Each row of Foods matrix is a vector holding D parameters to be optimized. The number of rows of Foods matrix equals to the FoodNumber*/
double f[FoodNumber];  /*f is a vector holding objective function values associated with food sources */
double fitness[FoodNumber]; /*fitness is a vector holding fitness (quality) values associated with food sources*/
double trial[FoodNumber]; /*trial is a vector holding trial numbers through which solutions can not be improved*/
double prob[FoodNumber]; /*prob is a vector holding probabilities of food sources (solutions) to be chosen*/
double solution [D]; /*New solution (neighbour) produced by v_{ij}=x_{ij}+\phi_{ij}*(x_{kj}-x_{ij}) j is a randomly chosen parameter and k is a randomlu chosen solution different from i*/
double ObjValSol; /*Objective function value of new solution*/
double FitnessSol; /*Fitness value of new solution*/
int neighbour, param2change; /*param2change corrresponds to j, neighbour corresponds to k in equation v_{ij}=x_{ij}+\phi_{ij}*(x_{kj}-x_{ij})*/
double GlobalMin; /*Optimum solution obtained by ABC algorithm*/
double GlobalParams[D]; /*Parameters of the optimum solution*/
double GlobalMins[runtime]; /*GlobalMins holds the GlobalMin of each run in multiple runs*/
double r; /*a random number in the range [0,1)*/

static int TAM; //number of variables considered in the problem. Must to be lower or equal D - Initialized in main function
double bestvalsol; //value of the best solution found
double bestsolution[D]; //vector holding the best solution found
int bestfound=0; //indicate if there is no best solution found (0) or if there is a best solution found (1)
double bestfood[D]; //vector holding the best food found

void savebest(double sol, double vetsol[D], int minormax);
void showbest(int mostrasol);

long int numsemmelhoras=0;

/*variables that store initial and final CPU time*/
clock_t start_time, mid, end_time;
double cpu_time_used;

/*a function pointer returning double and taking a D-dimensional array as argument */
/*If your function takes additional arguments then change function pointer definition and lines calling "...=function(solution);" in the code*/
//typedef double (*FunctionCallback)(double sol[D]);
typedef double (*FunctionCallback)(double sol[D], int gravaformulacao);

/*benchmark functions */
double sphere(double sol[D]);
double Rosenbrock(double sol[D]);
double Griewank(double sol[D]);
double Rastrigin(double sol[D]);

/*Write your own objective function name instead of sphere*/
FunctionCallback function = &steiner;

/*Initialize arrays Vlb and Vub with fixed values*/
void InitVlbub(double initlb, double initub)
{
	for(int i=0;i<TAM;i++){
		Vlb[i]=initlb;
		Vub[i]=initub;
	}
}

/*Initialize arrays Vlb and Vub with values of arryas*/
void InitVlbub2(double *initlb, double *initub)
{
	for(int i=0;i<TAM;i++){
		Vlb[i]=initlb[i];
		Vub[i]=initub[i];
	}
}

/*Fitness function*/
double CalculateFitness(double fun)
 {
	 double result=0;
	 if(fun>=0)
	 {
		 result=1/(fun+1);
	 }
	 else
	 {
		 result=1+fabs(fun);
	 }
	 return result;
 }

/*The best food source is memorized*/
void MemorizeBestSource()
{
   int i,j;

	for(i=0;i<FoodNumber;i++)
	{
	if (f[i]<GlobalMin)
		{
        GlobalMin=f[i];
        //for(j=0;j<D;j++)
        for(j=0;j<TAM;j++)
           GlobalParams[j]=Foods[i][j];
        }
	}
 }

 /*Variables are initialized in the range [lb,ub]. If each parameter has different range, use arrays lb[j], ub[j] instead of lb and ub */
 /* Counters of food sources are also initialized in this function*/
 void init2(int index, int restart)  //<<==
 {
    int j;
    //for (j=0;j<D;j++)
    for (j=0;j<TAM;j++)
 		{
         r = (   (double)rand() / ((double)(RAND_MAX)+(double)(1)) );
         //Foods[index][j]=r*(ub-lb)+lb;
 				if(restart==1 && j==0){
 				  solution[j]=bestsolution[j];
 					Foods[index][j]=solution[j];
 				}else{
					 double muda = (rand() % 11)/10.0;
					 //printf("\n\nmuda=%lf\n\n",muda);
					 if(muda>=0.7){
             Foods[index][j]=r*(Vub[j]-Vlb[j])+Vlb[j];
 		         solution[j]=Foods[index][j];
					 }
					 else{
						 solution[j]=bestsolution[j];
	  				 Foods[index][j]=solution[j];
					 }
 				}
 		}
 	f[index]=function(solution, 0);
 	fitness[index]=CalculateFitness(f[index]);
 	trial[index]=0;
 }

/*Variables are initialized in the range [lb,ub]. If each parameter has different range, use arrays lb[j], ub[j] instead of lb and ub */
/* Counters of food sources are also initialized in this function*/
void init(int index, int restart)  //<<==
{
   int j;
   //for (j=0;j<D;j++)
   for (j=0;j<TAM;j++)
		{
        r = (   (double)rand() / ((double)(RAND_MAX)+(double)(1)) );
        //Foods[index][j]=r*(ub-lb)+lb;
				if(restart==1 && j==0){
				  solution[j]=bestsolution[j];
					Foods[index][j]=solution[j];
				}else{
          Foods[index][j]=r*(Vub[j]-Vlb[j])+Vlb[j];
		      solution[j]=Foods[index][j];
				}
		}
	f[index]=function(solution, 0);
	fitness[index]=CalculateFitness(f[index]);
	trial[index]=0;
}

/*All food sources are initialized */
void initial(int restart)
{
	int i;
	for(i=0;i<FoodNumber;i++)
	{
		  if(restart==0){
	      init(i, restart);
			}
			else{
				init2(i, restart);
			}
	}
	GlobalMin=f[0];
    //for(i=0;i<D;i++)
    for(i=0;i<TAM;i++)
    GlobalParams[i]=Foods[0][i];
}

void SendEmployedBees()  //<<==
{
  int i,j;
  /*Employed Bee Phase*/
   for (i=0;i<FoodNumber;i++)
        {
        /*The parameter to be changed is determined randomly*/
        r = ((double)rand() / ((double)(RAND_MAX)+(double)(1)) );
        //param2change=(int)(r*D);
        param2change=(int)(r*TAM);

        /*A randomly chosen solution is used in producing a mutant solution of the solution i*/
        r = (   (double)rand() / ((double)(RAND_MAX)+(double)(1)) );
        neighbour=(int)(r*FoodNumber);

        /*Randomly selected solution must be different from the solution i*/
        while(neighbour==i)
        {
        r = (   (double)rand() / ((double)(RAND_MAX)+(double)(1)) );
        neighbour=(int)(r*FoodNumber);
        }
        //for(j=0;j<D;j++)
        for(j=0;j<TAM;j++)
        solution[j]=Foods[i][j];

        /*v_{ij}=x_{ij}+\phi_{ij}*(x_{kj}-x_{ij}) */
        r = (   (double)rand() / ((double)(RAND_MAX)+(double)(1)) );
        solution[param2change]=Foods[i][param2change]+(Foods[i][param2change]-Foods[neighbour][param2change])*(r-0.5)*2;

        /*if generated parameter value is out of boundaries, it is shifted onto the boundaries*/
        /*
        if (solution[param2change]<lb)
           solution[param2change]=lb;
        if (solution[param2change]>ub)
           solution[param2change]=ub;
        */

        if (solution[param2change]<Vlb[param2change])
           solution[param2change]=Vlb[param2change];
        if (solution[param2change]>Vub[param2change])
           solution[param2change]=Vub[param2change];
        ObjValSol=function(solution, 0);                //<<<===
        savebest(ObjValSol, solution, -1); //save bestsolution found (-1 -> minimization and 1 -> maximization)
        FitnessSol=CalculateFitness(ObjValSol);

        /*a greedy selection is applied between the current solution i and its mutant*/
        if (FitnessSol>fitness[i])
        {
        /*If the mutant solution is better than the current solution i, replace the solution with the mutant and reset the trial counter of solution i*/
        trial[i]=0;
        //for(j=0;j<D;j++)
        for(j=0;j<TAM;j++)
        Foods[i][j]=solution[j];
        f[i]=ObjValSol;
        fitness[i]=FitnessSol;
        }
        else
        {   /*if the solution i can not be improved, increase its trial counter*/
            trial[i]=trial[i]+1;
        }


        }

        /*end of employed bee phase*/

}

/* A food source is chosen with the probability which is proportioal to its quality*/
/*Different schemes can be used to calculate the probability values*/
/*For example prob(i)=fitness(i)/sum(fitness)*/
/*or in a way used in the metot below prob(i)=a*fitness(i)/max(fitness)+b*/
/*probability values are calculated by using fitness values and normalized by dividing maximum fitness value*/
void CalculateProbabilities()
{
     int i;
     double maxfit;
     maxfit=fitness[0];
  for (i=1;i<FoodNumber;i++)
        {
           if (fitness[i]>maxfit)
           maxfit=fitness[i];
        }

 for (i=0;i<FoodNumber;i++)
        {
         prob[i]=(0.9*(fitness[i]/maxfit))+0.1;
        }

}

void SendOnlookerBees()  //<<==
{

  int i,j,t;
  i=0;
  t=0;
  /*onlooker Bee Phase*/
  while(t<FoodNumber)
        {

        r = (   (double)rand() / ((double)(RAND_MAX)+(double)(1)) );
        if(r<prob[i]) /*choose a food source depending on its probability to be chosen*/
        {
        t++;

        /*The parameter to be changed is determined randomly*/
        r = ((double)rand() / ((double)(RAND_MAX)+(double)(1)) );
        //param2change=(int)(r*D);
        param2change=(int)(r*TAM);

        /*A randomly chosen solution is used in producing a mutant solution of the solution i*/
        r = (   (double)rand() / ((double)(RAND_MAX)+(double)(1)) );
        neighbour=(int)(r*FoodNumber);

        /*Randomly selected solution must be different from the solution i*/
        while(neighbour==i)
        {
        r = (   (double)rand() / ((double)(RAND_MAX)+(double)(1)) );
        neighbour=(int)(r*FoodNumber);
        }
        //for(j=0;j<D;j++)
        for(j=0;j<TAM;j++)
        solution[j]=Foods[i][j];

        /*v_{ij}=x_{ij}+\phi_{ij}*(x_{kj}-x_{ij}) */
        r = (   (double)rand() / ((double)(RAND_MAX)+(double)(1)) );
        solution[param2change]=Foods[i][param2change]+(Foods[i][param2change]-Foods[neighbour][param2change])*(r-0.5)*2;

        /*if generated parameter value is out of boundaries, it is shifted onto the boundaries*/
        if (solution[param2change]<lb)
           solution[param2change]=lb;
        if (solution[param2change]>ub)
           solution[param2change]=ub;
        ObjValSol=function(solution, 0);                        //<<<===
        savebest(ObjValSol, solution, -1); //save bestsolution found (-1 -> minimization and 1 -> maximization)
        FitnessSol=CalculateFitness(ObjValSol);

        /*a greedy selection is applied between the current solution i and its mutant*/
        if (FitnessSol>fitness[i])
        {
        /*If the mutant solution is better than the current solution i, replace the solution with the mutant and reset the trial counter of solution i*/
        trial[i]=0;
        //for(j=0;j<D;j++)
        for(j=0;j<TAM;j++)
        Foods[i][j]=solution[j];
        f[i]=ObjValSol;
        fitness[i]=FitnessSol;
        }
        else
        {   /*if the solution i can not be improved, increase its trial counter*/
            trial[i]=trial[i]+1;
        }
        } /*if */
        i++;
        if (i==FoodNumber-1)
        i=0;
        }/*while*/

        /*end of onlooker bee phase     */
}

/*determine the food sources whose trial counter exceeds the "limit" value. In Basic ABC, only one scout is allowed to occur in each cycle*/
void SendScoutBees()
{
int maxtrialindex,i;
maxtrialindex=0;
for (i=1;i<FoodNumber;i++)
        {
         if (trial[i]>trial[maxtrialindex])
         maxtrialindex=i;
        }
if(trial[maxtrialindex]>=limit)
{
	init(maxtrialindex,0);
}
}

//save the best solution found
void savebest(double sol, double vetsol[D], int minormax)
{
   if(bestfound==0){
      bestvalsol=sol;

      for(int i=0;i<TAM;i++){
        bestsolution[i]=vetsol[i];
      }
      bestfound=1;

			numsemmelhoras=0;
   }

   /*
   if(sol < bestvalsol){
   	  bestvalsol=sol;

      for(int i=0;i<TAM;i++){
        bestsolution[i]=vetsol[i];
      }
   }
   */

   //minormax determines if the problem is of minimization (-1) or maximization (1)

   if((minormax==-1 && sol < bestvalsol) || (minormax==1 && sol > bestvalsol)){
   	  bestvalsol=sol;

      for(int i=0;i<TAM;i++){
        bestsolution[i]=vetsol[i];
      }

		  numsemmelhoras=0;
   }

}

//show the best solution found
void showbest(int mostrasol)
{
	printf("\n\nValue of the Best Solution Found = %lf",bestvalsol);
	if(mostrasol==1){
	  printf("\nSolution Found\n");
	  for(int i=0;i<TAM;i++){
		printf("\nParameter %i = %lf",i+1,bestsolution[i]);
      }
      printf("\n\n");
      bestvalsol=function(bestsolution, 1); //grava a formulacao da melhor solucao
	}

	printf("\n\n");
}

/*Main program of the ABC algorithm*/
int main()
{
int iter,run,j;
double mean;

load(); //le os dados dos pontos fixos e inicializa as principais variaveis do programa

//Initialization of the main variable that determine the size of the soluction vector
TAM = numSteiner*numDim;

//inicializa as variaveis coordSteiner, vetLB e vetUB - deve ser executado sempre ap�s o procedimento load()
initvariables();

loadvetLBUB(&vetLB,&vetUB); //carrega LB e UB de cada dimens�o

//initialize lb and ub with fixed values - first value to lb and second value to ub
InitVlbub2(vetLB, vetUB);

mean=0;
srand(time(NULL));

for(run=0;run<runtime;run++)
{

//start time measure
start_time = clock();

initial(0);
MemorizeBestSource();
for (iter=1;iter<=maxCycle;iter++)
{
    SendEmployedBees();
    CalculateProbabilities();
    SendOnlookerBees();
    MemorizeBestSource();
    SendScoutBees();
    if(iter%10==0){
    	printf("\nIter %i",iter);
    	showbest(0);
	  }

		if(numsemmelhoras==0){
			limit=300;
		}

    numsemmelhoras++;

		//printf("\n\nIter=%i e numsemmelhoras=%i\n\n",iter,numsemmelhoras);

     ///**********************
    //re-start method after 50 iterations without improvement
		if(numsemmelhoras%30==0){
			limit=limit+100;
			printf("\n\nIter=%i, numsemmelhoras=%i e limit=%i\n\n",iter,numsemmelhoras,limit);
			//printf("\n\nRe-Start of Solution\n\n");
			//initial(1);
			//MemorizeBestSource();
		}
    //*********************/
		if(numsemmelhoras%299==0){
			 break;  //100 iterations without improvement - stop
		}
}

//finalize time measure
end_time = clock();
//calculate execution time in seconds
cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;

//printf("\nExecution time=%.20lf\n\n",cpu_time_used);
//for(j=0;j<D;j++)
for(j=0;j<TAM;j++)
		{
			printf("GlobalParam[%d]: %f\n",j+1,GlobalParams[j]);
		}
printf("%d. run: %e \n",run+1,GlobalMin);
GlobalMins[run]=GlobalMin;
mean=mean+GlobalMin;
}
mean=mean/runtime;
printf("Means of %d runs: %e\n",runtime,mean);

showbest(1);

printf("\n\nExecution time in seconds=%.20lf\n\n",cpu_time_used);

//printf("\n\nPress <ENTER> To Continue");
//getchar();

//system("gnuplot plota.gnu");
}


double sphere(double sol[D])
{
int j;
double top=0;
//for(j=0;j<D;j++)
for(j=0;j<TAM;j++)
{
top=top+sol[j]*sol[j];
}
return top;
}

double Rosenbrock(double sol[D])
{
int j;
double top=0;
//for(j=0;j<D-1;j++)
for(j=0;j<TAM-1;j++)
{
top=top+100*pow((sol[j+1]-pow((sol[j]),(double)2)),(double)2)+pow((sol[j]-1),(double)2);
}
return top;
}

 double Griewank(double sol[D])
 {
	 int j;
	 double top1,top2,top;
	 top=0;
	 top1=0;
	 top2=1;
	 //for(j=0;j<D;j++)
	 for(j=0;j<TAM;j++)
	 {
		 top1=top1+pow((sol[j]),(double)2);
		 top2=top2*cos((((sol[j])/sqrt((double)(j+1)))*M_PI)/180);

	 }
	 top=(1/(double)4000)*top1-top2+1;
	 return top;
 }

 double Rastrigin(double sol[D])
 {
	 int j;
	 double top=0;

	 //for(j=0;j<D;j++)
	 for(j=0;j<TAM;j++)
	 {
		 top=top+(pow(sol[j],(double)2)-10*cos(2*M_PI*sol[j])+10);
	 }
	 return top;
 }
