#ifndef QuranHelper_H_
#define QuranHelper_H_

#include <QVariant>

namespace canadainc {
    class DatabaseHelper;
}

namespace ilmtest {

using namespace canadainc;

class QuranHelper : public QObject
{
	Q_OBJECT

	DatabaseHelper* m_sql;
	QString m_name;

    void fetchSurahHeader(QObject* caller, int chapterNumber);

public:
	QuranHelper(DatabaseHelper* sql);
	virtual ~QuranHelper();

    Q_INVOKABLE void boolSurahLocation(QObject* caller);
    Q_INVOKABLE void numericMaxVerseCount(QObject* caller);
    Q_INVOKABLE void numericMinVerseCount(QObject* caller);
    Q_INVOKABLE void numericTotalAyatCount(QObject* caller);
    Q_INVOKABLE void numericTotalSurahCount(QObject* caller);
    Q_INVOKABLE void numericVerseCount(QObject* caller);
    Q_INVOKABLE void orderedSurahs(QObject* caller);
    Q_INVOKABLE void orderedRevelationSurahs(QObject* caller);
    Q_INVOKABLE void orderedSurahVerses(QObject* caller);
    Q_INVOKABLE void orderedSurahsByLength(QObject* caller);
    Q_INVOKABLE void standardSajdaSurah(QObject* caller);
    Q_INVOKABLE void standardVersesForSurah(QObject* caller);
	void lazyInit();
};

} /* namespace quran */
#endif /* QuranHelper_H_ */
