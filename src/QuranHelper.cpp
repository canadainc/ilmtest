#include "precompiled.h"

#include "QuranHelper.h"
#include "CommonConstants.h"
#include "DatabaseHelper.h"
#include "Logger.h"

namespace quran {

using namespace canadainc;

QuranHelper::QuranHelper(DatabaseHelper* sql) : m_sql(sql)
{
}


void QuranHelper::setDatabaseName(QString const& name)
{
    m_name = name;
}


void QuranHelper::lazyInit()
{
}


QuranHelper::~QuranHelper()
{
}

} /* namespace oct10 */
