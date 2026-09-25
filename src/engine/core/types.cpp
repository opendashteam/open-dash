#include "types.h"
#include "Director.h"

namespace opendash::engine
{

Size Size::inUnits() const {
    return *this * Director::get()->getInvertedContentScaleFactor();
}

Size Size::inPixels() const {
    return *this * Director::get()->getContentScaleFactor();
}

Point Point::inUnits() const {
    return *this * Director::get()->getInvertedContentScaleFactor();
}

Point Point::inPixels() const {
    return *this * Director::get()->getContentScaleFactor();
}

}

