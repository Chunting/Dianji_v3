#include <ilcplex/ilocplex.h>
#include <fstream>
#include <math.h>
#include <ilconcert/ilomodel.h>
#include <string>
//#include "Main.h"
#include "Region_1.h"
#include "Region_2.h"
#include "Region_3.h"
#include "Region_4.h"
int main(int argc, char *argv[])
{
	IloEnv env;
	float lamda1[5];
	float lamda2[4];
	float lamda3[7];
	float lamda4[6];
	float Pipe1[5][25];
	float Pipe2[4][25];
	float Pipe3[7][25];
	float Pipe4[6][25];
	for(int i=0; i<5; ++i) {
		lamda1[i] = 25;
	}
	for(int i=0; i<4; ++i) {
		lamda2[i] = 25;
	}
	for(int i=0; i<7; ++i) {
		lamda3[i] = 20;
	}
	for(int i=0; i<6; ++i) {
		lamda4[i] = 20;
	}
	for(int t=0; t<25; ++t) {
		for(int i=0; i<5; ++i) {
			Pipe1[i][t] = 1200;
		}
		for(int i=0; i<4; ++i) {
			Pipe2[i][t] = 1200;
		}
		for(int i=0; i<7; ++i) {
			Pipe3[i][t] = 0;
		}
		for(int i=0; i<6; ++i) {
			Pipe4[i][t] = 0;
		}

	}
	float beta = 0.382;
	for(int it=1; it<10; ++it) {
		env.out()<<"==========================  "<<it<<"  =========================="<< endl;
		Run_Output_1(env, it, lamda1, Pipe1);
		for(int t=1; t<25; ++t) {
			swap(Pipe1[0][t], Pipe4[3][t]);
			swap(Pipe1[1][t], Pipe4[2][t]);
			swap(Pipe1[2][t], Pipe3[3][t]);
			swap(Pipe1[3][t], Pipe3[0][t]);
			swap(Pipe1[4][t], Pipe3[1][t]);
		}

		Run_Output_2(env, it, lamda2, Pipe2);
		for(int t=1; t<25; ++t) {
			swap(Pipe2[0][t], Pipe4[1][t]);
			swap(Pipe2[1][t], Pipe4[4][t]);
			swap(Pipe2[2][t], Pipe3[4][t]);
			swap(Pipe2[3][t], Pipe3[2][t]);
		}

		Run_Output_3(env, it, lamda3, Pipe3);
		for(int t=1; t<25; ++t) {
			swap(Pipe1[2][t], Pipe3[3][t]);
			swap(Pipe1[3][t], Pipe3[0][t]);
			swap(Pipe1[4][t], Pipe3[1][t]);;
			swap(Pipe2[2][t], Pipe3[4][t]);
			swap(Pipe2[3][t], Pipe3[2][t]);
			swap(Pipe3[5][t], Pipe4[0][t]);
			swap(Pipe3[6][t], Pipe4[5][t]);	
		}

		Run_Output_4(env, it, lamda4, Pipe4);
		for(int t=1; t<25; ++t) {
			swap(Pipe1[0][t], Pipe4[3][t]);
			swap(Pipe1[1][t], Pipe4[2][t]);
			swap(Pipe2[0][t], Pipe4[1][t]);
			swap(Pipe2[1][t], Pipe4[4][t]);
			swap(Pipe3[5][t], Pipe4[0][t]);
			swap(Pipe3[6][t], Pipe4[5][t]);	
		}

		/*	for(int i=0; i<6; ++i) {
		for(int t=1; t<25; ++t) {
		env.out()<< Pipe4[i][t] << "\t";
		}
		env.out()<< endl<<"++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<< endl;
		}

		for(int t=1; t<25; ++t) {
		swap(Pipe1[0][t], Pipe4[3][t]);
		swap(Pipe1[1][t], Pipe4[2][t]);
		swap(Pipe1[2][t], Pipe3[3][t]);
		swap(Pipe1[3][t], Pipe3[0][t]);
		swap(Pipe1[4][t], Pipe3[1][t]);
		swap(Pipe2[0][t], Pipe4[1][t]);
		swap(Pipe2[1][t], Pipe4[4][t]);
		swap(Pipe2[2][t], Pipe3[4][t]);
		swap(Pipe2[3][t], Pipe3[2][t]);
		swap(Pipe3[5][t], Pipe4[0][t]);
		swap(Pipe3[6][t], Pipe4[5][t]);	
		}
		*/



		lamda1[0] = beta*lamda1[0] + (1-beta)* lamda4[2];
		lamda1[1] = beta*lamda1[1] + (1-beta)* lamda4[1];
		lamda1[2] = beta*lamda1[2] + (1-beta)* lamda3[3];
		lamda1[3] = beta*lamda1[3] + (1-beta)* lamda3[0];
		lamda1[4] = beta*lamda1[4] + (1-beta)* lamda3[1];
		lamda2[0] = beta*lamda2[0] + (1-beta)* lamda4[0];
		lamda2[1] = beta*lamda2[1] + (1-beta)* lamda4[4];
		lamda2[2] = beta*lamda2[2] + (1-beta)* lamda3[4];
		lamda2[3] = beta*lamda2[3] + (1-beta)* lamda3[2];
		lamda3[0] = beta*lamda3[0] + (1-beta)* lamda1[3];
		lamda3[1] = beta*lamda3[1] + (1-beta)* lamda1[4];
		lamda3[2] = beta*lamda3[2] + (1-beta)* lamda2[3];	
		lamda3[3] = beta*lamda3[3] + (1-beta)* lamda1[2];
		lamda3[4] = beta*lamda3[4] + (1-beta)* lamda2[2];
		lamda3[5] = beta*lamda3[5] + (1-beta)* lamda4[0];
		lamda3[6] = beta*lamda3[6] + (1-beta)* lamda4[5];
		lamda4[0] = beta*lamda4[0] + (1-beta)* lamda3[5];
		lamda4[1] = beta*lamda4[1] + (1-beta)* lamda2[0];
		lamda4[2] = beta*lamda4[2] + (1-beta)* lamda1[1];
		lamda4[3] = beta*lamda4[3] + (1-beta)* lamda1[0];
		lamda4[4] = beta*lamda4[4] + (1-beta)* lamda2[1];	
		lamda4[5] = beta*lamda4[5] + (1-beta)* lamda3[6];
	}

	env.out()<<"===================================================="<< endl;
	env.out()<<lamda1[0] -  lamda4[2]<<endl;
	env.out()<<lamda1[1] -  lamda4[1]<<endl;
	env.out()<<lamda1[2] -  lamda3[3]<<endl;
	env.out()<<lamda1[3] -  lamda3[0]<<endl;
	env.out()<<lamda1[4] -  lamda3[1]<<endl;
	env.out()<<lamda2[0] -  lamda4[0]<<endl;
	env.out()<<lamda2[1] -  lamda4[4]<<endl;
	env.out()<<lamda2[2] -  lamda3[4]<<endl;
	env.out()<<lamda2[3] -  lamda3[2]<<endl;
	env.out()<<lamda3[0] -  lamda1[3]<<endl;
	env.out()<<lamda3[1] -  lamda1[4]<<endl;
	env.out()<<lamda3[2] -  lamda2[3]<<endl;	
	env.out()<<lamda3[3] -  lamda1[2]<<endl;
	env.out()<<lamda3[4] -  lamda2[2]<<endl;
	env.out()<<lamda3[5] -  lamda4[0]<<endl;
	env.out()<<lamda3[6] -  lamda4[5]<<endl;
	env.out()<<lamda4[0] -  lamda3[5]<<endl;
	env.out()<<lamda4[1] -  lamda2[0]<<endl;
	env.out()<<lamda4[2] -  lamda1[1]<<endl;
	env.out()<<lamda4[3] -  lamda1[0]<<endl;
	env.out()<<lamda4[4] -  lamda2[1]<<endl;	
	env.out()<<lamda4[5] -  lamda3[6]<<endl;	
	env.out()<< endl<<"+++++++++++++++++++++++++++++++++++++++++++"<< endl;
	env.end();
	return 0;

}

void swap(int& a, int& b)
{
	a = a ^ b;
	b = a ^ b;
	a = a ^ b;
}