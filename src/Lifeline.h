#ifndef LIFELINE_H_
#define LIFELINE_H_

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
