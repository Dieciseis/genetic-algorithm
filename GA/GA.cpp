// GA.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include"math.h" 
using namespace std;
int accurate;//精确到小数点后几位
int generate;//演化代数
double MAX;


class individual {
public:
	
	double fitness;
	double x;//浮点数实际值
	int length;//编码串长度
	int *gene;//基因编码串。有时间的话做动态设置精确度
	int state;//是否超出[0,9]，1超出，0不超,2未初始化

	individual() {
		state = 2;
	}
	
	individual(int l) {
		length = l;
		creatGene_bin();
		toValue_bin();
		calcuFit();
	}
	
	individual(const individual& idv) {
		length = idv.length;
		gene = idv.gene;
	}
	
	void creatGene_bin() {
		for (int i = 0;i < length;i++) {
			gene[i] = rand() % 2;
		}
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

	void calcuFit() {
		fitness = x + 10 * sin(5 * x) + 7 * cos(4 * x);
	}

	void refresh() {//基因编码发生变化后刷新x和适应度
		toValue_bin();
		calcuFit();
	}
};

class population {
	public:
	int num;//种群总数
	int parentNum;//亲本数
	int birthNum;//交叉产生子代个数
	int mutNum;//变异产生个体数
	int geneLength;//基因编码长度
	individual *total;//总种群
	individual *parent_A;//亲本A组
	individual *parent_B;//亲本B组	
	population(int n, int p, int b,int l) {
		num = n;
		parentNum = p;
		birthNum = b;
		for (int i = 0;i < num;i++) {
			individual idv(geneLength);
			total[i] = idv;
		}
	}
	//封装的交换两个体某位基因的函数
	void swap(individual&a, individual&b, int pos) {
		int temp = a.gene[pos];
		a.gene[pos] = b.gene[pos];
		b.gene[pos] = temp;
	}

	//交叉算子
	//单点交叉
	void singleCross() {
		for (int i = 0;i < birthNum;i += 2) {
			int position = rand() % geneLength;//交叉点
			int pa = rand() % (parentNum / 2);
			int pb = rand() % (parentNum / 2);
			individual nexta(parent_A[pa]);
			individual nextb(parent_B[pb]);
			for (int j = position;j < geneLength;j++) {
				swap(nexta, nextb, j);
			}
			nexta.refresh();
			nextb.refresh();
			total[parentNum + i] = nexta;
			total[parentNum + i + 1] = nextb;
		}
	}
	//均匀交叉
	void avgCross(float crossRate) {
		int *shade;//屏蔽字
		for (int i = 0;i < geneLength;i++) {
			float temp = (rand() % 101)/100;
			if (temp < crossRate)
				shade[i] = 0;
			else
				shade[i] = 1;
		}
		for (int j = 0;j < birthNum;j += 2) {
			int pa = rand() % (parentNum / 2);
			int pb = rand() % (parentNum / 2);
			individual nexta(parent_A[pa]);
			individual nextb(parent_B[pb]);
			for (int k = 0;k < geneLength;k++) {
				if (shade[j] == 0)
					continue;
				else if (shade[j] == 1)
					swap(nexta, nextb, j);
				else { cout << "error!";exit(4); }
			}
			nexta.refresh();
			nextb.refresh();
			total[parentNum + j] = nexta;
			total[parentNum + j + 1] = nextb;
		}
	}
	//算术交叉
	void arithmeticCross(float crossRate) {
		for (int i = 0;i < birthNum;i += 2) {
			int pa = rand() % (parentNum / 2);
			int pb = rand() % (parentNum / 2);
			individual nexta(parent_A[pa]);
			individual nextb(parent_B[pb]);
			double xa = nexta.x;
			double xb = nextb.x;
			nexta.x = crossRate * xa + (1 - crossRate) * xb;
			nextb.x = crossRate * xb + (1 - crossRate) * xa;
			total[parentNum + i] = nexta;
			total[parentNum + i + 1] = nextb;
		}
	}


	//变异算子
	//均匀变异
	void uniformMutation(float mutRate) {//变异率
		int choose;
		for (int i = 0;i < mutNum;i++) {
			choose = rand() % parentNum;
			individual next(total[choose]);
			for (int j = 0;j < geneLength;j++) {
				float m = rand() % 101 / 100;
				if (m < mutRate) {
					next.gene[j] = (next.gene[j] + 1) % 2;
				}
				else continue;
			}
			next.refresh();
			total[parentNum + birthNum + i] = next;
		}
	}

	//非均匀变异

	//选择算子
	void choose(int tournamentSize) {
		float maxFit = -65536;
		int idvNum = 0;
		int paStart = 0 ;
		int pbStart = 0;
		for (int i = 0;i < parentNum;i++) {
			for (int j = 0;j < tournamentSize;j++) {
				int c = rand() % num;
				if (total[c].fitness > maxFit)
					idvNum = c;
				else continue;
			}
			individual chosen(total[idvNum]);
			if (paStart < parentNum / 2) {
				parent_A[paStart] = chosen;
				paStart++;
			}
			else if (pbStart < parentNum / 2) {
				parent_B[pbStart] = chosen;
				pbStart++;
			}
			else break;
		}
	}
};

int main()
{
    
}

