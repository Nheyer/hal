/*
 * Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
 * Copyright (C) 2012-2019 by UCSC Computational Genomics Lab
 *
 * Released under the MIT license, see LICENSE.txt
 */

#ifndef _HALBLOCKLIFTOVER_H
#define _HALBLOCKLIFTOVER_H

#include "halLiftover.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace hal {

    class BlockLiftover : public Liftover {
      public:
        BlockLiftover();
        virtual ~BlockLiftover();

      protected:
        void liftInterval(BedList &mappedBedLines);
        void visitBegin();

        void cleanTargetParalogies();
        void readPSLInfo(std::vector<MappedSegmentPtr> &fragments, BedLine &outBedLine);

      protected:
        MappedSegmentSet _mappedSegments;
        SegmentIteratorPtr _refSeg;
        hal_index_t _lastIndex;
        std::set<const Genome *> _downwardPath;
        const Genome *_mrca;
    };
}
#endif
// Local Variables:
// mode: c++
// End:
