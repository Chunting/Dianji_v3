#pragma once
#include <ilcplex/ilocplex.h>
#include <fstream>
#include <math.h>
#include <stdlib.h>
#include <ilconcert/ilomodel.h>
using namespace std;
ILOSTLBEGIN
typedef IloArray<IloNumArray> Matrix2;  //typedef为 IloArray<IloNumArray> 起别名Matrix2，以便于后面运用时的书写
typedef IloArray<Matrix2> Matrix3;
typedef IloArray<Matrix3> Matrix4;
typedef IloArray<Matrix4> Matrix5;

typedef IloArray<IloNumVarArray>VarMatrix2;
typedef IloArray<VarMatrix2>VarMatrix3;
typedef IloArray<VarMatrix3>VarMatrix4;
typedef IloArray<VarMatrix4>VarMatrix5;
typedef IloArray<VarMatrix5>VarMatrix6;

/************************************************************************/
/*				Read the variables of system		By Chun-Ting        */
/*		1. cycle, time periods,24;									    */
/*		2. thUnitNum, amount of thermal unit, 16;					    */
/*		3. windUnitNum, amount of wind unit, 3;					        */
/*		4. demandNum, amount of load, 11;							    */
/*		5. output, amount of output bus, 3;						        */
/*		6. lineNum, amount of transmission line, 39;				    */
/*		7. busNum, amount of bus, 31;								    */
/************************************************************************/	
int readSystemData(const char* systemdata,
				   IloEnv env,
				   IloInt& cycle,			
				   IloInt& thUnitNum,		
				   IloInt& windUnitNum,      
				   IloInt& demandNum,		
				   IloInt& outputNum,        
				   IloInt& lineNum,			
				   IloInt& busNum					
				   )
{

	ifstream fin(systemdata,ios::in);
	if(!fin) 
		env.out()<<"problem with file:"<<systemdata<<endl;
	fin>>cycle;
	cycle*=1;
	fin>>thUnitNum;
	fin>>windUnitNum;
	fin>>demandNum;
	fin>>outputNum;
	fin>>lineNum;
	fin>>busNum;
	fin.close();
	return 0;
}
/************************************************************************/
/*				Read net data			By Chun-Ting                    */
/*		1. unitLocation, bus the thermal unit located;	                */
/*		2. windLocation, bus the wind unit located;	                    */
/*		3. demandLocation, bus the demand located;  	                */
/*		4. outputLocation, bus the output located;   	                */
/*		5. demand, proportion of demand by demand i at time t;          */
/*		6. lineCap, transmitting capacity of line l;          */
/************************************************************************/
//************************************
// Method:    readNetData
// FullName:  readNetData
// Access:    public 
// Returns:   int
// Qualifier:
// Parameter: const char * netdata
// Parameter: IloEnv env
// Parameter: IloNumArray & unitLocation
// Parameter: IloNumArray & windLocation
// Parameter: IloNumArray & demandLocation
// Parameter: IloNumArray & outputLocation
// Parameter: Matrix2 & demand
// Parameter: IloNumArray & lineCap
//************************************
int readNetData(const char* netdata,
				IloEnv env, 
				IloInt& cycle,
				IloInt& thUnitNum,		
				IloInt& windUnitNum,      
				IloInt& demandNum,		
				IloInt& outputNum, 
				IloInt& lineNum,	
				IloNumArray& unitLocation,               
				IloNumArray& windLocation,               
				IloNumArray& demandLocation,            
				IloNumArray& outputLocation,            
				Matrix2& demand,           
				IloNumArray& lineCap 
				)
{
	ifstream fin(netdata,ios::in);
	if(!fin) env.out()<<"problem with file:"<<netdata <<endl;
	int i,t;

	for(i=0;i<thUnitNum;i++)
	{
		fin>> unitLocation[i];
	}

	for(i=0;i<windUnitNum;i++)
	{
		fin>> windLocation[i];
	}
	for(i=0;i< demandNum;i++)
	{
		fin>> demandLocation[i];
	}
	for(i=0;i< outputNum;i++)
	{
		fin>> outputLocation[i];
	}
	/**
	for(i=0;i< demandNum;i++)
	{
	fin>> demand[i];
	}
	**/

	for(i=0;i<demandNum;i++)
	{
		for(t=1;t<cycle+1;t++)
		{
			fin>>demand[i][t];
		}
	}


	for(i=0;i<lineNum;i++)
	{
		fin>> lineCap [i];
	}
	return 0;
}
/************************************************************************/
/*					Read gama					By Chun-Ting	        */
/************************************************************************/	
int readGama(const char* gamgadata,
			 IloEnv env,
			 IloInt& lineNum,			
			 IloInt& busNum,
			 Matrix2& gama)
{
	int i,j;
	ifstream fin(gamgadata,ios::in);		
	if(!fin) 
		env.out()<<"problem with "<< gamgadata<<endl;
	for(i=0;i<lineNum;i++)
	{
		for(j=0;j<busNum;j++)
		{
			fin>>gama[i][j];
		}
	}
	return 0;
}

/************************************************************************/
/*			Read thermal unit parameters		By Chun-Ting            */
/*		1. thminPower, minimum generation level of unit i at time t;    */
/*		2. thmaxPower, maximum generation level of unit i at time t;    */
/*		3. thminDown, minimum down-time of unit i;					    */
/*		4. thminUp, minimum up-time of unit i;						    */
/*		5. thcoldUpTime, cold start-up time of unit i, in hours;	    */
/*		6. thfuelCostPieceNum;											*/
/*		7. thhotUpCost, hot start-up cost of unit i;				    */
/*		8. thcoldUpCost, cold start-up cost of unit i;				    */
/*		7. thdelta, ramp rate constraints;							    */
/*		7. thfirstLast, if there is the up-down constraints for 	    */
/*						the first and last hour;						*/
/*		8. thmaxR, maximum spinning reserve requirement;			    */
/************************************************************************/
int readThUnitData(const char* thunitdata,
				   IloEnv env,  
				   IloInt& thUnitNum,		
				   IloNumArray& thminPower,                        
				   IloNumArray& thmaxPower,                       
				   IloNumArray& thminDown,                         
				   IloNumArray& thminUp,                          
				   IloNumArray& thcoldUpTime,                      
				   IloNumArray& thfuelCostPieceNum,                
				   IloNumArray& thhotUpCost,                       
				   IloNumArray& thcoldUpCost,			           
				   IloNumArray& thdelta,						   
				   IloNumArray& thfirstLast,                       
				   IloNumArray& thmaxR,                            	
				   IloNumArray& tha,							   		
				   IloNumArray& thb,							   		
				   IloNumArray& thc,						       
				   IloNumArray& thminFuelCost,                        //机组最小发电费用
				   IloNumArray& thmaxFuelCost,                        //机组最大发电费用
				   IloNumArray& thinitState,                        //机组初始状态
				   IloNumArray& thinitPower                        //机组初始发电量			 
				   )
{
	ifstream fin(thunitdata,ios::in);
	if(!fin) env.out()<<"problem with file:"<<thunitdata<<endl;
	int i;

	//read minPower
	for(i=0;i<thUnitNum;i++)
	{
		fin>>thminPower[i];
	}

	//read maxPower
	for(i=0;i<thUnitNum;i++)
	{
		fin>>thmaxPower[i];
	}

	//read minDown
	for(i=0;i<thUnitNum;i++)
	{
		fin>>thminDown[i];
	}

	//read minUp
	for(i=0;i<thUnitNum;i++)
	{
		fin>>thminUp[i];
	}

	//read coldUpTime
	for(i=0;i<thUnitNum;i++)
	{
		fin>>thcoldUpTime[i];
	}

	//read thfuelCostPieceNum	
	for(i=0;i<thUnitNum;i++)
	{
		fin>>thfuelCostPieceNum[i];
	}

	//read hotUpCost
	for(i=0;i<thUnitNum;i++)
	{
		fin>>thhotUpCost[i];
	}

	//read coldUpCost
	for(i=0;i<thUnitNum;i++)
	{
		fin>>thcoldUpCost[i];
	}

	//read delta
	for(i=0;i<thUnitNum;i++)
	{
		fin>>thdelta[i];
	}

	//read firstlast
	for(i=0;i<thUnitNum;i++)
	{
		fin>>thfirstLast[i];
	}

	//read maxR
	for(i=0;i<thUnitNum;i++)
	{
		fin>>thmaxR[i];
	}

	//read a
	for(i=0;i<thUnitNum;i++)
	{
		fin>>tha[i];
	}

	//read b
	for(i=0;i<thUnitNum;i++)
	{
		fin>>thb[i];
	}
	//read c
	for(i=0;i<thUnitNum;i++)
	{
		fin>>thc[i];
	}

	for(i=0;i<thUnitNum;i++)
	{
		thminFuelCost[i]=tha[i]*thminPower[i]*thminPower[i]+thb[i]*thminPower[i]+thc[i];
		thmaxFuelCost[i]=tha[i]*thmaxPower[i]*thmaxPower[i]+thb[i]*thmaxPower[i]+thc[i];
	}
	//read thinitState
	for(i=0;i<thUnitNum;i++)
	{
		fin>>thinitState[i];
	}
	//read thinitPower
	for(i=0;i<thUnitNum;i++)
	{
		fin>>thinitPower[i];
	}
	return 0;
}

/************************************************************************/
/*		Read Wind and Output Power			By Chun-Ting                */
/*	 1. setwindPower, power generated by wind unit i at time t, in MW;  */
/*	 2. windUnitMaxOutput, maximum output power by wind unit i, in MW;  */
/*	 3. windUnitMinOutput, minimum output power by wind unit i, in MW;  */
/*	 4. sysOutput, preset output power by system at time t, in MW;		*/
/*	 5. proOutput, proportion of output by wind unit i;					*/
/*	 6. sysMaxOutput, maximum output power by system at time t, in MW;	*/
/*	 7. sysOutput, minimum output power by system at time t, in MW;		*/
/************************************************************************/
int readWindUnitData (
					  const char* WINDDATA,
					  IloEnv env,				
					  IloInt& cycle,		
					  IloInt& windUnitNum,
					  IloNumArray& windUnitMaxOutput,
					  IloNumArray& windUnitMinOutput
					  ){
						  ifstream fin(WINDDATA,ios::in);
						  if(!fin) env.out()<<"problem with file:"<<WINDDATA<<endl;
						  int w = 0;
						  for(w=0;w<windUnitNum;++w){
							  fin>>windUnitMaxOutput[w];
						  }
						  for(w=0;w<windUnitNum;w++){
							  fin>>windUnitMinOutput[w];
						  }
						  return 0;
}
/************************************************************************/
/*				Fuel Cost Function			By Chun-Ting                */
/*		Suppose that the generation cost is a quadratic function of     */
/*  thermalPower[i][t], where tha,thb and thc are appropriate           */
/*	coefficients A piecewise linear approximation is usually obtained   */
/*	by linear interpolation in each subinterval.						*/
/*	thfuelCostPieceNum,the interval[thminPower,thmaxPower] is 			*/
/*	partitioned into thfuelCostPieceNum subintervals.					*/
/************************************************************************/
int pieceThUnitData(IloEnv env,
					IloInt& thUnitNum,		
					IloNumArray& tha,							   		
					IloNumArray& thb,							   	
					IloNumArray& thc,							  
					IloNumArray& thminPower,                        
					IloNumArray& thmaxPower,						
					IloNumArray& thfuelCostPieceNum,  
					Matrix2& thminPiecePower,                 
					Matrix2& thmaxPiecePower,
					Matrix2& thminFuelPieceCost,
					Matrix2& thmaxFuelPieceCost,
					Matrix2& thfuelCostPieceSlope
					) 
{
	int i,j,pieceNum;
	IloNum d;

	for(i=0;i<thUnitNum;i++)
	{
		pieceNum = thfuelCostPieceNum[i];
		d=(thmaxPower[i]-thminPower[i])/pieceNum;
		thminPiecePower[i][0]=thminPower[i];
		thminFuelPieceCost[i][0]=tha[i]*thminPiecePower[i][0]*thminPiecePower[i][0]+thb[i]*thminPiecePower[i][0]+thc[i];

		thmaxPiecePower[i][pieceNum-1]=thmaxPower[i];
		thmaxFuelPieceCost[i][pieceNum-1]=tha[i]*thmaxPiecePower[i][pieceNum-1]*thmaxPiecePower[i][pieceNum-1]+thb[i]*thmaxPiecePower[i][pieceNum-1]+thc[i];
		for(j=1;j<pieceNum;j++)
		{
			thminPiecePower[i][j]=thminPiecePower[i][j-1]+d;
			thmaxPiecePower[i][j-1]=thminPiecePower[i][j];
			thminFuelPieceCost[i][j]=tha[i]*thminPiecePower[i][j]*thminPiecePower[i][j]+thb[i]*thminPiecePower[i][j]+thc[i];
			thmaxFuelPieceCost[i][j-1]=thminFuelPieceCost[i][j];			
		}
		for(j=0;j<pieceNum;j++)
		{
			thfuelCostPieceSlope[i][j]=(thmaxFuelPieceCost[i][j]-thminFuelPieceCost[i][j])/d;
		}
	}
	return 0;
}
string int2str(int &i) {
  string s;
  stringstream ss(s);
  ss << i;

  return ss.str();
}