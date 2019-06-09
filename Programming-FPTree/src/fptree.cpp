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
    if(this->isRoot && this->nKeys == 0 && this->nChild == 1 && getChild(0)->ifLeaf())  {
        LeafNode* removeLeaf = (LeafNode *)getChild(0);
        ifRemove = removeLeaf->remove(k,0,this,ifDelete);
        if(ifDelete)    {
            nChild--;
            delete removeLeaf;
        }
        return ifRemove;
    }
    // recursive remove
    // TODO
    int keyIdx = findIndex(k);
    int childIdx = keyIdx;
    Node * removeChild = (Node *)getChild(childIdx);
    if (removeChild == NULL) {
        return false;
    }
    ifRemove = removeChild->remove(k, childIdx, this, ifDelete);
    if (ifDelete) { 
        ifDelete = false;
        if (degree + 1 > nChild && !this->isRoot) {

            InnerNode *leftBro, *rightBro;
            int left_nChild = 0, right_nChild = 0;
            getBrother(index, parent, leftBro, rightBro);
            if(leftBro != NULL)
                left_nChild = leftBro->getChildNum();
            if (rightBro != NULL)
                right_nChild = rightBro->getChildNum();

            if (degree + 1 <= right_nChild - 1) { 
              
                this->redistributeRight(index, rightBro, parent);
            }
            else if (degree + 1 <= left_nChild - 1) {
                this->redistributeLeft(index, leftBro, parent);
            }
            else if (right_nChild != 0 &&right_nChild + this->nChild <= 2 * degree + 1) {

                auto rk = this->keys[keyIdx];   
                this->mergeRight(rightBro, rk);
                ifDelete = true;
                parent->removeChild(keyIdx, index); 
            }
            else if (left_nChild != 0 && left_nChild + this->nChild <= 2 * degree + 1) {
                auto lk = this->keys[keyIdx - 1]; 
                this->mergeLeft(leftBro, lk);
                ifDelete = true;
                parent->removeChild(keyIdx - 1, index); 
            }
            else if (parent->isRoot && parent->getChildNum() == 2){
                if (leftBro != NULL) 
                    mergeParentLeft(parent, leftBro);
                else
                    mergeParentRight(parent, rightBro);
                ifDelete = true;
            }
        }
    }
    return ifRemove;
}

// If the leftBro and rightBro exist, the rightBro is prior to be used
void InnerNode::getBrother(const int& index, InnerNode* const& parent, InnerNode* &leftBro, InnerNode* &rightBro) {
    // TODO
    if(parent == NULL)  return;
    if(index - 1 < 0)   leftBro = NULL;
    else leftBro = (InnerNode *)parent->getChild(index-1);
    rightBro = (InnerNode *)parent->getChild(index+1);
}

// merge this node, its parent and left brother(parent is root)
void InnerNode::mergeParentLeft(InnerNode* const& parent, InnerNode* const& leftBro) {
    // TODO

    int left_key_num = leftBro->getKeyNum();
    int left_child_num = leftBro->getChildNum();

    auto mid = parent->getKey(0);

    for(int i = 0;i < left_key_num;i++) {
        parent->keys[i] = leftBro->keys[i];
    }
    for(int i = 0;i < left_child_num;i++)   {
        parent->childrens[i] = leftBro->childrens[i];
    }
    parent->keys[left_key_num] = mid;

    for(int i = left_key_num +1;i < left_key_num +1 + this->nKeys;i++)  {
        parent->keys[i] = this->keys[i];
    }
    for (int i = left_child_num; i < left_child_num + this->nChild; i ++ )  {
        parent->childrens[i] = this->childrens[i];
    }
       
    nKeys = 0;
    nChild = 0;
    delete leftBro;
    delete this;
}

// merge this node, its parent and right brother(parent is root)
void InnerNode::mergeParentRight(InnerNode* const& parent, InnerNode* const& rightBro) {
    // TODO
    int r_key_num = rightBro->getKeyNum();
    int r_child_num = rightBro->getChildNum();

    auto mid = parent->getKey(0);

    for (int i = 0; i < nKeys; i ++)    {
        parent->keys[i] = this->keys[i];
    }
        
    for (int i = 0; i < nChild; i ++ )  {
        parent->childrens[i] = this->childrens[i];
    }
        
    parent->keys[nKeys] = mid;

    for (int i = nKeys + 1; i < nKeys + 1 + r_key_num; i ++ )   {
        parent->keys[i] = rightBro->keys[i];
    }
        
    for (int i = nChild + 1; i < nChild + 1; i ++)  {
        parent->childrens[i] = rightBro->childrens[i];
    }
    
    nKeys = 0;
    nChild = 0;
    delete rightBro;
    delete this;

}

// this node and its left brother redistribute
// the left has more entries
void InnerNode::redistributeLeft(const int& index, InnerNode* const& leftBro, InnerNode* const& parent) {
    // TODO

    /* for(int i = this->nKeys; i >= 0; i --){
        keys[i + 1] = keys[i];
    }
    for(int i = this->nChild; i >= 0; i --){
        childrens[i + 1] = childrens[i];
    }
    
    childrens[0] = leftBro->childrens[leftBro->nChild - 1];

    keys[0] = leftBro->keys[leftBro->nKeys - 1];
    
    leftBro->nChild --;
    leftBro->nKeys --;
    nChild ++;
    nKeys ++;
    parent->keys[index - 1] = this->keys[0];

*/

    int left_child_num = leftBro->getChildNum();
    int left_num = (left_child_num + this->nChild + 1)/2;
    int this_num = (left_child_num +this->nChild)/2;

    for (int i = this->nKeys - 1; i >= 0; i --) {
        keys[i + this_num - this->nChild] = keys[i];
    }
        
    for (int i = this->nChild - 1; i >= 0; i -- ) {
        childrens[i + this_num - this->nChild] = childrens[i];
    }
        
    keys[this_num - this->nChild - 1] = parent->keys[index - 1];
    parent->keys[index - 1] = leftBro->getKey(left_num);
    for (int i = left_child_num - 1; i >= left_num; i -- )  {
        childrens[i] = leftBro->childrens[i - left_num];
    }
        
    for (int i = left_child_num - 2; i >= left_num; i --)  {
        keys[i] = leftBro->keys[i - left_num];
    }
        
    this->nKeys += left_child_num - left_num;
    this->nChild += left_child_num - left_num;


    for (int i = left_num; i < left_child_num; i ++ ) {
        leftBro->keys[i] = 0;
        leftBro->childrens[i] = NULL;
        leftBro->nKeys --;
        leftBro->nChild --;
    }
}

// this node and its right brother redistribute
// the right has more entries
void InnerNode::redistributeRight(const int& index, InnerNode* const& rightBro, InnerNode* const& parent) {
    // TODO
        // TODO
    //origin num
    int right_nChild = rightBro->getChildNum();
    //int right_nKeys = rightBro->getKeyNum();
    //get both node child num after adjust
    int nRight = (right_nChild + nChild + 1) / 2;
    //int nThis = (right_nChild + nChild) / 2;
    //pull down the middle key
    keys[this->nKeys] = parent->getKey(index);
    parent->keys[index] = rightBro->getKey(right_nChild - nRight - 1);
    //directly move node to left
    for (int i = 0; i < right_nChild - nRight; i ++ ) {
        this->keys[i + this->nKeys + 1] = rightBro->keys[i];
        this->childrens[i + this->nChild] = rightBro->childrens[i];
    }
    this->nKeys += right_nChild - nRight;
    this->nChild += right_nChild - nRight;
    
    for (int i = 0; i < right_nChild - nRight; i ++ ) {
        rightBro->keys[i] = rightBro->keys[right_nChild - nRight];
        rightBro->childrens[i] = rightBro->childrens[right_nChild - nRight];
        rightBro->nKeys --;
        rightBro->nChild --;
    }
}

// merge all entries to its left bro, delete this node after merging.
void InnerNode::mergeLeft(InnerNode* const& leftBro, const Key& k) {
    // TODO
    int left_nkeys = leftBro->getKeyNum();
    int left_nchild = leftBro->getChildNum();
    leftBro->keys[left_nkeys] = k;
    for (int i = left_nkeys + 1; i < left_nkeys + 1 + this->nKeys; i ++ )   {
        leftBro->keys[i] = this->keys[i];
    }
        
    for (int i = left_nchild; i < left_nchild + this->nChild; i ++) {
        leftBro->childrens[i] = this->childrens[i];
    }
        
    leftBro->nKeys += this->nKeys + 1;
    leftBro->nChild += this->nChild;

    nChild = 0;
    nKeys = 0;
}

// merge all entries to its right bro, delete this node after merging.
void InnerNode::mergeRight(InnerNode* const& rightBro, const Key& k) {
    // TODO
    int right_nkeys = rightBro->getKeyNum();
    int right_nchild = rightBro->getChildNum();
    for (int i = right_nkeys - 1; i >= 0; i -- )    {
        rightBro->keys[i + this->nKeys + 1] = rightBro->keys[i];
    }
        
    for (int i = right_nchild - 1; i >= 0; i -- )   {
        rightBro->childrens[i + this->nChild] = rightBro->childrens[i];
    }
        
    rightBro->keys[this->nKeys] = k;   

    for (int i = 0; i < this->nKeys; i ++ ) {
        rightBro->keys[i] = this->keys[i];
    }
        
    for (int i = 0; i < this->nChild; i ++ )    {
        rightBro->childrens[i] = this->childrens[i];
    }
        
    rightBro->nKeys += this->nKeys + 1;
    rightBro->nChild += this->nChild;

    nChild = 0;
    nKeys = 0;
}

// remove a children from the current node, used by remove func
void InnerNode::removeChild(const int& keyIdx, const int& childIdx) {
    // TODO
    delete childrens[childIdx];
    for (int i = keyIdx; i < nKeys - 1; i ++ )
        keys[i] = keys[i + 1];
    for (int i = childIdx; i < nChild - 1; i ++ )
        childrens[i] = childrens[i + 1];
    this->nChild --;
    this->nKeys --;
}

// update the target entry, return true if the update succeed.
bool InnerNode::update(const Key& k, const Value& v) {
    // TODO
    if(nKeys==0&&nChild==0) 
        return false;
    int idx=findIndex(k);
    return childrens[idx]->update(k,v);
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
    int hash =  keyHash(k);
    for(int i = 0;i < degree*2;i++) {
        if(getBit(i) == 1 && fingerprints[i] == hash)   {
            if(getKey(i) == k)  {
                int offset = i%8;
                int pos=i/8;
                Byte* cursor = bitmap;
                cursor += pos;
                Byte bits = *cursor;
                bits = ~((~bits) | (1<<offset));
                *(cursor) = bits;
            }
        }
    }
    if(n == 0)  {
        ifDelete = true;
        if(this->prev != NULL)  
            this->prev->next = this->next;
        if(this->next != NULL)  {
            this->next->prev = this->prev;
        }
        PAllocator *tmp = PAllocator::getAllocator();
        auto pth = this->getPPointer();
        tmp->freeLeaf(pth);
        parent->removeChild(index, index);
    }
    else this->persist();
    return ifRemove;
}

// update the target entry
// return TRUE if the update succeed
bool LeafNode::update(const Key& k, const Value& v) {
    bool ifUpdate = false;
    // TODO
    int hash = keyHash(k);
    for(int i = 0;i < degree*2;i++) {
        if(getBit(i)==1&&(fingerprints[i]==hash)){
            if(getKey(i)==k){
                kv[i].v=v;
                ifUpdate=true;
                break;
            }
        }
    }
    this->persist();
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
