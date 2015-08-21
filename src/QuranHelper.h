#ifndef QuranHelper_H_
#define QuranHelper_H_

#define ENGLISH_TRANSLATION "english"

#include "QueryId.h"

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

	void lazyInit();

    void setDatabaseName(QString const& name);
};

} /* namespace quran */
#endif /* QuranHelper_H_ */
