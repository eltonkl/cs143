/*
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */

#include "BTreeIndex.h"
#include "BTreeNode.h"
#include <cstring>
#include <cstdio>   // for debug

using namespace std;

/*
 * BTreeIndex constructor
 */
BTreeIndex::BTreeIndex()
{
    rootPid = -1;
}

/*
 * Open the index file in read or write mode.
 * Under 'w' mode, the index file should be created if it does not exist.
 * @param indexname[IN] the name of the index file
 * @param mode[IN] 'r' for read, 'w' for write
 * @return error code. 0 if no error
 */
RC BTreeIndex::open(const string& indexname, char mode)
{
    // store mode for BTreeIndex::close()
    if (mode == 'W' || mode == 'w')
        isPfWriteMode = true;
    else
        isPfWriteMode = false;
    
    RC rc = pf.open(indexname, mode);
    if (rc)
        return rc;
    
    if (pf.endPid() == 0) {
        // new PageFile, no page written yet
        rootPid = -1;
        treeHeight = -1;

        // we need to write to PageFile first to reserve the 0th page for TreeIndex's content
        // update buffer
        memcpy(buffer, &rootPid, sizeof(PageId));
        memcpy(buffer + OFFSET_TREE_HEIGHT, &treeHeight, sizeof(int));

        // write from buffer to disk
        rc = pf.write(PID_TREE_INDEX, buffer);
    } else {
        // old PageFile, need to read contents
        // read content from disk to buffer
        rc = pf.read(PID_TREE_INDEX, buffer);
        if (!rc) {
            // update member variables
            memcpy(&rootPid, buffer, sizeof(PageId));
            memcpy(&treeHeight, buffer + OFFSET_TREE_HEIGHT, sizeof(int));
        }
    }

    return rc;
}

/*
 * Close the index file.
 * @return error code. 0 if no error
 */
RC BTreeIndex::close()
{
    if (!isPfWriteMode)
        return pf.close();

    // update buffer
    memcpy(buffer, &rootPid, sizeof(PageId));
    memcpy(buffer + OFFSET_TREE_HEIGHT, &treeHeight, sizeof(int));

    // write from buffer to disk
    RC rc = pf.write(PID_TREE_INDEX, buffer);
    if (!rc) // write success
        return pf.close();
    
    return rc;
}

/*
 * Insert (key, RecordId) pair to the index.
 * @param key[IN] the key for the value inserted into the index
 * @param rid[IN] the RecordId for the record being inserted into the index
 * @return error code. 0 if no error
 */
RC BTreeIndex::insert(int key, const RecordId& rid)
{
    if (!isPfWriteMode)
        return RC_INVALID_FILE_MODE;

    RC rc;

    // base case: empty
    if (rootPid == -1) {
        BTLeafNode btln1, btln2;
        // set the right node because of the way B+ tree works
        rc = btln2.insert(key, rid);
        if (rc)
            return rc;

        // PageId to store btln1 and btln2
        PageId pid1 = pf.endPid(),
                pid2 = pid1 + 1;
        
        // set nextNodePtr
        btln1.setNextNodePtr(pid2);

        // write btln1 and btln2
        rc = btln1.write(pid1, pf);
        if (rc)
            return rc;
        rc = btln2.write(pid2, pf);
        if (rc)
            return rc;

        // initialize root
        BTNonLeafNode btnln;
        rc = btnln.initializeRoot(pid1, key, pid2);
        if (rc)
            return rc;
        rc = btnln.write(pf.endPid(), pf);
        if (rc)
            return rc;

        // update member variables
        rootPid = pf.endPid() - 1;
        treeHeight = 1;
        
        return 0;
    }

    // need to traverse and insert 
    int ofKey;
    PageId ofPid;
    rc = traverseAndInsert(key, rid, rootPid, 0, ofKey, ofPid);

    if (rc == RC_STATUS_INSERT_NEW_ROOT) {
        // new root
        BTNonLeafNode newRoot;
        rc = newRoot.initializeRoot(rootPid, ofKey, ofPid);
        if (rc)
            return rc;
        rootPid = pf.endPid();
        treeHeight++;
        return newRoot.write(rootPid, pf);
    }

    return rc;
}

/**
 * Run the standard B+Tree key search algorithm and identify the
 * leaf node where searchKey may exist. If an index entry with
 * searchKey exists in the leaf node, set IndexCursor to its location
 * (i.e., IndexCursor.pid = PageId of the leaf node, and
 * IndexCursor.eid = the searchKey index entry number.) and return 0.
 * If not, set IndexCursor.pid = PageId of the leaf node and
 * IndexCursor.eid = the index entry immediately after the largest
 * index key that is smaller than searchKey, and return the error
 * code RC_NO_SUCH_RECORD.
 * Using the returned "IndexCursor", you will have to call readForward()
 * to retrieve the actual (key, rid) pair from the index.
 * @param key[IN] the key to find
 * @param cursor[OUT] the cursor pointing to the index entry with
 *                    searchKey or immediately behind the largest key
 *                    smaller than searchKey.
 * @return 0 if searchKey is found. Othewise an error code
 */
RC BTreeIndex::locate(int searchKey, IndexCursor& cursor)
{
    if (rootPid == -1) {
        return RC_NO_SUCH_RECORD;
    }

    return traverseAndLocate(searchKey, cursor, rootPid, 0);
}

/*
 * Read the (key, rid) pair at the location specified by the index cursor,
 * and move foward the cursor to the next entry.
 * @param cursor[IN/OUT] the cursor pointing to an leaf-node index entry in the b+tree
 * @param key[OUT] the key stored at the index cursor location.
 * @param rid[OUT] the RecordId stored at the index cursor location.
 * @return error code. 0 if no error
 */
RC BTreeIndex::readForward(IndexCursor& cursor, int& key, RecordId& rid)
{
    if (cursor.pid == -1) {
        return RC_END_OF_TREE;
    }
    
    BTLeafNode btln;
    RC rc = btln.read(cursor.pid, pf);
    if (rc) // read failed
        return rc;

    rc = btln.readEntry(cursor.eid, key, rid);
    if (rc) // readEntry failed
        return rc;
    
    // move cursor to the next entry
    cursor.eid++;
    if (cursor.eid == btln.getKeyCount()) {
        // last eid in current leaf node
        cursor.pid = btln.getNextNodePtr();
        cursor.eid = 0;
    }
    
    return 0;
}


///////////////////////////
// private helper functions
///////////////////////////
RC BTreeIndex::traverseAndInsert(int key, const RecordId& rid, PageId pid, int curDepth, int& ofKey, PageId& ofPid)
{
    RC rc;

    if (curDepth == treeHeight) {
        // this is leaf
        BTLeafNode btln;
        rc = btln.read(pid, pf);
        if (rc) // read failed
            return rc;

        rc = btln.insert(key, rid);

        if (!rc)
            // successful insert
            return btln.write(pid, pf);

        if (rc == RC_NODE_FULL) {
            BTLeafNode sib;

            rc = btln.insertAndSplit(key, rid, sib, ofKey);
            if (!rc) {
                // successful split and insert
                ofPid = pf.endPid();    // get the next empty pid
                sib.setNextNodePtr(btln.getNextNodePtr());  // set sib's next ptr to cur's next ptr
                btln.setNextNodePtr(ofPid); // set cur's next ptr to sib's ptr

                // write two leaf nodes to disk
                rc = btln.write(pid, pf);
                if (rc)
                    return rc;
                rc = sib.write(ofPid, pf);
                if (rc)
                    return rc;

                return RC_STATUS_INSERT_LEAF_OF;
            }

            return rc;  // unsuccessful split and insert
        } else {
            // TODO: what now?
            return rc;
        }
    }

    // Non leaf node
    BTNonLeafNode btnln;
    rc = btnln.read(pid, pf);
    if (rc) // read failed
        return rc;

    PageId childPid;
    rc = btnln.locateChildPtr(key, childPid);
    if (rc) // locateChildPtr failed
        return rc;

    int childOfKey;
    PageId childOfPid;
    rc = traverseAndInsert(key, rid, childPid, curDepth + 1, childOfKey, childOfPid);
    
    if (!rc) // no errors or overflow
        return 0;

    if (rc < 0) // error
        return rc;

    // some overflow happened (don't need to check with OF, also rootOF will be in the original caller)
    rc = btnln.insert(childOfKey, childOfPid);

    if (!rc)
        // successful insert
        return btnln.write(pid, pf);
    
    if (rc == RC_NODE_FULL) {
        BTNonLeafNode sib;

        rc = btnln.insertAndSplit(childOfKey, childOfPid, sib, ofKey);
        if (!rc) {
            // successful insert and split
            ofPid = pf.endPid();    // get the next empty pid
            
            // write non leaf nodes to disk
            rc = btnln.write(pid, pf);
            if (rc)
                return rc;
            rc = sib.write(ofPid, pf);
            if (rc)
                return rc;

            if (curDepth == 0)
                return RC_STATUS_INSERT_NEW_ROOT;
            else
                return RC_STATUS_INSERT_NON_LEAF_OF;
        }

        return rc;  // unsuccessful insert and split
    } else {
        // TODO: what now?
        return rc;
    }
}

RC BTreeIndex::traverseAndLocate(int searchKey, IndexCursor& cursor, PageId pid, int curDepth)
{
    RC rc;

    if (curDepth == treeHeight) {
        // this is leaf
        BTLeafNode btln;
        rc = btln.read(pid, pf);
        if (rc) // read failed
            return rc;

        int eid;
        rc = btln.locate(searchKey, eid);

        if (rc == RC_NO_SUCH_RECORD && btln.getKeyCount() == 0) {
            // this leaf node is empty (in extreme edge case)
            cursor.pid = btln.getNextNodePtr();
            cursor.eid = 0;
            if (cursor.pid == -1)
                return RC_END_OF_TREE;
            return RC_NO_SUCH_RECORD;
        }

        cursor.pid = pid;
        cursor.eid = eid;
        return rc;
    } else {
        // Non leaf node
        BTNonLeafNode btnln;
        rc = btnln.read(pid, pf);
        if (rc) // read failed
            return rc;

        PageId childPid;
        rc = btnln.locateChildPtr(searchKey, childPid);
        if (rc) // locateChildPtr failed
            return rc;

        IndexCursor childCursor;
        rc = traverseAndLocate(searchKey, childCursor, childPid, curDepth + 1);
        cursor = childCursor;
        return rc;
    }
}

void BTreeIndex::debug() {
    fprintf(stdout, "==========Debug BTreeIndex==========\n");
    if (isPfWriteMode) {
        fprintf(stdout, "Current PageFile is opened with WRITE (RDWR) mode\n");
    } else {
        fprintf(stdout, "Current PageFile is opened with READ mode\n");
    }
    fprintf(stdout, "rootPid is %i\n", rootPid);
    fprintf(stdout, "current treeHeight is %i\n", treeHeight);
}