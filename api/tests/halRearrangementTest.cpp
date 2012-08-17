/*
 * Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
 *
 * Released under the MIT license, see LICENSE.txt
 */
#include <string>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include "halRearrangementTest.h"
#include "halTopSegmentTest.h"
#include "halBottomSegmentTest.h"

using namespace std;
using namespace hal;

void addIdenticalParentChild(hal::AlignmentPtr alignment,
                             size_t numSequences,
                             size_t numSegmentsPerSequence,
                             size_t segmentLength)
{
  vector<Sequence::Info> seqVec(numSequences);
  
  BottomSegmentIteratorPtr bi;
  BottomSegmentStruct bs;
  TopSegmentIteratorPtr ti;
  TopSegmentStruct ts;
  
  Genome* parent = alignment->addRootGenome("parent");
  Genome* child = alignment->addLeafGenome("child", "parent", 1);

  for (size_t i = 0; i < numSequences; ++i)
  {
    stringstream ss;
    ss << "Sequence" << i;
    string name = ss.str();
    seqVec[i] = Sequence::Info(name, segmentLength * numSegmentsPerSequence, 
                               numSegmentsPerSequence, 
                               numSegmentsPerSequence);
  }
  parent->setDimensions(seqVec);
  child->setDimensions(seqVec);

  bi = parent->getBottomSegmentIterator();
  for (; bi != parent->getBottomSegmentEndIterator(); bi->toRight())
  {
    bs.set(bi->getBottomSegment()->getArrayIndex() * segmentLength, 
           segmentLength);
    bs._children.clear();
    bs._children.push_back(pair<hal_size_t, bool>(
                            bi->getBottomSegment()->getArrayIndex(), 
                            false));
    bs.applyTo(bi);
  }
     
  ti = child->getTopSegmentIterator();
  for (; ti != child->getTopSegmentEndIterator(); ti->toRight())
  {
    ts.set(ti->getTopSegment()->getArrayIndex() * segmentLength, 
           segmentLength, 
           ti->getTopSegment()->getArrayIndex());
    ts.applyTo(ti);
  } 
}

// doesn't currently work on sequence endpoints
void makeInsertion(BottomSegmentIteratorPtr bi)
{
  assert(bi->getBottomSegment()->isLast() == false);
  TopSegmentIteratorPtr ti =
     bi->getBottomSegment()->getGenome()->getTopSegmentIterator();
  ti->toChild(bi, 0);
  hal_index_t pi = ti->getTopSegment()->getParentIndex();
  ti->getTopSegment()->setParentIndex(NULL_INDEX);
  ti->toRight();
  ti->getTopSegment()->setParentIndex(pi);

  hal_index_t ci = bi->getBottomSegment()->getChildIndex(0);
  bi->getBottomSegment()->setChildIndex(0, ci + 1);
  bi->toRight();
  bi->getBottomSegment()->setChildIndex(0, NULL_INDEX);
}

// designed to be used only on alignments made with 
// addIdenticalParentChild() 
void makeInsGap(TopSegmentIteratorPtr ti)
{
  Genome* genome = ti->getTopSegment()->getGenome();
  Genome* parent = genome->getParent();
  BottomSegmentIteratorPtr bi = parent->getBottomSegmentIterator();
  BottomSegmentIteratorConstPtr biEnd = parent->getBottomSegmentEndIterator();
  TopSegmentIteratorConstPtr tiEnd = genome->getTopSegmentEndIterator();
  bi->toParent(ti);
  while (bi != biEnd)
  {
    hal_index_t childIndex = bi->getBottomSegment()->getChildIndex(0);
    if (childIndex == genome->getNumTopSegments() - 1)
    {
      bi->getBottomSegment()->setChildIndex(0, NULL_INDEX);
    }
    else
    {
      bi->getBottomSegment()->setChildIndex(0, childIndex + 1);
    }
    bi->toRight();
  }
  TopSegmentIteratorPtr topIt = ti->copy();
  topIt->getTopSegment()->setParentIndex(NULL_INDEX);
  topIt->toRight();
  while (topIt != tiEnd)
  {
    hal_index_t parentIndex = topIt->getTopSegment()->getParentIndex();
    topIt->getTopSegment()->setParentIndex(parentIndex - 1);
    topIt->toRight();
  }
}

void makeDelGap(BottomSegmentIteratorPtr botIt)
{
  Genome* parent = botIt->getBottomSegment()->getGenome();
  Genome* genome = parent->getChild(0);
  BottomSegmentIteratorPtr bi = parent->getBottomSegmentIterator();
  BottomSegmentIteratorConstPtr biEnd = parent->getBottomSegmentEndIterator();
  TopSegmentIteratorPtr ti =  genome->getTopSegmentIterator();
  TopSegmentIteratorConstPtr tiEnd = genome->getTopSegmentEndIterator();
  ti->toChild(bi, 0);
  bi->getBottomSegment()->setChildIndex(0, NULL_INDEX);
  bi->toRight();
  while (bi != biEnd)
  {
    hal_index_t childIndex = bi->getBottomSegment()->getChildIndex(0);
    bi->getBottomSegment()->setChildIndex(0, childIndex - 1);
    bi->toRight();
  }

  while (ti != tiEnd)
  {
    
    hal_index_t parentIndex = ti->getTopSegment()->getParentIndex();
    if (parentIndex == parent->getNumBottomSegments() - 1)
    {
      ti->getTopSegment()->setParentIndex(NULL_INDEX);
    }
    else
    {
      ti->getTopSegment()->setParentIndex(parentIndex + 1);
    }
    ti->toRight();
  }
}

void RearrangementInsertionTest::createCallBack(AlignmentPtr alignment)
{
  size_t numSequences = 3;
  size_t numSegmentsPerSequence = 10;
  size_t segmentLength = 5;
  
  addIdenticalParentChild(alignment, numSequences, numSegmentsPerSequence,
                          segmentLength);

  Genome* parent = alignment->openGenome("parent");
  Genome* child = alignment->openGenome("child");

  BottomSegmentIteratorPtr bi = parent->getBottomSegmentIterator();
  
  // insertion smaller than gap threshold
  makeInsertion(bi);

  // insertion larger than gap threshold
  bi->toRight();
  bi->toRight();
  makeInsertion(bi);
  bi->toRight();
  makeInsertion(bi);

  // insertion larger than gap threshold but that contains gaps
}

void RearrangementInsertionTest::checkCallBack(AlignmentConstPtr alignment)
{
  BottomSegmentIteratorConstPtr bi;
  TopSegmentIteratorConstPtr ti;

  const Genome* child = alignment->openGenome("child");
  const Genome* parent = alignment->openGenome("parent");
  
  RearrangementPtr r = child->getRearrangement();
//  bool res = r->identifyFromLeftBreakpoint(parent->getTopSegmentIterator());
  
}

void RearrangementSimpleInversionTest::createCallBack(AlignmentPtr alignment)
{
  size_t numSequences = 3;
  size_t numSegmentsPerSequence = 10;
  size_t segmentLength = 50;
  
  addIdenticalParentChild(alignment, numSequences, numSegmentsPerSequence,
                          segmentLength);

  Genome* parent = alignment->openGenome("parent");
  Genome* child = alignment->openGenome("child");

  // inversion on 1st segment (interior)
  BottomSegmentIteratorPtr bi = parent->getBottomSegmentIterator(1);
  TopSegmentIteratorPtr ti = child->getTopSegmentIterator(1);
  bi->getBottomSegment()->setChildReversed(0, true);
  ti->getTopSegment()->setParentReversed(true);
  
  // inversion on last segment of first sequence
  bi = parent->getBottomSegmentIterator(numSegmentsPerSequence - 1);
  ti = child->getTopSegmentIterator(numSegmentsPerSequence - 1);
  bi->getBottomSegment()->setChildReversed(0, true);
  ti->getTopSegment()->setParentReversed(true);

  // inversion on first segment of 3rd sequence
  bi = parent->getBottomSegmentIterator(numSegmentsPerSequence * 2);
  ti = child->getTopSegmentIterator(numSegmentsPerSequence * 2);
  bi->getBottomSegment()->setChildReversed(0, true);
  ti->getTopSegment()->setParentReversed(true);  
}

void 
RearrangementSimpleInversionTest::checkCallBack(AlignmentConstPtr alignment)
{
  BottomSegmentIteratorConstPtr bi;
  TopSegmentIteratorConstPtr ti;

  const Genome* child = alignment->openGenome("child");
  const Genome* parent = alignment->openGenome("parent");
  
  size_t numSequences = child->getNumSequences();
  size_t numSegmentsPerSequence = 
     child->getSequenceIterator()->getSequence()->getNumTopSegments();
  size_t segmentLength = 
     child->getTopSegmentIterator()->getTopSegment()->getLength();
  
  RearrangementPtr r = child->getRearrangement();
  do
  {
    hal_index_t leftIdx = 
       r->getLeftBreakpoint()->getTopSegment()->getArrayIndex();
    if (leftIdx == 1 || leftIdx == numSegmentsPerSequence - 1 ||
        leftIdx == (numSegmentsPerSequence * 2) )
    {
      CuAssertTrue(_testCase, r->getID() == Rearrangement::Inversion);
    }
    else
    {
      CuAssertTrue(_testCase, r->getID() != Rearrangement::Inversion);
    }
  }
  while (r->identifyNext() == true);
}

void RearrangementGappedInversionTest::createCallBack(AlignmentPtr alignment)
{
  size_t numSequences = 3;
  size_t numSegmentsPerSequence = 10;
  size_t segmentLength = 5;
  
  addIdenticalParentChild(alignment, numSequences, numSegmentsPerSequence,
                          segmentLength);

  Genome* parent = alignment->openGenome("parent");
  Genome* child = alignment->openGenome("child");

  // 4-segment inversion. 
  // parent has gap-deletions at 2nd and 5th posstions
  // child has gap-insertions positions 3 and 5
  BottomSegmentIteratorPtr bi = parent->getBottomSegmentIterator(1);
  TopSegmentIteratorPtr ti = child->getTopSegmentIterator(1);
  bi->getBottomSegment()->setChildReversed(0, true);
  bi->getBottomSegment()->setChildIndex(0, 6);
  ti->getTopSegment()->setParentReversed(true);
  ti->getTopSegment()->setParentIndex(6);

  bi = parent->getBottomSegmentIterator(2);
  ti = child->getTopSegmentIterator(2);
  bi->getBottomSegment()->setChildIndex(0, NULL_INDEX);
  ti->getTopSegment()->setParentReversed(true);
  ti->getTopSegment()->setParentIndex(4);

  bi = parent->getBottomSegmentIterator(3);
  ti = child->getTopSegmentIterator(3);
  bi->getBottomSegment()->setChildReversed(0, true);
  bi->getBottomSegment()->setChildIndex(0, 4);
  ti->getTopSegment()->setParentIndex(NULL_INDEX);

  bi = parent->getBottomSegmentIterator(4);
  ti = child->getTopSegmentIterator(4);
  bi->getBottomSegment()->setChildReversed(0, true);
  bi->getBottomSegment()->setChildIndex(0, 2);
  ti->getTopSegment()->setParentIndex(3);
  ti->getTopSegment()->setParentReversed(true);
  
  bi = parent->getBottomSegmentIterator(5);
  ti = child->getTopSegmentIterator(5);
  bi->getBottomSegment()->setChildIndex(0, NULL_INDEX);
  ti->getTopSegment()->setParentIndex(NULL_INDEX);

  bi = parent->getBottomSegmentIterator(6);
  ti = child->getTopSegmentIterator(6);
  bi->getBottomSegment()->setChildReversed(0, true);
  bi->getBottomSegment()->setChildIndex(0, 1);
  ti->getTopSegment()->setParentIndex(1);
  ti->getTopSegment()->setParentReversed(true);

}

void 
RearrangementGappedInversionTest::checkCallBack(AlignmentConstPtr alignment)
{
  BottomSegmentIteratorConstPtr bi;
  TopSegmentIteratorConstPtr ti;

  const Genome* child = alignment->openGenome("child");
  const Genome* parent = alignment->openGenome("parent");
  
  size_t numSequences = child->getNumSequences();
  size_t numSegmentsPerSequence = 
     child->getSequenceIterator()->getSequence()->getNumTopSegments();
  size_t segmentLength = 
     child->getTopSegmentIterator()->getTopSegment()->getLength();
  
  RearrangementPtr r = child->getRearrangement();
  do
  {
    hal_index_t leftIdx = 
       r->getLeftBreakpoint()->getTopSegment()->getArrayIndex();
    if (leftIdx == 1)
    {
      CuAssertTrue(_testCase, r->getID() == Rearrangement::Inversion);
    }
    else
    {
      CuAssertTrue(_testCase, r->getID() != Rearrangement::Inversion);
    }
  }
  while (r->identifyNext() == true);
}

void halRearrangementInsertionTest(CuTest *testCase)
{
  try 
  {
    RearrangementInsertionTest tester;
    tester.check(testCase);
  }
   catch (...) 
  {
    CuAssertTrue(testCase, false);
  } 
}

void halRearrangementSimpleInversionTest(CuTest *testCase)
{
  try 
  {
    RearrangementSimpleInversionTest tester;
    tester.check(testCase);
  }
   catch (...) 
  {
    CuAssertTrue(testCase, false);
  } 
}

void halRearrangementGappedInversionTest(CuTest *testCase)
{
  try 
  {
    RearrangementGappedInversionTest tester;
    tester.check(testCase);
  }
   catch (...) 
  {
    CuAssertTrue(testCase, false);
  } 
}

CuSuite* halRearrangementTestSuite(void) 
{
  CuSuite* suite = CuSuiteNew();
//  SUITE_ADD_TEST(suite, halRearrangementInsertionTest);
//za  SUITE_ADD_TEST(suite, halRearrangementSimpleInversionTest);
  SUITE_ADD_TEST(suite, halRearrangementGappedInversionTest);
  return suite;
}

