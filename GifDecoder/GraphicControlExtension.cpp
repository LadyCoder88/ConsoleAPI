#include "GraphicControlExtension.h"
#include <assert.h>
using namespace Gif;

void GraphicControlExtension::Read(std::ifstream &file)
{
    unsigned char blockSize = 0;
    file.read((char*)&blockSize, sizeof(blockSize));

    assert(blockSize == 4);

    file.read((char*)&m_fields.value, sizeof(m_fields.value));

    assert(/*m_disposeMethod != EDisposeMethod::RestoreToBackgroundColor &&*/ static_cast<EDisposeMethod>(m_fields.fields.disposalMethodMask) != EDisposeMethod::RestoreToPrevious);

    file.read((char*)&m_delayTime, sizeof(m_delayTime));
    file.read((char*)&m_transparentColorIndex, sizeof(m_transparentColorIndex));

    unsigned char terminateByte;
    file.read((char*)&terminateByte, sizeof(terminateByte));

    assert(terminateByte == 0x00);
}