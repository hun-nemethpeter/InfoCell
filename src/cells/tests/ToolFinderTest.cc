#include "cells/tests/CellTestBase.h"
#include "cells/ToolFinder.h"
#include "util/Panic.h"

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "util/Log.h"

using namespace infocell;
using namespace infocell::cells;
using infocell::cells::test::TestBase;

static spdlog::logger* s_logger = nullptr;

namespace infocell {
class ToolFinderTest : public cells::test::CellTest,
                       public cells::ToolFinder
{
public:
    using ToolFinder::id;
    using ToolFinder::std;
    using ToolFinder::w;

    ToolFinderTest() :
        cells::ToolFinder(getWorld())
    {
    }

    void testPatternContinuation(std::vector<CellI*> items)
    {
        auto& toolFinder = w.stdLib().toolFinder();
        TrieMap origResults1(w, w.std.Cell, w.std.Cell);
        TrieMap origResults2(w, w.std.Cell, w.std.Cell);

        TrieMap* results1Ptr = &origResults1;
        TrieMap* results2Ptr = &origResults2;

        for (int i = 0; i < items.size() - 1; ++i) {
            CellI& conversionTools = toolFinder.findConversionTools(*items[i], *items[i + 1]);
            TrieMap& results1      = *results1Ptr;
            TrieMap& results2      = *results2Ptr;
            for (CellI& conversionToolKV : conversionTools) {
                CellI& serializedConversionTool = conversionToolKV[id.key];
                if (i == 0) {
                    results2.add(serializedConversionTool, conversionToolKV[id.value]);
                } else {
                    if (results1.hasKey(serializedConversionTool)) {
                        results2.add(serializedConversionTool, conversionToolKV[id.value]);
                    }
                }
            }
            results1.clear();
            results1Ptr = results2Ptr;
            results2Ptr = &results1;
        }
        for (CellI& conversionToolKV : origResults1[id.list]) {
            auto& conversionToolFn = static_cast<Object&>(conversionToolKV[id.value]);
//            printAs.value(conversionToolFn);
            conversionToolFn.createSelfStack({ "from", *items[items.size() - 1] });
            conversionToolFn();
            CellI& result = conversionToolFn[id.value];
            std::cout << "Pattern: ";
            for (auto* cell : items) {
                std::cout << cell->label() << " ";
            }
            std::cout << " ... ";
            std::cout << result.label() << " with tool ";
            for (auto& resultId : conversionToolKV[id.key]) {
                std::cout << resultId.label() << " ";
            }
            std::cout << std::endl;
        }
    }

};

class NewToolFinderTest : public cells::test::CellTest,
                          public cells::ToolFinder
{
public:
    using ToolFinder::w;
    using ToolFinder::id;
    using ToolFinder::std;

    NewToolFinderTest() :
        cells::ToolFinder(getWorld())
    {
    }

    class BuildContext
    {
    public:
        bool stepCommand()
        {
            auto& id = m_cell.w.id;
            if (!m_buildCommandNodePtr) {
                m_buildCommandNodePtr = &m_builder[id.first];
            } else {
                m_buildCommandNodePtr = m_buildCommandNodePtr->getNextOrNullptr();
            }
            if (!m_buildCommandNodePtr) {
                return false;
            }
            if (&(*m_buildCommandNodePtr)[id.value] != &id.op) {
                panic("Not a valid build command!");
            }
            m_buildCommandNodePtr = m_buildCommandNodePtr->getNextOrNullptr();
            if (!m_buildCommandNodePtr) {
                panic("No more build command!");
            }
            return true;
        }

        CellI& currentCommand()
        {
            auto& id = m_cell.w.id;
            return (*m_buildCommandNodePtr)[id.value];
        }

        CellI& stepArgument()
        {
            auto& id              = m_cell.w.id;
            m_buildCommandNodePtr = m_buildCommandNodePtr->getNextOrNullptr();
            if (!m_buildCommandNodePtr) {
                panic("No more build command!");
            }
            return (*m_buildCommandNodePtr)[id.value];
        }

        CellI& currentValue()
        {
            return m_cell[m_memberName];
        }

        CellI& m_cell;
        CellI& m_memberName;
        CellI& m_builder;
        CellI* m_buildCommandNodePtr;
        CellI& m_description;
    };

    List& createBuilder(CellI& tool, Map& memberIds, bool hasReturnInEffect) override
    {
        List& builder      = *new List(w, std.Cell, fmt::format("builder for {}", tool.label()));
        bool primitiveTool = tool[id.ast].has(id.primitiveTool);

        if (hasReturnInEffect) {
            builder.add(id.op);
            builder.add(id.new_);
            builder.add(std.op.Equal);

            builder.add(id.op);
            builder.add(id.push);
            builder.add(id.self);
        }
        builder.add(id.op);
        builder.add(id.new_);
        if (primitiveTool) {
            builder.add(tool[id.ast][id.primitiveTool]);
        } else {
            builder.add(std.op.Call);

            builder.add(id.op);
            builder.add(id.member);
            builder.add(id.method);
            builder.add(tool);
        }

#if 1
        for (CellI& parameterKV : tool[id.parameters].__type__()[id.members]) {
            CellI& key = parameterKV[id.key];
            builder.add(id.op);
            builder.add(id.input);
            builder.add(key);
            builder.add(id.path);
            builder.add(memberIds.getValue(key));
        }
#endif
        if (hasReturnInEffect) {
            builder.add(id.op);
            builder.add(id.pop);

            builder.add(id.op);
            builder.add(id.input);
            builder.add(id.other);
            builder.add(id.path);
            builder.add(memberIds.getValue(id.return_));
        }

        return builder;
    }

    class Builder : public NodeBase
    {
    public:
        Builder(World& w, BuildContext& buildContext) : NodeBase(w)
        {
            m_toCreate.push_back(buildContext);
        }

        void build()
        {
            while (!m_toCreate.empty()) {
                BuildContext& buildContext  = m_toCreate.back();
                if (!buildContext.stepCommand()) {
                    break;
                }
                CellI& buildCommandOp = buildContext.currentCommand();
                if (&buildCommandOp == &id.new_) {
                    handleCommandNew(buildContext);
                } else if (&buildCommandOp == &id.push) {
                    handleCommandPush(buildContext);
                } else if (&buildCommandOp == &id.pop) {
                    handleCommandPop();
                } else if (&buildCommandOp == &id.member) {
                    handleCommandMember(buildContext);
                }
            }
        }

        void handleCommandNew(BuildContext& buildContext)
        {
            CellI& type   = buildContext.stepArgument();
            CellI& newObj = *new Object(w, type, fmt::format("built from {}", buildContext.m_builder.label()));

            buildContext.m_cell.set(buildContext.m_memberName, newObj);
        }

        void handleCommandPush(BuildContext& buildContext)
        {
            CellI& cell                 = buildContext.m_cell;
            CellI& memberName           = buildContext.m_memberName;
            CellI& newMemberName        = buildContext.stepArgument();

            m_toCreate.push_back({ cell[memberName], newMemberName, buildContext.m_builder, buildContext.m_buildCommandNodePtr, buildContext.m_description });
        }

        void handleCommandPop()
        {
            CellI& previousBuilder     = m_toCreate.back().m_builder;
            CellI* buildCommandNodePtr = m_toCreate.back().m_buildCommandNodePtr;
            m_toCreate.pop_back();
            CellI& currentBuilder = m_toCreate.back().m_builder;
            if (&previousBuilder == &currentBuilder) {
                m_toCreate.back().m_buildCommandNodePtr = buildCommandNodePtr;
            }
        }

        void handleCommandMember(BuildContext& buildContext)
        {
            auto& id               = buildContext.m_cell.w.id;
            CellI& memberName      = buildContext.stepArgument();
            CellI& memberValueType = buildContext.stepArgument();

            if (&memberValueType == &id.path) {
                CellI& valueCell = buildContext.stepArgument();
                CellI* valuePtr  = &buildContext.m_description;
                for (CellI& pathItem : valueCell) {
                    CellI& currentValue = *valuePtr;
                    CellI* keyPtr       = &pathItem[id.key];
                    if (currentValue.__type__().has(id.primitiveTool)) {
                        Map& membersMapping = static_cast<Map&>(currentValue[id.ast][id.memberMapping]);
                        keyPtr = &membersMapping.getValue(*keyPtr);
                    }
                    valuePtr = &currentValue[*keyPtr];
                }
                buildContext.currentValue().set(memberName, *valuePtr);
            } else {
                panic("Invalid member value type!");
            }
        }

        std::deque<BuildContext> m_toCreate;
    };

    virtual void buildTool(const BuildToolInfo& buildToolInfo) override
    {
        BuildContext buildContext { buildToolInfo.m_outCell, buildToolInfo.m_outKey, buildToolInfo.m_builder, nullptr, buildToolInfo.m_matchedEffect };

        Builder builder(w, buildContext);
        builder.build();

        std::cout << "";
    }
};

} // namespace infocell


TEST_F(ToolFinderTest, FindConversionToolsFrom_2_To_4_)
{
    CellI& from = _2_;
    CellI& to   = _4_;

    CellI& conversionTools = w.stdLib().toolFinder().findConversionTools(from, to);
    EXPECT_EQ(&conversionTools[id.size], &_7_);
    for (CellI& conversionToolKV : conversionTools)
    {
        Object& conversionToolFn = static_cast<Object&>(conversionToolKV[id.value]);
        conversionToolFn.createSelfStack({ "from", from });
        conversionToolFn();
        CellI& result = conversionToolFn[id.value];
        if (&result != &to) {
            std::cerr << "ERROR" << std::endl;
            printAs.value(conversionToolFn);
        }
        EXPECT_EQ(&result, &to);
    }
}

TEST_F(ToolFinderTest, FindConversionToolsFrom_false_To_true)
{
    CellI& from = false_;
    CellI& to   = true_;

    CellI& conversionTools = w.stdLib().toolFinder().findConversionTools(from, to);
    EXPECT_EQ(&conversionTools[id.size], &_3_);
    for (CellI& conversionToolKV : conversionTools) {
        Object& conversionToolFn = static_cast<Object&>(conversionToolKV[id.value]);
        conversionToolFn.createSelfStack({ "from", from });
        conversionToolFn();
        CellI& result = conversionToolFn[id.value];
        if (&result != &to) {
            printAs.value(conversionToolFn);
        }
        EXPECT_EQ(&result, &to);
    }
}

TEST_F(ToolFinderTest, FindConversionToolsFrom_true_To_false)
{
    CellI& from = true_;
    CellI& to   = false_;

    CellI& conversionTools = w.stdLib().toolFinder().findConversionTools(from, to);
    EXPECT_EQ(&conversionTools[id.size], &_3_);
    for (CellI& conversionToolKV : conversionTools) {
        Object& conversionToolFn = static_cast<Object&>(conversionToolKV[id.value]);
        conversionToolFn.createSelfStack({ "from", from });
        conversionToolFn();
        CellI& result = conversionToolFn[id.value];
        if (&result != &to) {
            printAs.value(conversionToolFn);
        }
        EXPECT_EQ(&result, &to);
    }
}

TEST_F(ToolFinderTest, FindPatternContinuations)
{
    spdlog::get("toolFinder")->set_level(spdlog::level::off);
    spdlog::get("toolFinderConversion")->set_level(spdlog::level::off);
    spdlog::get("toolFinderLookup")->set_level(spdlog::level::off);
    spdlog::get("toolFinderExplore")->set_level(spdlog::level::off);
    spdlog::get("toolFinderGraphviz")->set_level(spdlog::level::off);

    testPatternContinuation({ &_1_, &_2_, &_3_ });
    testPatternContinuation({ &_1_, &_2_, &_4_ });
    testPatternContinuation({ &_8_, &_4_, &_2_ });
    testPatternContinuation({ &_1_, &_3_, &_9_ });
    testPatternContinuation({ &_1_, &_3_, &_5_ });
    testPatternContinuation({ &_4_, &_4_, &_4_ });
    std::cout << "";
}


TEST_F(NewToolFinderTest, DISABLED_NewBuilderTestAdd)
{
    // 1 + 2 = 3 => 3 - 2 = 1
    CellI& addFn = w.std.Number[id.methods][id.index]["add"][id.value][id.value];
    add(addFn);
    std::cout << printConsequenceNodesAsGrapviz() << std::endl;

    // prompt 2 + X == 4
    CellI& subFn = w.std.Number[id.methods][id.index]["subtract"][id.value][id.value];
    Object& x    = *new Object(w, std.Number, "X");
    auto& tool   = *new Object(w, subFn[id.ast][id.primitiveTool]);
    tool.set(id.lhs, op.const_(2));
    tool.set(id.rhs, op.unknown_(x));

    auto& opEqual = w.op.equal(tool, op.const_(4));
    spdlog::get("toolFinder")->set_level(spdlog::level::trace);
    spdlog::get("toolFinderConversion")->set_level(spdlog::level::trace);
    spdlog::get("toolFinderLookup")->set_level(spdlog::level::trace);
    spdlog::get("toolFinderExplore")->set_level(spdlog::level::trace);
    spdlog::get("toolFinderGraphviz")->set_level(spdlog::level::trace);

    auto buildersPtr = findBuildersForDescription(opEqual, DescriptionKind::consequence);
    if (!buildersPtr) {
        return;
    }
    for (auto& builder : *buildersPtr) {
        TRACE(toolFinder, "build with {}", builder.label());
        Object retVal(w, std.ast.ConstVar);
        buildTool({ retVal, id.value, opEqual, builder });
        std::cout << "";
    }
}

TEST_F(NewToolFinderTest, DISABLED_NewBuilderTestMapAdd)
{
    Map memberIds(w, w.std.Cell, w.std.Cell);
    // fn std::Map<keyType=Cell, valueType=Struct>::add(p_key: Cell, p_value: Struct)
    CellI& MapStruct = getStruct(w.templateId("std::Map", id.keyType, std.Cell, id.valueType, std.Struct));
    CellI& addFn     = MapStruct[id.methods][id.index]["add"][id.value][id.value];
    CellI& tool      = addFn[id.description][id.consequences][id.first][id.value];
    add(addFn);
    EXPECT_EQ(true, true);
}


#ifdef _WIN32
#include "Windows.h"
#endif

int main(int argc, char** argv)
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    TestBase::createWorld([]() {
        World::Logger::createLogger("edge");
        World::Logger::createLogger("shapeCorners");
        World::Logger::createLogger("shapeRelations");
        World::Logger::createLogger("shapeIdGrid");
        World::Logger::createLogger("grid");

        spdlog::get("cells")->set_level(spdlog::level::trace);
        spdlog::get("compileStruct")->set_level(spdlog::level::off);
        spdlog::get("compiledSymbols")->set_level(spdlog::level::off);
        spdlog::get("toolFinder")->set_level(spdlog::level::off);
        spdlog::get("toolFinderConversion")->set_level(spdlog::level::debug);
        spdlog::get("toolFinderLookup")->set_level(spdlog::level::off);
        spdlog::get("toolFinderExplore")->set_level(spdlog::level::debug);
        spdlog::get("toolFinderGraphviz")->set_level(spdlog::level::off);
        spdlog::get("edge")->set_level(spdlog::level::off);
        spdlog::get("shapeCorners")->set_level(spdlog::level::off);
        spdlog::get("shapeIdGrid")->set_level(spdlog::level::off);
        spdlog::get("grid")->set_level(spdlog::level::trace);
        spdlog::get("shapeRelations")->set_level(spdlog::level::off);
    });
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    DEBUG(cells, "Constructed: {}, destructed: {}, live: {}", CellI::s_constructed, CellI::s_destructed, CellI::s_constructed - CellI::s_destructed);
    TestBase::freeWorld();

    return ret;
}
