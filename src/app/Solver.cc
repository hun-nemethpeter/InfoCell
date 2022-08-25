#include "Solver.h"
#include <array>
#include <set>

namespace synth {

Logger* loggerPtr = nullptr;

const std::array<cells::Color, 10> cellColors = {
    cells::Color(0x00, 0x00, 0x00), /* black */
    cells::Color(0x00, 0x74, 0xD9), /* blue */
    cells::Color(0xFF, 0x41, 0x36), /* red */
    cells::Color(0x2E, 0xCC, 0x40), /* green */
    cells::Color(0xFF, 0xDC, 0x00), /* yellow */
    cells::Color(0xAA, 0xAA, 0xAA), /* grey */
    cells::Color(0xF0, 0x12, 0xBE), /* fuschia */
    cells::Color(0xFF, 0x85, 0x1B), /* orange */
    cells::Color(0x7F, 0xDB, 0xFF), /* teal */
    cells::Color(0x87, 0x0C, 0x25)  /* brown */
};

enum class Direction
{
    LeftToRight,
    RightToLeft
};

Solver::Solver(Logger& logger, const ArcTask& arcTask) :
    logger(logger), m_arcTask(arcTask)
{
    solve();
}

class Vector
{
public:
    int x;
    int y;
};

class Patch : public std::enable_shared_from_this<Patch>
{
public:
    Patch(cells::Color color) :
        m_color(color) { }

    const cells::Color& color() const
    {
        return m_color;
    }

    void addPixelCoordinate(int x, int y, Direction scanDir)
    {
        if (vectors.empty()) {
            m_startX  = x;
            m_startY  = y;
            m_leftX   = x;
            m_rightX  = x;
            m_topY    = y;
            m_bottomY = y;
            vectors.push_back({ x, y });
        } else {
            vectors.push_back({ x - m_lastX, y - m_lastY });
            if (x < m_leftX)
                m_leftX = x;
            if (x > m_rightX)
                m_rightX = x;
            if (y > m_bottomY)
                m_bottomY = y;
        }
        m_lastX = x;
        m_lastY = y;

        if (scanDir == Direction::LeftToRight)
          m_subscribeCb(shared_from_this(), x + 1, y);
        else
          m_subscribeCb(shared_from_this(), x - 1, y);

        m_subscribeCb(shared_from_this(), x + 1, y + 1);
        m_subscribeCb(shared_from_this(), x, y + 1);
        m_subscribeCb(shared_from_this(), x - 1, y + 1);
    }

    void merge(const Patch& other)
    {
        vectors.push_back({ other.m_startX - m_startX, other.m_startY - m_startY });
        for (const auto& vector : other.vectors) {
            vectors.push_back(vector);
        }
    }

    void setSubriberCb(std::function<void(std::shared_ptr<Patch>, int x, int y)> subscribeCb)
    {
        m_subscribeCb = subscribeCb;
    }

    bool operator==(const Patch& rhs) const
    {
        return rhs.color() == color() && rhs.m_startX == m_startX && rhs.m_startY == m_startY;
    }

    std::function<void(std::shared_ptr<Patch>, int x, int y)> m_subscribeCb;

    int m_startX = 0;
    int m_startY = 0;
    int m_lastX  = 0;
    int m_lastY  = 0;

    int m_leftX   = 0;
    int m_rightX  = 0;
    int m_topY    = 0;
    int m_bottomY = 0;

    cells::Color m_color;
    std::vector<cells::Vector> vectors;
};

struct PatchSlot
{
    void registerCandidate(std::shared_ptr<Patch> patch)
    {
        std::set<std::shared_ptr<Patch>>& patches = m_candidates[patch->color()];
        patches.insert(patch);
    }

    std::shared_ptr<Patch> getCandidate(const cells::Color& color)
    {
        auto findIt = m_candidates.find(color);

        // no patch
        if (findIt == m_candidates.end()) {
            return std::shared_ptr<Patch>();
        }

        std::set<std::shared_ptr<Patch>>& patches = m_candidates[color];

        // one patch
        auto firstPatchIt = patches.begin();
        if (patches.size() == 1)
            return *firstPatchIt;

        std::shared_ptr<Patch> returnPatch = *firstPatchIt;

        // multiple patch
        for (auto i = ++firstPatchIt; i != patches.end(); ++i) {
            std::shared_ptr<Patch> candidatePatch = *i;
            if (candidatePatch->m_startY < returnPatch->m_startY || (candidatePatch->m_startY == returnPatch->m_startY && candidatePatch->m_startX < returnPatch->m_startX)) {
                auto tmp       = returnPatch;
                returnPatch    = candidatePatch;
                candidatePatch = tmp;
            }
//            loggerPtr->log(DEBUG) << " - patch (" << candidatePatch.get() << ") merged to " << returnPatch.get();
            returnPatch->merge(*candidatePatch);
            m_deletePatchCb(candidatePatch);
        }

        return returnPatch;
    }

    void setDeletePatchCb(std::function<void(std::shared_ptr<Patch>)> deletePatchCb)
    {
        m_deletePatchCb = deletePatchCb;
    }

    std::function<void(std::shared_ptr<Patch>)> m_deletePatchCb;

    std::map<cells::Color, std::set<std::shared_ptr<Patch>>> m_candidates;
};

std::ostream& operator<<(std::ostream& os, const cells::Color& color)
{
    os << "[" << color.red.value << "," << color.green.value << "," << color.blue.value << "]";
    return os;
}

class PatchBoard
{
public:
    PatchBoard(int width, int height) :
        m_width(width), m_height(height)
    {
        const int boardSize = m_height * m_width;
        m_patchSlots.resize(boardSize);
        for (auto& patchSlot : m_patchSlots)
            patchSlot.setDeletePatchCb([this](std::shared_ptr<Patch> patch) { m_patches.erase(patch); });
    }

    int width() const
    {
        return m_width;
    }

    int height() const
    {
        return m_height;
    }

    void processPixel(int x, int y, const cells::Color& color, Direction scanDir)
    {
        PatchSlot& patchSlot             = getPatchSlot(x, y);
        std::shared_ptr<Patch> candidate = patchSlot.getCandidate(color);

        if (candidate) {
//            loggerPtr->log(DEBUG) << " - pixel[" << x << ", " << y << "] " << color << " - patch found " << "(" << candidate.get() << ")";
        } else {
            candidate = std::make_shared<Patch>(color);
//            loggerPtr->log(DEBUG) << " - pixel[" << x << ", " << y << "] " << color << " - patch created " << "(" << candidate.get() << ")";
            candidate->setSubriberCb([this](std::shared_ptr<Patch> patch, int x, int y) { subscribePatchForPixel(patch, x, y); });
            m_patches.insert(candidate);
        }
        candidate->addPixelCoordinate(x, y, scanDir);
    }

    int patchesCount() const
    {
        return m_patches.size();
    }

protected:
    void subscribePatchForPixel(std::shared_ptr<Patch> patch, int x, int y)
    {
        if (!isInRange(x, y))
            return;
        PatchSlot& patchSlot = getPatchSlot(x, y);
        patchSlot.registerCandidate(patch);
    }

    PatchSlot& getPatchSlot(int x, int y)
    {
        return m_patchSlots[currentIndex(x, y)];
    }

    int currentIndex(int x, int y) const
    {
        return y * m_width + x;
    }

    bool isInRange(int x, int y) const
    {
        if (y < 0 || x < 0 || x > m_width - 1 || y > m_height - 1) {
            return false;
        }

        return true;
    }

    int m_width;
    int m_height;
    std::vector<PatchSlot> m_patchSlots;
    std::set<std::shared_ptr<Patch>> m_patches;
};

void Solver::solve()
{
    loggerPtr = &logger;
    logger.clearLogFile();
    logger.log(INFO) << "There are " << m_arcTask.m_demonstrations.size() << " demo tasks";
    unsigned int i = 1;
    for (const auto& arcDemo : m_arcTask.m_demonstrations) {
        const cells::Sensor& m_input  = arcDemo.m_input;
        const cells::Sensor& m_output = arcDemo.m_output;
//        logger.log(INFO) << " (" << i << ") mapping[" << m_input.m_width << ", " << m_input.m_height << "] to[" << m_output.m_width << ", " << m_output.m_height << "] ";
        solveOne(m_input);
        solveOne(m_output);
        i++;
    }
//    solveOne(m_arcTask.m_testInput);

    const cells::Sensor& m_input = m_arcTask.m_testInput;
    logger.log(INFO) << "Mapping input[" << m_input.m_width << ", " << m_input.m_height << "] to ... ?";
}

void Solver::solveOne(const cells::Sensor& sensor)
{
    PatchBoard patchBoard(sensor.width(), sensor.height());
    Direction scanDir = Direction::LeftToRight;
    for (int y = 0; y < sensor.height(); ++y) {
        switch (scanDir) {
        case Direction::LeftToRight:
            for (int x = 0; x < sensor.width(); ++x) {
                const cells::Pixel& pixel = sensor.getPixel(x, y);
//                logger.log(DEBUG) << "Processing pixel[" << x << ", " << y << "]" << pixel.color;
                patchBoard.processPixel(x, y, pixel.color, scanDir);
            }
            break;
        case Direction::RightToLeft:
            for (int x = sensor.width() - 1; x >= 0; --x) {
                const cells::Pixel& pixel = sensor.getPixel(x, y);
//                logger.log(DEBUG) << "Processing pixel[" << x << ", " << y << "]" << pixel.color;
                patchBoard.processPixel(x, y, pixel.color, scanDir);
            }
            break;
        }
        scanDir = scanDir == Direction::LeftToRight ? Direction::RightToLeft : Direction::LeftToRight;
    }
    logger.log(DEBUG) << "Number of patches found: " << patchBoard.patchesCount();
}

/*

A program célja, hogy két pixelmátrixból megtalálja a transzformációs algoritmust.
A feladatok során mindig található egy transzformációs algoritmus, ami egyik mátrixból előállítja a másikat.

Első lépés, hogy a pixel mátrixban azonosítsuk az azonos színű alakzatokat. Az alakzat akár 1 pixeles is lehet.
Az egyik kitüntetett alakzat a "háttér" alakzat, ez jellemzően fekete. Sok példában, de nem mindben az alakzatok egy háttér előtt vannak.

Egy 2D alakzatot lehet forgatni, kicsinyíteni, nagyítani, mozgatni, átszínezni valamilyen szabály szerint.

Pixels => Shapes

Egyik fő stratégia, hogy megkeressük az egyik alakzatot a másikban.

Másik, hogy fő kategóriákat próbálunk végigpróbálni.

- az alakzat geometriai transzformációja
  az egyik a másiknak kicsinyítése - nagyítása, forgatása, x része (fele, negyede, ...), elmozgatása (más helyen van)
  Mert minden pixel egyben egy alakzat is. Az alakzaton lehet geometriai transzformációt végrehajtani.
  Minden pixel tetszőleges másik alakzatot alkothat, amin szintén lehet geometriai transzformációt végrehajtani.

- az alakzat darabszáma változik, ismétlődések, minták
  + az eredeti alakzat szimplán többször megtalálható a célban
  + az eredeti alakzat valamilyen formában megtalálható a célban, de hiányzik valami...

- az alakzat egy vagy több tulajdonságának vizuális leképzése jelenik meg
  pl. ahány kék négyzet van, annyi kék pixel lesz (1fad071e)

- reprezentáció
  pl. a piros reprezentálja a legkisebb alakzatot
  a pixelek rácsot alkotnak, amiben pixelek vannak
  van forrás-cél az alakzaton belül valamilyen elválasztó mentén

- kitüntett tulajdonság keresése
  pl. jelöld pirossal azokat a helyeket, ahol a legtöbb pixel van (29623171)

https://github.com/arc-community/wiki/wiki/Key-findings-by-various-people-and-FAQ#andreaskopf

Koncepciók, amik fontosak lehetnek:
 - alakzat / háttér felismerés
 - élek
 - sarok ( első körben 90 fokos egyenes vonalak esetén )
 - lekerekített / szögletes sarok
 - teli / üres
 - egyszerű alakzatok, pl. négyzet, téglalap, háromszög, kör
 - kívül / belül
 - ismétlődés
 - vonalak (egyenes, szaggatott, ...)
 - rendezés (pl. nagyság szerint)
 - szimmetriák
 - méretek
 - darabszám
 - távolság
 - minta folytatása
 - mozgások (visszapattan, vonz, taszít)
 - falak, labirintusok
 - logika
 - algoritmikus formák: spirál, hullámvonal
 - halmazműveletek
 - kivétel
 - zaj ( nem odaillő pixelek )

Két dimenziós világ:
 - a kép, mint x * y felbontású pixel mátrix
 - a mátrixon belül alakzatok lehetnek. Az alakzat lehet a háttér is.
 - a sík végtelen kiterjedésű, de a szenzor véges felbontású, tehát a szenzoron kívül is lehetnek pixelek, amiket a szenzor nem lát
 - az alakzatnak van körvonala
   - a körvonalnak van éle
   - az él lehet egyenes vagy görbe
   - egyenes élek találkozásakor sarokról beszélünk

Gondolkozási keret
==================

Forrás -> Cél
-------------
  - A feladat az, hogy rá kell jönni, mik azok a transzformációs lépések, amik a forrás alakzatból elvisznek a cél alakzatba.
  - A forrás és a cél egy pixel mátrix
  - A pixelnek lehet szine
  Cells: Szín, Pixel, Mátrix, Forrás, Cél

Az alakzat
----------
 - Az alakzat a forrás és cél alakzaton belüli pixel halmaz

 Nem tudjuk hány alakzat van, legalább egy van, de lehet több is.
 Az alakzatnak vannak tulajdonságai: pixelek száma, magasság - szélesség pixelben, pixelek színe
 Az alakzatokra bármilyen transzormáció alkalmazható.
 Az alakzat lehet háttér és objektum szerepben.
 A transzformáció bármilyen egy vagy többlépéses művelet lehet.
 A transzformációt műveletek írják le.

 A gondolkodási keret az, hogy van a forrás káp és a cél kép.
 A képen vannak alakzatok, amik megváltoznak valamilyen módon a cél képen.
 Az alakzatok valamilyen háttér előtt vannak.
 Az alakzatok kitakarják a hátteret.

Kategorizálás példák segítségével -> Kell egy önkategorizáló algoritmus
Felismerés, reprezentáció -> a pixelek reprezentálhatnak ismert dolgokat

Ismert dolgok: vonal (függőleges, vízszintes, ferde, görbe)
               A vonalak alkothatnak ábrákat.


Stratégia példák alapján:
https://qugank.github.io/pqa.github.io/

a. Körvonal által kijelölt terület beszínezése
   xxx.    xxx.
   x..x => xxxx
   .xx.    .xx.

b. Végpontok összekötese vonallá
   ....x...    ....x...
   x.......    x.......
   ........ => x.......
   x....x..    xxxxxx..
   ........    ........

c. Közelség értelmezése
   ....a...    ........
   b.......    ........
   ........ => ........
   c.dddd..    ..cccc..
   .....d..    .....c..
   A legközelebbi pixel színét veszi fel a több pixelből álló alakzat

d. Alakzat helyreállítása
   ........    ........
   xxx..x..    xxxxxx..
   ..xxx... => xxxxxx..
   x.xx.x..    xxxxxx..
   ........    ........
   Kiegészítés téglalappá

e. Alakzat és minta hasonlóság
   ab...a..    ab......
   .....b..    ........
   ........ => ........
   ..cccc..    ..aabb..
   ..cccc..    ..aabb..

f. Tükrözés 1 tengelyen
   ..aaa.a.    ..aaa.a.
   ..a...a.    ..a...a.
   ..bbbbb. => ..a...a.
   ........    ..aaa.a.
   ........    ........

g. 4 tengelyen tükrözött minta hiányzó részeinek megtalálása
   abcddcba    abcddcba
   bbadddab    bbadddab
   cbbbbbbc => cbbbbbbc
   cbb..bbc    cbbbbbbc
   bb....ab    bbadddab
   abcddcba    abcddcba
*/

} // namespace synth