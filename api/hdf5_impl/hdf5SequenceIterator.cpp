/*
 * Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
 *
 * Released under the MIT license, see LICENSE.txt
 */
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "hdf5SequenceIterator.h"
#include "hdf5BottomSegmentIterator.h"

using namespace std;
using namespace H5;
using namespace hal;

HDF5SequenceIterator::HDF5SequenceIterator(HDF5Genome* genome, 
                                           hal_index_t index) :
  _sequence(genome, &genome->_sequenceArray, index)
{
  
}

HDF5SequenceIterator::~HDF5SequenceIterator()
{

}
   
SequenceIteratorPtr HDF5SequenceIterator::copy()
{
  HDF5SequenceIterator* newIt = new HDF5SequenceIterator(
    _sequence._genome, _sequence._index);
  return SequenceIteratorPtr(newIt);
}

SequenceIteratorConstPtr HDF5SequenceIterator::copy() const
{
  HDF5SequenceIterator* newIt = new HDF5SequenceIterator(
    _sequence._genome, _sequence._index);
  return SequenceIteratorConstPtr(newIt);
}

void HDF5SequenceIterator:: toNext() const
{
  ++_sequence._index;
}

void HDF5SequenceIterator::toPrev() const
{
  --_sequence._index;
}

Sequence* HDF5SequenceIterator::getSequence()
{
  assert(_sequence._index >= 0 && _sequence._index < 
         (hal_index_t)_sequence._genome->_sequenceArray.getSize()); 
  return &_sequence;
}

const Sequence* HDF5SequenceIterator::getSequence() const
{
  assert(_sequence._index >= 0 && _sequence._index < 
         (hal_index_t)_sequence._genome->_sequenceArray.getSize());
  return &_sequence;
}
