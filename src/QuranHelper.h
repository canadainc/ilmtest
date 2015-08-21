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

public:
	QuranHelper(DatabaseHelper* sql);
	virtual ~QuranHelper();

    Q_INVOKABLE void fetchRandomSurahs(QObject* caller);
    Q_INVOKABLE void fetchRandomVerseCount(QObject* caller);
	void lazyInit();
};

} /* namespace quran */
#endif /* QuranHelper_H_ */
