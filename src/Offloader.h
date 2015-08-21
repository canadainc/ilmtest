#ifndef OFFLOADER_H_
#define OFFLOADER_H_

#include <QObject>
#include <QVariant>

namespace ilmtest {

class Offloader : public QObject
{
    Q_OBJECT

public:
    Offloader();
    virtual ~Offloader();
};

} /* namespace quran */

#endif /* OFFLOADER_H_ */
