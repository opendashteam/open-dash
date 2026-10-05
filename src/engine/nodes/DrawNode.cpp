#include "DrawNode.h"
#include "../utilities/log.h"

namespace opendash::engine {

// Mostly copied from cocos' CCDrawNode.cpp
void DrawNode::drawPolygon(
    std::span<Point> points,
    const Color4B& fillColor,
    float borderWidth,
    const Color4F& borderColor
) {
    struct ExtrudeVerts { Point offset, n; };

    std::vector<ExtrudeVerts> extrude;
    extrude.resize(points.size(), { {0, 0}, {0, 0} });

    u32 count = points.size();
	
	for (u32 i = 0; i < count; i++) {
		Point v0 = points[(i - 1 + count) % count];
		Point v1 = points[i];
		Point v2 = points[(i + 1) % count];
        
		Point n1 = (v1 - v0).getPerpCCW().normalize();
		Point n2 = (v2 - v1).getPerpCCW().normalize();
		
		Point offset = (n1 + n2) * (1.0 / (Point::dot(n1, n2) + 1.0));

		extrude[i] = {offset, n2};
	}
	
	bool outline = (borderColor.a > 0.0 && borderWidth > 0.0);
	
	u32 triangleCount = 3 * count - 2;
	u32 vertexCount = 3 * triangleCount;

    u32 prevVertexCount = mesh_->getVertexCount();
    mesh_->resize(prevVertexCount + vertexCount);

    MeshVertex* ptr = mesh_->getBuffer() + prevVertexCount;
	
	float inset = (outline == 0.0 ? 0.5 : 0.0);
	for (u32 i = 0; i < count - 2; i++) {
		Point v0 = points[0]     - (extrude[0].offset     * inset);
		Point v1 = points[i + 1] - (extrude[i + 1].offset * inset);
		Point v2 = points[i + 2] - (extrude[i + 2].offset * inset);

        *(ptr++) = {v0.toGLM(), fillColor, glm::vec2(0, 0)};
        *(ptr++) = {v1.toGLM(), fillColor, glm::vec2(0, 0)};
        *(ptr++) = {v2.toGLM(), fillColor, glm::vec2(0, 0)};
	}
	
	for (u32 i = 0; i < count; i++) {
		int j = (i + 1) % count;
		Point v0 = points[i];
		Point v1 = points[j];
		
		Point n0 = extrude[i].n;
		
		Point offset0 = extrude[i].offset;
		Point offset1 = extrude[j].offset;
		
		if (outline) {
			Point inner0 = v0 - (offset0 * borderWidth);
			Point inner1 = v1 - (offset1 * borderWidth);
			Point outer0 = v0 + (offset0 * borderWidth);
			Point outer1 = v1 + (offset1 * borderWidth);

            *(ptr++) = {inner0.toGLM(), fillColor, (-n0).toGLM()};
            *(ptr++) = {inner1.toGLM(), fillColor, (-n0).toGLM()};
            *(ptr++) = {outer1.toGLM(), fillColor, n0.toGLM()};
            
            *(ptr++) = {inner0.toGLM(), fillColor, (-n0).toGLM()};
            *(ptr++) = {outer0.toGLM(), fillColor, n0.toGLM()};
            *(ptr++) = {outer1.toGLM(), fillColor, n0.toGLM()};
		} else {
			Point inner0 = v0 - (offset0 * 0.5);
			Point inner1 = v1 - (offset1 * 0.5);
			Point outer0 = v0 + (offset0 * 0.5);
			Point outer1 = v1 + (offset1 * 0.5);
			
            *(ptr++) = {inner0.toGLM(), fillColor, glm::vec2(0, 0)};
            *(ptr++) = {inner1.toGLM(), fillColor, glm::vec2(0, 0)};
            *(ptr++) = {outer1.toGLM(), fillColor, n0.toGLM()};

            *(ptr++) = {inner0.toGLM(), fillColor, glm::vec2(0, 0)};
            *(ptr++) = {outer0.toGLM(), fillColor, n0.toGLM()};
            *(ptr++) = {outer1.toGLM(), fillColor, n0.toGLM()};
		}
	}

    markRangeDirty(prevVertexCount, vertexCount);
}

void DrawNode::draw(Graphics* _) {
    flushChanges();
    
    if (texture_)
        mesh_->drawTextured(getWorldTransform(), texture_, {1, 1, 1, 1}, additiveBlending_);
    else
        mesh_->drawSolid(getWorldTransform(), {1, 1, 1, 1}, additiveBlending_);
}

bool DrawNode::init() {
    if (!ColorNode::init())
        return false;

    mesh_ = Mesh::create();
    mesh_->reserve(256);
    return true;
}

void DrawNode::markRangeDirty(u32 index, u32 count) {
    if (count == 0)
        return;
    u32 min = index;
    u32 max = index + count - 1;
    if (!dirty_) {
        dirty_ = true;
        dirtyRangeMin_ = min;
        dirtyRangeMax_ = max;
    } else {
        dirtyRangeMin_ = std::min(dirtyRangeMin_, min);
        dirtyRangeMax_ = std::max(dirtyRangeMax_, max);
    }
}

void DrawNode::flushChanges() {
    if (dirty_) {
        mesh_->flushRange(dirtyRangeMin_, dirtyRangeMax_ - dirtyRangeMin_ + 1);
        dirty_ = false;
        dirtyRangeMin_ = 0;
        dirtyRangeMax_ = 0;
    }
}

};