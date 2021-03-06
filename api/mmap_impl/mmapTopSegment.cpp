#include "mmapTopSegment.h"
#include "halDnaIterator.h"
#include "mmapBottomSegment.h"

using namespace hal;

void MMapTopSegment::setCoordinates(hal_index_t startPos, hal_size_t length) {
    if (_genome &&
        (startPos >= (hal_index_t)_genome->getSequenceLength() || startPos + length > _genome->getSequenceLength())) {
        throw hal_exception("Trying to set top segment coordinate out of range");
    }

    _data->setStartPosition(startPos);
    (_data + 1)->setStartPosition(startPos + length);
}

hal_offset_t MMapTopSegment::getBottomParseOffset() const {
    assert(_index >= 0);
    hal_offset_t offset = 0;
    hal_index_t bottomIndex = getBottomParseIndex();
    if (bottomIndex != NULL_INDEX) {
        MMapBottomSegment bs(getMMapGenome(), bottomIndex);
        assert(bs.getStartPosition() <= getStartPosition());
        assert((hal_index_t)(bs.getStartPosition() + bs.getLength()) >= getStartPosition());
        offset = getStartPosition() - bs.getStartPosition();
    }
    return offset;
}

bool MMapTopSegment::isCanonicalParalog() const {
    bool isCanon = false;
    if (hasParent()) {
        MMapGenome *parGenome = const_cast<MMapGenome *>(dynamic_cast<const MMapGenome *>(_genome->getParent()));

        MMapBottomSegment parent(parGenome, getParentIndex());
        hal_index_t childGenomeIndex = parGenome->getChildIndex(_genome);
        isCanon = parent.getChildIndex(childGenomeIndex) == _index;
    }
    return isCanon;
}

void MMapTopSegment::print(std::ostream &os) const {
    os << "MMapTopSegment" << getStartPosition() << " " << getEndPosition() << std::endl;
}
