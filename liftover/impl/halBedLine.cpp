/*
 * Copyright (C) 2013 by Glenn Hickey (hickey@soe.ucsc.edu)
 *
 * Released under the MIT license, see LICENSE.txt
 */
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <algorithm>

#include "halBedLine.h"

using namespace std;
using namespace hal;

BedLine::BedLine() : _start(NULL_INDEX), _end(NULL_INDEX), _strand('+'),
                     _version(NULL_INDEX)
{

}

BedLine::~BedLine()
{

}

istream& BedLine::read(istream& is, int version, string& lineBuffer)
{
  _version = version;
  std::getline(is, lineBuffer);
  stringstream ss(lineBuffer);
  ss >> _chrName;
  if (ss.bad()) throw hal_exception("Error scanning BED chrom");
  ss >> _start;
  if (ss.bad()) throw hal_exception("Error scanning BED chromStart");
  ss >> _end;
  if (ss.bad()) throw hal_exception("Error scanning BED chromEnd");
  if (_version > 3)
  {
    ss >> _name;
    if (ss.bad()) throw hal_exception("Error scanning BED name");
  }
  if (_version > 4)
  {
    ss >> _score;
    if (ss.bad()) throw hal_exception("Error scanning BED score");
  }
  if (_version > 5)
  {
    ss >> _strand;
    if (ss.bad()) throw hal_exception("Error scanning BED strand");
  }
  if (_version > 6)
  {
    ss >> _thickStart;
    if (ss.bad()) throw hal_exception("Error scanning BED thickStart");
  }
  if (_version > 7)
  {
    ss >> _thickEnd;
    if (ss.bad()) throw hal_exception("Error scanning BED thickEnd");
  }
  if (_version > 8)
  {
    ss >> _itemRGB;
    if (ss.bad()) throw hal_exception("Error scanning BED itemRGB");
  }
  if (_version > 9)
  {
    size_t numBlocks;
    ss >> numBlocks;
    if (ss.bad()) throw hal_exception("Error scanning BED blockCount");
    string blockSizes;
    ss >> blockSizes;
    if (ss.bad()) throw hal_exception("Error scanning BED blockSizes");
    string blockStarts;
    ss >> blockStarts;
    if (ss.bad()) throw hal_exception("Error scanning BED blockStarts");
    _blocks.resize(numBlocks);
    vector<string> sizeBuf = chopString(blockSizes, ",");
    if (sizeBuf.size() != numBlocks)
       throw hal_exception("Error scanning BED blockSizes");
    vector<string> startBuf = chopString(blockStarts, ",");
    if (startBuf.size() != numBlocks)
       throw hal_exception("Error scanning BED blockStarts");
    for (size_t i = 0; i < numBlocks; ++i)
    {
      BedBlock& block = _blocks[i];
      stringstream ss1(sizeBuf[i]);
      ss1 >> block._length;
      if (ss1.bad()) throw hal_exception("Error scanning BED blockSizes");
      stringstream ss2(startBuf[i]);
      ss2 >> block._start;
      if (ss2.bad()) throw hal_exception("Error scanning BED blockStarts");
    }
  }
  _extra.clear();
  while (ss.good())
  {
    string extraBuf;
    ss >> extraBuf;
    if (extraBuf.length() > 0)
    {
      _extra.push_back(extraBuf);
    }
  }
  return is;
}

ostream& BedLine::write(ostream& os, int version)
{
  if (version == -1)
  {
    version = _version;
  }
  os << _chrName << '\t' << _start << '\t' << _end;
  
  if (version > 3)
  {
    os << '\t' << _name;
  }
  if (version > 4)
  {
    os << '\t' << _score;
  }
  if (version > 5)
  {
    os << '\t' << _strand;
  }
  if (version > 6)
  {
    os << '\t' << _thickStart;
  }
  if (version > 7)
  {
    os << '\t' << _thickEnd;
  }
  if (version > 8)
  {
    os << '\t' << _itemRGB;
  }
  if (version > 9)
  {
    os << '\t' << _blocks.size();
    for (size_t i = 0; i < _blocks.size(); ++i)
    {
      if (i == 0)
      {
        os << '\t';
      }
      else
      {
        os << ',';
      }
      os << _blocks[i]._length;
    }
    for (size_t i = 0; i < _blocks.size(); ++i)
    {
      if (i == 0)
      {
        os << '\t';
      }
      else
      {
        os << ',';
      }
      os << _blocks[i]._start;
    }
  }

  for (size_t i = 0; i < _extra.size(); ++i)
  {
    os << '\t' << _extra[i];
  }
  
  os << '\n';
  return os;
}



