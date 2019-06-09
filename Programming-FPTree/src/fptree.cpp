#include"fptree/fptree.h"
#include <sstream>
using namespace std;

// Initial the new InnerNode
InnerNode::InnerNode(const int& d, FPTree* const& t, bool _isRoot) {
    // TODO
    degree = d;
    tree = t;
    isRoot = _isRoot;
    nKeys = 0;
    nChild = 0;
    isLeaf = false;

    new Key[2 * d + 2];
    childrens = new Node*[2 * d + 2];
}

// delete the InnerNode
InnerNode::~InnerNode() {
    // TODO
    degree = 0;
    delete tree;
    tree = NULL;
    delete []keys;
    keys = NULL;
    delete []childrens;
    childrens = NULL;
}

// binary search the first key in the innernode larger than input key
int InnerNode::findIndex(const Key& k) {
    // TODO
    int low=0;
    int high = getKeyNum()-1;
    while (low<=high)
    {
        /* code */
        int mid = low + (high-low)/2;
        Key midKey = getKey(mid);
        if(k > midKey)  low = mid+1;
        else    high = mid-1;
    }
    return low;
}

// insert the node that is assumed not full
// insert format:
// ======================
// | key | node pointer |
// ======================
// WARNING: can not insert when it has no entry
void InnerNode::insertNonFull(const Key& k, Node* const& node) {
    // TODO
    if(nKeys == degree*2 + 1)   return;
    int index = findIndex(k);
    int i = nKeys;
    for(;i > index;--i) {
        keys[i] = keys[i-1];
    }
    for(i = nChild+1;i>index+1;--i) {
        childrens[i] = childrens[i-1];
    }

    keys[index] = k;
    childrens[index+1] = node;
    nKeys++;
    nChild++;
}

// insert func
// return value is not NULL if split, returning the new child and a key to insert
KeyNode* InnerNode::insert(const Key& k, const Value& v) {
    KeyNode* newChild = NULL;

    // 1.insertion to the first leaf(only one leaf)
    if (this->isRoot && this->nKeys == 0) {
        // TODO
        LeafNode *tmp = new LeafNode(tree);
        tmp->insert(k,v);
        childrens[0] = tmp;
        nChild++;
        return newChild;
    }
    
    // 2.recursive insertion
    // TODO

    int index = findIndex(k);
    newChild = childrens[index]->insert(k,v);
    if(newChild == NULL)    return newChild;
    else
    {
        if(this->nKeys>=degree*2){
            insertNonFull(newChild->key,newChild->node);
            newChild = this->split();
            if(this->isRoot){
                //setting new root
                InnerNode* newRoot= new InnerNode(degree,tree,true);
                this->isRoot=false;
                newRoot->childrens[0]=this;
                (newRoot->nChild)++;
                newRoot->insertNonFull(newChild->key,newChild->node);
                tree->changeRoot(newRoot);
                delete newChild;
            }
        }
        else{
            insertNonFull(newChild->key,newChild->node);
            delete newChild;
            newChild=NULL;
            return newChild;
        }
    }
    
    return newChild;
}

// ensure that the leaves inserted are ordered
// used by the bulkLoading func
// inserted data: | minKey of leaf | LeafNode* |
KeyNode* InnerNode::insertLeaf(const KeyNode& leaf) {
    KeyNode* newChild = NULL;
    // first and second leaf insertion into the tree
    if (this->isRoot && this->nKeys == 0) {
        // TODO
        childrens[0] = leaf.node;
        nChild++;
        return newChild;
    }
    
    // recursive insert
    // Tip: please judge whether this InnerNode is full
    // next level is not leaf, just insertLeaf
    // TODO



    // next level is leaf, insert to childrens array
    // TODO
    int index = findIndex(leaf.key);
    if(childrens[0]->ifLeaf()){ //if next level is leaf
        if(this->nKeys>=degree*2){
            insertNonFull(leaf.key,leaf.node);
            newChild = this->split();
            if(this->isRoot){
                InnerNode* newRoot= new InnerNode(degree,tree,true);
                this->isRoot=false;
                newRoot->childrens[0]=this;
                ++(newRoot->nChild);
                newRoot->insertNonFull(newChild->key,newChild->node);
                tree->changeRoot(newRoot);
                delete newChild;
                newChild=NULL;
            }
            return newChild;
        }
        else{
             insertNonFull(leaf.key,leaf.node);
        }
    }
    else
    { 
        newChild = ((InnerNode*)(childrens[index]))->insertLeaf(leaf);
        if(newChild==NULL) return newChild;
        else
        {
            
            if(this->nKeys>=degree*2){
                insertNonFull(newChild->key,newChild->node);//insert overflow key-value
                newChild = this->split();
                if(this->isRoot){
                    //setting new root
                    InnerNode* newRoot= new InnerNode(degree,tree,true);
                    this->isRoot=false;
                    newRoot->childrens[0]=this;
                    ++(newRoot->nChild);
                    newRoot->insertNonFull(newChild->key,newChild->node);
                    tree->changeRoot(newRoot);
                    delete newChild;
                    newChild=0;
                }
            }
            else
            {
                insertNonFull(newChild->key,newChild->node);
                delete newChild;
                newChild=NULL;
                return newChild;
            }
        }
    }
    return newChild;
}

KeyNode* InnerNode::split() {
    KeyNode* newChild = new KeyNode();
    // right half entries of old node to the new node, others to the old node. 
    // TODO
    int d=nKeys/2;
    int newMidKey=keys[d];
    InnerNode* newNode = new InnerNode(d,tree,false);
    newNode->nKeys=d;
    newNode->nChild=d+1;
    for(int i=d+1;i<nKeys;++i){
        newNode->keys[i-d-1]=keys[i];
        newNode->childrens[i-d-1]=childrens[i];
    }
    newNode->childrens[nKeys-d-1]=childrens[nKeys];
    nKeys = d;
    nChild = d+1;
    newChild->key=newMidKey;
    newChild->node=newNode;
    return newChild;
}

// remove the target entry
// return TRUE if the children node is deleted after removement.
// the InnerNode need to be redistributed or merged after deleting one of its children node.
bool InnerNode::remove(const Key& k, const int& index, InnerNode* const& parent, bool &ifDelete) {
    bool ifRemove = false;
    // only have one leaf
    // TODO
    
    // recursive remove
    // TODO
    return ifRemove;
}

// If the leftBro and rightBro exist, the rightBro is prior to be used
void InnerNode::getBrother(const int& index, InnerNode* const& parent, InnerNode* &leftBro, InnerNode* &rightBro) {
    // TODO
}

// merge this node, its parent and left brother(parent is root)
void InnerNode::mergeParentLeft(InnerNode* const& parent, InnerNode* const& leftBro) {
    // TODO
}

// merge this node, its parent and right brother(parent is root)
void InnerNode::mergeParentRight(InnerNode* const& parent, InnerNode* const& rightBro) {
    // TODO
}

// this node and its left brother redistribute
// the left has more entries
void InnerNode::redistributeLeft(const int& index, InnerNode* const& leftBro, InnerNode* const& parent) {
    // TODO
}

// this node and its right brother redistribute
// the right has more entries
void InnerNode::redistributeRight(const int& index, InnerNode* const& rightBro, InnerNode* const& parent) {
    // TODO
}

// merge all entries to its left bro, delete this node after merging.
void InnerNode::mergeLeft(InnerNode* const& leftBro, const Key& k) {
    // TODO
}

// merge all entries to its right bro, delete this node after merging.
void InnerNode::mergeRight(InnerNode* const& rightBro, const Key& k) {
    // TODO
}

// remove a children from the current node, used by remove func
void InnerNode::removeChild(const int& keyIdx, const int& childIdx) {
    // TODO
}

// update the target entry, return true if the update succeed.
bool InnerNode::update(const Key& k, const Value& v) {
    // TODO
    return false;
}

// find the target value with the search key, return MAX_VALUE if it fails.
Value InnerNode::find(const Key& k) {
    // TODO
    if(nKeys == 0&& nChild == 0) 
        return MAX_VALUE;
    int index = findIndex(k);
    return childrens[index]->find(k);
}

// get the children node of this InnerNode
Node* InnerNode::getChild(const int& idx) {
    // TODO
    if(idx < nChild)    return childrens[idx];

    return NULL;
}

// get the key of this InnerNode
Key InnerNode::getKey(const int& idx) {
    if (idx < this->nKeys) {
        return this->keys[idx];
    } else {
        return MAX_KEY;
    }
}

// print the InnerNode
void InnerNode::printNode() {
    cout << "||#|";
    for (int i = 0; i < this->nKeys; i++) {
        cout << " " << this->keys[i] << " |#|";
    }
    cout << "|" << "    ";
}

// print the LeafNode
void LeafNode::printNode() {
    cout << "||";
    for (int i = 0; i < 2 * this->degree; i++) {
        if (this->getBit(i)) {
            cout << " " << this->kv[i].k << " : " << this->kv[i].v << " |";
        }
    }
    cout << "|" << " ====>> ";
}

// new a empty leaf and set the valuable of the LeafNode
LeafNode::LeafNode(FPTree* t) {
    // TODO
    tree=t;
    isLeaf=true;
    degree=LEAF_DEGREE;
    PAllocator::getAllocator()->getLeaf(pPointer, pmem_addr);
    bitmap=(Byte*)pmem_addr;
    bitmapSize=(LEAF_DEGREE*2+7)/8;
    Byte* cursor = (Byte*) (pmem_addr);

    cursor = cursor+bitmapSize;
    
    pNext=(PPointer*) cursor;

    cursor = cursor+sizeof(PPointer);
    fingerprints=cursor;
    
    cursor = cursor+ LEAF_DEGREE*2 * sizeof(Byte);
    kv=(KeyValue*)cursor;
    n=0;
    prev=NULL;
    next=NULL;
    filePath=DATA_DIR+ to_string(pPointer.fileId);
}

// reload the leaf with the specific Persistent Pointer
// need to call the PAllocator
LeafNode::LeafNode(PPointer p, FPTree* t) {
    // TODO

    prev = NULL;
    next = NULL;
    tree = t;
    degree = LEAF_DEGREE;
    isLeaf = true;
    PAllocator::getAllocator()->getLeaf(pPointer, pmem_addr);
    pPointer = p;
    bitmap = (Byte*)pmem_addr;
    bitmapSize=(LEAF_DEGREE*2+7)/8;
    Byte* cursor = (Byte*) (pmem_addr);
    cursor = cursor+bitmapSize;
    pNext = (PPointer*) cursor;
    cursor = cursor + sizeof(PPointer);
    fingerprints=cursor;
    cursor = cursor+ LEAF_DEGREE*2 * sizeof(Byte);
    kv=(KeyValue*)cursor;
    cursor=bitmap;
    n=0;
    for(int i=0;i<bitmapSize;++i){
        n+=countOneBits(*cursor);
        cursor+=1;
    }
     if (pNext->fileId!=0){
        next = new LeafNode(*pNext, tree);
        next->prev = this;
    }
    filePath=DATA_DIR+ to_string(p.fileId);
}

LeafNode::~LeafNode() {
    // TODO
    this->persist();
    PAllocator::getAllocator()->freeLeaf(pPointer);
}

// insert an entry into the leaf, need to split it if it is full
KeyNode* LeafNode::insert(const Key& k, const Value& v) {
    KeyNode* newChild = NULL;
    // TODO
    if(n >= degree*2-1) {
        insertNonFull(k,v);
        newChild=split();
        next = (LeafNode*)(newChild->node);
        ((LeafNode*)(newChild->node))->prev= this;
    }
    else insertNonFull(k,v);
    return newChild;
}

// insert into the leaf node that is assumed not full
void LeafNode::insertNonFull(const Key& k, const Value& v) {
    // TODO
    int index = findFirstZero();

    int offset = index%8;
    int pos=index/8;
    Byte* cursor = bitmap;
    cursor += pos;
    Byte bits = *cursor;
    bits = bits | (1<<offset);
    *(cursor) = bits;
    fingerprints[index] = keyHash(k);
    kv[index].k=k;
    kv[index].v=v;
    n++;
    persist();
}

// split the leaf node
KeyNode* LeafNode::split() {
    KeyNode* newChild = new KeyNode();
    // TODO
    LeafNode* tmp = new LeafNode(tree);
    memset(bitmap,0,bitmapSize);
    int mid = findSplitKey();
    for(int i = 0;i < n/2;i++)  {
        fingerprints[i] = keyHash(getKey(i));
            int offset = i%8;
            int pos=i/8;
            Byte* cursor = bitmap;
            cursor += pos;
            Byte bits = *cursor;
            bits = bits | (1<<offset);
            *(cursor) = bits;
    }
    for(int i = n/2;i < n;i++)  {
        tmp->insertNonFull(getKey(i),getValue(i));
    }
    n /= 2;
    *pNext = tmp->getPPointer();
    newChild->node = tmp;
    newChild->key = mid;
    tmp->persist();
    this->persist();
    return newChild;
}

// use to find a mediant key and delete entries less then middle
// called by the split func to generate new leaf-node
// qsort first then find
inline int cmp_kv(const void* a,const void* b)
{
    return ((KeyValue*)a)->k>((KeyValue*)b)->k;
}
Key LeafNode::findSplitKey() {
    Key midKey = 0;
    // TODO
    qsort(kv,n,sizeof(KeyValue),cmp_kv);
    midKey = kv[n/2].k;
    return midKey;
}

// get the targte bit in bitmap
// TIPS: bit operation
int LeafNode::getBit(const int& idx) {
    // TODO
    
    int offset = idx%8;
    int pos=idx/8;
    Byte* cursor = bitmap;
    cursor += pos;
    Byte bits = *cursor;
    bits = (bits>>offset) & 1;
    return (int) bits;
    return 0;
}

Key LeafNode::getKey(const int& idx) {
    return this->kv[idx].k;
}

Value LeafNode::getValue(const int& idx) {
    return this->kv[idx].v;
}

PPointer LeafNode::getPPointer() {
    return this->pPointer;
}

// remove an entry from the leaf
// if it has no entry after removement return TRUE to indicate outer func to delete this leaf.
// need to call PAllocator to set this leaf free and reuse it
bool LeafNode::remove(const Key& k, const int& index, InnerNode* const& parent, bool &ifDelete) {
    bool ifRemove = false;
    // TODO
    return ifRemove;
}

// update the target entry
// return TRUE if the update succeed
bool LeafNode::update(const Key& k, const Value& v) {
    bool ifUpdate = false;
    // TODO
    return ifUpdate;
}

// if the entry can not be found, return the max Value
Value LeafNode::find(const Key& k) {
    // TODO
    int tmp = keyHash(k);
    Byte* cursor = fingerprints;
    for(int i = 0;i < degree*2;i++) {
        if(getBit(i) == 1 && (fingerprints[i] == tmp))  {
            if(getKey(i) == k)  return getValue(i);
        }
        cursor++;
    }
    return MAX_VALUE;
}

// find the first empty slot
int LeafNode::findFirstZero() {
    // TODO
    int index = -1;
    for(int i = 0;i < degree*2;i++) {
        if(getBit(i) == 0)  {
            index = i;
            return index;
        }
    }
    return -1;
}

// persist the entire leaf
// use PMDK
void LeafNode::persist() {
    // TODO
    pmem_msync(pmem_addr,calLeafSize());
}

// call by the ~FPTree(), delete the whole tree
void FPTree::recursiveDelete(Node* n) {
    if (n->isLeaf) {
        delete n;
    } else {
        for (int i = 0; i < ((InnerNode*)n)->nChild; i++) {
            recursiveDelete(((InnerNode*)n)->childrens[i]);
        }
        delete n;
    }
}

FPTree::FPTree(uint64_t t_degree) {
    FPTree* temp = this;
    this->root = new InnerNode(t_degree, temp, true);
    this->degree = t_degree;
    bulkLoading();
}

FPTree::~FPTree() {
    recursiveDelete(this->root);
}

// get the root node of the tree
InnerNode* FPTree::getRoot() {
    return this->root;
}

// change the root of the tree
void FPTree::changeRoot(InnerNode* newRoot) {
    this->root = newRoot;
}

void FPTree::insert(Key k, Value v) {
    if (root != NULL) {
        root->insert(k, v);
    }
}

bool FPTree::remove(Key k) {
    if (root != NULL) {
        bool ifDelete = false;
        InnerNode* temp = NULL;
        return root->remove(k, -1, temp, ifDelete);
    }
    return false;
}

bool FPTree::update(Key k, Value v) {
    if (root != NULL) {
        return root->update(k, v);
    }
    return false;
}

Value FPTree::find(Key k) {
    if (root != NULL) {
        return root->find(k);
    }
}

// call the InnerNode and LeafNode print func to print the whole tree
// TIPS: use Queue
#include<queue>
void FPTree::printTree() {
    // TODO
    queue<Node*> q;
    q.push(root);
    while(!q.empty()){
        Node* tmp = q.front();
        if(!tmp->isLeaf){
            for(int i=0;i<((InnerNode*)(tmp))->nChild;++i) 
                q.push(((InnerNode*)(tmp))->getChild(i));
        } 
        tmp->printNode();
        q.pop();
    }
}

// bulkLoading the leaf files and reload the tree
// need to traverse leaves chain
// if no tree is reloaded, return FALSE
// need to call the PALlocator
bool FPTree::bulkLoading() {
    // TODO
        PAllocator* PA = PAllocator::getAllocator();
    PPointer startPointer=PA->getStartPointer();
    if(startPointer.fileId==0) return false;
    LeafNode* startLeaf = new LeafNode(startPointer,this);
    //travesal list
    KeyNode bulkLeaf;
    for(auto leafnode=startLeaf;leafnode!=NULL;leafnode=leafnode->next){
        bulkLeaf.node=leafnode;
        Key min=leafnode->getKey(0);
        for(int i=1;i<leafnode->n;++i){
            if(leafnode->getKey(i)<min&&leafnode->getBit(i)) min=leafnode->getKey(i);
        }
        bulkLeaf.key=min;
        root->insertLeaf(bulkLeaf);
    }
    return true;
}
