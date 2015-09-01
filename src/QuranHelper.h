#ifndef QuranHelper_H_
#define QuranHelper_H_

#include <QVariant>

namespace canadainc {
    class DatabaseHelper;
}

namespace quran {

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

    Q_INVOKABLE void fetchRandomSurahs(QObject* caller);
    Q_INVOKABLE void fetchRandomRevelationSurahs(QObject* caller);
    Q_INVOKABLE void fetchVersesForRandomSurah(QObject* caller);
    Q_INVOKABLE void fetchRandomVerseCount(QObject* caller);
    Q_INVOKABLE void fetchRandomSajdaSurah(QObject* caller);
    Q_INVOKABLE void fetchRandomSurahLocation(QObject* caller);
    Q_INVOKABLE void fetchSurahRandomVerses(QObject* caller);
    Q_INVOKABLE void fetchTotalSurahCount(QObject* caller);
    Q_INVOKABLE void fetchTotalAyatCount(QObject* caller);
    Q_INVOKABLE void fetchMaxVerseCount(QObject* caller);
    Q_INVOKABLE void fetchMinVerseCount(QObject* caller);
	void lazyInit();
};

} /* namespace quran */
#endif /* QuranHelper_H_ */
