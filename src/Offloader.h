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

    Q_INVOKABLE QVariantList generateChoices(int correctAnswer);
};

} /* namespace quran */

#endif /* OFFLOADER_H_ */
