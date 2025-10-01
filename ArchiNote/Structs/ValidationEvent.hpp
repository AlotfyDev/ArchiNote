
#ifndef __VALIDATEEVENT_HPP__
#define __VALIDATEEVENT_HPP__


#include <string>




struct ValidationEvent {
    std::string attrId;
    bool isValid;
    std::string validationMsg;
};





#endif // __VALIDATEEVENT_HPP__