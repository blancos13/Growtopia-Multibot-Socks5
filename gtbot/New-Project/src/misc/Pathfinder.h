


#ifndef PATHFINDER_H_
#define PATHFINDER_H_

#include "../ItemInfoManager.h"
#include "../Struct.h"

#include <algorithm>
#include <iostream>
#include <vector>


struct Node {
	uint8_t x = 0;
	uint8_t y = 0;
	uint8_t colType = 0;
	Node* parent = NULL;
};


class Pathfinder {
private:
	std::vector<Tile>* nodes = NULL;
	std::vector<Node> node2;
	int width = 0;
	int height = 0;
public:
	Pathfinder(std::vector<Tile>* n) {
		nodes = n;
	}

	Node* GetNode(int x, int y) {
		int index = x + width * y;
		if (x > -1 && x < width && y > -1 && y < height && index > -1 && index < node2.size())
			return &node2[index];
		return NULL;
	}

	void Update(int w, int h) {
		width = w;
		height = h;
		node2.clear();
		for (int i = 0; i < (*nodes).size(); i++) {
			Node node;
			node.x = i % width;
			node.y = i / width;
			ItemInfo* info = itemDefs->Get((*nodes)[i].fg);
			if (info)
				node.colType = info->collisionType;
			node2.push_back(node);
		}
	}

	void Reset() {
		for (auto& node : node2)
			node.parent = NULL;
	}

	void Expand(Node* current, std::vector<Node*>& expanded) {
		for (int x = -1; x < 2; x++) {
			for (int y = -1; y < 2; y++) {
				if (abs(x) == abs(y))
					continue;
				Node* newnode = GetNode(current->x + x, current->y + y);
				if (!newnode)
					continue;
				if (newnode->parent)
					continue;
				bool skip = false;
				switch (newnode->colType) {
				case 0: {

				} break;
				case 1: {
					skip = true;
				} break;
				case 2: {
					if (y == 1)
						skip = true;
				} break;
				case 5: {

				} break;
				case 7: {
					if (y == -1)
						skip = true;
				} break;
				}
				if (skip)
					continue;
				newnode->parent = current;
				expanded.push_back(newnode);
			}
		}
	}

	std::vector<Node> GetBestPath(int fromX, int fromY, int toX, int toY) {
#ifdef _CONSOLE
		std::cout << "finding path: " << fromX << ", " << fromY << " -> " << toX << ", " << toY << std::endl;
#endif
		std::vector<Node> m_ret;
		if (!nodes)
			return m_ret;
		if ((*nodes).size() == 0)
			return m_ret;
		if (node2.size() == 0)
			return m_ret;
		if (fromX == toX && fromY == toY)
			return m_ret;

		Node* start = GetNode(fromX, fromY);
		Node* finish = GetNode(toX, toY);
		if (start == finish || !start || !finish)
			return m_ret;

		start->parent = NULL;

		std::vector<Node*> expand1;
		std::vector<Node*> expand2;

		expand1.push_back(start);
		while (!finish->parent) {
			for (auto& node : expand1)
				Expand(node, expand2);
			if (expand2.size() == 0)
				break;
			expand1.clear();
			expand1 = expand2;
			expand2.clear();
		}

		start->parent = NULL;

		expand1.clear();
		expand2.clear();

		Node* current = finish;
		bool found = current->parent != NULL;
		if (found) {
			while (current->parent) {
				m_ret.push_back(*current);
				current = current->parent;
			}
			m_ret.push_back(*start);
			std::reverse(m_ret.begin(), m_ret.end());
		}
		Reset();
		return m_ret;
	}


	void ClearNodes() {
		node2.clear();
	}
};




#endif