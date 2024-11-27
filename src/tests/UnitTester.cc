#include "UnitTester.h"

#include "app/Colors.h"
#include "app/Solver.h"
#include "cells/Brain.h"
#include "cells/Log.h"

#include <array>
#include <set>


namespace infocell {

UnitTester::UnitTester(Logger& logger) :
    logger(logger)
{
    start();
}

struct TestCase
{
    TestCase(const std::string& name, std::function<void()> testFn) :
        name(name), test(testFn) { }

    std::string name;
    std::function<void()> test;
};

class TestCases
{
public:
    TestCases() :
        kb([]() {
            spdlog::get("cells")->set_level(spdlog::level::trace);
            spdlog::get("compileStruct")->set_level(spdlog::level::off);
            spdlog::get("symbolResolver")->set_level(spdlog::level::off);
            spdlog::get("compiledSymbols")->set_level(spdlog::level::off);
        })
    {
        if (m_testCases.empty())
            addTestCases();
    }

    static TestCases& get()
    {
        static TestCases testCases;
        return testCases;
    }

    size_t size()
    {
        return m_testCases.size();
    }

    void add(const TestCase& testCase)
    {
        m_testCases.push_back(testCase);
    }

    const std::vector<TestCase>& testCases()
    {
        return m_testCases;
    }

private:
    cells::brain::Brain kb;
    void addTestCases();
    std::vector<TestCase> m_testCases;
};

void TestCases::addTestCases()
{
    cells::brain::Brain kb2;
    add(TestCase("ShapeTest", [this]() {
        input::Grid inputGrid("test", "[[0, 7, 7], [7, 7, 7], [0, 7, 7]]");
        cells::hybrid::Picture picture(kb, inputGrid);
        Shaper shaper(picture);
        shaper.process();
#if 0
        for (std::shared_ptr<Shape> shape : shaper.shapes()) {
            loggerPtr->log(DEBUG) << "Shape:";
            loggerPtr->logBoard(DEBUG) << shape->toString() << "\n";
        }
#endif
        loggerPtr->log(DEBUG) << "Number of shapes found: " << shaper.shapes().size();
        assert(shaper.shapes().size() == 3);
    }));

    add(TestCase("ShapeTestDiagonal", [this]() {
        input::Grid inputGrid("test", "[[7, 0, 0], [0, 7, 0], [0, 0, 7]]");
        cells::hybrid::Picture picture(kb, inputGrid);
        Shaper shaper(picture);
        shaper.process();
#if 0
        for (std::shared_ptr<Shape> shape : shaper.shapes()) {
            loggerPtr->log(DEBUG) << "Shape:";
            loggerPtr->logBoard(DEBUG) << shape->toString() << "\n";
        }
#endif
        loggerPtr->log(DEBUG) << "Number of shapes found: " << shaper.shapes().size();
        assert(shaper.shapes().size() == 2);
    }));

    add(TestCase("ShapeMergeTest", [this]() {
        input::Grid inputGrid("test", "[[7, 0, 7], [7, 0, 7], [7, 7, 7]]");
        cells::hybrid::Picture picture(kb, inputGrid);
        Shaper shaper(picture);
        shaper.process();
#if 0
        for (std::shared_ptr<Shape> shape : shaper.shapes()) {
            loggerPtr->log(DEBUG) << "Shape:";
            loggerPtr->logBoard(DEBUG) << shape->toString() << "\n";
        }
#endif
        loggerPtr->log(DEBUG) << "Number of shapes found: " << shaper.shapes().size();
        assert(shaper.shapes().size() == 2);
    }));

    add(TestCase("StretchTest", []() {
        std::vector<Vector> shapeVectors = {
            { 1, 0 },
            { -2, 1 },
            { 1, 0 },
            { 1, 0 },
            { -1, 1 },
            { 1, 0 }
        };
        VectorShape vectorShape(shapeVectors, color(arc::Colors::orange), { 3, 2 });

        DrawingBoard drawingBoard(18, 23);
        drawingBoard.renderVectorShape(2, 1, vectorShape.stretch(1, 1));
        drawingBoard.renderVectorShape(2, 10, vectorShape.stretch(1, 2));
        drawingBoard.renderVectorShape(10, 3, vectorShape.stretch(2, 1));
        drawingBoard.renderVectorShape(10, 9, vectorShape.stretch(2, 2));
        drawingBoard.renderVectorShape(10, 18, vectorShape.stretch(3, 3));

        loggerPtr->logBoard(DEBUG) << drawingBoard.toString() << "\n";
    }));

    add(TestCase("RotationTest", []() {
        std::vector<Vector> shapeVectors = {
            { 1, 0 },
            { -2, 1 },
            { 1, 0 },
            { 1, 0 },
            { -1, 1 },
            { 1, 0 }
        };
        DrawingBoard drawingBoard(12, 10);
        VectorShape vectorShape(shapeVectors, color(arc::Colors::orange), { 3, 2 });
        VectorShape rotatedShape = vectorShape.rotate(RotationDir::Degree_0);
        drawingBoard.renderVectorShape(rotatedShape);
        drawingBoard.renderVectorShape(rotatedShape.mirror(DistanceType::FromFirstPixel, { 3, 0 }, RotationDir::Degree_0));
        drawingBoard.renderVectorShape(rotatedShape.mirror(DistanceType::FromFirstPixel, { 0, 3 }, RotationDir::Degree_90));

        loggerPtr->logBoard(DEBUG) << drawingBoard.toString() << "\n";
    }));

    add(TestCase("LineDrawingAxisTest", []() {
        DrawingBoard drawingBoard(15, 15);

        drawingBoard.renderLine(6, 6, color(arc::Colors::grey), RotationDir::Degree_0);
        drawingBoard.renderLine(6, 6, color(arc::Colors::grey), RotationDir::Degree_45);
        drawingBoard.renderLine(6, 6, color(arc::Colors::grey), RotationDir::Degree_90);
        drawingBoard.renderLine(6, 6, color(arc::Colors::grey), RotationDir::Degree_135);

        loggerPtr->logBoard(DEBUG) << drawingBoard.toString() << "\n";
    }));

    add(TestCase("LineDrawingPositionTest", []() {
        DrawingBoard drawingBoard(15, 15);

        drawingBoard.renderLine(0, 0, color(arc::Colors::grey), RotationDir::Degree_0);
        drawingBoard.renderLine(0, 0, color(arc::Colors::grey), RotationDir::Degree_90);
        drawingBoard.renderLine(14, 0, color(arc::Colors::grey), RotationDir::Degree_0);
        drawingBoard.renderLine(0, 14, color(arc::Colors::grey), RotationDir::Degree_90);

        loggerPtr->logBoard(DEBUG) << drawingBoard.toString() << "\n";
    }));

    add(TestCase("BoundingBoxTest", []() {
        std::vector<Pixel> pixels = {
            { 4, 2 },
            { 4, 3 },
            { 4, 4 },
            { 4, 5 },
            { 4, 6 },
            { 3, 4 },
            { 2, 4 },
            { 5, 4 },
            { 6, 4 },
        };
        VectorShape vectorShape(color(arc::Colors::orange));
        vectorShape.fromPixels(pixels);
        BoundingBox boundingBox(pixels);

        DrawingBoard drawingBoard(8, 8);
        drawingBoard.renderBoundingBox(boundingBox);
        drawingBoard.renderVectorShape(vectorShape);

        loggerPtr->logBoard(DEBUG) << drawingBoard.toString() << "\n";
    }));

    add(TestCase("Mirroring vertically (0 or 180 degree)", []() {
        std::vector<Vector> shapeVectors = {
            { 1, 0 },
            { -2, 1 },
            { 1, 0 },
            { 1, 0 },
            { -1, 1 },
            { 1, 0 }
        };
        VectorShape vectorShape(shapeVectors, color(arc::Colors::orange), { 2, 1 });
        VectorShape mirrorShape = vectorShape.mirror(DistanceType::FromFirstPixel, { 3, 3 }, RotationDir::Degree_0);
        DrawingBoard drawingBoard(11, 5);
        drawingBoard.renderLine(5, 3, color(arc::Colors::grey), RotationDir::Degree_0);
        drawingBoard.renderVectorShape(vectorShape);
        drawingBoard.renderVectorShape(mirrorShape);

        loggerPtr->logBoard(DEBUG) << drawingBoard.toString() << "\n";
    }));

    add(TestCase("Mirroring horizontally (90 or 270 degree)", []() {
        std::vector<Vector> shapeVectors = {
            { 1, 0 },
            { -2, 1 },
            { 1, 0 },
            { 1, 0 },
            { -1, 1 },
            { 1, 0 }
        };
        VectorShape vectorShape(shapeVectors, color(arc::Colors::orange), { 2, 1 });
        VectorShape mirrorShape = vectorShape.mirror(DistanceType::FromFirstPixel, { 3, 4 }, RotationDir::Degree_90);
        DrawingBoard drawingBoard(5, 11);
        drawingBoard.renderLine(3, 5, color(arc::Colors::grey), RotationDir::Degree_90);
        drawingBoard.renderVectorShape(vectorShape);
        drawingBoard.renderVectorShape(mirrorShape);

        loggerPtr->logBoard(DEBUG) << drawingBoard.toString() << "\n";
    }));

    add(TestCase("Mirroring (45 or 225 degree)", []() {
        std::vector<Vector> shapeVectors = {
            { 1, 0 },
            { -2, 1 },
            { 1, 0 },
            { 1, 0 },
            { -1, 1 },
            { 1, 0 }
        };
        VectorShape vectorShape(shapeVectors, color(arc::Colors::orange), { 1, 2 });
        BoundingBox boundingBox(vectorShape);

        Vector distanceVector(2, 2);
        Pixel mirroringPixel = boundingBox.bottomRightPixel() + distanceVector;
        loggerPtr->log(DEBUG) << "Mirroring at [" << mirroringPixel.x << ", " << mirroringPixel.y << "]";

        VectorShape mirrorShape = vectorShape.mirror(DistanceType::FromBottomRight, distanceVector, RotationDir::Degree_45);
        DrawingBoard drawingBoard(10, 12);
        drawingBoard.renderLine(mirroringPixel.x, mirroringPixel.y, color(arc::Colors::grey), RotationDir::Degree_45);
        drawingBoard.setColor(mirroringPixel.x, mirroringPixel.y, color(arc::Colors::fuschia));
        drawingBoard.renderVectorShape(vectorShape);
        drawingBoard.renderVectorShape(mirrorShape);

        loggerPtr->logBoard(DEBUG) << drawingBoard.toString() << "\n";
    }));

    add(TestCase("Mirroring shape 45", []() {
        std::vector<Vector> shapeVectors = {
            { 0, 1 },
            { 0, 1 },
            { 0, 1 },
            { 1, 0 }
        };
        VectorShape vectorShape(shapeVectors, color(arc::Colors::orange), { 2, 2 });
        loggerPtr->log(DEBUG) << "vectorShape.vectors";
        for (const Vector& vector : vectorShape.vectors()) {
            loggerPtr->log(DEBUG) << " - " << vector;
        }

        BoundingBox boundingBox(vectorShape);
        Vector distanceVector(3, 0);
        Pixel mirroringPixel = boundingBox.bottomRightPixel() + distanceVector;
        loggerPtr->log(DEBUG) << "boundingBox.bottomRightPixel: " << boundingBox.bottomRightPixel();
        loggerPtr->log(DEBUG) << "Mirroring at " << mirroringPixel;


        VectorShape mirrorShape = vectorShape.mirror(DistanceType::FromBottomRight, distanceVector, RotationDir::Degree_45);
        loggerPtr->log(DEBUG) << "mirrorShape.vectors";
        for (const Vector& vector : mirrorShape.vectors()) {
            loggerPtr->log(DEBUG) << " - " << vector;
        }
        DrawingBoard drawingBoard(12, 12);
        drawingBoard.renderLine(mirroringPixel.x, mirroringPixel.y, color(arc::Colors::grey), RotationDir::Degree_45);
        drawingBoard.setColor(mirroringPixel.x, mirroringPixel.y, color(arc::Colors::fuschia));
        drawingBoard.renderVectorShape(vectorShape);
        drawingBoard.renderVectorShape(mirrorShape);

        loggerPtr->logBoard(DEBUG) << drawingBoard.toString() << "\n";
    }));

    add(TestCase("Mirroring (135 or 315 degree)", []() {
        std::vector<Vector> shapeVectors = {
            { 1, 0 },
            { -2, 1 },
            { 1, 0 },
            { 1, 0 },
            { -1, 1 },
            { 1, 0 }
        };
        VectorShape vectorShape(shapeVectors, color(arc::Colors::orange), { 2, 7 });
        BoundingBox boundingBox(vectorShape);
        Vector distanceVector(2, -2);
        Pixel mirroringPixel = boundingBox.topRightPixel() + distanceVector;
        loggerPtr->log(DEBUG) << "boundingBox.topRightPixel: " << boundingBox.topRightPixel();
        loggerPtr->log(DEBUG) << "Mirroring at " << mirroringPixel;

        VectorShape mirrorShape = vectorShape.mirror(DistanceType::FromTopRight, distanceVector, RotationDir::Degree_135);

        DrawingBoard drawingBoard(12, 12);
        drawingBoard.renderLine(mirroringPixel.x, mirroringPixel.y, color(arc::Colors::grey), RotationDir::Degree_135);
        drawingBoard.setColor(mirroringPixel.x, mirroringPixel.y, color(arc::Colors::fuschia));
        drawingBoard.renderVectorShape(vectorShape);
        drawingBoard.renderVectorShape(mirrorShape);

        loggerPtr->logBoard(DEBUG) << drawingBoard.toString() << "\n";
    }));
}

void UnitTester::start()
{
    static TestCases testCases;

    loggerPtr = &logger;
    logger.clearLogFile();
    logger.log(INFO) << "There are " << testCases.size() << " test cases";
    int i = 0;
    for (const TestCase& testCase : testCases.testCases()) {
        i++;
        logger.log(INFO) << i << ". " << testCase.name;
        testCase.test();
    }
}

} // namespace infocell