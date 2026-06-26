#include "ArcLib.h"
#include "Compiler.h"
#include "World.h"

namespace infocell {
namespace cells {

// ============================================================================
Arc::EColor::EColor(World& w, CellI& type, const std::string& label, Arc& arc) :
    Object(w, type, label),
    black(w, arc.Color, "arc::Color::black"),
    blue(w, arc.Color, "arc::Color::blue"),
    red(w, arc.Color, "arc::Color::red"),
    green(w, arc.Color, "arc::Color::green"),
    yellow(w, arc.Color, "arc::Color::yellow"),
    grey(w, arc.Color, "arc::Color::grey"),
    fuschia(w, arc.Color, "arc::Color::fuschia"),
    orange(w, arc.Color, "arc::Color::orange"),
    teal(w, arc.Color, "arc::Color::teal"),
    brown(w, arc.Color, "arc::Color::brown")
{
}

Arc::EDirection::EDirection(World& w, CellI& type, const std::string& label, Arc& arc) :
    Object(w, type, label),
    up(w, arc.Direction, "arc::Direction::up"),
    upRight(w, arc.Direction, "arc::Direction::upRight"),
    right(w, arc.Direction, "arc::Direction::right"),
    downRight(w, arc.Direction, "arc::Direction::downRight"),
    down(w, arc.Direction, "arc::Direction::down"),
    downLeft(w, arc.Direction, "arc::Direction::downLeft"),
    left(w, arc.Direction, "arc::Direction::left"),
    upLeft(w, arc.Direction, "arc::Direction::upLeft")
{
}

Arc::ELineSymmetry::ELineSymmetry(World& w, CellI& type, const std::string& label, Arc& arc) :
    Object(w, type, label),
    horizontal(w, arc.LineSymmetry, "arc::LineSymmetry::horizontal"),
    vertical(w, arc.LineSymmetry, "arc::LineSymmetry::vertical"),
    diagonalLowerLeft(w, arc.LineSymmetry, "arc::LineSymmetry::diagonalLowerLeft"),
    diagonalUpperLeft(w, arc.LineSymmetry, "arc::LineSymmetry::diagonalUpperLeft")
{
}

Arc::ERotationDir::ERotationDir(World& w, CellI& type, const std::string& label, Arc& arc) :
    Object(w, type, label),
    Degree_0(w, arc.RotationDir, "arc::RotationDir::Degree_0"),
    Degree_45(w, arc.RotationDir, "arc::RotationDir::Degree_45"),
    Degree_90(w, arc.RotationDir, "arc::RotationDir::Degree_90"),
    Degree_135(w, arc.RotationDir, "arc::RotationDir::Degree_135"),
    Degree_180(w, arc.RotationDir, "arc::RotationDir::Degree_180"),
    Degree_225(w, arc.RotationDir, "arc::RotationDir::Degree_225"),
    Degree_270(w, arc.RotationDir, "arc::RotationDir::Degree_270"),
    Degree_315(w, arc.RotationDir, "arc::RotationDir::Degree_315")
{
}

Arc::EShapeEdgeKind::EShapeEdgeKind(World& w, CellI& type, const std::string& label, Arc& arc) :
    Object(w, type, label),
    ExternalEdge(w, arc.ShapeEdgeKind, "arc::ShapeEdgeKind::ExternalEdge"),
    InternalEdge(w, arc.ShapeEdgeKind, "arc::ShapeEdgeKind::InternalEdge")
{
}

// ============================================================================
Arc::Arc(World& w) :
    w(w),
    Color(w, w.std.Enum, "arc::Color", *this),
    Direction(w, w.std.Enum, "arc::Direction", *this),
    LineSymmetry(w, w.std.Enum, "arc::LineSymmetry", *this),
    RotationDir(w, w.std.Enum, "arc::RotationDir", *this),
    ShapeEdgeKind(w, w.std.Enum, "arc::ShapeEdgeKind", *this),
    Example(w, w.std.Struct, "arc::Example"),
    Frame(w, w.std.Struct, "arc::Frame"),
    Grid(w, w.std.Struct, "arc::Grid"),
    Pixel(w, w.std.Struct, "arc::Pixel"),
    RenderedShape(w, w.std.Struct, "arc::RenderedShape"),
    Shape(w, w.std.Struct, "arc::Shape"),
    ShapeEdge(w, w.std.Struct, "arc::ShapeEdge"),
    ShapeEdgeJoint(w, w.std.Struct, "arc::ShapeEdgeJoint"),
    ShapeEdgeMirroringCorners(w, w.std.Struct, "arc::ShapeEdgeMirroringCorners"),
    ShapeEdgeNode(w, w.std.Struct, "arc::ShapeEdgeNode"),
    ShapeEdgeRotationCorners(w, w.std.Struct, "arc::ShapeEdgeRotationCorners"),
    ShapeField(w, w.std.Struct, "arc::ShapeField"),
    ShapePixel(w, w.std.Struct, "arc::ShapePixel"),
    ShapePoint(w, w.std.Struct, "arc::ShapePoint"),
    Task(w, w.std.Struct, "arc::Task"),
    Vector(w, w.std.Struct, "arc::Vector"),
    VectorShape(w, w.std.Struct, "arc::VectorShape")
{
}

class ArcLibAst : public AstHelper
{
public:
    ArcLibAst(World& w, Ast::Scope& scope);
    Arc& arc = w.arc;
};

ArcLibAst::ArcLibAst(World& w, Ast::Scope& scope) :
    AstHelper(w)
{
    scope.add<Struct>("Example")
        .members(
            member("input", "Grid"),
            member("output", "Grid"));

    scope.add<Struct>("Grid")
        .members(
            member("width", "std::Number"),
            member("height", "std::Number"),
            member("pixels", tt_("std::List", "valueType", "Pixel")),
            member("pixelsMap", _(std.TrieMap))
        );

    // struct Pixel
    auto& pixelStruct
        = scope.add<Struct>("Pixel")
              .members(
                  member("x", "std::Number"),
                  member("y", "std::Number"));

    pixelStruct.addMethod("constructor")
        .parameters(
            parameter("x", _(std.Number)),
            parameter("y", _(std.Number)))
        .instructions(
            m_("x") = p_("x"),
            m_("y") = p_("y"));


    scope.add<Struct>("Task")
        .members(
            member("examples", tt_("std::List", "valueType", "Example")),
            member("tests", tt_("std::List", "valueType", "Example")),
            member("solution", "Grid"));

    scope.add<Enum>("Color")
        .values(
            ev_("black", _(_0_)),
            ev_("blue", _(_1_)),
            ev_("red", _(_2_)),
            ev_("green", _(_3_)),
            ev_("yellow", _(_4_)),
            ev_("grey", _(_5_)),
            ev_("fuschia", _(_6_)),
            ev_("orange", _(_7_)),
            ev_("teal", _(_8_)),
            ev_("brown", _(_9_)));

    scope.add<Enum>("RotationDir")
        .values(
            ev_("Degree_0"),   // 🡩
            ev_("Degree_45"),  // 🡭
            ev_("Degree_90"),  // 🡪
            ev_("Degree_135"), // 🡮
            ev_("Degree_180"), // 🡫
            ev_("Degree_225"), // 🡯
            ev_("Degree_270"), // 🡨
            ev_("Degree_315")  // 🡬
        );

    scope.add<Enum>("Direction")
        .values(
            ev_("up"),        // 🡩
            ev_("upRight"),   // 🡭
            ev_("right"),     // 🡪
            ev_("downRight"), // 🡮
            ev_("down"),      // 🡫
            ev_("downLeft"),  // 🡯
            ev_("left"),      // 🡨
            ev_("upLeft"));   // 🡬

    scope.add<Enum>("LineSymmetry")
        .values(
            ev_("horizontal"),         // │
            ev_("vertical"),           // ──
            ev_("diagonalLowerLeft"),  // /
            ev_("diagonalUpperLeft")); // \

    // struct Vector
    auto& vectorStruct
        = scope.add<Struct>("Vector")
              .members(
                  member("x", _(std.Number)),
                  member("y", _(std.Number)));

    vectorStruct.addMethod("constructor")
        .parameters(
            parameter("x", _(std.Number)),
            parameter("y", _(std.Number)))
        .instructions(
            m_("x") = p_("x"),
            m_("y") = p_("y"));

    // Vector rotate(RotationDir rotationDir) const;
    vectorStruct.addMethod("rotate")
        .parameters(
            parameter("rotationDir", __type__("RotationDir")))
        .returnType(__type__("Vector"))
        .instructions(
            var_("ret") = new_("Vector", "constructor")("x", m_("x"))("y", m_("y")),
            match_(p_("rotationDir"))
                // 🡩🡩🡩🡩🡩🡩🡩🡩🡩🡩🡩🡩🡩🡩🡩
                .case_(
                    "Degree_0",
                    return_(*var_("ret")))
                // 🡭🡭🡭🡭🡭🡭🡭🡭🡭🡭🡭🡭🡭🡭🡭
                .case_(
                    "Degree_45",
                    block(
                        set(*var_("ret"), "x", subtract(m_("x"), m_("y"))),
                        set(*var_("ret"), "y", add(m_("y"), m_("x")))))
                // 🡪🡪🡪🡪🡪🡪🡪🡪🡪🡪🡪🡪🡪🡪🡪
                .case_(
                    "Degree_90",
                    block(
                        set(*var_("ret"), "x", subtract(_(_0_), m_("y"))),
                        set(*var_("ret"), "y", m_("x"))))
                // 🡮🡮🡮🡮🡮🡮🡮🡮🡮🡮🡮🡮🡮🡮🡮
                .case_(
                    "Degree_135",
                    block(
                        set(*var_("ret"), "x", subtract(subtract(_(_0_), m_("x")), m_("y"))), // TODO maybe ast.negative(m_("x")) ?
                        set(*var_("ret"), "y", subtract(m_("x"), m_("y")))))
                // 🡫🡫🡫🡫🡫🡫🡫🡫🡫🡫🡫🡫🡫🡫🡫
                .case_(
                    "Degree_180",
                    block(
                        set(*var_("ret"), "x", subtract(_(_0_), m_("x"))),
                        set(*var_("ret"), "y", subtract(_(_0_), m_("y")))))
                // 🡯🡯🡯🡯🡯🡯🡯🡯🡯🡯🡯🡯🡯🡯🡯
                .case_(
                    "Degree_225",
                    block(
                        set(*var_("ret"), "x", add(subtract(_(_0_), m_("x")), m_("y"))),
                        set(*var_("ret"), "y", subtract(subtract(_(_0_), m_("x")), m_("y")))))
                // 🡨🡨🡨🡨🡨🡨🡨🡨🡨🡨🡨🡨🡨🡨🡨
                .case_(
                    "Degree_270",
                    block(
                        set(*var_("ret"), "x", m_("y")),
                        set(*var_("ret"), "y", subtract(_(_0_), m_("x")))))
                // 🡬🡬🡬🡬🡬🡬🡬🡬🡬🡬🡬🡬🡬🡬🡬
                .case_(
                    "Degree_315",
                    block(
                        set(*var_("ret"), "x", add(m_("x"), m_("y"))),
                        set(*var_("ret"), "y", add(subtract(_(_0_), m_("x")), m_("y"))))),

            return_(*var_("ret")));

    // struct VectorShape
    auto& vectorShapeStruct
        = scope.add<Struct>("VectorShape")
              .members(
                  member("color", "Color"),
                  member("vectors", tt_("std::List", "valueType", "Vector")),
                  member("firstPixel", "Pixel"));

    vectorShapeStruct.addMethod("constructor")
        .parameters(
            parameter("color", "Color"))
        .instructions(
            m_("color")   = p_("color"),
            m_("vectors") = new_(tt_("std::List", "valueType", "Vector"), "constructor"));

    /*
     void VectorShape::fromPixels(const std::vector<Pixel>& pixels)
     {
         m_vectors.clear();
         firstPixel(pixels.front());
         const Pixel* prevPixel = &pixels.front();
         bool isFirstPixel      = true;

         for (const auto& pixel : pixels) {
             if (isFirstPixel) {
                 isFirstPixel = false;
                 continue;
             }
             const Pixel* currPixel = &pixel;
             m_vectors.push_back({ currPixel->x - prevPixel->x, currPixel->y - prevPixel->y });
             prevPixel = currPixel;
         }
     }
    */
    vectorShapeStruct.addMethod("fromPixels")
        .parameters(
            parameter("pixels", tt_("std::List", "valueType", "Pixel")))
        .instructions(
            m_("firstPixel")     = p_("pixels") / "first" / "value",
            var_("prevPixel")    = m_("firstPixel"),
            var_("isFirstPixel") = true_(),
            var_("pixel")        = _(id.emptyObject),
            if_(has(p_("pixels"), "first"))
                .then_(var_("pixel") = p_("pixels") / "first"),
            while_(notSame(*var_("pixel"), _(id.emptyObject)))
                .do_(block(
                    if_(same(*var_("isFirstPixel"), true_()))
                        .then_(block(
                            var_("isFirstPixel") = false_(),
                            if_(has(*var_("pixel"), "next"))
                                .then_(var_("pixel") = *var_("pixel") / "next")
                                .else_(var_("pixel") = _(id.emptyObject)),
                            continue_())),
                    var_("vector") = new_("Vector", "constructor")("x", subtract(*var_("pixel") / "value" / "x", *var_("prevPixel") / "x"))("y", subtract(*var_("pixel") / "value" / "y", *var_("prevPixel") / "y")),
                    m_("vectors")("add")("value", *var_("vector")),
                    var_("prevPixel") = *var_("pixel") / "value",
                    if_(has(*var_("pixel"), "next"))
                        .then_(var_("pixel") = *var_("pixel") / "next")
                        .else_(var_("pixel") = _(id.emptyObject)))));

    /*
    VectorShape VectorShape::rotate(RotationDir rotationDir) const
    {
        std::vector<Vector> rotatedVectors;

        for (const Vector& vector : m_vectors) {
            Vector newVector = vector.rotate(rotationDir);
            rotatedVectors.push_back(newVector);
            //            loggerPtr->log(DEBUG) << " rotate vector [" << vector.x << ", " << vector.y << "] => [" << newVector.x << ", " << newVector.y << "]";
        }

        return VectorShape(std::move(rotatedVectors), m_color, m_firstPixel);
    }
    */
    vectorShapeStruct.addMethod("rotate")
        .parameters(
            parameter("rotationDir", __type__("RotationDir")))
        .returnType(__type__("VectorShape"))
        .instructions(
            var_("ret")            = new_(__type__("VectorShape")),
            var_("rotatedVectors") = new_(tt_("std::List", "valueType", "Vector"), "constructor"),
            set(*var_("ret"), "color", m_("color")),
            set(*var_("ret"), "firstPixel", m_("firstPixel")),
            set(*var_("ret"), "vectors", *var_("rotatedVectors")),
            var_("vector") = _(id.emptyObject),
            if_(has(m_("vectors"), "first"))
                .then_(var_("vector") = m_("vectors") / "first"),
            while_(notSame(*var_("vector"), _(id.emptyObject)))
                .do_(block(
                    var_("newVector") = ((*var_("vector") / "value")("rotate")("rotationDir", p_("rotationDir"))),
                    var_("rotatedVectors")("add")("value", *var_("newVector")),
                    if_(has(*var_("vector"), "next"))
                        .then_(var_("vector") = *var_("vector") / "next")
                        .else_(var_("vector") = _(id.emptyObject)))),
            return_(*var_("ret")));

    // struct ShapeEdgeKind
    scope.add<Enum>("ShapeEdgeKind")
        .values(
            ev_("ExternalEdge"),
            ev_("InternalEdge"));

    // struct ShapeEdgeRotationCorners
    auto& ShapeEdgeRotationCornersStruct
        = scope.add<Struct>("ShapeEdgeRotationCorners")
              .members(
                  member("upLeftNode", "ShapeEdgeNode"),
                  member("upRightNode", "ShapeEdgeNode"),
                  member("downLeftNode", "ShapeEdgeNode"),
                  member("downRightNode", "ShapeEdgeNode"));

    // struct ShapeEdgeMirroringCorners
    auto& ShapeEdgeMirroringCornersStruct
        = scope.add<Struct>("ShapeEdgeMirroringCorners")
              .members(
                  member("upRightNode", "ShapeEdgeNode"),
                  member("downLeftNode", "ShapeEdgeNode"));

    // struct ShapeEdge
    auto& ShapeEdgeStruct
        = scope.add<Struct>("ShapeEdge")
              .members(
                  member("id", _(std.Number)),
                  member("shape", "RenderedShape"),
                  member("kind", "ShapeEdgeKind"),
                  member("fromExternalX", _(std.Number)),
                  member("fromExternalY", _(std.Number)),
                  member("shapes", tt_("std::Set", "valueType", "RenderedShape")),
                  member("shapePixels", tt_("std::List", "valueType", "ShapePixel")),
                  member("rotationCorners", "ShapeEdgeRotationCorners"),
                  member("mirroringCorners", "ShapeEdgeMirroringCorners"),
                  member("upLeftNode", "ShapeEdgeNode"),
                  member("upRightNode", "ShapeEdgeNode"),
                  member("downLeftNode", "ShapeEdgeNode"),
                  member("downRightNode", "ShapeEdgeNode"),
                  member("edgeNodes", tt_("std::List", "valueType", "ShapeEdgeNode")));

    // struct ShapeEdgeNode
    auto& ShapeEdgeNodeStruct
        = scope.add<Struct>("ShapeEdgeNode")
              .members(
                  member("edge", "ShapeEdge"),
                  member("from", "ShapePoint"),
                  member("direction", "Direction"),
                  member("externalShape", "RenderedShape"),
                  member("next", "ShapeEdgeNode"),
                  member("previous", "ShapeEdgeNode"));

    // struct ShapeEdge
    auto& ShapeEdgeJointStruct
        = scope.add<Struct>("ShapeEdgeJoint")
              .members(
                  member("upLeft", "ShapeEdgeNode"),
                  member("upRight", "ShapeEdgeNode"),
                  member("downLeft", "ShapeEdgeNode"),
                  member("downRight", "ShapeEdgeNode"),
                  member("leftUp", "ShapeEdgeNode"),
                  member("leftDown", "ShapeEdgeNode"),
                  member("rightUp", "ShapeEdgeNode"),
                  member("rightDown", "ShapeEdgeNode"));

    // struct ShapePoint
    auto& shapePointStruct
        = scope.add<Struct>("ShapePoint")
              .members(
                  member("edgeJoint", "ShapeEdgeJoint"),
                  member("up", "ShapePoint"),
                  member("down", "ShapePoint"),
                  member("left", "ShapePoint"),
                  member("right", "ShapePoint"),
                  member("upLeftPixel", "ShapePixel"),
                  member("upRightPixel", "ShapePixel"),
                  member("downLeftPixel", "ShapePixel"),
                  member("downRightPixel", "ShapePixel"),
                  member("x", _(std.Number)),
                  member("y", _(std.Number)));

    // struct ShapePixel
    auto& shapePixelStruct
        = scope.add<Struct>("ShapePixel")
              .members(
                  member("shape", "RenderedShape"),
                  member("up", "ShapePixel"),
                  member("down", "ShapePixel"),
                  member("left", "ShapePixel"),
                  member("right", "ShapePixel"),
                  member("upLeftPoint", "ShapePoint"),
                  member("upRightPoint", "ShapePoint"),
                  member("downLeftPoint", "ShapePoint"),
                  member("downRightPoint", "ShapePoint"),
                  member("pixel", _(std.Pixel)));

    shapePixelStruct.addMethod("constructor")
        .parameters(
            parameter("shape", __type__("RenderedShape")),
            parameter("pixel", _(std.Pixel)))
        .instructions(
            m_("shape") = p_("shape"),
            m_("pixel") = p_("pixel"));

    // struct RenderedShape
    auto& renderedShapeStruct
        = scope.add<Struct>("RenderedShape")
              .typeAliases(
                  typeAlias("InternalEdgeLookup", tt_("std::Map", "keyType", _(std.Number), "valueType", tt_("std::Map", "keyType", _(std.Number), "valueType", "ShapeEdge"))))
              .members(
                  member("id", _(std.Number)),
                  member("color", "Color"),
                  member("width", _(std.Number)),
                  member("height", _(std.Number)),
                  member("lastEdgeId", _(std.Number)),
                  member("edges", tt_("std::Map", "keyType", _(std.Number), "valueType", "ShapeEdge")),
                  member("internalEdges", ta_("InternalEdgeLookup")),
                  member("pixels", tt_("std::List", "valueType", "Pixel")),
                  member("shapePixels", tt_("std::List", "valueType", "ShapePixel")),
                  member("shapePoints", tt_("std::List", "valueType", "ShapePoint")));

    /*
    RenderedShape(int id, input::Color color, int width, int height) :
        m_id(id), m_color(color), m_width(width), m_height(height) { }
    */
    renderedShapeStruct.addMethod("constructor")
        .parameters(
            parameter("id", _(std.Number)),
            parameter("color", __type__("Color")),
            parameter("width", _(std.Number)),
            parameter("height", _(std.Number)))
        .instructions(
            m_("id")           = p_("id"),
            m_("color")        = p_("color"),
            m_("width")        = p_("width"),
            m_("height")       = p_("height"),
            m_("lastEdgeId")   = _(_0_),
            m_("edges")        = new_(tt_("std::Map", "keyType", _(std.Number), "valueType", "ShapeEdge"), "constructor"));

    renderedShapeStruct.addMethod("toVectorShape")
        .returnType(__type__("VectorShape"))
        .instructions(
            var_("ret") = new_("VectorShape", "constructor")("color", m_("color")),
            var_("ret")("fromPixels")("pixels", m_("pixels")),
            return_(*var_("ret")));

    // struct Shape
    auto& shapeStruct
        = scope.add<Struct>("Shape")
              .members(
                  member("color", "Color"),
                  member("externalEdgeLine", tt_("std::List", "valueType", "Direction")),
                  member("internalEdges", tt_("std::TrieMap", "keyType", "Vector", "valueType", tt_("std::List", "valueType", "Direction"))));

    // struct ShapeField
    auto& shapeFieldStruct
        = scope.add<Struct>("ShapeField")
              .members(
                  member("width", _(std.Number)),
                  member("height", _(std.Number)),
                  member("internalEdges", tt_("std::TrieMap", "keyType", "Vector", "valueType", "Shape")));

    // struct Frame
    auto& frameStruct
        = scope.add<Struct>("Frame")
              .typeAliases(
                  typeAlias("tableType", tt_("std::Map", "keyType", _(std.Number), "valueType", tt_("std::Map", "keyType", _(std.Number), "valueType", "RenderedShape"))))
              .members(
                  member("width", _(std.Number)),
                  member("height", _(std.Number)),
                  member("grid", "Grid"),
                  member("upLeftPoint", "ShapePoint"),
                  member("upRightPoint", "ShapePoint"),
                  member("downLeftPoint", "ShapePoint"),
                  member("downRightPoint", "ShapePoint"),
                  member("shapes", tt_("std::List", "valueType", "RenderedShape")),
                  member("shapeMap", tt_("std::Map", "keyType", _(std.Number), "valueType", "RenderedShape")),
                  member("shapePixels", ta_("tableType")),
                  member("inputPixels", tt_("std::Set", "valueType", _(std.Pixel))));

    // Frame::Frame
    frameStruct.addMethod("constructor")
        .parameters(
            parameter("grid", "Grid"))
        .instructions(
            m_("grid")        = p_("grid"),
            m_("width")       = p_("grid") / "width",
            m_("height")      = p_("grid") / "height",
            m_("shapes")      = new_(tt_("std::List", "valueType", "RenderedShape"), "constructor"),
            m_("shapePixels") = new_(ta_("tableType"), "constructor"),
            m_("shapeMap")    = new_(tt_("std::Map", "keyType", _(std.Number), "valueType", "RenderedShape"), "constructor"),
            m_("inputPixels") = new_(tt_("std::Set", "valueType", _(std.Pixel)), "constructor"),
            self()("processInputPixels"));

    // Frame::processInputPixels
    frameStruct.addMethod("processInputPixels")
        .instructions(
            var_("pixels") = m_("grid") / "pixels",
            if_(missing(*var_("pixels"), "first"))
                .then_(return_()),
            var_("pixel") = *var_("pixels") / "first",
            while_(true_())
                .do_(block(
                    m_("inputPixels")("add")("value", *var_("pixel") / "value"),
                   if_(missing(*var_("pixel"), "next"))
                       .then_(return_()),
                   var_("pixel") = *var_("pixel") / "next")));

#if 0
    // Frame::processInputPixels
    frameStruct.addMethod("processInputPixelsTest")
        .instructions(
            for_("pixel").in(m_("grid") / "pixels")(block(
                m_("inputPixels")("add")("value", *var_("pixel"))
            )));
#endif

    // Frame::process
    frameStruct.addMethod("process")
        .instructions(
            var_("shapeId") = _(_1_),
            while_(not_(m_("inputPixels")("empty")))
                .do_(block(
                    var_("firstPixel")  = m_("inputPixels")("first"),
                    var_("shape")       = new_("RenderedShape", "constructor")("id", *var_("shapeId"))("color", *var_("firstPixel") / "color")("width", m_("width"))("height", m_("height")),
                    var_("shapeId")     = add(*var_("shapeId"), _(_1_)),
                    var_("checkPixels") = new_(tt_("std::Set", "valueType", _(std.Pixel)), "constructor"),
                    var_("checkPixels")("add")("value", *var_("firstPixel")),
                    while_(not_(var_("checkPixels")("empty")))
                        .do_(block(
                            var_("checkPixel") = var_("checkPixels")("first"),
                            self()("processPixel")("shape", *var_("shape"))("checkPixels", *var_("checkPixels"))("checkPixel", *var_("checkPixel")),
                            var_("checkPixels")("remove")("value", *var_("checkPixel")))))),
            var_("y") = _(_0_),
            while_(lessThan(*var_("y"), m_("height")))
                .do_(block(
                    var_("colX") = m_("shapePixels")("getValue")("key", *var_("y")),
                    var_("x")    = _(_0_),
                    while_(lessThan(*var_("x"), m_("width")))
                        .do_(block(
                            var_("shapePixel") = var_("colX")("getValue")("key", *var_("x")),
                            var_("shape")      = *var_("shapePixel") / "shape",
                            var_("pixel")      = *var_("shapePixel") / "pixel",
                            if_(not_(m_("shapeMap")("hasKey")("key", *var_("shape") / "id")))
                                .then_(block(
                                    m_("shapeMap")("add")("key", *var_("shape") / "id")("value", *var_("shape")),
                                    m_("shapes")("add")("value", *var_("shape")))),
                            var_("x") = add(*var_("x"), _(_1_)))),
                    var_("y") = add(*var_("y"), _(_1_)))));

    // Frame::processPixel
    frameStruct.addMethod("processPixel")
        .parameters(
            parameter("shape", __type__("RenderedShape")),
            parameter("checkPixels", tt_("std::Set", "valueType", "Pixel")),
            parameter("checkPixel", __type__("Pixel")))
        .instructions(
            if_(not_(m_("shapePixels")("hasKey")("key", p_("checkPixel") / _(id.coordinates.y))))
                .then_(m_("shapePixels")("add")("key", p_("checkPixel") / _(id.coordinates.y))("value", new_(ta_("tableType"), "constructor"))), // TODO just a TableRow, not a full TableType
            var_("colX") = m_("shapePixels")("getValue")("key", p_("checkPixel") / _(id.coordinates.y)),
            var_("colX")("add")("key", p_("checkPixel") / _(id.coordinates.x))("value", new_("ShapePixel", "constructor")("shape", p_("shape"))("pixel", p_("checkPixel"))),
            m_("inputPixels")("remove")("value", p_("checkPixel")),
            self()("processAdjacentPixel")("direction", _(id.directions.up))("shape", p_("shape"))("checkPixels", p_("checkPixels"))("checkPixel", p_("checkPixel")),
            self()("processAdjacentPixel")("direction", _(id.directions.down))("shape", p_("shape"))("checkPixels", p_("checkPixels"))("checkPixel", p_("checkPixel")),
            self()("processAdjacentPixel")("direction", _(id.directions.left))("shape", p_("shape"))("checkPixels", p_("checkPixels"))("checkPixel", p_("checkPixel")),
            self()("processAdjacentPixel")("direction", _(id.directions.right))("shape", p_("shape"))("checkPixels", p_("checkPixels"))("checkPixel", p_("checkPixel")));

    // Frame::processAdjacentPixel
    frameStruct.addMethod("processAdjacentPixel")
        .parameters(
            parameter("direction", _(std.Direction)),
            parameter("shape", __type__("RenderedShape")),
            parameter("checkPixels", tt_("std::Set", "valueType", _(std.Pixel))),
            parameter("checkPixel", _(std.Pixel)))
        .instructions(
            if_(has(p_("checkPixel"), p_("direction")))
                .then_(block(
                    var_("pixel") = p_("checkPixel") / p_("direction"),
                    if_(m_("shapePixels")("hasKey")("key", *var_("pixel") / _(id.coordinates.y)))
                        .then_(block(
                            var_("colX") = m_("shapePixels")("getValue")("key", *var_("pixel") / _(id.coordinates.y)),
                            if_(var_("colX")("hasKey")("key", *var_("pixel") / _(id.coordinates.x)))
                                .then_(block(
                                    var_("shape") = get(var_("colX")("getValue")("key", *var_("pixel") / _(id.coordinates.x)), "shape"),
                                    if_(same(p_("shape"), *var_("shape")))
                                        .then_(return_()))))),
                    if_(same(*var_("pixel") / "color", p_("shape") / "color"))
                        .then_(p_("checkPixels")("add")("value", *var_("pixel"))))));
}

ArcLib::ArcLib(World& w, Ast::Scope& parentScope, Compiler& compiler) :
    Library(w, parentScope)
{
    Arc& arc = w.arc;
    ArcLibAst stdLibAst(w, parentScope.add<Ast::Scope>("arc"));

    // enums
    compiler.registerBuiltInStruct("arc::Color", arc.Color);
    compiler.registerBuiltInEnumValue("arc::Color::black", arc.Color.black);
    compiler.registerBuiltInEnumValue("arc::Color::blue", arc.Color.blue);
    compiler.registerBuiltInEnumValue("arc::Color::red", arc.Color.red);
    compiler.registerBuiltInEnumValue("arc::Color::green", arc.Color.green);
    compiler.registerBuiltInEnumValue("arc::Color::yellow", arc.Color.yellow);
    compiler.registerBuiltInEnumValue("arc::Color::grey", arc.Color.grey);
    compiler.registerBuiltInEnumValue("arc::Color::fuschia", arc.Color.fuschia);
    compiler.registerBuiltInEnumValue("arc::Color::orange", arc.Color.orange);
    compiler.registerBuiltInEnumValue("arc::Color::teal", arc.Color.teal);
    compiler.registerBuiltInEnumValue("arc::Color::brown", arc.Color.brown);

    compiler.registerBuiltInStruct("arc::Direction", arc.Direction);
    compiler.registerBuiltInEnumValue("arc::Direction::up", arc.Direction.up);
    compiler.registerBuiltInEnumValue("arc::Direction::upRight", arc.Direction.upRight);
    compiler.registerBuiltInEnumValue("arc::Direction::right", arc.Direction.right);
    compiler.registerBuiltInEnumValue("arc::Direction::downRight", arc.Direction.downRight);
    compiler.registerBuiltInEnumValue("arc::Direction::down", arc.Direction.down);
    compiler.registerBuiltInEnumValue("arc::Direction::downLeft", arc.Direction.downLeft);
    compiler.registerBuiltInEnumValue("arc::Direction::left", arc.Direction.left);
    compiler.registerBuiltInEnumValue("arc::Direction::upLeft", arc.Direction.upLeft);

    compiler.registerBuiltInStruct("arc::LineSymmetry", arc.LineSymmetry);
    compiler.registerBuiltInEnumValue("arc::LineSymmetry::horizontal", arc.LineSymmetry.horizontal);
    compiler.registerBuiltInEnumValue("arc::LineSymmetry::vertical", arc.LineSymmetry.vertical);
    compiler.registerBuiltInEnumValue("arc::LineSymmetry::diagonalLowerLeft", arc.LineSymmetry.diagonalLowerLeft);
    compiler.registerBuiltInEnumValue("arc::LineSymmetry::diagonalUpperLeft", arc.LineSymmetry.diagonalUpperLeft);

    compiler.registerBuiltInStruct("arc::RotationDir", arc.RotationDir);
    compiler.registerBuiltInEnumValue("arc::RotationDir::Degree_0", arc.RotationDir.Degree_0);
    compiler.registerBuiltInEnumValue("arc::RotationDir::Degree_45", arc.RotationDir.Degree_45);
    compiler.registerBuiltInEnumValue("arc::RotationDir::Degree_90", arc.RotationDir.Degree_90);
    compiler.registerBuiltInEnumValue("arc::RotationDir::Degree_135", arc.RotationDir.Degree_135);
    compiler.registerBuiltInEnumValue("arc::RotationDir::Degree_180", arc.RotationDir.Degree_180);
    compiler.registerBuiltInEnumValue("arc::RotationDir::Degree_225", arc.RotationDir.Degree_225);
    compiler.registerBuiltInEnumValue("arc::RotationDir::Degree_270", arc.RotationDir.Degree_270);
    compiler.registerBuiltInEnumValue("arc::RotationDir::Degree_315", arc.RotationDir.Degree_315);

    compiler.registerBuiltInStruct("arc::ShapeEdgeKind", arc.ShapeEdgeKind);
    compiler.registerBuiltInEnumValue("arc::ShapeEdgeKind::ExternalEdge", arc.ShapeEdgeKind.ExternalEdge);
    compiler.registerBuiltInEnumValue("arc::ShapeEdgeKind::InternalEdge", arc.ShapeEdgeKind.InternalEdge);

    // structs
    compiler.registerBuiltInStruct("arc::Example", arc.Example);
    compiler.registerBuiltInStruct("arc::Frame", arc.Frame);
    compiler.registerBuiltInStruct("arc::Grid", arc.Grid);
    compiler.registerBuiltInStruct("arc::Pixel", arc.Pixel);
    compiler.registerBuiltInStruct("arc::RenderedShape", arc.RenderedShape);
    compiler.registerBuiltInStruct("arc::Shape", arc.Shape);
    compiler.registerBuiltInStruct("arc::ShapeEdge", arc.ShapeEdge);
    compiler.registerBuiltInStruct("arc::ShapeEdgeJoint", arc.ShapeEdgeJoint);
    compiler.registerBuiltInStruct("arc::ShapeEdgeMirroringCorners", arc.ShapeEdgeMirroringCorners);
    compiler.registerBuiltInStruct("arc::ShapeEdgeNode", arc.ShapeEdgeNode);
    compiler.registerBuiltInStruct("arc::ShapeEdgeRotationCorners", arc.ShapeEdgeRotationCorners);
    compiler.registerBuiltInStruct("arc::ShapeField", arc.ShapeField);
    compiler.registerBuiltInStruct("arc::ShapePixel", arc.ShapePixel);
    compiler.registerBuiltInStruct("arc::ShapePoint", arc.ShapePoint);
    compiler.registerBuiltInStruct("arc::Task", arc.Task);
    compiler.registerBuiltInStruct("arc::Vector", arc.Vector);
    compiler.registerBuiltInStruct("arc::VectorShape", arc.VectorShape);
}

} // namespace cells
} // namespace infocell
