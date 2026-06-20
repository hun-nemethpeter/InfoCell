#pragma once
#include "Cells.h"

namespace infocell {
namespace cells {

class World;
class Pools;

class ID
{
    World& w;
    std::map<List*, const char*> m_ids;
    friend class Pools;

public:
    class Directions
    {
    public:
        Directions(World& w);
        List up;
        List down;
        List left;
        List right;
    };

    class Coordinates
    {
    public:
        Coordinates(World& w);
        List x;
        List y;
    };

    ID(World& w);

    Directions directions;
    Coordinates coordinates;

    List __type__;
    List argument;
    List ast;
    List asts;
    List blue;
    List break_;
    List cell;
    List children;
    List color;
    List compiled;
    List condition;
    List constructor;
    List container;
    List continue_;
    List currentFn;
    List currentParam;
    List currentStruct;
    List data;
    List description;
    List destructor;
    List else_;
    List enum_;
    List emptyObject;
    List first;
    List functions;
    List globalScope;
    List green;
    List height;
    List id;
    List index;
    List input;
    List instances;
    List instructions;
    List item;
    List itemType;
    List key;
    List keyType;
    List last;
    List lastOp;
    List lhs;
    List list;
    List listType;
    List localVars;
    List memberOf;
    List members;
    List method;
    List methods;
    List name;
    List next;
    List objectType;
    List op;
    List ops;
    List output;
    List parameters;
    List parent;
    List pixels;
    List pixelsMap;
    List pop;
    List previous;
    List process;
    List push;
    List red;
    List resolvedScope;
    List result;
    List return_;
    List returnType;
    List rhs;
    List rootNode;
    List scope;
    List scopes;
    List self;
    List size;
    List slots;
    List stack;
    List state;
    List stateCondition;
    List stateElse;
    List stateLhs;
    List statement;
    List stateParam1;
    List stateParam2;
    List stateParam3;
    List stateParamEval;
    List stateParamInit;
    List stateRhs;
    List stateStackCall;
    List stateStatement;
    List stateThen;
    List static_;
    List status;
    List structs;
    List structType;
    List typeAliases;
    List tag;
    List templateId;
    List templateParams;
    List then;
    List throw_;
    List type;
    List unknownInstances;
    List unknownStructs;
    List value;
    List valueType;
    List variable;
    List variables;
    List width;
};

} // namespace cells
} // namespace infocell