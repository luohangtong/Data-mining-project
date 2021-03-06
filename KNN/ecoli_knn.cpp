#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <fstream>
using namespace std;

//宏定义
#define ATTR_NUM 7                       //属性数目
#define MAX_SIZE_OF_TRAINING_SET 1000    //训练数据集的最大大小
#define MAX_SIZE_OF_TEST_SET 100         //测试数据集的最大大小
#define MAX_VALUE 10000.0                //属性最大值
#define K 9

//结构体
struct dataVector {
	int ID;                      //ID号
	char classLabel[5];          //分类标号
	char attributes0[15];        //属性0	
	double attributes[ATTR_NUM]; //属性	
};
struct distanceStruct {
	int ID;                      //ID号
	double distance;             //距离
	char classLabel[15];         //分类标号
};

//全局变量
struct dataVector gTrainingSet[MAX_SIZE_OF_TRAINING_SET]; //训练数据集
struct dataVector gTestSet[MAX_SIZE_OF_TEST_SET];         //测试数据集
struct distanceStruct gNearestDistance[K];                //K个最近邻距离
int curTrainingSetSize=0;                                 //训练数据集的大小
int curTestSetSize=0;                                     //测试数据集的大小

//求 vector1=(x1,x2,...,xn)和vector2=(y1,y2,...,yn)的欧几里德距离
double Distance(struct dataVector vector1,struct dataVector vector2)
{
	double dist,sum=0.0;
	for(int i=0;i<ATTR_NUM;i++)
	{
		sum+=(vector1.attributes[i]-vector2.attributes[i])*(vector1.attributes[i]-vector2.attributes[i]);
	}
	dist=sqrt(sum);
	return dist;
}

//得到gNearestDistance中的最大距离，返回下标
int GetMaxDistance()
{
	int maxNo=0;
	for(int i=1;i<K;i++)
	{
		if(gNearestDistance[i].distance>gNearestDistance[maxNo].distance)	maxNo = i;
	}
	return maxNo;
}

//对未知样本Sample分类
char* Classify(struct dataVector Sample)
{
	double dist=0;
	int maxid=0,freq[K],i,tmpfreq=1;;
	char *curClassLable=gNearestDistance[0].classLabel;
	memset(freq,1,sizeof(freq));
	//初始化距离为最大值
	for(i=0;i<K;i++)
	{
		gNearestDistance[i].distance=MAX_VALUE;
	}
	//计算K-最近邻距离
	for(i=0;i<curTrainingSetSize;i++)
	{
		//计算未知样本和每个训练样本的距离
		dist=Distance(gTrainingSet[i],Sample);
		//得到gNearestDistance中的最大距离
		maxid=GetMaxDistance();
		//如果距离小于gNearestDistance中的最大距离，则将该样本作为K-最近邻样本
		if(dist<gNearestDistance[maxid].distance) 
		{
			gNearestDistance[maxid].ID=gTrainingSet[i].ID;
			gNearestDistance[maxid].distance=dist;
			strcpy(gNearestDistance[maxid].classLabel,gTrainingSet[i].classLabel);
		}
	}
	//统计每个类出现的次数
	for(i=0;i<K;i++)  
	{
		for(int j=0;j<K;j++)
		{
			if((i!=j)&&(strcmp(gNearestDistance[i].classLabel,gNearestDistance[j].classLabel)==0))
			{
				freq[i]+=1;
			}
		}
	}
	//选择出现频率最大的类标号
	for(i=0;i<K;i++)
	{
		if(freq[i]>tmpfreq)  
		{
			tmpfreq=freq[i];
			curClassLable=gNearestDistance[i].classLabel;
		}
	}
	return curClassLable;
}

//    主函数
void main()
{  
	FILE *fp;
	double mResult[10]={0};
	for(int m = 0; m < 10; m++)
	{	
		ifstream filein("ecoli.data");
		if(filein.fail())
		{
			cout<<"Can't open data.txt"<<endl; 
			return;
		}
		curTestSetSize = 0;
		curTrainingSetSize = 0;
		char c; 
		char *classLabel="";
		int i,j, rowNo=-1,TruePositive=0,FalsePositive=0;

		//读文件	
		while(!filein.eof()&&rowNo < 300) 
		{
			rowNo++;
			if(curTrainingSetSize>=MAX_SIZE_OF_TRAINING_SET) 
			{
				cout<<"The training set has "<<MAX_SIZE_OF_TRAINING_SET<<" examples!"<<endl<<endl; 
				break ;
			}

			//rowNo%10!=m的270组数据作为训练数据集
			if(rowNo%10!=m)
			{
				gTrainingSet[curTrainingSetSize].ID=rowNo;
				filein>>gTrainingSet[curTrainingSetSize].attributes0;
				filein>>c;
				for(int i = 0;i < ATTR_NUM;i++) 
				{
					filein>>gTrainingSet[curTrainingSetSize].attributes[i];
					if(i < ATTR_NUM - 1)
						filein>>c;
				}
				filein>>gTrainingSet[curTrainingSetSize].classLabel;
				curTrainingSetSize++;

			}
			//剩下rowNo%10==m的30组做测试数据集
			else if(rowNo%10==m)
			{
				gTestSet[curTestSetSize].ID=rowNo;
				filein>>gTestSet[curTestSetSize].attributes0;
				filein>>c;
				for(int i = 0;i < ATTR_NUM;i++) 
				{				
					filein>>gTestSet[curTestSetSize].attributes[i];
					if(i < ATTR_NUM - 1)
						filein>>c;
				}
				filein>>gTestSet[curTestSetSize].classLabel;
				curTestSetSize++;
			}		
		}
		filein.close();

		curTrainingSetSize -= 1;
		curTestSetSize -= 1;
		//KNN算法进行分类，并将结果写到文件iris_OutPut.txt
		fp=fopen("ecoli_OutPut%d.txt","w+t");
		//用KNN算法进行分类
		fprintf(fp,"************************************程序说明***************************************\n");
		fprintf(fp,"** 采用KNN算法对ecoli.data分类。为了操作方便，对各组数据添加rowNo属性,第一组rowNo=1!\n");
		fprintf(fp,"** 共有300组数据,选择rowNo模10不等于0的270组作为训练数据集，剩下的30组做测试数据集\n");
		fprintf(fp,"***********************************************************************************\n\n");
		fprintf(fp,"************************************实验结果***************************************\n\n");
		for(i=0;i<curTestSetSize ;i++)
		{
			fprintf(fp,"************************************第%d组数据**************************************\n",i+1);
			classLabel =Classify(gTestSet[i]);
			if(strcmp(classLabel,gTestSet[i].classLabel)==0)//相等时，分类正确
			{
				TruePositive++;
			}
			cout<<"rowNo:	";
			cout<<gTestSet[i].ID<<"    \t";
			cout<<"KNN分类结果:      "; 

			cout<<classLabel<<"(正确类标号: ";
			cout<<gTestSet[i].classLabel<<")\n";
			fprintf(fp,"rowNo:  %3d   \t  KNN分类结果:  %s ( 正确类标号:  %s )\n",
				gTestSet[i].ID,classLabel,gTestSet[i].classLabel);
			if(strcmp(classLabel,gTestSet[i].classLabel)!=0)//不等时，分类错误
			{
				cout<<"   ***分类错误***\n";
				fprintf(fp,"                                                                      ***分类错误***\n");
			}
			fprintf(fp,"%d-最临近数据:\n",K);
			for(j=0;j<K;j++)
			{
				//cout<<gNearestDistance[j].ID<<"\t"<<gNearestDistance[j].distance<<"\t"<<gNearestDistance[j].classLabel[15]<<endl;
				fprintf(fp,"rowNo:	 %3d   \t   Distance:  %f   \tClassLable:    %s\n",
					gNearestDistance[j].ID,gNearestDistance[j].distance,gNearestDistance[j].classLabel);
			}
			fprintf(fp,"\n"); 
		}
		FalsePositive=curTestSetSize-TruePositive;
		fprintf(fp,"***********************************结果分析**************************************\n",i);
		fprintf(fp,"TP(True positive): %d\nFP(False positive): %d\naccuracy: %f\n",
			TruePositive,FalsePositive,double(TruePositive)/(curTestSetSize-1));
		fclose(fp);
		mResult[m]=double(TruePositive)/(curTestSetSize-1);
		
		TruePositive=0;
		FalsePositive=0;

	}
	cout<<endl<<endl<<"  K = "<< K <<"时训练结果："<<endl;
	double t=0;
	for(int i = 0; i < 10; i++)
	{
		cout<<"  第 " << i + 1 <<" 次测试的正确率为：" << mResult[i] <<endl;
		t+=mResult[i] ;
	}
	cout<<"  十次结果的正确率平均为："<< t/10<<endl;
	getchar();
	return;
}
