#pragma once
#include "Cells.h"

namespace infocell {
namespace cells {

class World;
class Pools;

class ID
{
    World& w;
    friend class Pools;

public:
    class Direction
    {
    public:
        Direction(World& w);
        String& up;
        String& down;
        String& left;
        String& right;
    };

    class Coordinates
    {
    public:
        Coordinates(World& w);
        String& x;
        String& y;
    };

    ID(World& w);

    Direction directions;
    Coordinates coordinates;

    String& __type__;
    String& argument;
    String& ast;
    String& asts;
    String& blue;
    String& break_;
    String& builder;
    String& cell;
    String& children;
    String& color;
    String& compiled;
    String& condition;
    String& constructor;
    String& container;
    String& continue_;
    String& currentFn;
    String& currentParam;
    String& currentStruct;
    String& data;
    String& description;
    String& destructor;
    String& effect;
    String& else_;
    String& emptyObject;
    String& enum_;
    String& first;
    String& fullyQualifiedName;
    String& functions;
    String& globalScope;
    String& green;
    String& height;
    String& id;
    String& index;
    String& input;
    String& instances;
    String& instructions;
    String& key;
    String& keyType;
    String& last;
    String& lastOp;
    String& lhs;
    String& list;
    String& listType;
    String& localVars;
    String& memberMapping;
    String& memberOf;
    String& members;
    String& method;
    String& methods;
    String& name;
    String& next;
    String& node;
    String& nodeType;
    String& objectType;
    String& op;
    String& ops;
    String& other;
    String& output;
    String& parameters;
    String& parent;
    String& parentFunction;
    String& pixels;
    String& pixelsMap;
    String& pop;
    String& previous;
    String& primitiveTool;
    String& process;
    String& push;
    String& red;
    String& resolvedScope;
    String& result;
    String& return_;
    String& returnType;
    String& rhs;
    String& rootNode;
    String& scope;
    String& scopes;
    String& self;
    String& sign;
    String& size;
    String& slots;
    String& stack;
    String& state;
    String& statement;
    String& static_;
    String& status;
    String& structs;
    String& structType;
    String& tag;
    String& templateId;
    String& templateParams;
    String& then;
    String& throw_;
    String& type;
    String& typeAliases;
    String& unknownInstances;
    String& unknownStructs;
    String& value;
    String& valueType;
    String& variable;
    String& variables;
    String& width;
};

} // namespace cells
} // namespace infocell