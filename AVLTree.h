#ifndef __DS_WET1_GR_AVL_TREE
#define __DS_WET1_GR_AVL_TREE

#include <assert.h>
#include <algorithm>
#include "AVLExceptions.h"

namespace DSWet1 {

	template <class K, class T>
	class AVLTree {

	private:
		friend class AVLTests;
		friend class PlantationTest;

		enum Rotation { LL, LR, RL, RR };
		class Node;
		Node* root;
		int size;

		Node* insertAux(Node* currNode, const K& key, const T& data);
		template <class F>
		void inorderAux(Node* currNode, F& functor);
		template <class F>
		void preorderAux(Node* currNode, F& functor);
		int BF(Node* node) const;
		void rotateRight(Node* node);
		void rotateLeft(Node* node);
		void setHeight(Node* currNode);

		void linkParentAndSon(Node *nodeToRemove);
		Node* findNode(Node* tree, K key);
		void destroyTree(Node* node);
		Rotation getRotation(Node *nodeParent);
		void rotateNode(Node *nodeParent);
		Node *insert(const K& key, const T& data);

		T& getLeftLeafAux(Node* currNode);
	public:
		bool exists(const K& key);
		void remove(const K key);
		void removeAVL(K key);
		void insertAVL(const K& key, const T& data);

		AVLTree() : root(NULL), size(0){};
		~AVLTree();

		T& getLeftLeaf();

		T& get(K key);
		int getSize() { return size; }
		template <class F>
		void inorder(F& functor);

		template <class F>
		void preorder(F& functor);

		template <class U, class W>
		friend bool operator==(AVLTree<U, W>& tree1, AVLTree<U, W>& tree2);

		template <class U, class W>
		friend bool opEqualAux(typename AVLTree<U, W>::Node* node1, typename AVLTree<U, W>::Node* node2);
	};

	template <class K, class T>
	class AVLTree<K, T>::Node {
	public:
		Node* left;
		K* key;
		T* data;
		Node* right;
		Node* parent;
		int height;

		Node(const K& key, const T& data) :
			left(NULL), key(new K(key)), data(new T(data)),
						right(NULL), parent(NULL), height(0) {}

		~Node() {
			delete key;
			delete data;
		}
	};

	template <class K, class T>
	typename AVLTree<K, T>::Node* AVLTree<K, T>::insert(const K& key, const T& data) {
		if (root == NULL) {
			root = new Node(key, data);
			root->parent = NULL;
			root->height = 0;
			size++;
			return root;
		}
		else {
			Node *nodeAdded = insertAux(root, key, data);
			size++;
			return nodeAdded;
		}
	}

	template <class K, class T>
	typename AVLTree<K, T>::Node* AVLTree<K, T>::insertAux(Node* currNode, const K& key, const T& data) {
		if (key == *(currNode->key)) {
			throw KeyAlreadyExistsException();
		}
		else if (key < *(currNode->key)) {
			if (currNode->left == NULL) {
				Node* newNode = new Node(key, data);
				currNode->left = newNode;
				currNode->left->parent = currNode;
				currNode->height = 0;

				return newNode;
			}
			else {
				return insertAux(currNode->left, key, data);
			}
		}
		else {
			if (currNode->right == NULL) {
				Node* newNode = new Node(key, data);
				currNode->right = newNode;
				currNode->right->parent = currNode;
				currNode->height = 0;

				return newNode;
			}
			else {
				return insertAux(currNode->right, key, data);
			}
		}
	}

	template <class K, class T>
	template <class F>
	void AVLTree<K, T>::inorder(F& functor) {
		inorderAux(root, functor);
	}

	template <class K, class T>
	template <class F>
	void AVLTree<K, T>::inorderAux(Node* currNode, F& functor) {
		if (currNode == NULL) return;
		inorderAux(currNode->left, functor);
		
		// Return the data to the functor to perform the action
		functor(*(currNode->key), *(currNode->data));

		inorderAux(currNode->right, functor);
	}

	template <class K, class T>
	template <class F>
	void AVLTree<K, T>::preorder(F& functor) {
		preorderAux(root, functor);
	}

	template <class K, class T>
	template <class F>
	void AVLTree<K, T>::preorderAux(Node* currNode, F& functor) {
		if (currNode == NULL) return;

		// Return the data to the functor to perform the action
		functor(*(currNode->key), *(currNode->data));
		preorderAux(currNode->left, functor);
		preorderAux(currNode->right, functor);
	}

	template <class K, class T>
	void AVLTree<K,T>::destroyTree(typename AVLTree<K, T>::Node* node) {
		if (node == NULL) return;

		destroyTree(node->left);
		destroyTree(node->right);
		delete node;
	}

	template <class K, class T>
	typename AVLTree<K, T>::Node* AVLTree<K, T>::findNode(typename AVLTree<K, T>::Node* tree, K key) {
		if (tree == NULL) {
			return NULL;
		}
		if (key == *(tree->key)) {
			return tree;
		}
		else if (key < *(tree->key)) {
			return findNode(tree->left, key);
		}
		else /* (key > tree->key) */ {
			return findNode(tree->right, key);
		}
	}

	template <class K, class T>
	void AVLTree<K, T>::remove(const K key) {
		Node *nodeRemove = findNode(root, key);
		if (nodeRemove == NULL) {
			throw KeyDoesntExistException();
		}
		bool rootRemove = (nodeRemove == root) ? true : false;
		Node *realParent = NULL;
		if (nodeRemove->left != NULL && nodeRemove->right != NULL) {
			rootRemove = false;
			Node *nodeNext = nodeRemove->right;
			while (nodeNext->left != NULL) {
				nodeNext = nodeNext->left;
			}
			// replace data between nodeRemove and nodeNext
			Node *nodeNextRight = nodeNext->right;
			realParent = nodeRemove->parent;
			bool isRight = (nodeRemove->right == nodeNext);
			if (isRight == true) {
				nodeNext->right = nodeRemove;
				nodeRemove->parent = nodeNext;
			}
			else {
				nodeRemove->right->parent = nodeNext;
				nodeNext->right = nodeRemove->right;
				nodeRemove->parent = nodeNext->parent;
				nodeNext->parent->left = nodeRemove;
			}
			if (realParent == NULL) {
				nodeNext->parent = NULL;
				root = nodeNext;
			}
			else {
				nodeNext->parent = realParent;
				if (realParent->left == nodeRemove) {
					realParent->left = nodeNext;
				}
				else {
					realParent->right = nodeNext;
				}
			}
			nodeRemove->right = nodeNextRight;
			if (nodeNextRight != NULL) {
				nodeNextRight->parent = nodeRemove;
			}
			assert(nodeNext->left == NULL && nodeRemove->left != NULL);
			nodeNext->left = nodeRemove->left;
			nodeNext->left->parent = nodeNext;
			nodeRemove->left = NULL;
		}
		if (rootRemove == false) {
			linkParentAndSon(nodeRemove);
		}
		else {
			root = nodeRemove->left != NULL ? nodeRemove->left : nodeRemove->right;
			if (root != NULL) {
				root->parent = NULL;
			}
		}
		delete nodeRemove;
		size--;
	}

	/* When node to be removed has by most 1 son, link it's dad and the son */
	template <class K, class T>
	void AVLTree<K, T>::linkParentAndSon(typename AVLTree<K, T>::Node *nodeToRemove) {
		// Asserts to make sure pointers are OK (first 3) and node only has one son
		assert(nodeToRemove != NULL);
		assert(nodeToRemove->parent != NULL);
		assert(nodeToRemove->parent->left == nodeToRemove || nodeToRemove->parent->right == nodeToRemove);
		assert(nodeToRemove->left == NULL || nodeToRemove->right == NULL);
		if (nodeToRemove->parent->left == nodeToRemove) {
			nodeToRemove->parent->left = nodeToRemove->left != NULL ? nodeToRemove->left : nodeToRemove->right;
			if (nodeToRemove->left != NULL) {
				nodeToRemove->left->parent = nodeToRemove->parent;
			}
			else if (nodeToRemove->right != NULL) {
				nodeToRemove->right->parent = nodeToRemove->parent;
			}
		}
		else {
			nodeToRemove->parent->right = nodeToRemove->left != NULL ? nodeToRemove->left : nodeToRemove->right;
			if (nodeToRemove->left != NULL) {
				nodeToRemove->left->parent = nodeToRemove->parent;
			}
			else if (nodeToRemove->right != NULL) {
				nodeToRemove->right->parent = nodeToRemove->parent;
			}
		}
	}

	template <class U, class W>
	bool opEqualAux(typename AVLTree<U, W>::Node* node1, typename AVLTree<U, W>::Node* node2) {
		if (node1 == NULL && node2 == NULL) return true;
		if ((node1 != NULL && node2 == NULL) || (node1 == NULL && node2 != NULL)) return false;
		if ((node1->parent != NULL && node2->parent == NULL) || (node1->parent == NULL && node2->parent != NULL)) return false;
		if (node1->parent != NULL && node2->parent != NULL && *(node1->parent->key) != *(node2->parent->key)) return false;

		return *(node1->key) == *(node2->key) &&
			node1->height == node2->height &&
			opEqualAux<U, W>(node1->left, node2->left) &&
			opEqualAux<U, W>(node1->right, node2->right);
	}

	template <class U, class W>
	bool operator==(AVLTree<U, W>& tree1, AVLTree<U, W>& tree2) {
		return (tree1.size == tree2.size) && opEqualAux<U, W>(tree1.root, tree2.root);
	}

	template <class K, class T>
	int AVLTree<K, T>::BF(Node* node) const {
		assert(node != NULL);
		int height_left = node->left != NULL ? node->left->height : -1;
		int height_right = node->right != NULL ? node->right->height : -1;
		return height_left - height_right;
	}

	template <class K, class T>
	void AVLTree<K, T>::rotateRight(Node* node) {
		bool isRoot = node == root;
		Node *base = node->left; // B
		Node *right = node; // A
		Node *rightLeft = node->left->right;
		Node *baseParent = node->parent;
		right->left = rightLeft;
		if (rightLeft != NULL) {
			rightLeft->parent = node;
		}
		right->parent = base;
		base->right = right;
		if (isRoot) {
			root = base;
		}
		else {
			if (baseParent->right == right) {
				baseParent->right = base;
			}
			else {
				baseParent->left = base;
			}
		}
		base->parent = baseParent;
	}

	template <class K, class T>
	void AVLTree<K, T>::rotateLeft(Node* node) {
		bool isRoot = node == root;
		Node *base = node->right; // B
		Node *left = node; // A
		Node *leftRight = node->right->left;
		Node *baseParent = node->parent;
		left->right = leftRight;
		if (leftRight != NULL) {
			leftRight->parent = node;
		}
		left->parent = base;
		base->left = left;
		if (isRoot) {
			root = base;
		}
		else {
			if (baseParent->left == left) {
				baseParent->left = base;
			}
			else {
				baseParent->right = base;
			}
		}
		base->parent = baseParent;
	}

	template <class K, class T>
	void AVLTree<K, T>::insertAVL(const K& key, const T& data) {
		Node *nodeAdd = insert(key, data);
		nodeAdd->height = 0;
		Node *nodeParent = NULL;
		while (nodeAdd != root) {
			nodeParent = nodeAdd->parent;
			if (nodeParent->height >= nodeAdd->height + 1) {
				break;
			}
			nodeParent->height = nodeAdd->height + 1;
			int parentBF = BF(nodeParent);
			if (parentBF == -2 || parentBF == 2) {
				rotateNode(nodeParent);
				break;
			}
			nodeAdd = nodeParent;
		}
	}

	template <class K, class T>
	typename AVLTree<K, T>::Rotation AVLTree<K, T>::getRotation(Node *nodeParent) {
		assert(nodeParent != NULL);
		int BFParent = BF(nodeParent);
		assert(BFParent == 2 || BFParent == -2);
		switch (BFParent) {
		case 2:
			assert(nodeParent->left != NULL);
			if (BF(nodeParent->left) >= 0) {
				return LL;
			}
			return LR;
		case -2:
			assert(nodeParent->right != NULL);
			if (BF(nodeParent->right) <= 0) {
				return RR;
			}
			return RL;
		default: return RL;
		}
	}

	template <class K, class T>
	void AVLTree<K, T>::setHeight(typename AVLTree<K, T>::Node* currNode) {
		assert(currNode != NULL);
		currNode->height = 1 + std::max(currNode->left == NULL ? -1 : currNode->left->height,
			currNode->right == NULL ? -1 : currNode->right->height);
	}


	template <class K, class T>
	void AVLTree<K, T>::removeAVL(K key) {
		Node *deletedNode = findNode(root, key);
		if (deletedNode == NULL) {
			throw KeyDoesntExistException();
		}
		Node *currNode = deletedNode->parent;
		deletedNode = NULL;
		remove(key);
		while (currNode != NULL) {
			int previousHeight = currNode->height;
			setHeight(currNode);
			bool isLeft;
			Node *parentNode = currNode->parent;
			if (parentNode != NULL) { //
				isLeft = parentNode->left == currNode;
			}
			int currBF = BF(currNode);
			if (currBF == -2 || currBF == 2) {
				rotateNode(currNode);
			}
			if (parentNode == NULL) {
				break;
			}
			currNode = isLeft ? parentNode->left : parentNode->right;
			if (previousHeight == currNode->height) {
				break;
			}
			currNode = currNode->parent;
		}
	}

	template <class K, class T>
	bool AVLTree<K, T>::exists(const K& key) {
		return findNode(this->root, key) != NULL;
	}

	template <class K, class T>
	T& AVLTree<K, T>::get(K key) {
		Node *nodeSearch = findNode(this->root, key);
		if (nodeSearch == NULL) {
			throw KeyDoesntExistException();
		}
		return *(nodeSearch->data);
	}

	template <class K, class T>
	T& AVLTree<K, T>::getLeftLeaf() {
		assert(root != NULL);

		return getLeftLeafAux(root);
	}

	template <class K, class T>
	T& AVLTree<K, T>::getLeftLeafAux(Node* currNode) {
		if (currNode->left == NULL) return *(currNode->data);
		return getLeftLeafAux(currNode->left);
	}

	template <class K, class T>
	void AVLTree<K, T>::rotateNode(Node *nodeParent) {
		assert(nodeParent != NULL);
		Rotation rotation = getRotation(nodeParent);
		switch (rotation) {
		case RR:
		{
			rotateLeft(nodeParent);
			setHeight(nodeParent);
			setHeight(nodeParent->parent);
			break;
		}
		case LL:
		{
			rotateRight(nodeParent);
			setHeight(nodeParent);
			setHeight(nodeParent->parent);
			break;
		}
		case RL:
		{
			Node *rightSon = nodeParent->right; // parent->right
			Node *rightLeftSon = nodeParent->right->left; // parent->right->left
			rotateRight(nodeParent->right);
			rotateLeft(nodeParent);
			/*nodeParent->height -= 2;
			rightLeftSon->height++;
			rightSon->height--;*/ // Before changing height formula
			setHeight(rightSon);
			setHeight(nodeParent);
			setHeight(rightLeftSon);
			break;
		}
		case LR:
		{
			Node *leftSon = nodeParent->left; // parent->left
			Node *leftRightSon = nodeParent->left->right; // parent->left->right
			rotateLeft(nodeParent->left);
			rotateRight(nodeParent);
			/*nodeParent->height -= 2;
			leftRightSon->height++;
			leftSon->height--; */ // Before changing height formula
			setHeight(leftSon);
			setHeight(nodeParent);
			setHeight(leftRightSon);
			break;
		}
		}
	}

	template <class K, class T>
	AVLTree<K, T>::~AVLTree() {
		if (root != NULL) {
			destroyTree(root);
		}
	}
}

#endif