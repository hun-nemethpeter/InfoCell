#include "ArcLib.h"

namespace infocell {
namespace cells {

class ArcLibAst : public AstHelper
{
public:
    ArcLibAst(World& w, Ast::Scope& scope);
};

ArcLibAst::ArcLibAst(World& w, Ast::Scope& scope) :
    AstHelper(w)
{
    auto& exampleStruct
        = scope.add<Struct>("Example")
              .members(
                  member("input", _(std.Grid)),
                  member("output", _(std.Grid)));

    auto& taskStruct
        = scope.add<Struct>("Task")
              .members(
                  member("examples", tt_("std::List", "valueType", "Example")),
                  member("tests", tt_("std::List", "valueType", "Example")),
                  member("solution", _(std.Grid)));

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

    scope.add<Enum>("Directions")
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

    auto& colorStruct
        = scope.add<Struct>("Color")
              .members(
                  member("red", _(std.Number)),
                  member("green", _(std.Number)),
                  member("blue", _(std.Number)));

    // struct Pixel
    auto& pixelStruct
        = scope.add<Struct>("Pixel")
              .members(
                  member("x", _(std.Number)),
                  member("y", _(std.Number)));

    pixelStruct.addMethod("constructor")
        .parameters(
            parameter("x", _(std.Number)),
            parameter("y", _(std.Number)))
        .instructions(
            m_("x") = p_("x"),
            m_("y") = p_("y"));

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
            parameter("rotationDir", struct_("RotationDir")))
        .returnType(struct_("Vector"))
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
                  member("color", _(std.Color)),
                  member("vectors", tt_("std::List", "valueType", "Vector")),
                  member("firstPixel", "Pixel"));

    vectorShapeStruct.addMethod("constructor")
        .parameters(
            parameter("color", _(std.Color)))
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
            var_("pixel")        = _(ids.emptyObject),
            if_(has(p_("pixels"), "first"))
                .then_(var_("pixel") = p_("pixels") / "first"),
            while_(notSame(*var_("pixel"), _(ids.emptyObject)))
                .do_(block(
                    if_(same(*var_("isFirstPixel"), true_()))
                        .then_(block(
                            var_("isFirstPixel") = false_(),
                            if_(has(*var_("pixel"), "next"))
                                .then_(var_("pixel") = *var_("pixel") / "next")
                                .else_(var_("pixel") = _(ids.emptyObject)),
                            continue_())),
                    var_("vector") = new_("Vector", "constructor")("x", subtract(*var_("pixel") / "value" / "x", *var_("prevPixel") / "x"))("y", subtract(*var_("pixel") / "value" / "y", *var_("prevPixel") / "y")),
                    m_("vectors")("add")("value", *var_("vector")),
                    var_("prevPixel") = *var_("pixel") / "value",
                    if_(has(*var_("pixel"), "next"))
                        .then_(var_("pixel") = *var_("pixel") / "next")
                        .else_(var_("pixel") = _(ids.emptyObject)))));

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
            parameter("rotationDir", struct_("RotationDir")))
        .returnType(struct_("VectorShape"))
        .instructions(
            var_("ret")            = new_(struct_("VectorShape")),
            var_("rotatedVectors") = new_(tt_("std::List", "valueType", "Vector"), "constructor"),
            set(*var_("ret"), "color", m_("color")),
            set(*var_("ret"), "firstPixel", m_("firstPixel")),
            set(*var_("ret"), "vectors", *var_("rotatedVectors")),
            var_("vector") = _(ids.emptyObject),
            if_(has(m_("vectors"), "first"))
                .then_(var_("vector") = m_("vectors") / "first"),
            while_(notSame(*var_("vector"), _(ids.emptyObject)))
                .do_(block(
                    var_("newVector") = ((*var_("vector") / "value")("rotate")("rotationDir", p_("rotationDir"))),
                    var_("rotatedVectors")("add")("value", *var_("newVector")),
                    if_(has(*var_("vector"), "next"))
                        .then_(var_("vector") = *var_("vector") / "next")
                        .else_(var_("vector") = _(ids.emptyObject)))),
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
                  member("shape", "Shape"),
                  member("kind", "ShapeEdgeKind"),
                  member("fromExternalX", _(std.Number)),
                  member("fromExternalY", _(std.Number)),
                  member("shapes", tt_("std::Set", "valueType", "Shape")),
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
                  member("direction", "Directions"),
                  member("externalShape", "Shape"),
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
                  member("shape", "Shape"),
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
            parameter("shape", struct_("Shape")),
            parameter("pixel", _(std.Pixel)))
        .instructions(
            m_("shape") = p_("shape"),
            m_("pixel") = p_("pixel"));

    // struct Shape
    auto& shapeStruct
        = scope.add<Struct>("Shape")
              .subTypes(
                  parameter("InternalEdgeLookup", tt_("std::Map", "keyType", _(std.Number), "valueType", tt_("std::Map", "keyType", _(std.Number), "valueType", "ShapeEdge"))))
              .members(
                  member("id", _(std.Number)),
                  member("color", "Color"),
                  member("width", _(std.Number)),
                  member("height", _(std.Number)),
                  member("lastEdgeId", _(std.Number)),
                  member("edges", tt_("std::Map", "keyType", _(std.Number), "valueType", "ShapeEdge")),
                  member("internalEdges", st_("InternalEdgeLookup")),
                  member("pixels", tt_("std::List", "valueType", "Pixel")),
                  member("shapePixels", tt_("std::List", "valueType", "ShapePixel")),
                  member("shapePoints", tt_("std::List", "valueType", "ShapePoint")));

    /*
    Shape(int id, input::Color color, int width, int height) :
        m_id(id), m_color(color), m_width(width), m_height(height) { }
    */
    shapeStruct.addMethod("constructor")
        .parameters(
            parameter("id", _(std.Number)),
            parameter("color", struct_("Color")),
            parameter("width", _(std.Number)),
            parameter("height", _(std.Number)))
        .instructions(
            m_("id")           = p_("id"),
            m_("color")        = p_("color"),
            m_("width")        = p_("width"),
            m_("height")       = p_("height"),
            m_("lastEdgeId")   = _(_0_),
            m_("edges")        = new_(tt_("std::Map", "keyType", _(std.Number), "valueType", "ShapeEdge"), "constructor"));

    shapeStruct.addMethod("toVectorShape")
        .returnType(struct_("VectorShape"))
        .instructions(
            var_("ret") = new_("VectorShape", "constructor")("color", m_("color")),
            var_("ret")("fromPixels")("pixels", m_("pixels")),
            return_(*var_("ret")));

    // struct Frame
    auto& frameStruct
        = scope.add<Struct>("Frame")
              .subTypes(
                  parameter("tableType", tt_("std::Map", "keyType", _(std.Number), "valueType", tt_("std::Map", "keyType", _(std.Number), "valueType", "Shape"))))
              .members(
                  member("width", _(std.Number)),
                  member("height", _(std.Number)),
                  member("grid", _(std.Grid)),
                  member("upLeftPoint", "ShapePoint"),
                  member("upRightPoint", "ShapePoint"),
                  member("downLeftPoint", "ShapePoint"),
                  member("downRightPoint", "ShapePoint"),
                  member("shapes", tt_("std::List", "valueType", "Shape")),
                  member("shapeMap", tt_("std::Map", "keyType", _(std.Number), "valueType", "Shape")),
                  member("shapePixels", st_("tableType")),
                  member("inputPixels", tt_("std::Set", "valueType", _(std.Pixel))));

    // Frame::Frame
    frameStruct.addMethod("constructor")
        .parameters(
            parameter("grid", _(std.Grid)))
        .instructions(
            m_("grid")        = p_("grid"),
            m_("width")       = p_("grid") / "width",
            m_("height")      = p_("grid") / "height",
            m_("shapes")      = new_(tt_("std::List", "valueType", "Shape"), "constructor"),
            m_("shapePixels") = new_(st_("tableType"), "constructor"),
            m_("shapeMap")    = new_(tt_("std::Map", "keyType", _(std.Number), "valueType", "Shape"), "constructor"),
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
                    var_("shape")       = new_("Shape", "constructor")("id", *var_("shapeId"))("color", *var_("firstPixel") / "color")("width", m_("width"))("height", m_("height")),
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
            parameter("shape", struct_("Shape")),
            parameter("checkPixels", tt_("std::Set", "valueType", "Pixel")),
            parameter("checkPixel", struct_("Pixel")))
        .instructions(
            if_(not_(m_("shapePixels")("hasKey")("key", p_("checkPixel") / _(coordinates.y))))
                .then_(m_("shapePixels")("add")("key", p_("checkPixel") / _(coordinates.y))("value", new_(st_("tableType"), "constructor"))), // TODO just a TableRow, not a full TableType
            var_("colX") = m_("shapePixels")("getValue")("key", p_("checkPixel") / _(coordinates.y)),
            var_("colX")("add")("key", p_("checkPixel") / _(coordinates.x))("value", new_("ShapePixel", "constructor")("shape", p_("shape"))("pixel", p_("checkPixel"))),
            m_("inputPixels")("remove")("value", p_("checkPixel")),
            self()("processAdjacentPixel")("direction", _(directions.up))("shape", p_("shape"))("checkPixels", p_("checkPixels"))("checkPixel", p_("checkPixel")),
            self()("processAdjacentPixel")("direction", _(directions.down))("shape", p_("shape"))("checkPixels", p_("checkPixels"))("checkPixel", p_("checkPixel")),
            self()("processAdjacentPixel")("direction", _(directions.left))("shape", p_("shape"))("checkPixels", p_("checkPixels"))("checkPixel", p_("checkPixel")),
            self()("processAdjacentPixel")("direction", _(directions.right))("shape", p_("shape"))("checkPixels", p_("checkPixels"))("checkPixel", p_("checkPixel")));

    // Frame::processAdjacentPixel
    frameStruct.addMethod("processAdjacentPixel")
        .parameters(
            parameter("direction", _(std.Directions)),
            parameter("shape", struct_("Shape")),
            parameter("checkPixels", tt_("std::Set", "valueType", _(std.Pixel))),
            parameter("checkPixel", _(std.Pixel)))
        .instructions(
            if_(has(p_("checkPixel"), p_("direction")))
                .then_(block(
                    var_("pixel") = p_("checkPixel") / p_("direction"),
                    if_(m_("shapePixels")("hasKey")("key", *var_("pixel") / _(coordinates.y)))
                        .then_(block(
                            var_("colX") = m_("shapePixels")("getValue")("key", *var_("pixel") / _(coordinates.y)),
                            if_(var_("colX")("hasKey")("key", *var_("pixel") / _(coordinates.x)))
                                .then_(block(
                                    var_("shape") = get(var_("colX")("getValue")("key", *var_("pixel") / _(coordinates.x)), "shape"),
                                    if_(same(p_("shape"), *var_("shape")))
                                        .then_(return_()))))),
                    if_(same(*var_("pixel") / "color", p_("shape") / "color"))
                        .then_(p_("checkPixels")("add")("value", *var_("pixel"))))));
}

ArcLib::ArcLib(World& w, Ast::Scope& parentScope) :
    Library(w, parentScope)
{
    ArcLibAst stdLibAst(w, parentScope.add<Ast::Scope>("arc"));
}

} // namespace cells
} // namespace infocell
