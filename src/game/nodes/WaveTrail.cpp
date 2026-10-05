#include "WaveTrail.h"
#include <array>

using namespace opendash::engine;

namespace opendash {

void WaveTrail::addPoint(Point point) {
    points_.push_back(point);
}

void WaveTrail::clearAboveXPos(float x) {
    while (points_.size() > 1) {
        if (points_[1].x <= x)
            break;
        points_.erase(points_.begin());
    }
}

void WaveTrail::clearBehindXPos(float x) {
    while (points_.size() > 1) {
        if (points_[1].x >= x)
            break;
        points_.erase(points_.begin());
    }
}

void WaveTrail::firstSetup() {
    addPoint({0, 0});
    currentPoint_ = {10.0f, 10.0f};
    update(0.0f);
    visit(Graphics::get());
    reset();
}

void WaveTrail::reset() {
    clear();
    points_.clear();
}

void WaveTrail::stopStroke() {
    unscheduleUpdate();
    drawStreak_ = false;
    reset();
}

void WaveTrail::resumeStroke() {
    drawStreak_ = true;
    update(0.0f);
}

void WaveTrail::scheduleAutoUpdate() {
    scheduleUpdate();
}

void WaveTrail::update(float dt) {
    if (!drawStreak_)
        return;

    clear();
    if (points_.size() == 0 || getOpacity() == 0)
        return;

    int strokeCount = isSolid_ ? 1 : 2;

    Point pos = getPosition();

    for (auto& point : points_)
        point -= pos;
    currentPoint_ -= pos;

    for (int stroke = 0; stroke < strokeCount; stroke++) {
        for (int i = 0; i < points_.size(); i++) {
            Point currPoint = points_[i];
            
            Point nextPoint;
            if (i >= points_.size() - 1)
                nextPoint = currentPoint_;
            else
                nextPoint = points_[i + 1];

            if (currPoint == nextPoint)
                continue;

            if (isFlipped_)
                std::swap(currPoint, nextPoint);

            std::array<Point, 4> points;

            float width = (stroke == 0 ? 6.0f : 2.0f) * waveSize_ * pulseSize_;
            Point cornerOffset = (nextPoint - currPoint).normalize().getPerpCCW() * width * 0.5f;

            float  absCorX = fabsf(cornerOffset.x);
            double tanVal  = tanh(asinh(absCorX / (width * 0.5)));
            float  unkVal  = fabsf((float)tanVal * absCorX);

            bool someBool;

            if (i >= points_.size() - 1 && stroke == 0 && isFlipped_) {
                if (fabsf(currPoint.getDistance(nextPoint)) > 10.0f) {
                    float value = fabsf((float)(tanVal * 4.0f));
                    if (isFlipped_) {
                        currPoint.x += 4.0f;
                        if (currPoint.y >= nextPoint.y)
                            currPoint.y -= value;
                        else
                            currPoint.y += value;
                    } else {
                        currPoint.x -= 4.0f;
                        if (currPoint.y >= nextPoint.y)
                            currPoint.y += value;
                        else
                            currPoint.y -= value;
                    }
                }

                someBool = true;
            } else {
                someBool = false;
            }

            Point currOPoint = currPoint;
            Point nextOPoint = nextPoint;
            currOPoint.x -= absCorX;
            nextOPoint.x += absCorX;

            if (stroke != 0) {
                currPoint.x += absCorX;
                nextPoint.x -= absCorX;

                if (nextOPoint.y > currOPoint.y) {
                    currPoint.y += unkVal;
                    nextPoint.y -= unkVal;
                } else {
                    currPoint.y -= unkVal;
                    nextPoint.y += unkVal;
                }
            }

            if (nextOPoint.y > currOPoint.y) {
                currOPoint.y -= unkVal;
                nextOPoint.y += unkVal;

                points[0] = currOPoint - cornerOffset;
                points[1] = currPoint  + cornerOffset;
                points[2] = (someBool ? nextPoint : nextOPoint) + cornerOffset;
                points[3] = nextPoint - cornerOffset;
            } else {
                currOPoint.y += unkVal;
                nextOPoint.y -= unkVal;

                points[0] = currPoint  - cornerOffset;
                points[1] = currOPoint + cornerOffset;
                points[2] = nextPoint  + cornerOffset;
                points[3] = (someBool ? nextPoint : nextOPoint) - cornerOffset;
            }

            Color3B color;
            u8 opacity;

            if (stroke == 0) {
                color = getColor();
                opacity = getOpacity();
            } else {
                color = Color3B::White;
                opacity = (float)getOpacity() * 0.65f;
            }

            Color4B fillColor = { color.r, color.g, color.b, opacity };

            drawPolygon(points, fillColor, 0.0f, {0.0f, 0.0f, 0.0f, 0.0f});
        }
    }

    for (auto& point : points_)
        point += pos;
    currentPoint_ += pos;
}

bool WaveTrail::init() {
    if (!DrawNode::init())
        return false;

    firstSetup();
    return true;
}

};