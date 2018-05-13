/*
 * Node.h
 *
 *  Created on: Mar 3, 2018
 *  Author: Sergelenbayar G.
 */
#ifndef NODE_H_
#define NODE_H_
#include <vector>
#include <iterator>
#include <iostream>
#include <map>

using namespace std;

class Node
{
private:
	int val;
public:
	//vector of connected components's indexes
	std::vector <int> componentIndexSet;
	bool isRemoved;
	bool isMarked;
	//neighbors of current node
	std::map <int, Node> neighbors;

	Node();
	Node(int val);
	virtual ~Node();

	int getVal() const;
	void setVal(int val);

	bool operator==(const Node & obj2) const
	{
		if(this->val == obj2.val)
			return true;
		else return false;
	}
	bool operator=(const Node & obj2) const
	{
		if(this->val == obj2.val)
			return true;
		else return false;
	}
};
#endif /* NODE_H_ */
