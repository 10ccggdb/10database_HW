#include"utility/p_allocator.h"
#include<iostream>
using namespace std;

// the file that store the information of allocator
const string P_ALLOCATOR_CATALOG_NAME = "p_allocator_catalog";
// a list storing the free leaves
const string P_ALLOCATOR_FREE_LIST    = "free_list";

PAllocator* PAllocator::pAllocator = new PAllocator();

PAllocator* PAllocator::getAllocator() {
    if (PAllocator::pAllocator == NULL) {
        PAllocator::pAllocator = new PAllocator();
    }
    return PAllocator::pAllocator;
}

/* data storing structure of allocator
   In the catalog file, the data structure is listed below
   | maxFileId(8 bytes) | freeNum = m | treeStartLeaf(the PPointer) |
   In freeList file:
   | freeList{(fId, offset)1,...(fId, offset)m} |
*/
PAllocator::PAllocator() {
    string allocatorCatalogPath = DATA_DIR + P_ALLOCATOR_CATALOG_NAME;
    string freeListPath         = DATA_DIR + P_ALLOCATOR_FREE_LIST;
    ifstream allocatorCatalog(allocatorCatalogPath, ios::in|ios::binary);
    ifstream freeListFile(freeListPath, ios::in|ios::binary);
    // judge if the catalog exists
    if (allocatorCatalog.is_open() && freeListFile.is_open()) {
        // exist
        // TODO

        allocatorCatalog.read((char *)(&maxFileId), sizeof(maxFileId));
        allocatorCatalog.read((char *)(&freeNum), sizeof(freeNum));
        allocatorCatalog.read((char *)(&startLeaf), sizeof(startLeaf));
        allocatorCatalog.close();
        for(int i =0;i < freeNum;i++)  {
            PPointer tmp;
            freeListFile.read( (char*) (&tmp),sizeof(tmp) );
            freeList.push_back(tmp);
        }
        freeListFile.close();
    } else {
        // not exist, create catalog and free_list file, then open.
        // TODO
        ofstream catalog(allocatorCatalogPath, ios::out);
        ofstream freelist(freeListPath, ios::out);
        catalog.close();
        freelist.close();
       fopen(allocatorCatalogPath , "wb");
       fclose(allocatorCatalogPath);
       maxFileId = 1;
        freeNum = 0;
    }
    this->initFilePmemAddr();
}

PAllocator::~PAllocator() {
    // TODO
    persistCatalog();
    string tmppath = DATA_DIR + P_ALLOCATOR_FREE_LIST;
    ofstream freeListFile(tmppath,ios::out|ios::binary);
    //open file
    uint16_t i;
    //xuanhuan
    for(i = 0;i < freeNum;i++)  {
        PPointer tow = freeList[i];
        //write
        freeListFile.write((char *)(&tow), sizeof(tow)); 
    }
    //close
    freeListFile.close();
    pAllocator = NULL;
    //finish
}

// memory map all leaves to pmem address, storing them in the fId2PmAddr
void PAllocator::initFilePmemAddr() {
    // TODO
    uint64_t i;
    size_t mapped_len;
    int is_pmem;
    size_t leafgroup_len = LEAF_GROUP_HEAD+LEAF_GROUP_AMOUNT*calLeafSize();
    for(i = 1;i < maxFileId;i++)    {
        string filepath = DATA_DIR + to_string(i);
        
        fId2PmAddr[i] = (char*)pmem_map_file(filepath.c_str(), leafgroup_len, PMEM_FILE_CREATE,
                                      0666, &mapped_len, &is_pmem); 

    }   

    
}

// get the pmem address of the target PPointer from the map fId2PmAddr
char* PAllocator::getLeafPmemAddr(PPointer p) {
    // TODO
    if(p.fileId <= maxFileId && p.fileId != ILLEGAL_FILE_ID)    {
        return fId2PmAddr[p.fileId] + p.offset;
    }
    else
        return NULL;
}

// get and use a leaf for the fptree leaf allocation
// return 
bool PAllocator::getLeaf(PPointer &p, char* &pmem_addr) {
    // TODO
    if(freeList.empty())    {
        newLeafGroup();
    }
    p = freeList.back();
    freeList.pop_back();
    freeNum--;
    pmem_addr = getLeafPmemAddr(p);
    
    string path = DATA_DIR + to_string(p.fileId);
    fstream LeafGroupfile(path,ios::in|ios::out|ios::binary);
    uint64_t usedtmie;
    uint8_t bitmap[LEAF_GROUP_AMOUNT];
    LeafGroupfile.read((char*)&bitmap,sizeof(bitmap));
     bitmap[(p.offset-LEAF_GROUP_HEAD)/calLeafSize()] = 1;
    LeafGroupfile.read((char*)&usedtmie,sizeof(usedtmie));
    usedtmie++;
    LeafGroupfile.seekg(0,ios::beg);
    LeafGroupfile.write((char*)&usedtmie,sizeof(usedtmie));
    LeafGroupfile.write((char*)&bitmap,sizeof(bitmap));
    LeafGroupfile.close();
    return true;

}

bool PAllocator::ifLeafUsed(PPointer p) {
    // TODO
    if(ifLeafExist(p))  {
        for(int i = 0;i < freeList.size();i++)  {
            if(p == freeList[i])
                return false;
        }
        return true;
    }
    return false;
}

bool PAllocator::ifLeafFree(PPointer p) {
    // TODO
    if(ifLeafExist(p) && !ifLeafUsed(p))    
        return true;
    return false;
}

// judge whether the leaf with specific PPointer exists. 
bool PAllocator::ifLeafExist(PPointer p) {
    // TODO
    if (p.fileId < maxFileId && p.fileId != ILLEGAL_FILE_ID)
        return true;
    return false;
}

// free and reuse a leaf
bool PAllocator::freeLeaf(PPointer p) {
    // TODO
    Byte bit = 0;
    uint64_t usedtime;
    string path = DATA_DIR + to_string(p.fileId);
    fstream file(path,ios::in|ios::binary|ios::out);
    if(!file.good())    return false;
    file.read((char*)&(usedtime), sizeof(uint64_t));
    usedtime--;

    file.seekg(0,ios::beg);
    file.write((char*)&(usedtime), sizeof(uint64_t));
    file.seekg((sizeof(uint64_t) + ((p.offset - LEAF_GROUP_HEAD) / calLeafSize()), ios::beg));
    file.write((char*)&(bit), sizeof(Byte));
    freeList.push_back(p);
    freeNum++;
    return true;
}

bool PAllocator::persistCatalog() {
    // TODO
    string allocatorCatalogPath = DATA_DIR + P_ALLOCATOR_CATALOG_NAME;
    ofstream allocatorCatalog(allocatorCatalogPath, ios::out|ios::binary);
    if (!allocatorCatalog)
        return false;
    allocatorCatalog.write((char *)(&maxFileId), sizeof(maxFileId));
    allocatorCatalog.write((char *)(&freeNum), sizeof(freeNum));
    allocatorCatalog.write((char *)(&startLeaf), sizeof(startLeaf));
    return true;
}

/*
  Leaf group structure: (uncompressed)
  | usedNum(8b) | bitmap(n * byte) | leaf1 |...| leafn |
*/
// create a new leafgroup, one file per leafgroup
bool PAllocator::newLeafGroup() {
    // TODO
string fileIdPath=DATA_DIR + to_string(maxFileId);
    ofstream leafGroup(fileIdPath,ios::out|ios::binary);
    if(leafGroup.is_open())
    {
        uint64_t usedNum = 0;
        uint8_t bitmap[LEAF_GROUP_AMOUNT*(1+calLeafSize())] = {0};        
        leafGroup.write((char*)&usedNum,sizeof(usedNum));
        leafGroup.write((char*)&bitmap,sizeof(bitmap));
        int i = 0;
        for (i = 0; i < LEAF_GROUP_AMOUNT; i++) {
            PPointer p;
            p.fileId = maxFileId;
            p.offset = LEAF_GROUP_HEAD + i * calLeafSize();
            freeList.push_back(p);
        } 
        freeNum += LEAF_GROUP_AMOUNT;   
        maxFileId++;    
        leafGroup.close();
        return true;
    }
    return false;
}