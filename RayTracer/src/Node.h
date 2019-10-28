#pragma once
#include "Tools.h"
#include "Leaf.h"
#include <variant>
#include <algorithm>

class Node
{
public:
	BoundingBox boundingBox;
	std::variant<Node*, Leaf> left;
	std::variant<Node*, Leaf> right;
	
	Node(std::variant<Node*, Leaf> l, std::variant<Node*, Leaf> r, BoundingBox b);
};