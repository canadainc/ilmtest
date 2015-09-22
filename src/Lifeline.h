#ifndef LIFELINE_H_
#define LIFELINE_H_

#define LID_TO_QSTR(t) QString( TextUtils::e2s<Lifeline>(t) )

namespace ilmtest {

class Lifeline
{
    Q_GADGET
    Q_ENUMS(Type)

public:
    enum Type {
        Unknown,
        AskAnExpert,
        ChangeQuestion,
        FiftyFifty,
        FreezeTime,
        PhoneFriend,
        PopularOpinion,
        SecondChance,
        TakeOne
    };
};

} /* namespace ilmtest */

#endif /* LIFELINE_H_ */
