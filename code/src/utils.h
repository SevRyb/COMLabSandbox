
#ifndef UTILS_H
#define UTILS_H

#include <QChar>

enum BIT_TYPE
{
    IDLE_BIT,
    START_BIT,
    DATA_BIT,
    PARITY_BIT,
    STOP_BIT,
    NONE_BIT = -1
};

/* Bit */

class Bit
{
public:
    Bit(QChar value = '0', BIT_TYPE type = BIT_TYPE::NONE_BIT);
    void Init(QChar value, BIT_TYPE type);
    QChar value();
    BIT_TYPE type();
private:
    QChar m_value;
    BIT_TYPE m_type;
};

int _moveChunk(int &indexLeft, int &indexRight, int size, int maxChunkSize, bool right=true);

#endif // UTILS_H
