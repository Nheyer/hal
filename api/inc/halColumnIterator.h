/*
 * Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
 *
 * Released under the MIT license, see LICENSE.txt
 */

#ifndef _HALCOLUMNITERATOR_H
#define _HALCOLUMNITERATOR_H

#include <list>
#include <map>
#include <set>
#include "halDefs.h"
#include "halDNAIterator.h"
#include "halSequence.h"

namespace hal {

/** 
 * Interface Column iterator for allowing traditional maf-like (left-to-right)  
 * parsing of a hal alignment.  Columns are iterated with respect to
 * a specified reference genome.  This isn't the most efficient way
 * to explore the hal structure, which is designed for bottom-up and/or
 * top-down traversal.  
 */
class ColumnIterator 
{
public:

   /// @cond TEST
   // we can compare genomes by pointers (because they are persistent
   // and unique, though it's still hacky) but we can't do the same 
   // for anything else, including sequences.  
   struct SequenceLess { bool operator()(const hal::Sequence* s1,
                                         const hal::Sequence* s2) const {
     return s1->getGenome() < s2->getGenome() || (
       s1->getGenome() == s2->getGenome() && 
       s1->getStartPosition() < s2->getStartPosition()); }
   };
   /// @endcond

   typedef std::vector<hal::DNAIteratorConstPtr> DNASet;
   typedef std::map<const hal::Sequence*, DNASet*, SequenceLess> ColumnMap;

   /** Move column iterator one column to the right along reference
    * genoem sequence */
   virtual void toRight() const = 0;

   /** Move column iterator to arbitrary site in genome -- effectively
    * resetting the iterator (convenience function to avoid creation of
    * new iterators in some cases).  
    * @param columnIndex position of column in forward genome coordinates 
    * @param lastIndex last column position (for iteration).  must be greater
    *  than columnIndex */
   virtual void toSite(hal_index_t columnIndex, 
                       hal_index_t lastIndex) const = 0;

   /** Use this method to bound iteration loops.  When the column iterator
    * is retrieved from the sequence or genome, the last column is specfied.
    * toRight() cna then be called until lastColumn is true.  */
   virtual bool lastColumn() const = 0;
   
   /** Get a pointer to the reference genome for the column iterator */
   virtual const hal::Genome* getReferenceGenome() const = 0;

   /** Get a pointer to the reference sequence for the column iterator */
   virtual const hal::Sequence* getReferenceSequence() const = 0;

   /** Get the position in the reference sequence 
    * NOTE
    * Seems to be returning the next position, rather than the current.
    * Must go back and review but it is concerning. */
   virtual hal_index_t getReferenceSequencePosition() const = 0;

   /** Get a pointer to the column map */
   virtual const ColumnMap* getColumnMap() const = 0;

   /** Get the index of the column in the reference genome's array */
   virtual hal_index_t getArrayIndex() const = 0;

   /** As we iterate along, we keep a column map entry for each sequence
    * visited.  This works out pretty well except for extreme cases (such
    * as iterating over entire fly genomes where we can accumulate 10s of 
    * thousands of empty entries for all the different scaffolds when 
    * in truth we only need a handful at any given time). Under these
    * circumstances, calling this method every 1M bases or so will help
    * reduce memory as well as speed up queries on the column map. Perhaps
    * this should eventually be built in and made transparent? */
   virtual void defragment() const = 0;
   
protected:
   friend class counted_ptr<ColumnIterator>;
   friend class counted_ptr<const ColumnIterator>;
   virtual ~ColumnIterator() = 0;
};

inline ColumnIterator::~ColumnIterator() {}

}

#ifndef NDEBUG
#include "halGenome.h"
namespace hal {
inline std::ostream& operator<<(std::ostream& os, ColumnIteratorConstPtr ci)
{
  const ColumnIterator::ColumnMap* cmap = ci->getColumnMap();
  for (ColumnIterator::ColumnMap::const_iterator i = cmap->begin();
       i != cmap->end(); ++i)
  {
    os << i->first->getName() << ": ";
    for (size_t j = 0; j < i->second->size(); ++j)
    {
      os << i->second->at(j)->getArrayIndex() << ", ";
    }
    os << "\n";
  }

  return os;
}
}
#endif


#endif
