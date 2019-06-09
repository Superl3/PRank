
#include<iostream>
#include<algorithm>
#include<vector>
#include<string>
#include<fstream>
using namespace std;
vector<int> nodeList;
const float constant = 0.8, beta = 0.5;
int nodeCnt;
bool graph[1000][1000];
float indp[1000][1000][6];
float outdp[1000][1000][6];
float pRankDP[1000][1000][6];
vector<int> in[1000], out[1000];
float SimRank(int p, int q, int step) {

	if (indp[p][q][step] != 0) {
		return indp[p][q][step] != -1 ? indp[p][q][step] : 0;
	}

	if (step == 0) {
		indp[p][q][step] = indp[q][p][step] = p == q ? 1 : -1;
		return p == q;
	}

	float ret = 0;
	if (p == q)
		ret = 1;

	else {
		int pSize = in[p].size(), qSize = in[q].size();
		for (int i = 0; i < pSize; i++) {
			for (int j = 0; j < qSize; j++) {
				ret += SimRank(in[p][i], in[q][j], step - 1);
			}
		}
		if (pSize * qSize != 0)
			ret *= constant / (pSize * qSize);
	}
	//ret = round(ret * 100000) / 100000;
	indp[p][q][step] = indp[q][p][step] = ret != 0 ? ret : -1;
	return ret;
}
float rvsSimRank(int p, int q, int step) {

	if (outdp[p][q][step] != 0) {
		return outdp[p][q][step] != -1 ? outdp[p][q][step] : 0;
	}

	if (step == 0) {
		outdp[p][q][step] = outdp[q][p][step] = p == q ? 1 : -1;
		return p == q;
	}

	float ret = 0;
	if (p == q)
		ret = 1;

	else {
		int pSize = out[p].size(), qSize = out[q].size();
		for (int i = 0; i < pSize; i++) {
			for (int j = 0; j < qSize; j++) {
				ret += rvsSimRank(out[p][i], out[q][j], step - 1);
			}
		}
		if (pSize * qSize != 0)
			ret *= constant / (pSize * qSize);
	}
	//ret = round(ret * 100000) / 100000;
	outdp[p][q][step] = outdp[q][p][step] = ret != 0 ? ret : -1;
	return ret;
}
float pRank(int p, int q, int step) {

	if (pRankDP[p][q][step] != 0) {
		return pRankDP[p][q][step] != -1 ? pRankDP[p][q][step] : 0;
	}

	if (step == 0) {
		return p == q;
	}

	float ret = 0;
	if (p == q)
		ret = 1;

	else {
		float inVal = 0, outVal = 0;
		int pSize, qSize;

		pSize = in[p].size();
		qSize = in[q].size();
		for (int i = 0; i < pSize; i++) {
			for (int j = 0; j < qSize; j++) {
				inVal += SimRank(in[p][i], in[q][j], step - 1);
			}
		}
		if (pSize * qSize != 0)
			inVal *= (constant * beta) / (pSize * qSize);

		pSize = out[p].size();
		qSize = out[q].size();
		for (int i = 0; i < pSize; i++) {
			for (int j = 0; j < qSize; j++) {
				outVal += rvsSimRank(out[p][i], out[q][j], step - 1);
			}
		}
		if (pSize * qSize != 0)
			outVal *= (constant * (1 - beta)) / (pSize * qSize);

		ret = inVal + outVal;
	}
	//ret = round(ret * 100000) / 100000;
	pRankDP[p][q][step] = pRankDP[q][p][step] = ret != 0 ? ret : -1;
	return ret;
}
void LookUpScore(int node_1, int node_2, int iterationNo) {
	cout << pRank(node_1, node_2, iterationNo);
}
void compute_pRank(int step) {
	string s = "ResultOnIteration_";
	s += (char)(step + '0');
	s += ".txt";

	FILE *out;
	fopen_s(&out, s.c_str(), "w");

	for (int i = 0; i < nodeCnt; i++) {
		for (int j = 0; j < nodeCnt; j++) {
			float val = pRank(nodeList[i], nodeList[j], step);
			if (val != 0)
				fprintf(out, "%d,%d : %.5f\n", nodeList[i], nodeList[j], val);

		}
	}
	fclose(out);

}

int main(int argc, char **argv) {
	bool compute = false, calc = false;
	int node_1, node_2, iterationNo;
	for (int i = 1; i < argc; i++) {
		string str = argv[i];
		if (str == "compute")
			compute = true;
		else if(!calc){
			node_1 = atoi(argv[i]);
			node_2 = atoi(argv[i + 1]);
			iterationNo = atoi(argv[i + 2]);
			calc = true;
		}
	}
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
	sort(nodeList.begin(), nodeList.end());
	for (int i = 0; i < nodeCnt; i++) {
		for (int j = 0; j < nodeCnt; j++) {
			if (graph[nodeList[i]][nodeList[j]]) {
				in[nodeList[j]].push_back(nodeList[i]);
				out[nodeList[i]].push_back(nodeList[j]);
			}
		}
	}
	if (calc) {
		LookUpScore(node_1, node_2, iterationNo);
	}
	if(compute)
		for (int i = 1; i <= 5; i++)
			compute_pRank(i);
}