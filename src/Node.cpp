#include "Node.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
using namespace std;

Node::Node()
{
	isRemoved=false;
	isMarked = false;
	val =  0;
}

Node::Node(int val)
{
	// TODO Auto-generated constructor stub
	isRemoved=false;
	isMarked = false;
	this->val = val;
}

Node::~Node()
{
	// TODO Auto-generated destructor stub
}

int Node::getVal() const
{
    return val;
}


void Node::setVal(int val)
{
    this->val = val;
}



