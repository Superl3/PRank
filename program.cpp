#include<iostream>
#include<vector>
#include<string>
#include<fstream>
using namespace std;
vector<int> nodeList;
int nodeCnt;
bool graph[1000][1000];
double indp[1000][1000][6];
double outdp[1000][1000][6];
double pRankDP[1000][1000][6];
vector<int> in[1000], out[1000];
double SimRank(const float constant, int p, int q, int step) {

	if (indp[p][q][step] != 0) {
		return indp[p][q][step] != -1 ? indp[p][q][step] : 0;
	}

	if (step == 0) {
		indp[p][q][step] = indp[q][p][step] = p == q ? 1 : -1;
		return p == q;
	}

	double ret = 0;
	if (p == q)
		ret = 1;

	else {
		int pSize = in[p].size(), qSize = in[q].size();
		for (int i = 0; i < pSize; i++) {
			for (int j = 0; j < qSize; j++) {
				ret += SimRank(constant, in[p][i], in[q][j], step - 1);
			}
		}
		if (pSize * qSize != 0)
			ret *= constant / (pSize * qSize);
	}
	indp[p][q][step] = indp[q][p][step] = ret != 0 ? ret : -1;
	return ret;
}
double rvsSimRank(const float constant, int p, int q, int step) {

	if (outdp[p][q][step] != 0) {
		return outdp[p][q][step] != -1 ? outdp[p][q][step] : 0;
	}

	if (step == 0) {
		outdp[p][q][step] = outdp[q][p][step] = p == q ? 1 : -1;
		return p == q;
	}

	double ret = 0;
	if (p == q)
		ret = 1;

	else {
		int pSize = out[p].size(), qSize = out[q].size();
		for (int i = 0; i < pSize; i++) {
			for (int j = 0; j < qSize; j++) {
				ret += rvsSimRank(constant, out[p][i], out[q][j], step - 1);
			}
		}
		if (pSize * qSize != 0)
			ret *= constant / (pSize * qSize);
	}
	outdp[p][q][step] = outdp[q][p][step] = ret != 0 ? ret : -1;
	return ret;
}
double pRank(const float constant, const float beta, int p, int q, int step) {

	if (pRankDP[p][q][step] != 0) {
		return pRankDP[p][q][step] != -1 ? pRankDP[p][q][step] : 0;
	}

	if (step == 0) {
		return p == q;
	}

	double ret = 0;
	if (p == q)
		ret = 1;

	else {
		double inVal = 0, outVal = 0;
		int pSize, qSize;

		pSize = in[p].size();
		qSize = in[q].size();
		for (int i = 0; i < pSize; i++) {
			for (int j = 0; j < qSize; j++) {
				inVal += SimRank(constant, in[p][i], in[q][j], step - 1);
			}
		}
		if(pSize * qSize != 0)
			inVal *= (constant * beta) / (pSize * qSize);

		pSize = out[p].size();
		qSize = out[q].size();
		for (int i = 0; i < pSize; i++) {
			for (int j = 0; j < qSize; j++) {
				outVal += rvsSimRank(constant, out[p][i], out[q][j], step - 1);
			}
		}
		if (pSize * qSize != 0)
			outVal *= (constant * (1 - beta)) / (pSize * qSize);

		ret = inVal + outVal;
	}
	pRankDP[p][q][step] = pRankDP[q][p][step] = ret != 0 ? ret : -1;
	return ret;
}
void compute_pRank(const float constant, float beta, int step) {
	string s = "ResultOnIteration_";
	s += (char)(step + '0');
	s += ".txt";
	ofstream fout(s);
	for (int i = 0; i < nodeCnt; i++) {
		for (int j = 0; j < nodeCnt; j++) {
			fout << nodeList[i] << ',' << nodeList[j] << " :" << pRank(constant, beta, nodeList[i], nodeList[j], step) << '\n';
		}
	}
	fout.close();
}

int main(int argc, char **argv) {
	int a, b;
	ifstream fin("graph.txt");
	int chk[1001];
	while (!fin.eof()) {
		fin >> a >> b;
		if (chk[a] != 1) nodeList.push_back(a);
		if (chk[b] != 1) nodeList.push_back(b);
		chk[a] = chk[b] = 1;
		graph[a][b] = true;
	}
	fin.close();
	nodeCnt = nodeList.size();

	for (int i = 0; i < nodeCnt; i++) {
		for (int j = 0; j < nodeCnt; j++) {
			if (graph[nodeList[i]][nodeList[j]]) {
				in[nodeList[j]].push_back(nodeList[i]);
				out[nodeList[i]].push_back(nodeList[j]);
			}
		}
	}
	//cout << SimRank(0.8, 8, 9, 2);
	//compute_pRank(0.8, 0.5, 2);
	for (int i = 1; i <= 5; i++)
		compute_pRank(0.8, 0.5, i);


}