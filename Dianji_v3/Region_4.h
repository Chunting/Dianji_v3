#pragma once
#include <ilcplex/ilocplex.h>
#include <fstream>
#include <ilconcert/ilomodel.h>
#include"Thermal_Output.h"

int Run_Output_4(IloEnv env, int it,float lamda[],float Pipe[][25])
{
	env.out() << "--------------------------- Region 4 ------------------------ " << endl;
	//**********定义全局变量************
	IloInt cycle;			
	IloInt thUnitNum;  
	IloInt windUnitNum;
	IloInt outputNum;

	//网络数据
	IloInt demandNum;
	IloInt lineNum;
	IloInt busNum;
	const float outputprice=5;
	const float rp=3;
	const float nrp=3;
	const float r=0.3;
	const float _INF=1E-7;//add by hx
	 //#define声明以及初始化符号常量；此外还可用关键字const unsigned float OUTFILEDATA="check.dat"声明
	
	const char* lp = "./Bus31_4/hydrothermal.lp" ;    
	const char* SYSTEMDATA= "./Bus31_4/SystemData.dat" ;  //cycle J horizon flk tlk
	const char* APPDATA ="./Bus31_4/AppData.dat";     //I,K,LAMDA,PR DISK
	const char* THUNITDATA= "./Bus31_4/thunitdata.dat";
	const char* WINDDATA="./Bus31_4/windpower.dat";
	const char* NETDATA= "./Bus31_4/netdata.dat";
	const char* GAMADATA ="./Bus31_4/gamadata.dat";

	string _output = "./Bus31_4/Result_Output_"+int2str(it)+".dat";
	string _thunit = "./Bus31_4/Result_ThUnit_"+int2str(it)+".dat";
	string _result = "./Bus31_4/Result_"+int2str(it)+".dat";
	string _check = "./Bus31_4/Check_"+int2str(it)+".dat";
	const char* OUTFILEDATA = _check.c_str() ; 
	const char* Result_Output = _output.c_str();
	const char* OUTFILERESULT = _result.c_str();
	const char* Resutl_ThUnit = _thunit.c_str();
	const char* Result_Line="./Bus31_4/Result_Line.dat";


	//IloEnv env;
	int t,i,j,k,l,d,w,s;
	try
	{
		IloModel model(env);
		IloTimer timer(env);
		timer.start();
		readSystemData(SYSTEMDATA,
			env,
			cycle,			
			thUnitNum,		
			windUnitNum,
			demandNum,		
			outputNum,
			lineNum,			
			busNum
			);
		ifstream fin(APPDATA,ios::in);
		if(!fin) 
			env.out()<<"problem with file:"<<APPDATA<<endl;
		IloNumArray sysDemand(env,cycle+1);
		IloNumArray sysReserve(env,cycle+1);
		Matrix2 outputMaxPower(env,outputNum);
		Matrix2 outputMinPower(env,outputNum);
		Matrix2 windPower(env,windUnitNum);

		for (w=0;w<windUnitNum;++w)
		{
			windPower[w]=IloNumArray(env,cycle+1);
		}
		for (s=0;s<outputNum;++s)
		{
			outputMaxPower[s]=IloNumArray(env,cycle+1);
		}
		for (s=0;s<outputNum;++s)
		{
			outputMinPower[s]=IloNumArray(env,cycle+1);
		}
		for(t=1;t<cycle+1;t++)
			fin>>sysDemand[t];
		for(t=1;t<cycle+1;t++)
			fin>>sysReserve[t];
		for(w=0;w<windUnitNum;++w)
		{
			for(t=1;t<cycle+1;++t)
				fin>>windPower[w][t];
		}
		for(s=0;s<outputNum;++s){
			for(t=1;t<cycle+1;++t)
				fin>>outputMaxPower[s][t];
			//outputMaxPower[s][t] = Pipe[s][t];
		}

		for(s=0;s<outputNum;++s){
			for(t=1;t<cycle+1;++t)
				fin>>outputMinPower[s][t];
				//outputMinPower[s][t] = Pipe[s][t];
		}

		IloNumArray unitLocation(env,thUnitNum);
		IloNumArray windLocation(env,windUnitNum);
		IloNumArray demandLocation(env,demandNum);
		IloNumArray outputLocation(env,outputNum);
		//	IloNumArray demand(env,demandNum);
		Matrix2 demand(env,demandNum);//二维数组
		for(i=0;i<demandNum;i++)
			demand[i]= IloNumArray(env,cycle+1);

		IloNumArray lineCap(env,lineNum);

		readNetData(NETDATA,env, 
			cycle,
			thUnitNum,		
			windUnitNum,      
			demandNum,		
			outputNum, 
			lineNum,	
			unitLocation,              
			windLocation,
			demandLocation,            
			outputLocation,
			demand,                    
			lineCap
			);


		//************* define the thermal unit variable **************************
		IloNumArray thminPower(env,thUnitNum);                                    
		IloNumArray thmaxPower(env,thUnitNum);                                    
		IloNumArray thminFuelCost(env,thUnitNum);
		IloNumArray thmaxFuelCost(env,thUnitNum);
		IloNumArray thminUp(env,thUnitNum);                                       
		IloNumArray thminDown(env,thUnitNum);                                     
		IloNumArray thcoldUpTime(env,thUnitNum);
		IloNumArray thfuelCostPieceNum(env,thUnitNum);
		IloNumArray thhotUpCost(env,thUnitNum);
		IloNumArray thcoldUpCost(env,thUnitNum);
		IloNumArray thdelta(env,thUnitNum);			
		IloNumArray thfirstLast(env,thUnitNum);
		IloNumArray thmaxR(env,thUnitNum);
		IloNumArray tha(env,thUnitNum);
		IloNumArray thb(env,thUnitNum);
		IloNumArray thc(env,thUnitNum);
		IloNumArray thinitState(env,thUnitNum);
		IloNumArray thinitPower(env,thUnitNum);
		//*************读火电取机组数据***************************
		readThUnitData(THUNITDATA,
			env,
			thUnitNum,	
			thminPower,                        
			thmaxPower,                        
			thminDown,                        
			thminUp,                           
			thcoldUpTime,                      //冷启动时间
			thfuelCostPieceNum,                //燃料费用曲线段数
			thhotUpCost,                       //热启动费用
			thcoldUpCost,			           //冷启动费用
			thdelta,						      //爬升
			thfirstLast,                       //首末开机约束，取0，1
			thmaxR,                          //机组最大备用	
			tha,							    	
			thb,							     		
			thc,
			thminFuelCost,
			thmaxFuelCost,
			thinitState,
			thinitPower
			);
		//***************** Read initialed wind power ******
		IloNumArray windMaxPower(env,windUnitNum);
		IloNumArray windMinPower(env,windUnitNum);
		readWindUnitData(WINDDATA,env,cycle,
			windUnitNum,
			windMaxPower,
			windMinPower);
		//******************读取火电分段线性数据*************
		Matrix2      thfuelCostPieceSlope(env,thUnitNum);
		for(i=0;i<thUnitNum;i++)
		{
			thfuelCostPieceSlope[i]=IloNumArray(env,thfuelCostPieceNum[i]);
		}

		Matrix2      thminFuelPieceCost(env,thUnitNum);
		for(i=0;i<thUnitNum;i++)
		{
			thminFuelPieceCost[i]=IloNumArray(env,thfuelCostPieceNum[i]);
		}

		Matrix2      thmaxFuelPieceCost=Matrix2(env,thUnitNum);
		for(i=0;i<thUnitNum;i++)
		{
			thmaxFuelPieceCost[i]=IloNumArray(env,thfuelCostPieceNum[i]);
		}

		Matrix2     thminPiecePower(env,thUnitNum);
		for(i=0;i<thUnitNum;i++)
		{
			thminPiecePower[i]=IloNumArray(env,thfuelCostPieceNum[i]);
		}

		Matrix2      thmaxPiecePower(env,thUnitNum);
		for(i=0;i<thUnitNum;i++)
		{
			thmaxPiecePower[i]=IloNumArray(env,thfuelCostPieceNum[i]);
		}

		pieceThUnitData(env,
			thUnitNum,
			tha,							   		
			thb,							   		
			thc,							   
			thminPower,                        
			thmaxPower,                       
			thfuelCostPieceNum,
			thminPiecePower,
			thmaxPiecePower,
			thminFuelPieceCost,
			thmaxFuelPieceCost,
			thfuelCostPieceSlope
			); 


		Matrix2      gama(env, lineNum);
		for(i=0;i<lineNum;i++)
		{
			gama[i]=IloNumArray(env, busNum);
		}
		readGama(GAMADATA, env, lineNum, busNum, gama);

		//输出
		ofstream tfile(OUTFILEDATA,ios::out);
		if(!tfile)
			cout<<"cannot open "<<OUTFILEDATA<<endl;
		tfile<<"System data"<<endl;
		tfile<<"cycle\t"<<cycle<<endl;
		tfile<<"thUnitNum\t"<<thUnitNum<<endl;
		tfile<<"lineNum\t"<<lineNum<<endl;
		tfile<<"busNum\t"<<busNum<<endl;
		tfile<<"demandNum\t"<<demandNum<<endl;

		tfile<<endl<<"Net data"<<endl;
		tfile<<endl<<"unit location"<<endl;
		for (i=0;i<thUnitNum;i++)
		{
			tfile<<unitLocation[i]<<"\t";
		}

		tfile<<endl<<"demand location"<<endl;
		for (i=0;i<demandNum;i++)
		{
			tfile<<demandLocation[i]<<"\t";
		}

		tfile<<endl<<"System Demand"<<endl;
		for(t=1;t<cycle+1;++t){
			tfile<<sysDemand[t]<<"\t";
		}
		tfile<<endl<<"System Reserve"<<endl;
		for(t=1;t<cycle+1;++t){
			tfile<<sysReserve[t]<<"\t";
		}
		tfile<<endl<<"Proportion of Demand"<<endl;
		for (i=0;i<demandNum;i++)
		{
			for(t=1;t<cycle+1;t++)
				tfile<<demand[i][t]<<"\t";
			tfile<<endl;
		}
		tfile<<endl<<"Line Capacity"<<endl;
		for (i=0;i<lineNum;i++)
		{
			tfile<<lineCap[i]<<"\t";

		}
		/*
		tfile<<endl<<"Gama"<<endl; 
		tfile<<"busNum "<<busNum<<" linNum "<<lineNum<<endl;
		for(j=0;j<lineNum;j++)
		{
		for(i=0;i<busNum;i++)
		{
		if(i%10==0) tfile<<endl;
		tfile<<gama[j][i]<<"\t";
		}
		tfile<<endl;
		}	
		*/
		tfile<<endl<<"Thermal Unit data"<<endl;
		tfile<<endl<<"thminPower"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			tfile<<thminPower[i]<<"\t";
		}
		tfile<<endl<<"thmaxPower"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			tfile<<thmaxPower[i]<<"\t";
		}
		tfile<<endl<<"thminDown"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			tfile<<thminDown[i]<<"\t";
		}
		tfile<<endl<<"thminUp"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			tfile<<thminUp[i]<<"\t";
		}
		tfile<<endl<<"thcoldUpTime"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			tfile<<thcoldUpTime[i]<<"\t";
		}
		tfile<<endl<<"thfuelCostPieceNum"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			tfile<<thfuelCostPieceNum[i]<<"\t";
		}
		tfile<<endl<<"thhotUpCost"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			tfile<<thhotUpCost[i]<<"\t";
		}
		tfile<<endl<<"thcoldUpCost"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			tfile<<thcoldUpCost[i]<<"\t";
		}
		tfile<<endl<<"thdelta"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			tfile<<thdelta[i]<<"\t";
		}
		tfile<<endl<<"thfirstLast"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			tfile<<thfirstLast[i]<<"\t";
		}
		tfile<<endl<<"thmaxR"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			tfile<<thmaxR[i]<<"\t";
		}
		tfile<<endl<<"tha"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			tfile<<tha[i]<<"\t";
		}
		tfile<<endl<<"thb"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			tfile<<thb[i]<<"\t";
		}
		tfile<<endl<<"thc"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			tfile<<thc[i]<<"\t";
		}
		tfile<<endl<<"thminFuelCost"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			tfile<<thminFuelCost[i]<<"\t";
		}
		tfile<<endl<<"thmaxFuelCost"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			tfile<<thmaxFuelCost[i]<<"\t";
		}
		tfile<<endl<<"thinitState"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			tfile<<thinitState[i]<<"\t";
		}
		tfile<<endl<<"thinitPower"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			tfile<<thinitPower[i]<<"\t";
		}
		tfile<<endl<<"Optimal piecewise linear approximation"<<endl;
		tfile<<"thminFuelPieceCost"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			for(j=0;j<thfuelCostPieceNum[i];j++)
			{
				tfile<<thminFuelPieceCost[i][j]<<"\t";
			}
			tfile<<endl;
		}
		tfile<<endl<<"thmaxFuelPieceCost"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			for(j=0;j<thfuelCostPieceNum[i];j++)
			{
				tfile<<thmaxFuelPieceCost[i][j]<<"\t";
			}
			tfile<<endl;
		}
		tfile<<endl<<"thminPiecePower"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			for(j=0;j<thfuelCostPieceNum[i];j++)
			{
				tfile<<thminPiecePower[i][j]<<"\t";
			}
			tfile<<endl;
		}
		tfile<<endl<<"thmaxPiecePower"<<endl;
		for(i=0;i<thUnitNum;i++)                                                        
		{
			for(j=0;j<thfuelCostPieceNum[i];j++)
			{
				tfile<<thmaxPiecePower[i][j]<<"\t";
			}
			tfile<<endl;
		}
		tfile<<endl<<"thfuelCostPieceSlope"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			for(j=0;j<thfuelCostPieceNum[i];j++)
			{
				tfile<<thfuelCostPieceSlope[i][j]<<"\t";
			}
			tfile<<endl;
		}

		tfile<<endl<<"Forecasted wind generation "<<endl;
		for(i=0;i<windUnitNum;i++)
		{
			for(t=1;t<cycle+1;++t)
			{
				tfile<<windPower[i][t]<<"\t";
			}
			tfile<<endl;
		}
		tfile<<endl<<" Maximum wind generation"<<endl;
		for(i=0;i<windUnitNum;++i)
		{
			tfile<<windMaxPower[i]<<"\t";
		}

		tfile<<endl<<"Maximum output of system "<<endl;
		for(s=0;s<outputNum;++s){
			for(t=1;t<cycle+1;++t)
			{
				tfile<<outputMaxPower[s][t]<<"\t";
			}
			tfile<<endl;
		}

		tfile<<endl<<"Minimum output of system "<<endl;
		for(s=0;s<outputNum;++s){
			for(t=1;t<cycle+1;++t)
			{
				tfile<<outputMinPower[s][t]<<"\t";
			}
			tfile<<endl;
		}
		tfile<<endl<<"Pipe "<<endl;
		for(s=0;s<outputNum;++s){
			for(t=1;t<cycle+1;++t)
			{
				tfile<<Pipe[s][t]<<"\t";
			}
			tfile<<endl;
		}
		tfile<<endl;
		tfile.close();

		/************************************************************************/
		/*			Define variable of thermal unit		By Chun-Ting            */
		/*	1. State of unit, 1 is up and 0 is down;                            */
		/*	2. thermalPower[i][t], power generated by unit i at time t,in MW;   */
		/*	3. thermalR[i][t], spinning contribution of unit i at time t,in MW; */
		/*	4. startUp[i][t], action of start up of unit i at time t,0 or 1;    */
		/*	5. shutDown[i][t], action of shut down of unit i at time t,0 or 1;  */
		/*	6. upCost[i][t], startup cost of unit i at time t;					*/
		/*	7. fuelCost[i][t], fuel cost of unit i at time t;					*/
		/************************************************************************/
		VarMatrix2 state(env,thUnitNum);
		VarMatrix2 thermalPower(env,thUnitNum);
		VarMatrix2 thermalR(env,thUnitNum);
		VarMatrix2 thermalRN(env,thUnitNum);
		VarMatrix2 startUp= VarMatrix2(env,thUnitNum);
		VarMatrix2 shutDown= VarMatrix2(env,thUnitNum);
		VarMatrix2 upCost(env,thUnitNum);
		VarMatrix2 fuelCost=VarMatrix2(env,thUnitNum);
		for(i = 0; i < thUnitNum; i++)
		{
			state[i] = IloNumVarArray(env, cycle+1, 0, 1, ILOINT);
			thermalPower[i]=IloNumVarArray(env,cycle+1,0,thmaxPower[i],ILOFLOAT);
			thermalR[i]=IloNumVarArray(env,cycle+1,0,thmaxR[i],ILOFLOAT);
			thermalRN[i]=IloNumVarArray(env,cycle+1,0,thmaxR[i],ILOFLOAT);
			startUp[i] = IloNumVarArray(env, cycle+1, 0, 1, ILOINT);
			shutDown[i]=IloNumVarArray(env,cycle+1,0,1,ILOINT);	
			upCost[i]=IloNumVarArray(env,cycle+1,0,thcoldUpCost[i],ILOFLOAT);
			fuelCost[i]=IloNumVarArray(env,cycle+1,0,thmaxFuelCost[i],ILOFLOAT);
		}

		//************与分段线性有关的变量*************************
		//分段线性状态
		VarMatrix3 pieceState = VarMatrix3(env, thUnitNum);
		for(i = 0; i < thUnitNum; i++)
		{
			pieceState[i] = VarMatrix2(env, cycle+1);
			for(t = 0; t < cycle+1; t++)
			{
				pieceState[i][t] = IloNumVarArray(env, thfuelCostPieceNum[i], 0, 1, ILOINT);
			}
		}
		//发电量
		VarMatrix3 thermalPiecePower=VarMatrix3(env,thUnitNum);
		for(i=0;i<thUnitNum;i++)
		{
			thermalPiecePower[i]=VarMatrix2(env,cycle+1);
			for(t=0;t<cycle+1;t++)
			{
				thermalPiecePower[i][t]=IloNumVarArray(env,thfuelCostPieceNum[i],0,thmaxPower[i],ILOFLOAT);
			}
		}
		//分段线性部分上的燃料费用
		VarMatrix3 fuelPieceCost=VarMatrix3(env,thUnitNum);
		for(i=0;i<thUnitNum;i++)
		{	
			fuelPieceCost[i]=VarMatrix2(env,cycle+1);
			for(t=0;t<cycle+1;t++)
			{
				fuelPieceCost[i][t]=IloNumVarArray(env,thfuelCostPieceNum[i],0,thmaxFuelCost[i],ILOFLOAT);
			}
		}
		/************************************************************************/
		/*					System demand and output			By Chun-Ting    */
		/*	  In this expression, I introduce wind power and power output;      */
		/*	  however, regard outputPower as a demand in the system.		    */
		/************************************************************************/
		VarMatrix2 outputPower(env,outputNum);
		VarMatrix2 windR(env,windUnitNum);
		VarMatrix2 _outputR(env,outputNum);
		VarMatrix2 _outputRN(env,outputNum);

		for(w=0;w<windUnitNum;++w)
			windR[w]=IloNumVarArray(env,cycle+1,0,windMaxPower[w],ILOFLOAT);
		for(s=0;s<outputNum;++s){
			outputPower[s]=IloNumVarArray(env,outputMinPower[s],outputMaxPower[s],ILOFLOAT);
			_outputR[s]=IloNumVarArray(env,outputMinPower[s],outputMaxPower[s],ILOFLOAT);
			_outputRN[s]=IloNumVarArray(env,outputMinPower[s],outputMaxPower[s],ILOFLOAT);
		}

		for(t=1;t<cycle+1;t++)
		{

			IloExpr thsummaxp(env);
			IloExpr thsumminp(env);
			IloExpr thsum(env);
			IloExpr windsum(env);
			IloExpr outputsum(env);
			for(i=0;i<thUnitNum;i++)
			{
				thsum += thermalPower[i][t];
				thsummaxp += thmaxPower[i]*state[i][t];
				thsumminp += thminPower[i]*state[i][t];
			}

			for(i=0;i<windUnitNum;i++)
			{
				windsum += windPower[i][t];
			}

			for(i=0;i<outputNum;i++)
			{
				outputsum += outputPower[i][t];
			}
			model.add(thsum + windsum == sysDemand[t] - outputsum);
			//	model.add(thsummaxp>=sysDemand[t]+sysReserve[t]);
			//	model.add(thsummaxp+windsum >= sysDemand[t]+sysReserve[t]+outputsum);
			//	model.add(thsumminp+windsum<=sysDemand[t]+outputsum);
		}

		/************************************************************************/
		/*					Spinning reserve	       		By Chun-Ting		*/
		/*	  In this expression, I introduce wind power and power output;      */
		/*	  however, I trait outputPower as a demand in the system.		        */
		/************************************************************************/

		for(t=1;t<cycle+1;t++)
		{
			IloExpr thsum(env);
			IloExpr thsumN(env);
			IloExpr windRP(env);
			IloExpr windRN(env);
			IloExpr outputRP(env);
			IloExpr outputRN(env);
			for(i=0;i<thUnitNum;i++)
			{
				model.add(thermalR[i][t]<=IloMin(thmaxPower[i]*state[i][t]-thermalPower[i][t],thmaxR[i]*state[i][t]));
				model.add(thermalRN[i][t]<=IloMin(thermalPower[i][t]-thminPower[i]*state[i][t],thmaxR[i]*state[i][t]));
				thsum+=thermalR[i][t];
				thsumN+=thermalRN[i][t];
			}
			for(w=0;w<windUnitNum;++w)
			{
				//	model.add(windR[w][t]==IloMin(r*windPower[w][t],windMaxPower[w]-windPower[w][t]));
				//windRP+=windR[w][t];
				//windRN+=r*windPower[w][t];
			}
			for(s=0;s<outputNum;++s)
			{
				outputRP+=_outputR[s][t];
				outputRN+=_outputRN[s][t];
			}
			model.add(thsum+outputRP == sysReserve[t]);
			//model.add(thsumN+outputRN==0.375*sysReserve[t]+windRP);
			//model.add(thsumN+outputRN==0.375*sysReserve[t]);
		}
		/************************************************************************/
		/*			Constraints of output transmission capacity		By Chun-Ting    */
		/************************************************************************/
		for(t=1;t<cycle+1;t++)
		{
			for(s=0;s<outputNum;++s)
			{
				model.add(outputPower[s][t]+_outputR[s][t]<=outputMaxPower[s][t]);
				model.add(outputPower[s][t]-_outputRN[s][t]>=outputMinPower[s][t]);
			}
		}
		/************************************************************************/
		/*						Ramp rate			By Chun-Ting                */
		/* Maximum allowable change in generation between two consecutive hours;*/
		/************************************************************************/
		for (i=0;i<thUnitNum; i++)
		{
			for (t=1; t<cycle+1;t++)
			{
				model.add(IloAbs(thermalPower[i][t]-thermalPower[i][t-1])<=thdelta[i]);
			}
		}
		/************************************************************************/
		/*             Constraints of thermal unit     By Chun-Ting             */
		/*       1.  Initial state;                                             */
		/*       2.  Startup and shutdown;										*/
		/*       3.  State transition;											*/
		/*       4.  Minimum up/down time;										*/
		/*       5.  Must-run or must-not-run with C1 and C4;					*/
		/*       6.  Action of start up and shut down;							*/
		/************************************************************************/

		for(i = 0; i < thUnitNum; i++)
		{
			if(thinitState[i] < 0)
			{
				model.add(state[i][0] == 0);
				model.add(thermalPower[i][0] == 0);
				model.add(thermalR[i][0]==0);
				model.add(thermalRN[i][0]==0);
			}
			else if(thinitState[i] > 0)
			{
				model.add(state[i][0] == 1);								
				model.add(thermalR[i][0]==0);
				model.add(thermalRN[i][0]==0);
			}

			model.add(startUp[i][0] == 0);
			model.add(shutDown[i][0] == 0);

			for(t = 1; t < cycle+1; t++)
			{
				model.add(state[i][t]-state[i][t-1]-startUp[i][t]+shutDown[i][t]==0);
				model.add(startUp[i][t] + shutDown[i][t]<=1);
			}

			for(t = 1; t < cycle+1; t++) 
			{
				IloNum temp1 = IloMin(cycle, t+thminUp[i]-1);
				IloNum temp2 = IloMin(cycle, t+thminDown[i]-1);
				IloExpr sum1(env);
				IloExpr sum2(env);
				for(k = t+1; k <= temp1; k++)
				{
					sum1 += shutDown[i][k];
				}
				for(k = t+1; k <= temp2; k++)
				{
					sum2 += startUp[i][k];
				}
				model.add(startUp[i][t] + sum1 <= 1);
				model.add(shutDown[i][t] + sum2 <= 1);
			}

		}

		for(i=0;i<thUnitNum;i++)
		{
			if(thinitState[i]<0&&IloAbs(thinitState[i])<thminDown[i])
			{
				for(t=1;t<=(thminDown[i]+thinitState[i]);t++)
				{
					model.add(startUp[i][t]==0);
					model.add(state[i][t]==0);
				}
			}
			if(thinitState[i]>0&&thinitState[i]<thminUp[i])
			{
				for(t=1;t<=(thminUp[i]-thinitState[i]);t++)
				{
					model.add(shutDown[i][t]==0);
					model.add(state[i][t]==1);
				}
			}
		}

		for(i=0;i<thUnitNum;i++)
		{
			for(t=1;t<cycle+1;t++)
			{
				IloNum temp=IloMin(cycle,(t+thminUp[i]+thminDown[i]-1));
				IloExpr sum1(env);
				IloExpr sum2(env);
				for(j=t;j<=temp;j++)
				{
					sum1+=startUp[i][j];
				}
				for(j=t;j<=temp;j++)
				{
					sum2+=shutDown[i][j];
				}
				model.add(sum1-1<=0);
				model.add(sum2-1<=0);
			}
		}

		/************************************************************************/
		/*	Constraints of  piecewise linear cost formulation	By Chun-Ting    */
		/*		  The nonlinear fuel costs are all replaced by their piecewise  */
		/*	linear approximations  with thfuelCostPieceNum[i] segments. With    */
		/*	a sharply contrast, there are some key continuous variables and some*/
		/*	new bound constraints formulated by mixed variables are introduced. */
		/************************************************************************/
		for(i=0;i<thUnitNum;i++)
		{
			for(t=1;t<cycle+1;t++)
			{
				for(j=0;j<thfuelCostPieceNum[i];j++)
				{					
					model.add(thermalPiecePower[i][t][j]<=thmaxPiecePower[i][j]*pieceState[i][t][j]);
					model.add(thermalPiecePower[i][t][j]>=thminPiecePower[i][j]*pieceState[i][t][j]);
					model.add(fuelPieceCost[i][t][j]<=thmaxFuelPieceCost[i][j]*pieceState[i][t][j]);
					model.add(fuelPieceCost[i][t][j]>=thminFuelPieceCost[i][j]*pieceState[i][t][j]);
					//model.add(fuelPieceCost[i][t][j]==thminFuelPieceCost[i][j]*pieceState[i][t][j]+thfuelCostPieceSlope[i][j]*(thermalPiecePower[i][t][j]-thminPiecePower[i][j]*pieceState[i][t][j]));
					//修改by xjlei
					model.add(fuelPieceCost[i][t][j] == thminFuelPieceCost[i][j]*pieceState[i][t][j]+(thmaxFuelPieceCost[i][j]-
						thminFuelPieceCost[i][j])/(thmaxPiecePower[i][j]-thminPiecePower[i][j])*(thermalPiecePower[i][t][j]-
						thminPiecePower[i][j]*pieceState[i][t][j])-tha[i]/8*(thmaxPiecePower[i][j]-
						thminPiecePower[i][j])*(thmaxPiecePower[i][j]-thminPiecePower[i][j])*pieceState[i][t][j]);
				}
			}
		}
		//发电状态只能同时处于分段上的一段
		for(i=0;i<thUnitNum;i++)
		{
			for(t=1;t<cycle+1;t++)
			{
				IloExpr sum(env);
				for(j=0;j<thfuelCostPieceNum[i];j++)
				{
					sum += pieceState[i][t][j];
				}
				model.add(startUp[i][t] - sum <= 0);
				model.add(sum-1+shutDown[i][t] <= 0);
				model.add(sum-state[i][t] == 0);
			}
		}
		/************************************************************************/
		/*		Generation and cost constraints about piecewise linear cost     */
		/*	formulation and start-up cost constraints.   By Chun-Ting		    */
		/************************************************************************/
		for(i=0;i<thUnitNum;i++)
		{
			for(t=1;t<cycle+1;t++)
			{
				IloExpr sumPower(env);
				IloExpr sumCost(env);
				for(j=0;j<thfuelCostPieceNum[i];j++)
				{
					sumPower += thermalPiecePower[i][t][j];
					sumCost += fuelPieceCost[i][t][j];
				}
				model.add(thermalPower[i][t] == sumPower);
				model.add(fuelCost[i][t] == sumCost);
				model.add(upCost[i][t] <= thcoldUpCost[i]*startUp[i][t]);
				model.add(upCost[i][t] >= thhotUpCost[i]*startUp[i][t]);
			}
		}
		/************************************************************************/
		/*			Constraints of transmission line		By Chun-Ting        */
		/************************************************************************/
		for (t = 1; t < cycle+1; t++)
		{				 		 
			for(l= 0; l< lineNum; l++)
			{
				IloExpr sumGamaP(env);
				IloExpr sumGamaD(env);
				IloExpr sumGamaW(env);
				IloExpr sumGamaO(env);
				for (i = 0; i < thUnitNum; i++)
				{
					sumGamaP += gama[l][unitLocation[i]-1]*thermalPower[i][t];
				}
				for (w = 0; w < windUnitNum; ++w)
				{
					sumGamaW += gama[l][windLocation[w]-1]*windPower[w][t];
				}

				for (d = 0; d < demandNum; d++)
				{
					sumGamaD += gama[l][demandLocation[d]-1]*sysDemand[t]*demand[d][t];
				} 
				for (s= 0; s < outputNum; ++s)
				{
					sumGamaO += gama[l][outputLocation[s]-1]*outputPower[s][t];
				}
				model.add(sumGamaP - sumGamaD + sumGamaO + 2*lineCap[l] >= 0);
				model.add(sumGamaP - sumGamaD + sumGamaO - 2*lineCap[l]<=0);
				//	model.add(sumGamaW + sumGamaP - sumGamaD - sumGamaO + 2*lineCap[l] >= 0);				 		
				//	model.add(sumGamaW + sumGamaP - sumGamaD - sumGamaO - 2*lineCap[l]<=0); 
			}

		}

		/************************************************************************/
		/*				Objective function                                      */
		/************************************************************************/
		IloExpr obj(env);
		IloExpr upCostsum(env);
		IloExpr fuelCostsum(env);
		IloExpr outputCostsum(env);
		IloExpr outputRCost(env);
		IloExpr outputRNCost(env);
		for(t=1;t<cycle+1;t++)
		{

			for(i=0;i<thUnitNum;i++)  
			{
				upCostsum+=upCost[i][t];
				fuelCostsum+=fuelCost[i][t];
			}
			for(s=0;s<outputNum;++s)
			{
				outputRCost+=rp*_outputR[s][t];
				outputRNCost+=nrp*_outputRN[s][t];
				outputCostsum+= IloAbs(lamda[s]*(outputPower[s][t] - Pipe[s][t]));
			}
		}
		/*model.add(upCostsum);
		model.add(fuelCostsum);
		model.add(outputCostsum);*/
		obj = upCostsum + fuelCostsum + outputCostsum + outputRCost;// + outputRNCost;
		IloObjective objective = IloMinimize(env, obj);
		model.add(objective);

		IloCplex cplex(model);
		cplex.setParam(cplex.EpGap,0.001);//relative MIP gap tolerance
		//	cplex.setParam(cplex.NodeFileInd,3);
		//	cplex.setParam(cplex.TiLim,100);

		cplex.extract(model);
		cplex.solve();
		cplex.exportModel(lp);//与IloCplex.importModel对应，可以读回模型

		env.out() << "Solution status = " << cplex.getStatus() << endl;
		env.out() << "Solution value  = " << cplex.getObjValue() << endl;
		env.out() << "solution time   = " <<timer.getTime()<<endl;
		env.out() << "EpGap           = " <<cplex.getParam(cplex.EpGap)<<endl;


		ofstream outf(OUTFILERESULT,ios::out);
		if(!outf)
			cout<<"cannot open "<<OUTFILERESULT<<endl; 
		outf<<"Result output by cplex"<<endl;
		outf<<"Solution status\t"<<cplex.getStatus()<<endl;
		outf<<"Solution value\t"<<cplex.getObjValue()<<endl;
		outf<<"Solution time\t"<<timer.getTime()<<endl;
		outf<<"EpGap\t"<<cplex.getParam(cplex.EpGap)<<endl; 
		outf<<"UpCost\t"<<cplex.getValue(upCostsum)<<endl;
		outf<<"FuelCostsum\t"<<cplex.getValue(fuelCostsum)<<endl;
		outf<<"OutputProfile\t"<<cplex.getValue(outputCostsum)<<endl;
		outf<<"OutputRCost\t"<<cplex.getValue(outputRCost)<<endl;
		outf<<"OutputRNCost\t"<<cplex.getValue(outputRNCost)<<endl;
	//	outf<<"OutPut Power\t"<<cplex.getValue(outputsum)<<endl;
		float allFuelCost=0;
		for(i=0;i<thUnitNum;i++)
		{
		for(t=1;t<cycle+1;t++)
		{
		allFuelCost+=cplex.getValue(fuelCost[i][t]);
		}
		}
		cout<<endl<<"allFuelCost = "<<allFuelCost<<endl;
		outf<<"allFuelCost\t"<<allFuelCost<<endl;
		outf.close();
		ofstream outf_ThUnit(Resutl_ThUnit,ios::out);
		if(!outf_ThUnit)
			cout<<"cannot open "<<Resutl_ThUnit<<endl;
		outf_ThUnit<<endl<<"thermalState[i][t]"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			for(t=1;t<cycle+1;t++)
			{


				if(cplex.getValue(state[i][t])<_INF)
					outf_ThUnit<<"0\t";
				else
					outf_ThUnit<<cplex.getValue(state[i][t])<<"\t";
			}
			outf_ThUnit<<endl;        
		}  
		outf_ThUnit<<endl<<"thermalPower"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			for(t=1;t<cycle+1;t++)
			{
				IloNum temppower=cplex.getValue(thermalPower[i][t]);
				if(temppower<_INF)
					outf_ThUnit<<"0\t";
				else
					outf_ThUnit<<temppower<<"\t";
			}
			outf_ThUnit<<endl;

		}
		outf_ThUnit<<endl<<"Up Reserve"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			for(t=1;t<cycle+1;t++)
			{

				if(cplex.getValue(thermalR[i][t])<1e-7)
					outf_ThUnit<<"0\t";
				else
					outf_ThUnit<<cplex.getValue(thermalR[i][t])<<"\t";
			}
			outf_ThUnit<<endl;
		}
		outf_ThUnit<<endl<<"Down Reserve"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			for(t=1;t<cycle+1;t++)
			{

				if(cplex.getValue(thermalRN[i][t])<1e-7)
					outf_ThUnit<<"0\t";
				else
					outf_ThUnit<<cplex.getValue(thermalRN[i][t])<<"\t";
			}
			outf_ThUnit<<endl;
		}
		outf_ThUnit<<endl<<"fuelCost "<<endl;
		for(t=1;t<cycle+1;t++)
		{
			for(i=0;i<thUnitNum;i++)
			{
				IloNum unitcost=cplex.getValue(fuelCost[i][t]);
				if(unitcost<_INF)
					outf_ThUnit<<"0\t";
				else
					outf_ThUnit<<unitcost<<"\t";
			}
			outf_ThUnit<<endl;

		}
		outf_ThUnit.close();
		ofstream outf_wind(Result_Output,ios::out);
		if (!outf_wind)
			cout<<"can't open "<<Result_Output<<endl;
		IloNum temp;
		/*
		outf_wind<<"wind power generation"<<endl;
		for(t=1;t<cycle+1;t++)
		{
		for(w=0;w<windUnitNum;++w)
		{
		outf_wind<<windPower[w][t]<<"\t";
		}
		outf_wind<<endl;        
		}
		*/
		outf_wind<<endl<<" OutputPower - Pipe"<<endl;
		for(s=0;s<outputNum;++s)
		{
			for(t=1;t<cycle+1;t++)
			{
				temp=cplex.getValue(outputPower[s][t] - Pipe[s][t]);
				outf_wind<<temp<<"\t";
			}
			outf_wind<<endl;        
		} 
		outf_wind<<"output power"<<endl;

		for(s=0;s<outputNum;++s)
		{
			for(t=1;t<cycle+1;t++)
			{

				temp=cplex.getValue(outputPower[s][t]);
				Pipe[s][t] = temp;
				/*	if (temp<_INF-1200)
				outf_wind<<"0\t";
				else
				*/
				outf_wind<<temp<<"\t";
			}
			
			outf_wind<<endl;        
		} 
		outf_wind<<endl<<"output up reserve"<<endl;
		for(s=0;s<outputNum;++s)
		{
			for(t=1;t<cycle+1;t++)
			{
				temp=cplex.getValue(_outputR[s][t]);
				if (temp<_INF)
					outf_wind<<"0\t";
				else
					outf_wind<<temp<<"\t";
			}
			outf_wind<<endl;        
		} 
		outf_wind<<endl<<"output down reserve"<<endl;
		for(s=0;s<outputNum;++s)
		{
			for(t=1;t<cycle+1;t++)
			{
				temp=cplex.getValue(_outputRN[s][t]);
				if (temp<_INF)
					outf_wind<<"0\t";
				else
					outf_wind<<temp<<"\t";
			}
			outf_wind<<endl;        
		} 

		outf_wind.close();


		ofstream outf_Line(Result_Line,ios::out);
		if(!outf_Line)
			cout<<"cannot open "<<Result_Line<<endl;
		outf_Line<<"current[l][t]"<<endl;
		float current[100][200];
		for (t = 1; t < cycle+1; t++)
		{				 		 
			for(l= 0; l< lineNum; l++)
			{
				float gamaP=0;
				float gamaD=0;
				float gamaW=0;
				float gamaO=0;
				for (i = 0; i < thUnitNum; i++)
				{
					gamaP+=gama[l][unitLocation[i]-1]*cplex.getValue(thermalPower[i][t]);
				}
				for (d = 0; d < demandNum; d++)
				{
					gamaD+=gama[l][demandLocation[d]-1]*sysDemand[t]*demand[d][t];
				}
				/*
				for (w = 0; w < windUnitNum; ++w)
				{
				gamaW+=gama[l][windLocation[w]-1]*windPower[w][t];
				}
				*/
				for (s= 0; s < outputNum; ++s)
				{
					gamaO+=gama[l][outputLocation[s]-1]*cplex.getValue(outputPower[s][t]);
				} 
				current[l][t] = gamaP - gamaD -gamaO;	
				//	current[l][t] = gamaP + gamaW - gamaD -gamaO;	
				//	if (fabs(current[l][t])<_INF)  current[l][t]=0;
				outf_Line<<current[l][t]<<"\t";

			}
			outf_Line<<endl;
		}
		outf_Line.close();

	}
	catch (IloException& e) 
	{
		cerr << "Concert exception caught: " << e << endl;
	}

	catch (...) 
	{
		cerr << "Unknown exception caught" << endl;
	}

	//	env.end();
	return 0;
}