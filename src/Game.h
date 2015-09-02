#ifndef GAME_H_
#define GAME_H_

#include <QSet>

#include "IlmHelper.h"
#include "QueryId.h"
#include "QuranHelper.h"

namespace ilmtest {

class Game : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantMap currentQuestion READ currentQuestion NOTIFY currentQuestionChanged)
    Q_PROPERTY(QString arg1 READ arg1 FINAL)
    Q_PROPERTY(bool numeric READ numeric NOTIFY currentQuestionChanged)
    Q_PROPERTY(bool multipleChoice READ multipleChoice NOTIFY currentQuestionChanged)

    QuranHelper m_quran;
    IlmHelper m_ilm;
    QVariantMap m_currentQuestion;
    QVariantList m_tempList;
    QString m_arg1;

private slots:
    void onDataLoaded(QVariant id, QVariant data);

signals:
    void currentQuestionChanged();

public:
    Game(DatabaseHelper* db);
    virtual ~Game();

    void lazyInit();
    QVariantMap currentQuestion() const;
    QString arg1() const;
    bool multipleChoice() const;
    bool numeric() const;
    Q_SLOT void nextQuestion(int q);
    Q_INVOKABLE QString formatQuestion(QString input);
};

} /* namespace ilmtest */

#endif /* GAME_H_ */
