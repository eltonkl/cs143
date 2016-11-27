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
{
    // TODO: behavior is undefined with few entries (like < 3)

    // first LeafEntry to move
    int splitIndex = (currentKeyCount + 1) / 2;
    for (int i = splitIndex; i < currentKeyCount; i++) {
        // move second half of LeafEntry to sibling
        int curKey;
        RecordId curRid;
        readEntry(i, curKey, curRid);
        sibling.insert(curKey, curRid);
    }

    // update member variables
    currentKeyCount = splitIndex;
    int curLastKey;
    RecordId curLastRid;
    readEntry(currentKeyCount - 1, curLastKey, curLastRid);

    // check where to insert current key
    if (key < curLastKey) {
        if (currentKeyCount > sibling.getKeyCount()) {
            // uneven, move cur last to sibling
            sibling.insert(curLastKey, curLastRid);
            currentKeyCount--;
        }

        insert(key, rid);
    } else {
        sibling.insert(key, rid);
    }

    RecordId dummyRid;
    sibling.readEntry(0, siblingKey, dummyRid);
    return 0;
    // TODO error checking
}

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
{ 
    if (currentKeyCount == 0) {
        eid = 0;
        return RC_NO_SUCH_RECORD;
    }

    if (currentKeyCount == 1) {
        int key;
        RecordId rid;
        readEntry(0, key, rid);

        if (key == searchKey) {
            eid = 0;
            return 0;
        }

        if (searchKey > key) {
            eid = 1;
        } else {
            eid = 0;
        }
        return RC_NO_SUCH_RECORD;
    }

    int key;
    RecordId rid;
    readEntry(0, key, rid);

    if (searchKey < key) {
        eid = 0;
        return RC_NO_SUCH_RECORD;
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

        if (searchKey == midKey) {
            // found
            eid = midIndex;
            return 0;
        }

        if (midKey > searchKey) {
            rightIndex = midIndex;
        } else {
            leftIndex = midIndex;
        }
    }

    // check
    int leftKey,
        rightKey;
    RecordId dummyRid;
    readEntry(leftIndex, leftKey, dummyRid);
    readEntry(rightIndex, rightKey, dummyRid);

    if (searchKey == leftKey) {
        eid = leftIndex;
        return 0;
    }

    if (searchKey == rightKey) {
        eid = rightIndex;
        return 0;
    }

    // not found
    if (searchKey < leftKey) {
        eid = leftIndex;
    } else if (searchKey < rightKey) {
        eid = rightIndex;
    } else {
        eid = rightIndex + 1;
    }

    return RC_NO_SUCH_RECORD;
}

/*
 * Read the (key, rid) pair from the eid entry.
 * @param eid[IN] the entry number to read the (key, rid) pair from
 * @param key[OUT] the key from the entry
 * @param rid[OUT] the RecordId from the entry
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::readEntry(int eid, int& key, RecordId& rid)
{
    if (eid >= currentKeyCount || eid < 0) {
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
{
    // TODO: check end of tree RC_END_OF_TREE
    // TODO: set to -1 initially so check this as invalid
    return nextNodePtr;
}

/*
 * Set the pid of the next slibling node.
 * @param pid[IN] the PageId of the next sibling node 
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::setNextNodePtr(PageId pid)
{
    // TODO: when can an error occur??
    nextNodePtr = pid;
    return 0;
}

////////////////////////////
// private helpers and public utility functions
////////////////////////////
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
    fprintf(stdout, "==========Debug BTLeafNode==========\n");
    fprintf(stdout, "currentKeyCount is %i\n", currentKeyCount);
    fprintf(stdout, "nextNodePtr is %i\n", nextNodePtr);
    
    int key;
    RecordId rid;
    for (int i = 0; i < currentKeyCount; i++) {
        readEntry(i, key, rid);
        fprintf(stdout, "LeafEntry %i has key %i\n", i, key);
    }
}


////////////////////////////
// Non leaf
////////////////////////////

/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::read(PageId pid, const PageFile& pf)
{
    RC rc = pf.read(pid, buffer);

    if (!rc) {
        // success
        memcpy(&firstPageId, buffer + OFFSET_FIRST_PAGE_ID, sizeof(PageId));
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
RC BTNonLeafNode::write(PageId pid, PageFile& pf)
{
    memcpy(buffer + OFFSET_FIRST_PAGE_ID, &firstPageId, sizeof(PageId));
    memcpy(buffer + OFFSET_CURRENT_KEY_COUNT, &currentKeyCount, sizeof(int));
    return pf.write(pid, buffer);
}

/*
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
int BTNonLeafNode::getKeyCount()
{ return currentKeyCount; }

/*
 * Set the first page's PageId.
 * @param pid[IN] the PageId of the first child node 
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::setFirstPageId(PageId pid) {
    firstPageId = pid;
    return 0;
}


/*
 * Insert a (key, pid) pair to the node.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTNonLeafNode::insert(int key, PageId pid)
{
    if (currentKeyCount == ENTRY_LIMIT)
        return RC_NODE_FULL;

    NonLeafEntry nle(key, pid);

    // no NonLeafEntry yet, trivial
    if (currentKeyCount == 0) {
        insertNonLeafEntry(0, &nle);
        return 0;
    }

    // only one NonLeafEntry
    if (currentKeyCount == 1) {
        int firstKey;
        PageId firstPid;
        readEntry(0, firstKey, firstPid);

        if (key > firstKey) {
            insertNonLeafEntry(1, &nle);
        } else {
            insertNonLeafEntry(0, &nle);
        }

        return 0;
    }

    // binary search
    int leftIndex = 0,
        rightIndex = currentKeyCount - 1,
        midIndex,
        midKey;
    PageId midPid;

    while (leftIndex < rightIndex - 1) {
        midIndex = leftIndex + (rightIndex - leftIndex) / 2;
        readEntry(midIndex, midKey, midPid);

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
    PageId dummyPageId;
    readEntry(leftIndex, leftKey, dummyPageId);
    readEntry(rightIndex, rightKey, dummyPageId);

    if (key < leftKey) {
        insertNonLeafEntry(leftIndex, &nle);
    } else if (key < rightKey) {
        insertNonLeafEntry(rightIndex, &nle);
    } else {
        insertNonLeafEntry(rightIndex + 1, &nle);
    }

    return 0;
}

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
{
    // TODO: behavior is undefined with few entries (like < 3)

    // determine split position
    bool shouldInsertNewKey = true;
    int midIndex = currentKeyCount / 2,
        splitIndex = midIndex + 1,  // first entry of the right sibling
        leftKey;
    PageId dummyPid;
    readEntry(midIndex, midKey, dummyPid);
    readEntry(midIndex - 1, leftKey, dummyPid);

    if (currentKeyCount % 2 == 0) {
        if (key < midKey) {
            // left is going to be 2+ more than right
            if (key < leftKey) {
                // just use cur left as new mid
                midIndex--;
                splitIndex--;
                midKey = leftKey;
            } else {
                // key > leftKey, key to "insert" will be midKey
                midKey = key;
                shouldInsertNewKey = false;

                // since we are not inserting new key, use midIndex as splitIndex
                splitIndex = midIndex;

                // since we are not actually inserting the new key, the sibling's firstPageId will be the pid we want to insert
                sibling.setFirstPageId(pid);
            }
        }
    }

    if (shouldInsertNewKey) {
        // update sibling's firstPageId
        int dummyKey;
        PageId midPid;
        readEntry(midIndex, dummyKey, midPid);
        sibling.setFirstPageId(midPid);
    }

    // move second half of NonLeafEntries to sibling
    for (int i = splitIndex; i < currentKeyCount; i++) {
        int curKey;
        PageId curPageId;
        readEntry(i, curKey, curPageId);
        sibling.insert(curKey, curPageId);
    }

    // update member variables
    currentKeyCount = splitIndex;
    if (shouldInsertNewKey) {
        // the last element in the current half of is used as midKey
        // it should be invalidated
        currentKeyCount--;

        if (key < midKey)
            insert(key, pid);
        else
            sibling.insert(key, pid);
    }

    return 0;
}

/*
 * Given the searchKey, find the child-node pointer to follow and
 * output it in pid.
 * @param searchKey[IN] the searchKey that is being looked up.
 * @param pid[OUT] the pointer to the child node to follow.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::locateChildPtr(int searchKey, PageId& pid)
{
    if (currentKeyCount == 0)
        return RC_NO_SUCH_RECORD;

    // check first
    int firstKey;
    PageId firstPid;
    readEntry(0, firstKey, firstPid);
    if (searchKey < firstKey) {
        pid = firstPageId;
        return 0;
    }

    // check only one entry
    if (currentKeyCount == 1) {
        pid = firstPid;
        return 0;
    }


    // binary search
    int leftIndex = 0,
        rightIndex = currentKeyCount - 1,
        midIndex,
        midKey;
    PageId midPid;

    while (leftIndex < rightIndex - 1) {
        midIndex = leftIndex + (rightIndex - leftIndex) / 2;
        readEntry(midIndex, midKey, midPid);

        if (searchKey == midKey) {
            // found
            pid = midPid;
            return 0;
        }

        if (midKey > searchKey) {
            rightIndex = midIndex;
        } else {
            leftIndex = midIndex;
        }
    }

    // check
    int leftKey,
        rightKey;
    PageId leftPid,
            rightPid;
    readEntry(leftIndex, leftKey, leftPid);
    readEntry(rightIndex, rightKey, rightPid);

    if (searchKey < rightKey) {
        pid = leftPid;
    } else {
        pid = rightPid;
    }

    return 0;
}

/*
 * Initialize the root node with (pid1, key, pid2).
 * @param pid1[IN] the first PageId to insert
 * @param key[IN] the key that should be inserted between the two PageIds
 * @param pid2[IN] the PageId to insert behind the key
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::initializeRoot(PageId pid1, int key, PageId pid2)
{

    firstPageId = pid1;
    insert(key, pid2);
    return 0;
}


////////////////////////////
// private helpers and public utility functions
////////////////////////////
void BTNonLeafNode::insertNonLeafEntry(int eid, NonLeafEntry* ptr) {
    // assuming eid is valid (<= currentKeyCount) and we won't go above limit
    for (int i = currentKeyCount - 1; i >= eid; i--) {
        // push the entries back 
        memcpy(buffer + (i+1) * sizeof(NonLeafEntry), buffer + i * sizeof(NonLeafEntry), sizeof(NonLeafEntry));
    }

    // insert the NonLeafEntry
    memcpy(buffer + eid * sizeof(NonLeafEntry), ptr, sizeof(NonLeafEntry));
    currentKeyCount++;
}


/**
 * Read the (key, pid) pair from the eid entry.
 * @param eid[IN] the entry number to read the (key, pid) pair from
 * @param key[OUT] the key from the slot
 * @param pid[OUT] the PageId from the slot
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::readEntry(int eid, int& key, PageId& pid) {
    if (eid >= currentKeyCount) {
        // out of bound
        return RC_INVALID_CURSOR;
    }

    NonLeafEntry nle;
    memcpy(&nle, buffer + eid * sizeof(NonLeafEntry), sizeof(NonLeafEntry));
    key = nle.key;
    pid = nle.pid;
    return 0;
}

// TODO: debug
void BTNonLeafNode::debug() {
    fprintf(stdout, "==========Debug BTNonLeafNode==========\n");
    fprintf(stdout, "currentKeyCount is %i\n", currentKeyCount);
    fprintf(stdout, "firstPageId is %i\n", firstPageId);
    
    int key;
    PageId pid;
    for (int i = 0; i < currentKeyCount; i++) {
        readEntry(i, key, pid);
        fprintf(stdout, "LeafEntry %i has key %i and PageId %i\n", i, key, pid);
    }
}