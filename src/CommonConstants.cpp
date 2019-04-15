#include "precompiled.h"

#include "CommonConstants.h"

namespace ilmtest {

QUrl CommonConstants::generateHostUrl(QString const& path)
{
    QUrl url;
    url.setScheme("http");
    url.setUserName("username");
    url.setPassword("password");
    url.setHost("host");
    url.setPath("/path/"+path);

    return url;
}


}
