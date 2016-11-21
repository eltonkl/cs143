/**
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <climits>
#include "Bruinbase.h"
#include "SqlEngine.h"
#include "BTreeIndex.h"

using namespace std;

// external functions and variables for load file and sql command parsing 
extern FILE* sqlin;
int sqlparse(void);


RC SqlEngine::run(FILE* commandline)
{
  fprintf(stdout, "Bruinbase> ");

  // set the command line input and start parsing user input
  sqlin = commandline;
  sqlparse();  // sqlparse() is defined in SqlParser.tab.c generated from
               // SqlParser.y by bison (bison is GNU equivalent of yacc)

  return 0;
}

RC SqlEngine::select(int attr, const string& table, const vector<SelCond>& cond)
{
  RecordFile rf;   // RecordFile containing the table
  RecordId   rid;  // record cursor for table scanning

  RC     rc;
  int    key;     
  string value;
  int    count;
  int    diff;

  // open the table file
  if ((rc = rf.open(table + ".tbl", 'r')) < 0) {
    fprintf(stderr, "Error: table %s does not exist\n", table.c_str());
    return rc;
  }

  BTreeIndex bti;
  rc = bti.open(table + ".idx", 'r');

  bool hasIndex = rc == 0;
  int keyConditions = 0;
  bool keyInequality = false;

  for (int i = 0; i < cond.size(); i++) {
    if (cond[i].attr == 1) {
      keyConditions++;
      if (cond[i].comp == SelCond::NE)
        keyInequality = true;
    }
  }

  if (hasIndex && !keyInequality && (keyConditions > 0 || attr == 4 && keyConditions == 0)) {
    vector<pair<int, RecordId> > rids;
    IndexCursor cursor;

    bool ge = true;
    int min = INT_MIN;

    bool le = true;
    int max = INT_MAX;

    for (unsigned i = 0; i < cond.size(); i++) {
      if (cond[i].attr == 1) {
        int val = atoi(cond[i].value);
        //fprintf(stderr, "old %d %d", min, max);
        switch (cond[i].comp) {
        case SelCond::EQ:
          if (ge && val < min || !ge && val <= min || le && val > max || !le & val >= max)
            goto no_values;
          else {
            ge = true;
            le = true;
            min = val;
            max = val;
          }
          break;
        case SelCond::NE:
        // this shouldn't happen
          break;
        case SelCond::LT:
          if (ge && val < min || !ge && val <= min)
            goto no_values;
          else {
            if (le && val <= max || !le && val < max) {
              le = false;
              max = val;
            }
          }
          break;
        case SelCond::GT:
          if (le && val > max || !le && val >= max)
            goto no_values;
          else {
            if (ge && val >= min || !ge && val > min) {
              ge = false;
              min = val;
            }
          }
          break;
        case SelCond::LE:
          if (val < min)
            goto no_values;
          else {
            if (val < max) {
              le = true;
              max = val;
            }
          }
          break;
        case SelCond::GE:
          if (val > max)
            goto no_values;
          else {
            if (val > min) {
              ge = true;
              min = val;
            }
          }
          break;
        }
        //fprintf(stderr, " new %d %d\n", min, max);
      }
    }

    //fprintf(stderr, "final %d %d\n", min, max);
    rc = bti.locate(min, cursor);
    //fprintf(stderr, "first rc: %d\n", rc);
    while (true) {
      RecordId rid;
      rc = bti.readForward(cursor, key, rid);
      if (rc == 0) {
        if (ge && key < min)
          continue;
        else if (!ge && key <= min)
          continue;
        else if (le && key > max)
          break;
        else if (!le && key >= max)
          break;
        rids.push_back(make_pair(key, rid));
      }
      else
        break;
    }

    count = 0;
    //fprintf(stderr, "size of rids: %u\n", rids.size());
    {
      vector<pair<int, RecordId> >::iterator it = rids.begin();
      while (it != rids.end()) {
        bool read = false;
        key = it->first;
        if (attr == 2 || attr == 3) {
          // can't just operate on keys only, so read the tuple
          if ((rc = rf.read(it->second, key, value)) < 0) {
            fprintf(stderr, "Error: while reading a tuple from table %s\n", table.c_str());
            goto exit_select;
          }
          read = true;
        }

        // check the non-key conditions on the tuple
        for (unsigned i = 0; i < cond.size(); i++) {
          if (cond[i].attr == 2) {
            if (!read) {
              // if we haven't yet read the value and need to compare it, read the tuple
              if ((rc = rf.read(it->second, key, value)) < 0) {
                fprintf(stderr, "Error: while reading a tuple from table %s\n", table.c_str());
                goto exit_select;
              }
              read = true;
            }
            // compute the difference between the tuple value and the condition value
            diff = strcmp(value.c_str(), cond[i].value);

            // skip the tuple if any condition is not met
            switch (cond[i].comp) {
            case SelCond::EQ:
              if (diff != 0) goto next_tuple2;
              break;
            case SelCond::NE:
              if (diff == 0) goto next_tuple2;
              break;
            case SelCond::GT:
              if (diff <= 0) goto next_tuple2;
              break;
            case SelCond::LT:
              if (diff >= 0) goto next_tuple2;
              break;
            case SelCond::GE:
              if (diff < 0) goto next_tuple2;
              break;
            case SelCond::LE:
              if (diff > 0) goto next_tuple2;
              break;
            }
          }
        }

        // the condition is met for the tuple.
        // increase matching tuple counter
        count++;

        // print the tuple
        switch (attr) {
        case 1:  // SELECT key
          fprintf(stdout, "%d\n", key);
          break;
        case 2:  // SELECT value
          fprintf(stdout, "%s\n", value.c_str());
          break;
        case 3:  // SELECT *
          fprintf(stdout, "%d '%s'\n", key, value.c_str());
          break;
        }
        // move to the next tuple
        next_tuple2:
        ++it;
      }
    }

    no_values:
    // print matching tuple count if "select count(*)"
    if (attr == 4) {
      fprintf(stdout, "%d\n", count);
    }

    bti.close();
  }
  else {
    // scan the table file from the beginning
    rid.pid = rid.sid = 0;
    count = 0;
    while (rid < rf.endRid()) {
      // read the tuple
      if ((rc = rf.read(rid, key, value)) < 0) {
        fprintf(stderr, "Error: while reading a tuple from table %s\n", table.c_str());
        goto exit_select;
      }

      // check the conditions on the tuple
      for (unsigned i = 0; i < cond.size(); i++) {
        // compute the difference between the tuple value and the condition value
        switch (cond[i].attr) {
        case 1:
    diff = key - atoi(cond[i].value);
    break;
        case 2:
    diff = strcmp(value.c_str(), cond[i].value);
    break;
        }

        // skip the tuple if any condition is not met
        switch (cond[i].comp) {
        case SelCond::EQ:
    if (diff != 0) goto next_tuple;
    break;
        case SelCond::NE:
    if (diff == 0) goto next_tuple;
    break;
        case SelCond::GT:
    if (diff <= 0) goto next_tuple;
    break;
        case SelCond::LT:
    if (diff >= 0) goto next_tuple;
    break;
        case SelCond::GE:
    if (diff < 0) goto next_tuple;
    break;
        case SelCond::LE:
    if (diff > 0) goto next_tuple;
    break;
        }
      }

      // the condition is met for the tuple. 
      // increase matching tuple counter
      count++;

      // print the tuple 
      switch (attr) {
      case 1:  // SELECT key
        fprintf(stdout, "%d\n", key);
        break;
      case 2:  // SELECT value
        fprintf(stdout, "%s\n", value.c_str());
        break;
      case 3:  // SELECT *
        fprintf(stdout, "%d '%s'\n", key, value.c_str());
        break;
      }

      // move to the next tuple
      next_tuple:
      ++rid;
    }

    // print matching tuple count if "select count(*)"
    if (attr == 4) {
      fprintf(stdout, "%d\n", count);
    }
  }

  rc = 0;
  // close the table file and return
  exit_select:
  rf.close();
  return rc;
}

RC SqlEngine::load(const string& table, const string& loadfile, bool index)
{
    RecordFile rf;
    RC rc = rf.open(table + ".tbl", 'w');
    if (rc)
        return rc;

    BTreeIndex bti;
    if (index) {
      rc = bti.open(table + ".idx", 'w');
      if (rc)
        return rc;
    }

    ifstream ifs;
    ifs.open(loadfile.c_str());
    if (!ifs.is_open())
        return RC_FILE_OPEN_FAILED;

    string line;
    while (getline(ifs, line))
    {
        int key;
        string value;
        RecordId rid;

        rc = parseLoadLine(line, key, value);
        if (rc)
            return rc;
        rc = rf.append(key, value, rid);
        if (rc)
            return rc;

        // add to index
        if (index) {
          rc = bti.insert(key, rid);
          if (rc)
            return rc;
        }
    }

    if (index) {
      rc = bti.close();
      return rc;
    }

    return 0;
}

RC SqlEngine::parseLoadLine(const string& line, int& key, string& value)
{
    const char *s;
    char        c;
    string::size_type loc;
    
    // ignore beginning white spaces
    c = *(s = line.c_str());
    while (c == ' ' || c == '\t') { c = *++s; }

    // get the integer key value
    key = atoi(s);

    // look for comma
    s = strchr(s, ',');
    if (s == NULL) { return RC_INVALID_FILE_FORMAT; }

    // ignore white spaces
    do { c = *++s; } while (c == ' ' || c == '\t');
    
    // if there is nothing left, set the value to empty string
    if (c == 0) { 
        value.erase();
        return 0;
    }

    // is the value field delimited by ' or "?
    if (c == '\'' || c == '"') {
        s++;
    } else {
        c = '\n';
    }

    // get the value string
    value.assign(s);
    loc = value.find(c, 0);
    if (loc != string::npos) { value.erase(loc); }

    return 0;
}
