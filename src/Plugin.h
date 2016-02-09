#ifndef PLUGIN_H_
#define PLUGIN_H_

#define PLUGID_TO_QSTR(t) QString( CommonConstants::e2s<Plugin>(t) )

namespace ilmtest {

class Plugin
{
    Q_GADGET
    Q_ENUMS(Type)

public:
    enum Type {
        Unknown,
        ExposeAnswer
    };
};

} /* namespace ilmtest */

#endif /* PLUGIN_H_ */
