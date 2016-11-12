#include "BTreeNode.h"
#include <cstring>
#include <cstdio>   // for debug

using namespace std;

/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::read(PageId pid, const PageFile& pf)
{ 
    RC rc = pf.read(pid, buffer);
    
    if (!rc) {
        // success
        memcpy(&nextNodePtr, buffer + OFFSET_NEXT_NODE_PTR, sizeof(PageId));
        memcpy(&currentKeyCount, buffer + OFFSET_CURRENT_KEY_COUNT, sizeof(int));
    }

    return rc;
}
    
/*
 * Write the content of the node to the page pid in the PageFile pf.
 * @param pid[IN] the PageId to write to
 * @param pf[IN] PageFile to write to
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::write(PageId pid, PageFile& pf)
{
    memcpy(buffer + OFFSET_NEXT_NODE_PTR, &nextNodePtr, sizeof(PageId));
    memcpy(buffer + OFFSET_CURRENT_KEY_COUNT, &currentKeyCount, sizeof(int));

    return pf.write(pid, buffer);
}

/*
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
int BTLeafNode::getKeyCount()
{ return currentKeyCount; }

/*
 * Insert a (key, rid) pair to the node.
 * @param key[IN] the key to insert
 * @param rid[IN] the RecordId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTLeafNode::insert(int key, const RecordId& rid)
{
    if (currentKeyCount == ENTRY_LIMIT)
        return RC_NODE_FULL;

    LeafEntry le(key, rid);
    
    // no LeafEntry yet, trivial
    if (currentKeyCount == 0) {
        insertLeafEntry(0, &le);
        return 0;
    }

    // only one LeafEntry
    if (currentKeyCount == 1) {
        int firstKey;
        RecordId firstRid;
        readEntry(0, firstKey, firstRid);

        if (key > firstKey) {
            insertLeafEntry(1, &le);
        } else {
            insertLeafEntry(0, &le);
        }

        return 0;
    }

    // binary search
    int leftIndex = 0,
        rightIndex = currentKeyCount - 1,
        midIndex,
        midKey;
    RecordId midRid;

    while (leftIndex < rightIndex - 1) {
        midIndex = leftIndex + (rightIndex - leftIndex) / 2;
        readEntry(midIndex, midKey, midRid);

        if (midKey > key) {
            rightIndex = midIndex;
        } else {
            // assume no duplicate
            leftIndex = midIndex;
        }
    }
    
    // find position to insert
    int leftKey,
        rightKey;
    RecordId recordIdTemp;
    readEntry(leftIndex, leftKey, recordIdTemp);
    readEntry(rightIndex, rightKey, recordIdTemp);

    if (key < leftKey) {
        insertLeafEntry(leftIndex, &le);
    } else if (key < rightKey) {
        insertLeafEntry(rightIndex, &le);
    } else {
        insertLeafEntry(rightIndex + 1, &le);
    }

    return 0;
}

/*
 * Insert the (key, rid) pair to the node
 * and split the node half and half with sibling.
 * The first key of the sibling node is returned in siblingKey.
 * @param key[IN] the key to insert.
 * @param rid[IN] the RecordId to insert.
 * @param sibling[IN] the sibling node to split with. This node MUST be EMPTY when this function is called.
 * @param siblingKey[OUT] the first key in the sibling node after split.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::insertAndSplit(int key, const RecordId& rid, 
                              BTLeafNode& sibling, int& siblingKey)
{ return 0; }

/**
 * If searchKey exists in the node, set eid to the index entry
 * with searchKey and return 0. If not, set eid to the index entry
 * immediately after the largest index key that is smaller than searchKey,
 * and return the error code RC_NO_SUCH_RECORD.
 * Remember that keys inside a B+tree node are always kept sorted.
 * @param searchKey[IN] the key to search for.
 * @param eid[OUT] the index entry number with searchKey or immediately
                   behind the largest key smaller than searchKey.
 * @return 0 if searchKey is found. Otherwise return an error code.
 */
RC BTLeafNode::locate(int searchKey, int& eid)
{ return 0; }

/*
 * Read the (key, rid) pair from the eid entry.
 * @param eid[IN] the entry number to read the (key, rid) pair from
 * @param key[OUT] the key from the entry
 * @param rid[OUT] the RecordId from the entry
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::readEntry(int eid, int& key, RecordId& rid)
{
    if (eid >= currentKeyCount) {
        // out of bound
        return RC_INVALID_CURSOR;
    }

    LeafEntry le;
    memcpy(&le, buffer + eid * sizeof(LeafEntry), sizeof(LeafEntry));
    key = le.key;
    rid = le.rid;
    return 0;
}

/*
 * Return the pid of the next slibling node.
 * @return the PageId of the next sibling node 
 */
PageId BTLeafNode::getNextNodePtr()
{ return 0; }

/*
 * Set the pid of the next slibling node.
 * @param pid[IN] the PageId of the next sibling node 
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::setNextNodePtr(PageId pid)
{ return 0; }

// helpers
void BTLeafNode::insertLeafEntry(int eid, LeafEntry* ptr) {
    // assuming eid is valid (<= currentKeyCount) and we won't go above limit
    for (int i = currentKeyCount - 1; i >= eid; i--) {
        // push the entries back 
        memcpy(buffer + (i+1) * sizeof(LeafEntry), buffer + i * sizeof(LeafEntry), sizeof(LeafEntry));
    }

    // insert the LeafEntry
    memcpy(buffer + eid * sizeof(LeafEntry), ptr, sizeof(LeafEntry));
    currentKeyCount++;
}


// TODO: debug
void BTLeafNode::debug() {
    fprintf(stdout, "==========Debug==========\n");
    fprintf(stdout, "currentKeyCount is %i\n", currentKeyCount);
    
    int key;
    RecordId rid;
    for (int i = 0; i < currentKeyCount; i++) {
        readEntry(i, key, rid);
        fprintf(stdout, "LeafEntry %i has key %i\n", i, key);
    }
}




/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::read(PageId pid, const PageFile& pf)
{ return 0; }
    
/*
 * Write the content of the node to the page pid in the PageFile pf.
 * @param pid[IN] the PageId to write to
 * @param pf[IN] PageFile to write to
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::write(PageId pid, PageFile& pf)
{ return 0; }

/*
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
int BTNonLeafNode::getKeyCount()
{ return 0; }


/*
 * Insert a (key, pid) pair to the node.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTNonLeafNode::insert(int key, PageId pid)
{ return 0; }

/*
 * Insert the (key, pid) pair to the node
 * and split the node half and half with sibling.
 * The middle key after the split is returned in midKey.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @param sibling[IN] the sibling node to split with. This node MUST be empty when this function is called.
 * @param midKey[OUT] the key in the middle after the split. This key should be inserted to the parent node.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::insertAndSplit(int key, PageId pid, BTNonLeafNode& sibling, int& midKey)
{ return 0; }

/*
 * Given the searchKey, find the child-node pointer to follow and
 * output it in pid.
 * @param searchKey[IN] the searchKey that is being looked up.
 * @param pid[OUT] the pointer to the child node to follow.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::locateChildPtr(int searchKey, PageId& pid)
{ return 0; }

/*
 * Initialize the root node with (pid1, key, pid2).
 * @param pid1[IN] the first PageId to insert
 * @param key[IN] the key that should be inserted between the two PageIds
 * @param pid2[IN] the PageId to insert behind the key
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::initializeRoot(PageId pid1, int key, PageId pid2)
{ return 0; }
