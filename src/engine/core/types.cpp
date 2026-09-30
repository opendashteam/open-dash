#include "types.h"
#include "Director.h"

namespace opendash::engine
{

Size Size::toPoints() const {
    return *this * Director::get()->getInvertedContentScaleFactor();
}

Size Size::toPixels() const {
    return *this * Director::get()->getContentScaleFactor();
}

Point Point::toPoints() const {
    return *this * Director::get()->getInvertedContentScaleFactor();
}

Point Point::toPixels() const {
    return *this * Director::get()->getContentScaleFactor();
}

}

