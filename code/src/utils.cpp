
#include "utils.h"


/* Bit */

Bit::Bit(QChar value, BIT_TYPE type)
{
    m_value = value;
    m_type = type;
}

void Bit::Init(QChar value, BIT_TYPE type)
{
    m_value = value;
    m_type = type;
}

QChar Bit::value()
{
    return m_value;
}

BIT_TYPE Bit::type()
{
    return m_type;
}

/* Useful functions */

int Utils::moveChunk(int &indexLeft, int &indexRight, int size, int maxChunkSize, bool right)
{
    if (right)
    {
        int last = size - 1;
        if (indexRight < last)
        {
            if (indexRight + 1 + maxChunkSize <= last)
            {
                indexLeft = indexRight + 1;
                indexRight += maxChunkSize;
            }
            else
            {
                indexLeft = indexRight + 1;
                indexRight += last;
            }
            return (indexRight - indexLeft) + 1;
        }
    }
    else
    {
        if (indexLeft > 0)
        {
            if (indexLeft - 1 - maxChunkSize >= 0)
            {
                indexRight = indexLeft - 1;
                indexLeft -= maxChunkSize;
            }
            else
            {
                indexRight = indexLeft - 1;
                indexLeft = 0;
            }
            return (indexRight - indexLeft) + 1;
        }
    }
    return 0;
}
