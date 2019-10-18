#pragma once
#include "Node.h"

Node::Node(std::variant<Node*, Leaf> l, std::variant<Node*, Leaf> r, BoundingBox b) 
	: left(l), right(r), boundingBox(b){};
