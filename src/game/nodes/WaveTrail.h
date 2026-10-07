#pragma once
#include "../engine.h"

namespace opendash {

class WaveTrail : public engine::DrawNode {
public:
    CREATE_FUNC(WaveTrail);

    void addPoint(engine::Point point);

    void clearAboveXPos(float x);
    void clearBehindXPos(float x);

    void firstSetup();

    void reset();

    void stopStroke();
    void resumeStroke();

    void scheduleAutoUpdate();

    void update(float dt) override;

    inline void setCurrentPoint(const engine::Point& point) { currentPoint_ = point; }

    inline void setWaveSize(float size) { waveSize_ = size; }
    inline void setPulseSize(float size) { pulseSize_ = size; }

    inline void setSolid(bool value) { isSolid_ = value; }
    inline void setFlipped(bool value) { isFlipped_ = value; }

private:
    bool init() override;

private:
    std::vector<engine::Point> points_;
    engine::Point currentPoint_;

    float waveSize_ = 1.0f;
    float pulseSize_ = 1.0f;

    bool isSolid_ = false;
    bool isFlipped_ = false;
    bool drawStreak_ = false;
};

};