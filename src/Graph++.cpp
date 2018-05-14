//============================================================================
// Name        : MIS-Heuristic.cpp
// Author      : Sergelenbayar Ganbaatar
// Description : if you work in graph with more than 100 nodes,
//	  	 I recommended to you don't use localSearch procedure. Another word
//		 if you use localSearchItr = 2, it will work very slowly, and you don't wish it.
//		 you can use localSearchItr = 0. In other case, maxItr set equal to k, 
//	         and the localSearchItr set to 2.
//
//============================================================================
#include <fstream>
#include <iostream>
#include <vector>
#include <iterator>
#include "Node.h"
#include <cstdlib>
#include <time.h>
#include <algorithm>
#include <map>
#include <set>

using namespace std;

void LocalSearch(vector<Node> &G);
vector<Node> CriticalNode(vector<Node> G, int k);
vector<Node> CriticalNodeLS(vector<Node> G,int k);

//this call computMIS() until all node's isRemoved
//attribute become true
vector<Node> mainComputeMIS(vector<Node> G);
//compute MIS elements
void computeMIS(vector<Node> &v);
//this function generate random values([0,1])
//this values need to find MIS elements
vector<double> randValsGenerate(int size);
//this recursive function iterate component's elements
void checkElementsCurrCom(Node node, bool visited[]);
//if all node's isRemoved attribute is true,
//return true, else false
bool IsAllItemDeleted(vector<Node> G);
//get objective function of G and return
int getObjVal(vector<Node> G);
//read input file and build main graph using input data
void buildGraph();
//set initial values to properties
void setInit();
//print computed result
void printResult(vector<Node> Xod);
//print graph at parameter
void printGraph(vector<Node> graph);
bool checkError(vector<Node> G);
//src/WattsStrogatz_n250.txt
//src/ForestFire_n250.txt

//input file's path
string filePath = "src/terroristNetwork62.txt";
//count of Critical node
int k = 10	;
//iteration count of loop at CriticalNodeLS()
int maxIter = k;
//iteration count of loop at localSearch
int localSearchItr = 2;

struct Components{
	map<int, Node> elements;
	bool isRemoved;
	Components(const bool x, map<int, Node> el)
	: isRemoved(x), elements(el){}
};
//main vector of this program,
//which contains data of input file
vector <Node> V;
//for use: get objective value of Graph
vector<Node> dynamicGraph;
//this vector contains real index of components
//root fake index at node's componentIndexSet to real index at allComponents
vector<int> componentRoot;
//this vector contains all components of Graph
vector<Components> allComponents;
//save last index of allComponents
int componentIndex=0;
//element count of current component
int elCounter = 1;
//size of input graph
int Graph_Size=0;

///set initial values to properties
void setInit(){
	dynamicGraph.clear();
	componentRoot.clear();
	allComponents.clear();
	componentIndex = 0;
	elCounter = 1;
}
int main() {
	//read data from input file and build graph
	buildGraph();
	clock_t begin = clock();
	Graph_Size = V.size();
	if(checkError(V))
		printResult(CriticalNodeLS(V, k));

	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	cout << "****** EXECUTION TIME "<< time_spent<<" second ******"<<endl;
	return 0;
}
//check format and error of input graph
//if occur error, return false
bool checkError(vector<Node> G){
	bool sentinel=true;
	for(vector<Node>::iterator it=G.begin(); it!=G.end(); ++it)
	{
		for(map<int, Node>::iterator nit=(*it).neighbors.begin();
				nit!=(*it).neighbors.end();++nit)
		{
			cout << (nit->second).getVal()<<", ";
			sentinel = true;
			for(map<int, Node>::iterator snit=G.at((nit->second).getVal()).neighbors.begin();
					snit!=G.at((nit->second).getVal()).neighbors.end();++snit)
			{
				if((snit->second).getVal()==(*it).getVal()){
					sentinel = false;
				}
			}
			if(sentinel && G.at((nit->second).getVal()).neighbors.size() > 0){
				cout<<"Error: "<<(*it).getVal()<<" of "<<(nit->second).getVal()<<endl;
				break;
			}
		}
		cout<<endl;
		if(sentinel && (*it).neighbors.size() > 0) break;
	}
	if(!sentinel){
		cout<<"not found error"<<endl;
		return true;
	}else return false;
}
vector<Node> CriticalNodeLS(vector<Node> G,int k){
	//this vector will contain better result
	vector<Node> Xod;
	vector<Node> X;
	int objValXod = INT_MAX;

	for(int i=0; i<maxIter;i++){
		setInit();
		X.clear();

		X = CriticalNode(G, k);
		LocalSearch(X);

		int objValX = getObjVal(X);
		printf("%d-th iterator: objVal=%d\n", i,objValX);

		if(objValX < objValXod){
			Xod.clear();
			Xod = X;
			objValXod = objValX;
		}
	}
	return Xod;
}

vector<Node> CriticalNode(vector<Node> V, int k){
	//compute initial MIS from V graph and set values to computedMIS
	vector<Node> computedMIS = mainComputeMIS(V);

	//current componentIndex is allComponents's last index
	//Another word this contain allComponents's size
	if(componentIndex >= Graph_Size - k){
		return computedMIS;
	}

	vector<Node>::iterator minEl;
	int misSize = componentIndex;
	//initial objective value of computedMIS
	int oldNetObjVal = getObjVal(computedMIS);
	//add element to MIS one by one, the element create minimum objective function
	while(misSize != Graph_Size-k){
		int min = INT_MAX;
		//find element, which is create minimum objective function
		for(vector<Node>::iterator it=computedMIS.begin(); it!=computedMIS.end(); ++it)
		{
			//iterate on V\MIS
			if(!(*it).isMarked){
				int itemCount=1;
				//this vector contains not duplicated real index of allComponents
				std::set<int> allComponentsRealIndexSet;
				//set values to allComponentsRealIndexSet
				for(vector<int>::iterator cit=(*it).componentIndexSet.begin();
						cit!=(*it).componentIndexSet.end();++cit){
					int realIndex = componentRoot.at((*cit));
					allComponentsRealIndexSet.insert(realIndex);
				}
				//sum of objVals of current node's connected components
				int oldObjValsOfComponents = 0;

				for(set<int>::iterator rit = allComponentsRealIndexSet.begin();
						rit!= allComponentsRealIndexSet.end();++rit){
					int count = allComponents.at((*rit)).elements.size();
					oldObjValsOfComponents += (count * (count-1)) / 2;
					itemCount += count;
				}
				//this compute change of objective function after added current node to MIS
				int objFuncVal = oldNetObjVal - oldObjValsOfComponents +(itemCount * (itemCount-1)) / 2;
				if(min > objFuncVal){
					min = objFuncVal;
					minEl = it;
					cout<<"\nMin El: "<<(*it).getVal()<<" ObjVal: "<<min;
				}
			}
		}
		//iterate on neighbors of minEl and combine required allComponents
		{
			int firstCompIndex=0;
			//this vector contains not duplicated real index
			// of current node's componentIndexSet property
			std::set<int> allComponentsRealIndexSet;
			oldNetObjVal = min;
			//set values to allComponentsRealIndexSet
			for(vector<int>::iterator cit=(*minEl).componentIndexSet.begin();
					cit!=(*minEl).componentIndexSet.end();++cit){

				int realIndex = componentRoot.at((*cit));
				allComponentsRealIndexSet.insert(realIndex);
			}
			int indexOfCompRoot = -1;
			//iterate on connectedComponent of minEl and
			//combine elements of connected components to first component
			for(set<int>::iterator rit = allComponentsRealIndexSet.begin();
					rit!= allComponentsRealIndexSet.end();++rit)
			{
				int realIndex = (*rit);

				if(rit==allComponentsRealIndexSet.begin()){

					firstCompIndex = realIndex;
					allComponents.at(firstCompIndex).elements
							.insert(pair<int,Node>((*minEl).getVal(), (*minEl)));

				}else if(realIndex != firstCompIndex){
					//iterate on connected allComponents and combine
					//to first component from other allComponents
					for(map<int, Node>::iterator it = allComponents.at(realIndex).elements.begin();
							it != allComponents.at(realIndex).elements.end(); ++it)
					{
						allComponents.at(firstCompIndex).elements
								.insert(pair<int,Node>((*it).second.getVal(), (*it).second));
					}
					allComponents.at(realIndex).isRemoved = true;
					//update root index of other components at componentRoot by
					//first component's index
					for(unsigned int i = 0; i < componentRoot.size(); i++)
					{
						if(componentRoot.at(i) == realIndex)
						{
							componentRoot.at(i) = firstCompIndex;
						}
					}
				}
			}
			indexOfCompRoot = componentRoot.at(firstCompIndex);
			//add index of created component to componentIndexSet of minEl's neighbors
			for(map<int, Node>::iterator nit=(*minEl).neighbors.begin();
					nit!=(*minEl).neighbors.end();++nit){
				if(!computedMIS.at((nit)->second.getVal()).isMarked){
					computedMIS.at((nit)->second.getVal()).componentIndexSet.push_back(indexOfCompRoot);
				}
			}
			cout<<"Added element to "<<firstCompIndex
					<<"-th component: "<< (*minEl).getVal()<<endl;

			computedMIS.at((*minEl).getVal()).componentIndexSet.clear();
			computedMIS.at((*minEl).getVal()).componentIndexSet
					.push_back(indexOfCompRoot);
			computedMIS.at((*minEl).getVal()).isMarked =true;
			misSize++;
		}
	}
	cout<<"\n end of computeMIS\n";
	return computedMIS;
}
void LocalSearch(vector<Node> &G){
	//this vector contain better result of net results
	vector<Node> Xod = G;
	cout<<"+++++Local Search Called+++++\n";
	//iteration count
	int improvement_count = localSearchItr;
	while(improvement_count != 0)
	{
		cout<<">>>>>>>>>>>>>>>>>>";
		bool isFirstItr = true;
		int xObjVal;
		for(vector<Node>::iterator iNode = G.begin();
				iNode!=G.end(); ++iNode)
		{
			//iterate on MIS elements
			if((*iNode).isMarked == true)
			{
				for(vector<Node>::iterator jNode = G.begin();
						jNode!=G.end(); ++jNode)
				{
					//iterate on V\MIS elements
					if((*jNode).isMarked == false){
						//MIS <- MIS \ iNode
						G.at((*iNode).getVal()).isMarked = false;
						//MIS <- MIS + jNode
						G.at((*jNode).getVal()).isMarked = true;
						cout<<"iNode: "<<(*iNode).getVal()<<"  jNode: "<<(*jNode).getVal()<<endl;

						int afterObjVal = getObjVal(G);

						if(isFirstItr == true){
							xObjVal = getObjVal(Xod);
							isFirstItr = false;
						}
						//if after swap objective Value of G less than xObjVal
						//then copy G to Xod
						if(afterObjVal < xObjVal){
							Xod.clear();
							Xod = G;
							xObjVal = afterObjVal;
						}else{//undo swap
							//MIS <- MIS \ iNode
							G.at((*iNode).getVal()).isMarked = true;
							//MIS <- MIS + jNode
							G.at((*jNode).getVal()).isMarked = false;
						}
					}
				}
			}
		}
		improvement_count--;
	}
	G.clear();
	G = Xod;
}
void buildGraph(){
	ifstream fin;
	char ch;

	fin.open(filePath);
	int index=0;
	bool sentinel=false;
	bool isSpace=false;
	cout << "build Graph begin"<<endl;
	while(fin){
		if(ch=='\n' || ch==':'){
			if(ch==':'){
				//create node
				Node node;
				node.setVal(index);
				V.push_back(node);
			}
			index = 0;
			isSpace = false;
		}else if(ch==' '){

			if(isSpace && sentinel){
				Node nn;
				nn.setVal(index);
				V.back().neighbors.insert(pair<int, Node>(index, nn));
				sentinel = false;
			}
			isSpace = true;
			index = 0;
		}

		if((ch >='0' && ch <= '9')){
			index = 10*index + ((int)ch - 48);
			sentinel = true;
		}

		fin.get(ch);
		//cout << ch;
	}
	fin.close();
}
vector<Node> mainComputeMIS(vector<Node> G){

	while(!IsAllItemDeleted(G)){
		computeMIS(G);
	}
	return G;

}
//if all items are removed, return true
bool IsAllItemDeleted(vector<Node> G){
	for (int i=G.size()-1; i > 0; --i) {
		if(!G.at(i).isRemoved){
			return false;
		}
	}
	return true;
}

void computeMIS(vector<Node> &Vgraph){
	srand(time(0));
	vector<double> randVals;
	vector <int> :: iterator itr;
	int Vsize = Vgraph.size();
	//generate random values
	randVals = randValsGenerate(Graph_Size);
	bool sentinel;
	//iterate on elements of graph and choose element with minimum randValue
	for(int k = 0; k< Vsize; k++){
		//iterate on elements with isRemoved property is false
		if(!Vgraph.at(k).isRemoved){
			sentinel = true;
			int tmpVal = Vgraph.at(k).getVal();
			//if at least one of the neighbors of current node
			//is removed or marked, then sentinel false and break loop
			//else will be sentinel true
			for(map<int, Node>::iterator Vit = Vgraph.at(k).neighbors.begin();
					Vit!=Vgraph.at(k).neighbors.end(); ++Vit)
			{
				int nindex = (Vit)->second.getVal();
				if(Vgraph.at(nindex).isMarked == true || ( !Vgraph.at(nindex).isRemoved
						&& randVals.at(tmpVal) > randVals.at(nindex))){
					sentinel = false;
					break;
				}
			}
			//if sentinel is true,marked current element and
			//remove its neighbors. And create new component,
			//add current element to this component and add
			//new component index to componentIndexSet of its neighbors
			if(sentinel){
				Vgraph.at(k).isMarked = true;
				for(map<int, Node>::iterator nbit = (Vgraph.at(k)).neighbors.begin();
						nbit != (Vgraph.at(k)).neighbors.end(); ++nbit){
					int nborIndex = (nbit)->second.getVal();
					//remove neighbors of current node
					if(!Vgraph.at(nborIndex).isRemoved){
						Vgraph.at(nborIndex).isRemoved = true;
					}
					//add component index to componentIndexSet of its neighbors
					Vgraph.at(nborIndex).componentIndexSet.push_back(componentIndex);
				}
				(Vgraph.at(k)).isRemoved = true;
				(Vgraph.at(k)).componentIndexSet.push_back(componentIndex);
				//create new component and add current node to this component
				map<int, Node> newComp ;
				newComp.insert(pair<int, Node>(Vgraph.at(k).getVal(), Vgraph.at(k)));
				allComponents.push_back(Components(false, newComp));
				componentRoot.push_back(componentIndex);
				componentIndex++;
			}
		}
	}
}

int getObjVal(vector<Node> G){
	dynamicGraph.clear();
	dynamicGraph = G;
	int objVal = 0;
	bool *visited = new bool[dynamicGraph.size()];
	// Mark all the nodes as not visited
	for(unsigned int v = 0; v < dynamicGraph.size(); v++)
		visited[v] = false;
	//this iterate marked and not visited nodes,
	//because marked nodes are not critical nodes
	for(unsigned int k=0; k < dynamicGraph.size(); k++){
		elCounter = 1;
		if(dynamicGraph.at(k).isMarked == true && visited[k]==false){
			//current component size
			int compSize=0;

			checkElementsCurrCom(dynamicGraph.at(k), visited);

			compSize = elCounter;
			//computing objective function
			objVal += (compSize*(compSize-1))/2;
		}
	}
	return objVal;
}
void checkElementsCurrCom(Node node, bool visited[]){
	// Mark the current node as visited
	visited[node.getVal()] = true;
	//iterate marked and not visited neighbors of current node
	for(map<int, Node>::iterator Vit = (node).neighbors.begin();
			Vit!=(node).neighbors.end(); ++Vit){
		if(!visited[(Vit)->second.getVal()]
		            && dynamicGraph.at((Vit)->second.getVal()).isMarked == true)
		{
			checkElementsCurrCom(dynamicGraph.at((Vit)->second.getVal()),  visited);
			//count nodes of current component
			elCounter++;
		}

	}
	return;
}
void printGraph(vector<Node> graph){
	cout<<"------Print Begin------"<<endl;
	//print nodes of graph
	for(vector<Node>::iterator it = graph.begin();
			it!= graph.end(); it++){
		cout << (*it).getVal()<<"(R:"<<(*it).isRemoved<<
				", M"<<(*it).isMarked<<" : ";
		//print neighbors of current node
		for(map<int, Node>::iterator nbrs = (*it).neighbors.begin();
				nbrs!=(*it).neighbors.end(); ++nbrs){
			cout << (nbrs)->second.getVal()<<"(R:"<<(nbrs)->second.isRemoved<<
					", M"<<(nbrs)->second.isMarked<<")";
		}
		printf("\n");
	}
	cout << "size"<< graph.size()<<endl;
}
//print computed result
void printResult(vector<Node> Xod){
	int elCounter = 0;

	cout <<"\n<<<<<<<< k nodes >>>>>>>>\n";
	for(vector<Node>::iterator it=Xod.begin(); it!=Xod.end(); ++it){
		if((*it).isMarked==false){
			cout<<" "<<(*it).getVal();
			elCounter++;
		}
	}
	int objValX = getObjVal(Xod);
	cout<<"\n<<<<<<<<<<<<<<<"<<elCounter<<">>>>>>>>>>>>>>>";
	cout <<"\nNet ObjVal = "<<objValX<<endl;
}
//generate random values
vector<double> randValsGenerate(int size){
	vector<double> randVals;
	randVals.reserve(size);
	//set random value to every node
	for(int i=0; i < size; i++){
		double rnum = (double)rand();
		//compute random values between 0 to 1, and set to node
		randVals.emplace_back(rnum/(rnum+rand()));
	}
	return randVals;
}

