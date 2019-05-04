// GA.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include"math.h" 
#include<fstream>
#include<string>
using namespace std;


class individual {
public:
	
	double fitness;
	double x;//浮点数实际值
	int length;//编码串长度
	int *gene;//基因编码串。有时间的话做动态设置精确度

	individual() {
	}
	
	individual(int l) {
		length = l;
		gene = new int[length];
		creatGene_bin();
		toValue_bin();
		calcuFit();
	}
	
	individual(individual& idv) {
		length = idv.length;
		gene = new int[length];
		for (int i = 0;i < length;i++) {
			gene[i] = idv.gene[i];
		}
		refresh();
	}
	
	void creatGene_bin() {
		for (int i = 0;i < length;i++) {
			gene[i] = rand() % 2;
		}
	}

	void toValue_bin() {//将二进制编码串转化为十进制数
		x = 0;
		double temp = 0;
		for (int j = length-1;j >= 0;j--) {
			temp += gene[j]*pow(2,length-j-1);
		}
		x = temp / (pow(2, length)-1) * 9;
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
	int geneLength;//基因编码长度
	int generateNum;//代数
	int maxGenerateNum;//最大代数
	individual *total;//总种群
	individual *parent_A;//亲本A组
	individual *parent_B;//亲本B组	
	population(int n, int p, int b,int l,int g) {
		num = n;
		parentNum = p;
		birthNum = b;
		maxGenerateNum = g;
		geneLength = l;
		generateNum = g;
		total = new individual[100];
		parent_A = new individual[p / 2];
		parent_B = new individual[p / 2];
		for (int i = 0;i < num;i++) {
			individual idv(geneLength);
			total[i] = idv;
		}
	}

	//封装的交换两个体某位基因的函数
	void swap(individual&a, individual&b, int pa,int pb) {
		int temp = a.gene[pa];
		a.gene[pa] = b.gene[pb];
		b.gene[pb] = temp;
	}

	//交叉
	//单点交叉
	void singleCross() {
		for (int i = 0;i < birthNum;i += 2) {
			int position = rand() % geneLength;//交叉点
			int pa = rand() % (parentNum / 2);
			int pb = rand() % (parentNum / 2);
			individual nexta(parent_A[pa]);
			individual nextb(parent_B[pb]);
			for (int j = position;j < geneLength;j++) {
				swap(nexta, nextb, j, j);
			}
			nexta.refresh();
			nextb.refresh();
			total[parentNum + i] = nexta;
			total[parentNum + i + 1] = nextb;
		}
	}
	//均匀交叉
	void avgCross(float crossRate) {
		int *shade = new int[geneLength];//屏蔽字
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
				if (shade[k] == 0)
					continue;
				else if (shade[k] == 1)
					swap(nexta, nextb, k, k);
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
		for (int i = 0;i < birthNum;i++) {
			float m = rand() % 101 / 100;
			if (m < mutRate) {
				for (int j = 0;j < geneLength;j++) {
					float m = rand() % 101 / 100;
					if (m < mutRate) {
						total[i+parentNum].gene[j] = (total[i + parentNum].gene[j] + 1) % 2;
					}
					else continue;
				}
				total[i + parentNum].refresh();
			}
			else continue;
		}
	}

	//非均匀变异
	//一个思路，对求极限函数求导，取导数值为权值控制扰动大小，即对陡峭的区间大幅度扰动（远离极值），对平缓的区间小幅度扰动（靠近极值）
	//参考资料提供了按代数降低扰动的算法原型
	void nonUniformMutation(float mutRate, float rely) {
		for (int i = 0;i < birthNum;i++) {
			float m = rand() % 101 / 100;
			if (m < mutRate) {
				double vx = total[parentNum + i].x;
				float r = rand() % 101 / 100;
				if (rand() % 2 == 0) 
					total[parentNum + i].x += (9 - vx)*(1+50*cos(5*vx)-28*sin(4*vx))*(1-pow(r,(1-generateNum/maxGenerateNum)*rely));
				else total[parentNum + i].x += vx*(1 + 50 * cos(5 * vx) - 28 * sin(4 * vx))*(1 - pow(r, (1 - generateNum / maxGenerateNum)*rely));		
			}
			else continue;
		}
	}
	
	//倒位算子
	void inverseOperator(float p) {
		for (int i = 0;i < birthNum;i++) {
			float don = rand() % 101 / 100;//是否倒位
			if (don < p) {
				int start = rand() % geneLength;
				if (start + 1 != geneLength) {
					int end = start + rand() % (geneLength - start);
					for (int j = 0;j < (end - start) / 2;j++)
						swap(total[i+parentNum], total[i + parentNum], start + j, end - j);
				}
			}
			else continue;
			total[i + parentNum].refresh();
		}
	}

	//选择过程
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
		paStart = 0;
		pbStart = 0;
		for (int k = 0;k < parentNum;k++) {
			if (rand() % 2 == 0) {
				if (paStart < parentNum / 2) {
					total[k] = parent_A[paStart];
					paStart++;
				}
				else if (pbStart < parentNum / 2) {
					total[k] = parent_B[pbStart];
					pbStart++;
				}
				else break;
			}
			else {
				if (pbStart < parentNum / 2) {
					total[k] = parent_B[pbStart];
					pbStart++;
				}
				else if (paStart < parentNum / 2) {
					total[k] = parent_A[paStart];
					paStart++;
				}
				else break;
			}
		}
	}
	
	int getMax() {
		double max=0;
		int maxidv = -1;
		for (int i = 0;i < num;i++) {
			total[i].refresh();
			if (total[i].fitness > max) {
				maxidv = i;
				max = total[i].fitness;
			}
		}
		return maxidv;
	}
};

void printResult(population& p,int genNum,ofstream& res,int max,string name) {
	res << R"(<generate ID=")" <<name<< genNum<< R"(">)"<<endl;
	res <<"<max>"<< p.total[max].x<< R"(</max>)"<<"\n";
	for (int j = 0;j < 100;j++) {
		res << "<X"<<j<<">"<<p.total[j].x <<R"(</X)"<<j<<">"<< endl;
	}
	res << R"(</generate>)" << endl;
	cout << "genetate" << genNum << "done.\n";
}

int main()
{
	double max = 0;
	ofstream outAsXML("out.txt");
	
	//单点交叉种群
	srand(0);
	population test0(100, 10, 89, 24, 100);
	individual best0(24);
	for (int i = 0;i < 100;i++) {
		int max = 0;
		max = test0.getMax();
		individual temp;
		if (test0.total[max].fitness < best0.fitness) {
			individual a(best0);
			temp = a;
		}
		else {
			individual b(test0.total[max]);
			temp = b;
			best0 = b;
		}
		printResult(test0, i, outAsXML, max,"singleCross");
		test0.choose(10);
		test0.singleCross();
		test0.uniformMutation(0.001);
		test0.total[99] = temp;	
	}

	//均匀交叉种群
	srand(1);
	population test1(100, 10, 89, 24, 100);
	individual best1(24);
	for (int i = 0;i < 100;i++) {
		int max = 0;
		max = test1.getMax();
		individual temp;
		if (test1.total[max].fitness < best1.fitness) {
			individual a(best1);
			temp = a;
		}
		else {
			individual b(test1.total[max]);
			temp = b;
			best1 = b;
		}
		printResult(test1, i, outAsXML, max,"avgCross");
		test1.choose(10);
		test1.avgCross(0.1);
		test1.uniformMutation(0.001);
		test1.total[99] = temp;
	}

	//算数交叉种群
	srand(10);
	population test2(100, 10, 89, 24, 100);
	individual best2(24);
	for (int i = 0;i < 100;i++) {
		int max = 0;
		max = test2.getMax();
		individual temp;
		if (test2.total[max].fitness < best2.fitness) {
			individual a(best2);
			temp = a;
		}
		else {
			individual b(test2.total[max]);
			temp = b;
			best2 = b;
		}
		printResult(test2, i, outAsXML, max,"arithmeticCross");
		test2.choose(10);
		test2.arithmeticCross(0.1);
		test2.nonUniformMutation(0.001,1);
		test2.total[99] = temp;
	}
}

