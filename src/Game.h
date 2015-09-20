#ifndef GAME_H_
#define GAME_H_

#include <QSet>

#include "IlmHelper.h"

namespace ilmtest {

struct Destiny
{
    QueryId::Type formatType;
    QueryId::Type questionType;
    QueryId::Type truthType;
    qint64 questionId;

    Destiny(int format=QueryId::MultipleChoice, int q=QueryId::Unknown, int tt=QueryId::GenerateTruth) :
        formatType( (QueryId::Type)format ), questionType( (QueryId::Type)q ), truthType( (QueryId::Type)tt ), questionId(0) {}
};

class Game : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantMap currentQuestion READ currentQuestion NOTIFY currentQuestionChanged)
    Q_PROPERTY(QString arg1 READ arg1 FINAL)
    Q_PROPERTY(bool numeric READ numeric NOTIFY currentQuestionChanged)
    Q_PROPERTY(bool multipleChoice READ multipleChoice NOTIFY currentQuestionChanged)
    Q_PROPERTY(bool booleanQuestion READ booleanQuestion NOTIFY currentQuestionChanged)
    Q_PROPERTY(bool level READ level NOTIFY levelChanged)

    IlmHelper m_ilm;
    QVariantMap m_currentQuestion;
    QString m_arg1;
    QVariantMap m_reference;
    Destiny m_destiny;
    int m_level;
    QQueue<Destiny> m_questionPath;

    QVariantList generateNumericBoolean(int id, QVariantList data, QString const& key=QString());
    QVariantList generateNumeric(QVariantList data, QString const& key=QString());
    void processCustom(QueryId::Type t);
    QVariantList processAnswersForCustomQuestion(QueryId::Type t, QVariantList data);
    QVariantList processOrdered(QVariantList data, bool before);
    void setReference(QVariantMap const& qvm, QString const& idKey, QString const& authorKey, QString const& titleKey, QString const& headingKey, QString const& targetId="com.canadainc.Quran10.tafsir.previewer", QString const& uriPrefix="quran://tafsir/");

private slots:
    void onDataLoaded(QVariant id, QVariant data);

signals:
    void currentQuestionChanged();
    void error(QString const& q);
    void levelChanged();

public:
    Game(DatabaseHelper* db);
    virtual ~Game();

    void lazyInit();
    QVariantMap currentQuestion() const;
    QString arg1() const;
    bool multipleChoice() const;
    bool numeric() const;
    bool booleanQuestion() const;
    int level() const;

    /**
     * @param numericFlag Either MultipleChoice or TextInput
     */
    Q_SLOT void nextQuestion(int q, int requestedFormat=QueryId::MultipleChoice, int requestedBool=QueryId::GenerateTruth);
    Q_INVOKABLE QString formatQuestion(QString const& input);
    Q_INVOKABLE void reset();
    Q_INVOKABLE static QString queryToString(int q);
    Q_SLOT void reloadQuestions();
};

} /* namespace ilmtest */

#endif /* GAME_H_ */
