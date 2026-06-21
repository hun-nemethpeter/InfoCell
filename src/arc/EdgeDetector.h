#pragma once

#include "arc/hybridcells/Frame.h"
#include "arc/hybridcells/Grid.h"
#include "arc/Grid.h"
#include "cells/NodeBase.h"
#include "cells/World.h"

#include <memory>

namespace infocell {
namespace arc {

class EdgeDetector : public cells::NodeBase
{
public:
    EdgeDetector(cells::World& w);

    void detect(const std::string& jsonStr);
    void detect(cells::arc::Grid& inputHybridGrid);

    cells::CellI& frame();
    cells::arc::Grid& inputHybridGrid();

protected:
    enum class ScanLineState
    {
        Up,
        Middle,
        Down
    };

    void detect();

    virtual void frameProcess();
    virtual void sortShapePixelsAndCreateShapePoints();
    virtual void sortShapePoints();
    virtual void calculateEdgesForShapes();
    virtual void processEdgeNodes();
    virtual void findRotationCornersUpLeft();
    virtual void findRotationCornersUpRight();
    virtual void findRotationCornersDownLeft();
    virtual void findRotationCornersDownRight();
    virtual void findMirroringCornersUpRight();
    virtual void findMirroringCornersDownLeft();
    virtual void findPossibleBackgroundWithShapes();
    virtual void createResult();

    cells::CellI* firstShapePixelPtr();
    void addEdgeToShape(cells::CellI& shape, cells::CellI& newEdgeId, cells::CellI& newEdge);
    int getShapeEdgesSize(cells::CellI& shape);

    std::unique_ptr<arc::native::Grid> m_inputGrid;
    std::unique_ptr<cells::arc::Grid> m_inputHybridGrid;
    cells::arc::Grid* m_inputHybridGridPtr = nullptr;
    std::unique_ptr<cells::Object> m_frame;
    std::unique_ptr<cells::arc::Frame> m_hybridFrame;
};

} // namespace arc
} // namespace infocell
