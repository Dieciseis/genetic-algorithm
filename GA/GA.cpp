// GA.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include"math.h" 
using namespace std;
int accurate;//精确到小数点后几位
int generate;//演化代数
double max;


struct individual {
	double fitness;
	double x;
	int length;//编码串长度
	int *gene;//基因编码串。有时间的话做动态设置精确度
	int state;//是否超出[0,9]，1超出，0不超
	void creatGene_bin() {
		for (int i = 0;i < length;i++) {
			gene[i] = rand() % 2;
		}
	}

	void creatGene_float() {
		
	}
	void toValue_bin() {//将二进制编码串转化为十进制数
		x = 0;
		state = 0;
		int temp = 0;
		for (int j = 0;j < length;j++) {
			temp += gene[length - j - 1];
			temp *= 2;
		}
		x = double(temp) / pow(10, accurate);
		if (x > 9 || x < 0)
			state = 1;
	}

	void toValue_float() {
		
	}
	void calcuFit() {
		fitness = x + 10 * sin(5 * x) + 7 * cos(4 * x);
	}
};

struct population {
	int num;//种群总数
	individual* p;//种群
};

//交叉算子


//变异算子


int main()
{
    
}

