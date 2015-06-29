#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

using namespace std;

class Node {
private:
	int id;
	int in_degree;
	int out_degree;
	float toInf;
	float fromInf;
	vector<int> in_neighbor;
	vector<int> out_neighbor;

	/*	
	save obj to file without get_method()

	friend ostream& operator<<(ostream& os, Node& node) {
		os << node.id << "\t" << node.in_degree << "\t" << node.out_degree << endl;
		
		for (vector<int>::iterator iter = node.neighbor_out.begin(); iter != node.neighbor_out.end(); iter++) {
			os << *iter << "\t";
		}
		os << endl;
		for (vector<int>::iterator iter = node.neighbor_in.begin(); iter != node.neighbor_in.end(); iter++) {
			os << *iter << "\t";
		}
		os << endl;
		return os;
	}
	*/
public:
	Node();
	Node(int source, int target, bool flag);
	int get_id();
	int get_indegree();
	int get_outdegree();
	float get_toinf();
	float get_fromInf();
	vector<int> get_in_neighbor();
	vector<int> get_out_neighbor();

	void set_id(int _id);

	void add_outneighbor(int neighbor);
	void add_inneighbor(int neighbor);
	void writeTofile(ofstream *out);
	void readFromfile(ifstream *in);

	void print_neighbor();
};
/* constructor for each node*/
Node::Node() {
	id = 0;
	toInf = 0.0;
	fromInf = 0.0;
	in_degree = 0;
	out_degree = 0;
}
Node::Node(int source, int target, bool flag) {
	/* for source node */
	if (flag == false) {
		id = source;
		out_neighbor.push_back(target);
		out_degree += 1;
	}
	/* for target node */
	else {
		id = target;
		out_neighbor.push_back(source);
		in_degree += 1;
	}
	toInf = 0.0;
	fromInf = 0.0;
	in_degree = 0;
	out_degree = 0;
}

/* getter */
int Node::get_id() {
	return id;
}
int Node::get_indegree() {
	return in_degree;
}
int Node::get_outdegree() {
	return out_degree;
}
float Node::get_toinf() {
	return toInf;
}
float Node::get_fromInf() {
	return fromInf;
}
vector<int> Node::get_in_neighbor(){
	return in_neighbor;
}
vector<int> Node::get_out_neighbor(){
	return out_neighbor;
}

/* setter */
void Node::set_id(int _id) {
	id = _id;
}
void Node::add_outneighbor(int neighbor) {
	out_neighbor.push_back(neighbor);
	out_degree += 1;
}
void Node::add_inneighbor(int neighbor) {
	in_neighbor.push_back(neighbor);
	in_degree += 1;
}

/* read/write a object from/to */
void Node::writeTofile(ofstream *out) {
	out->write((char*) this, sizeof(this));
}
void Node::readFromfile(ifstream *in) {
	in->read((char*) this, sizeof(this));
}
/* for debuggin */
void Node::print_neighbor() {
	cout << "Node id: " << id << endl;
	cout << "out-neighbor: " << out_neighbor.size() << endl;
	for (vector<int>::iterator iter = out_neighbor.begin(); iter != out_neighbor.end(); iter++) {
		cout << *iter << "\t";
	}
	cout << endl;
	cout << "in-neighbor: " << in_neighbor.size() << endl;
	for (vector<int>::iterator iter = in_neighbor.begin(); iter != in_neighbor.end(); iter++) {
		cout << *iter << "\t";
	}
	cout << endl;
}
/* function define */
void preprocessing(string filename, Node node[]);
float TargerBasedInfluence(Node node[], int i, float thresh);

int main() {
	int seed_size = 0;
	float path_threshold = 0.01;

	Node* node = new Node[281904];	//	There are 281,903 nodes 2312497 edges in stanford data.
	preprocessing("web-Stanford.txt", node);
	
	/* data structure for CELF Queue, sorted by toInfluence */
	multimap< float, Node > Nodes;
	multimap< float, Node >::reverse_iterator Iter_node;
	typedef pair< float, Node > NodePair;

	/* 
	select first node and initiate CELF Queue
	Node[] index from 1 to 281,903 	
	*/
	for (int i = 1; i < 281904; i++){
		/* small size for testing */
		if (node[i].get_id() == 0){
			continue;
		}
		else{
			float tmp_inf = TargerBasedInfluence(node, i, path_threshold);
			cout << tmp_inf << endl;
			char tmp;
			cout << "for viewing the intermediate values" << endl;
			cin >> tmp;
		}
	}

	//Nodes.insert(NodePair(test1.get_outdegree(), test1));


	for (Iter_node = Nodes.rbegin(); Iter_node != Nodes.rend(); ++Iter_node){
		Iter_node->second.get_id();
		Iter_node->first;
	}

	return 0;
}
/* preprocessing the origin data*/
void preprocessing(string filename, Node node[]) {
	ifstream fin;
	fin.open(filename);
	string line;

	/* except header*/
	fin.seekg(169);

	int i = 0;
	while (getline(fin, line)) {
		int tmp = 0;
		char tmp_strTochar[30];
		/* string line to char array for strtok_s()*/
		strcpy_s(tmp_strTochar, 30, line.c_str());

		/*separate source and target by using strtok_s()*/
		char* target = NULL;
		char* source = strtok_s(tmp_strTochar, "\t", &target);

		/* assign values*/
		int idx_source = atoi(source);
		int idx_target = atoi(target);

		/* if source node not initialized */
		if (node[idx_source].get_id() == 0) {
			node[idx_source].set_id(idx_source);
			node[idx_source].add_outneighbor(idx_target);
		}
		else {
			node[idx_source].add_outneighbor(idx_target);
		}
		/* if target node not initialized */
		if (node[idx_target].get_id() == 0) {
			node[idx_target].set_id(idx_target);
			node[idx_target].add_inneighbor(idx_source);
		}
		else {
			node[idx_target].add_inneighbor(idx_source);
		}

		/* check current % */
		i++;
		if ((i%230000) == 0) {
			cout << (i/230000)*10 <<" % read" << endl;
			break;
		}
	}
	cout << "Input file read successfully" << endl;
	fin.close();
}
/* computing the influence of a node based on target nodes */
float TargerBasedInfluence(Node node[], int i, float thresh){
	int outdegree = node[i].get_outdegree();
	cout << "id:" << i << endl;
	cout << "outdegree: " << outdegree << endl;
	/* if there is no out-neighbor */
	if (outdegree == 0){
		return 1;
	}
	/* computing influence of children recursively */
	else{
		float inf = 0.0;
		vector<int> out_neighbor = node[i].get_out_neighbor();
		for (vector<int>::iterator iter = out_neighbor.begin(); iter != out_neighbor.end(); iter++){
			float inf_child = TargerBasedInfluence(node, *iter, thresh);
			float weight_child = (1 / node[*iter].get_indegree());
			inf += weight_child*(1 + inf_child);
		}
		return inf;
	}
}