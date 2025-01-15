#include <gtest/gtest.h>

#include "Cells.h"
#include "SVGPrinter.h"
#include "SVGStructPrinter.h"
#include "StructPrinter.h"
#include "ValuePrinter.h"

namespace infocell {
namespace cells {
namespace test {

class PrintAs
{
public:
    PrintAs(const std::string& postfix);
    ~PrintAs();

    void value(CellI& cell, const std::string& label = "");
    void cell(CellI& cell, const std::string& label = "");
    void svg(CellI& cell, const std::string& caseName = "Case");
    void svgStruct(CellI& cell, const std::string& caseName = "Case");

    std::string m_postfix;
    svg::Printer m_svgPrinter;
    svg::StructPrinter m_svgStructPrinter;
};

class CellTest : public ::testing::Test
{
public:
    static void freeKb();

protected:
    CellTest(std::function<void()> loggerLevelInit = []() {});

    void printMethodInType(CellI& type, const std::string& method);

    CellI& getVariable(const std::string& name);
    CellI& getStruct(const std::string& name);
    CellI& getStruct(CellI& id);
    CellI& toCellNumber(int number);

    static std::unique_ptr<brain::Brain> m_kb;
    brain::Brain& kb;
    brain::ID& ids = kb.ids;
    PrintAs printAs;
    CellI& _0_    = kb._0_;
    CellI& _1_    = kb._1_;
    CellI& _2_    = kb._2_;
    CellI& _3_    = kb._3_;
    CellI& _4_    = kb._4_;
    CellI& _5_    = kb._5_;
    CellI& _6_    = kb._6_;
    CellI& _7_    = kb._7_;
    CellI& _8_    = kb._8_;
    CellI& _9_    = kb._9_;
    CellI& true_  = kb.boolean.true_;
    CellI& false_ = kb.boolean.false_;
};

} // namespace test
} // namespace cells
} // namespace infocell