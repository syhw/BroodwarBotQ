#include <vector>
#include <map>
#include <RectangleArray.h>
using namespace std;
using namespace Util;
map<int, int> makeAssignments(RectangleArray< double> &cost)
{
  map<int, int> assignments;
  vector<bool> deletedRows;
  vector<bool> deletedColumns;
  for(int r=0;r<(int)cost.getHeight();r++)
    deletedRows.push_back(false);
  for(int c=0;c<(int)cost.getWidth();c++)
    deletedColumns.push_back(false);

  bool outer_done=false;
  while(outer_done==false)
  {
    outer_done=true;
    bool done=false;
    while(done==false)
    {
      done=true;
      for(int r=0;r<(int)cost.getHeight();r++)
      {
        if (deletedRows[r]) continue;
        int firstZero=-1;
        int lastZero=-2;
        for(int c=0;c<(int)cost.getWidth();c++)
        {
          if (deletedColumns[c]) continue;
          if (cost[r][c]==0)
          {
            if (firstZero==-1)
              firstZero=c;
            lastZero=c;
          }
        }
        if (firstZero==lastZero)
        {
          assignments[r]=firstZero;
          deletedRows[r]=true;
          deletedColumns[firstZero]=true;
          done=false;
        }
      }
      for(int c=0;c<(int)cost.getWidth();c++)
      {
        if (deletedColumns[c]) continue;
        int firstZero=-1;
        int lastZero=-2;
        for(int r=0;r<(int)cost.getHeight();r++)
        {
          if (deletedRows[r]) continue;
          if (cost[r][c]==0)
          {
            if (firstZero==-1)
              firstZero=r;
            lastZero=r;
          }
        }
        if (firstZero==lastZero)
        {
          assignments[firstZero]=c;
          deletedRows[firstZero]=true;
          deletedColumns[c]=true;
          done=false;
        }
      }
    }
    for(int r=0;r<(int)cost.getHeight();r++)
    {
      if (deletedRows[r]) continue;
      for(int c=0;c<(int)cost.getWidth();c++)
      {
        if (deletedColumns[c]) continue;
        if (cost[r][c]==0)
        {
          assignments[r]=c;
          deletedRows[r]=true;
          deletedColumns[c]=true;
          outer_done=false;
          break;
        }
      }
      if (outer_done==false) break;
    }
  }
  return assignments;
}

// Hungarian Algorithm
// http://en.wikipedia.org/wiki/Hungarian_algorithm

map<int, int> computeAssignments(RectangleArray< double> &cost)
{
  if (cost.getWidth()==0 || cost.getHeight()==0)
  {
    map<int, int> nothing;
    return nothing;
  }
  //cost is an n by n matrix
  //Step 1: From each row subtract off the row min
  for(int r=0;r<(int)cost.getHeight();r++)
  {
    double minCost=cost[r][0];
    for(int c=0;c<(int)cost.getWidth();c++)
      if (cost[r][c]<minCost)
        minCost=cost[r][c];
    for(int c=0;c<(int)cost.getWidth();c++)
      cost[r][c]-=minCost;
  }

  //Step 2: From each column subtract off the column min
  for(int c=0;c<(int)cost.getWidth();c++)
  {
    double minCost=cost[0][c];
    for(int r=0;r<(int)cost.getHeight();r++)
      if (cost[r][c]<minCost)
        minCost=cost[r][c];
    for(int r=0;r<(int)cost.getHeight();r++)
      cost[r][c]-=minCost;
  }
  //Step 3: Use as few lines as possible to cover all zeros in the matrix
  for(;;)
  {
    //Assign as many task as possible
    map<int, int> assignments=makeAssignments(cost);
    if (assignments.size() == cost.getHeight())
    {
      return assignments;
    }
    vector<bool> markedRows;
    vector<bool> markedColumns;
    vector<bool> linedRows;
    vector<bool> linedColumns;
    for(int r=0;r<(int)cost.getHeight();r++)
    {
      markedRows.push_back(false);
      linedRows.push_back(false);
    }
    for(int c=0;c<(int)cost.getWidth();c++)
    {
      markedColumns.push_back(false);
      linedColumns.push_back(false);
    }
    //Mark all rows having no assignments
    for(int r=0;r<(int)cost.getHeight();r++)
    {
      if (assignments.find(r)==assignments.end())
      {
        markedRows[r]=true;
      }
    }
    bool done=false;
    while(done==false)
    {
      done=true;
      for(int r=0;r<(int)cost.getHeight();r++)
      {
        if (!markedRows[r]) continue;
        for(int c=0;c<(int)cost.getWidth();c++)
        {
          if (cost[r][c]==0)
          {
            if (!markedColumns[c]) done=false;
            markedColumns[c]=true;
          }
        }
      }
      for(int c=0;c<(int)cost.getWidth();c++)
      {
        if (!markedColumns[c]) continue;
        for(int r=0;r<(int)cost.getHeight();r++)
        {
          if (assignments.find(r)!=assignments.end() && assignments.find(r)->second==c)
          {
            if (!markedRows[r]) done=false;
            markedRows[r]=true;
          }
        }
      }
    }
    int k=0;
    for(int r=0;r<(int)cost.getHeight();r++)
    {
      linedRows[r]=!markedRows[r];
      if (linedRows[r])
        k++;
    }
    for(int c=0;c<(int)cost.getWidth();c++)
    {
      linedColumns[c]=markedColumns[c];
      if (linedColumns[c])
        k++;
    }
    if (k==cost.getWidth())
      break;

    //set m to the minimum uncovered number
    double m;
    bool setM=false;
    for(int r=0;r<(int)cost.getHeight();r++)
      for(int c=0;c<(int)cost.getWidth();c++)
        if (!linedRows[r] && !linedColumns[c])
          if (!setM || cost[r][c]<m)
          {
            m=cost[r][c];
            setM=true;
          }

    //Subtract m from every uncovered number
    for(int r=0;r<(int)cost.getHeight();r++)
      for(int c=0;c<(int)cost.getWidth();c++)
        if (!linedColumns[c] && !linedRows[r])
          cost[r][c]-=m;

    //Add m to every number covered with 2 lines
    for(int r=0;r<(int)cost.getHeight();r++)
      for(int c=0;c<(int)cost.getWidth();c++)
        if (linedColumns[c] && linedRows[r])
          cost[r][c]+=m;

  }
  map<int, int> assignments=makeAssignments(cost);
  return assignments;
}
