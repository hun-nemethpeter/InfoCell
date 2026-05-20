#pragma once

#include "arc/hybridcells/Frame.h"
#include "arc/hybridcells/Grid.h"
#include "arc/Grid.h"
#include "cells/Brain.h"
#include "cells/NodeBase.h"

#include <memory>

namespace infocell {
namespace arc {

class EdgeDetector : public cells::NodeBase
{
public:
    EdgeDetector(cells::brain::Brain& kb);

    void detect(const std::string& jsonStr);
    void detect(cells::arc::Grid& inputHybridGrid);

    cells::CellI& frame();
    cells::arc::Grid& inputHybridGrid();

protected:
    void detect();

    virtual void frameProcess();
    virtual void sortShapePixelsAndCreateShapePoints();
    virtual void sortShapePoints();
    virtual void calculateEdgesForShapes();
    virtual void processEdgeNodes();

    cells::CellI* firstShapePixelPtr();
    void addEdgeToShape(cells::CellI& shape, cells::CellI& newEdgeId, cells::CellI& newEdge);
    int getShapeEdgesSize(cells::CellI& shape);
    void findRotationCornersUpLeft();
    void findRotationCornersUpRight();
    void findRotationCornersDownLeft();
    void findRotationCornersDownRight();
    void findMirroringCornersUpRight();
    void findMirroringCornersDownLeft();
    void findPossibleBackgroundWithShapes();

    enum class ScanLineState
    {
        Up,
        Middle,
        Down
    };

public:
    cells::CellI& FrameStruct;
    cells::CellI& ShapeStruct;
    cells::CellI& ShapePointStruct;
    cells::CellI& ShapePixelStruct;
    cells::CellI& ShapeEdgeStruct;
    cells::CellI& ShapeEdgeJointStruct;
    cells::CellI& ShapeEdgeNodeStruct;
    cells::CellI& ShapeEdgeRotationCornersStruct;
    cells::CellI& ShapeEdgeMirroringCornersStruct;
    cells::CellI& ExternalEdgeEV;
    cells::CellI& InternalEdgeEV;
    cells::CellI& DirectionUpEV;
    cells::CellI& DirectionDownEV;
    cells::CellI& DirectionLeftEV;
    cells::CellI& DirectionRightEV;
    cells::CellI& Degree_0;
    cells::CellI& Degree_90;
    cells::CellI& Degree_180;
    cells::CellI& Degree_270;
    cells::CellI& Symmetry_Horizontal;
    cells::CellI& Symmetry_Vertical;

    std::unique_ptr<arc::native::Grid> m_inputGrid;
    std::unique_ptr<cells::arc::Grid> m_inputHybridGrid;
    cells::arc::Grid* m_inputHybridGridPtr = nullptr;
    std::unique_ptr<cells::Object> m_frame;
    std::unique_ptr<cells::arc::Frame> m_hybridFrame;
};

} // namespace arc
} // namespace infocell
