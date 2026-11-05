# Solving ARC-AGI with the InfoCell symbolic AI programming language
Using mathematical equations as a regex like search expression on extended truth tables that can be "solved" to a regular program

## Abstract

The InfoCell programming language is embedded in the C++ syntax, so it is a regular C++ program. This syntax creates InfoCell AST cells during runtime and it can be compiled to InfoCell OP cells also in runtime that are executable cells. Regular object oriented programs can be created with this language. This work extends the regular OOP constructs with a new concept, called descriptions. A regular OOP function/method can be considered as parametrised instructions that corresponds to the imperative grammar mood. We giving out orders to the computer with parametrized commands. With the new description concept we can describe how these regular OOP methods behave. This corresponds to the indicative grammar mood. We can use these descriptions as an extended mathematical truth table. These truth tables behave very similarly to the context window concept in LLMs. We can put parametrised expressions also to the truth tables and we can use this concept to solve mathematical equations. We can use unknown variables as parameters and we can treat them as a regex like captured variable concept. So mathemtacal equatins became search expressions. We use this equation solving capability to create conversion tools that convert one number to another. We can put the generated conversion tool program back to the truth table and can filter repeating ones. I find this approach similar to the attention mechanism in LLMs. We can also put container contents to these extended truth tables and introduce a new kind of mathematical expression the hierarhical equations where one equation can be considered as a parent equation and the solutions for that equation can be used in the child equation. "Solving" this hierarhical equation results in a regular for loop as we can connect container iterator(s) to the container getter(s). As these mathematical search expressions was introduced as descriptions (with C++ terms post conditions) of regular OOP methods or for loops we can generate a regular OOP solver code for search expressions. The InfoCell objects are typed so we can use runtime reflections on these nodes. We can get every getters from an object in runtime. This is crucial as we can only consider getters used in search expressions that is already true for an object. These concepts are already implemented and contains unit tests for it. The implementation of for loop generation is partial currently.

## The InfoCell program

There are regular classes, methodes, templates and traits in the language. There is a complete standard library and a dedicated ARC-AGI related library. The classes are in namespaces. A dedicated compiler class responsible to compile the source code encoded AST cells to runnable OP cells.

Because the language has no native syntax, it must be embedded in a host language. The reference implementation uses C++, leveraging its operator overloading capabilities to express and manipulate infocells effectively. The following example illustrates this representation.

Here is an example of a source code encoded AST nodes. It is very similar to a builder pattern.
```C++
    listStructT.addMethod("add")
        .parameters(
            parameter("value", tp_("valueType")))
        .returnType(ta_("nodeType"))
        .instructions(
            var_("node") = new_(ta_("nodeType"), "constructor")("value", p_("value")),
            if_(not_(m_("first").exist()))
                .then_(m_("first") = *var_("node"))
                .else_(block(
                    set(m_("last"), "next", *var_("node")),
                    set(*var_("node"), "previous", m_("last")))),
            m_("last") = *var_("node"),
            m_("size") = add(m_("size"), _(_1_)),
            return_(*var_("node")));
```
Here `var_("name")` is a variable, `p_("name")` is a function parameter, `m_("name")` is a member, `_()` is a const value, `ta_` type alias, `tp_()` i stemplate parameter, and so on. Similarly other common instruction keywords can be used in this way. We have `if(...).then(...).else(...)`, `return_()`, `do_(...).while()`, `new_(...)`. The methodes are in actual classes. For more examples see `src\cells\StdLib.cc` or `src\arc\ArcLib.cc`.

### The description segment

The idea of this project is a new component, the `ToolFinder` and the description segment for AST nodes (which compiles to OP descriptions). The description segment provides a declerative way to express what is happening between two program state. We express what is the next state according to the current one. We can store this segment in extended truth tables.

### The individual information cell (InfoCell)

The fundamental unit of the InfoCell programming language is the information cell (infocells), which is the only primitive type in the language. As in LISP, code and data share a unified representation. Unlike LISP, however, the core structure here is an object composed of key–value pairs, where both keys and values are object references.

![SampleCell](https://www.googleapis.com/download/storage/v1/b/kaggle-user-content/o/inbox%2F11326355%2Feadfa86d349e7fbdb4305d2db655a6c6%2FSampleCell.svg?generation=1793303235535402&alt=media)

Every AST cell compiles to OP cells, these are assemmbly instructions of the InfoCell language. Every instruction is compiled to these cells. There are cases when a direct translation is possible, for example logical `OR` AST can be directly compiled to an `OR` OP cell. In other cases, for example a `for` loop, or accessing a variable compiles to multiple `OP` cells. When a method or function has a description segment we can refere to this thing as "tool". Basically intruction manual + instruction = tool.


## The ARC-AGI solver concept

We can decompose an ARC-AGI task to conversion between primitive values, objects and containers. An ARC-AGI task can be viewed as finding a conversion function from value `A` (refered as input or from value) to value `B` (refered as output or to value). The type of the value `A` and `B` in case of ARC-AGI is a pixel matrix which is a type of container. A pixel matrix contains pixels. The pixel has colors and the pixels are connected to each other (up, down, left and right). Also we can assign `X` and `Y` coordinate numbers to the pixels. We can create relative vectors to express connection between two pixels by counting how many up, down, left and right connections are in use.

### Conversion between primitive values (boolean, numbers)

Creating conversion function for numbers and boolean values are quite different but we can still use a common algorith for it.

#### Numbers

If one pixel at [X:1, Y:1] and the expected output is at [X:1, Y:2] the conversion function will be something like `pixel.Y + 1` or `pixel.Y * 2`. We have to take into account every operation (add, subtract, multiply, ...). For every operation we can form an equation for it, something like: `1 + X == 2` or `1 * X == 2` and so on. Solving these equations need to take into account how the mathematical functions are interact. We have to define how mathematical functions interact, so how can we measure the world state. With C++ concepts terms, what is the post condition.

For addition we can state, that
```
Number::add(other: Number) =>  return - other == self
Number::divide(other: Number) =>  return * other" == self
Number::multiply(other: Number) =>  return / self == other
Number::subtract(other: Number) =>  return + other == self
```
Where `self` is refering to the current object (same as in Python, in C++ it is called this pointer). The `return` keyword here is the value from an equation, for example the equation `1 + 2 == 3`, the `1` is the self, `2` is the other, `3` is the return. This conclusion tells us how the expression behaves in an equation. We can rebuild our equation to an other one. From `1 + 2 == 3` we can build a `3 - 2 == 1` equation. The interesting thing, is we can use unknown values here also. For
example we can change the value `2` with an uninitialized variable, called `X`.

We can generate conversion tools for the numbers with the help of the `consequences` subsegment. The actual code is in `ToolFinder::exploreSlotManipulations()`.
The algorithm calculates how the consequences behaves when an unknown variable is given. We rebuild the equation with the help self builders. Self builder tells the algorithm that `2 + X` is the same as `X + 2`. So this is the experimenting phase for the tools, here we can remember how an uninitialized variable (the unknown X) interacts with the tool's consequeences. Here we can store which const/unknown combination leads to a simpler case (in code: `ToolFinder::isConstantFoldingPossible`), where a consequence tools all input's will be const variable. So we can transform an equation from one form to a simpler one.

We store these changes in the solver subsegment. Basically we just pattern match for function + const/unknown input params, then just reapply the tarsformation steps, just like solving the Rubik's cube: pattern match for color combination and apply rotations.

Basically something like this, actual code:
```
  equation: 2 + X == 4
recombined: X + 2 == 4 // see ToolFinder::recombine
recombined: 4 == 2 + X
recombined: 4 == X + 2 * // this path leads to a simpler case, we save this builder
  1. result: 4 - X == 2
  1. result: 4 - 2 == X
  1. result: 2 == 4 - X
  1. result: X == 4 - 2 * // we found a simpler case, we save this builder also
```

Current known limitiation is that we don't handle proconditions, for example to expressing the fact, that dividing with `0` is not possible. The planned solution is to introduce a new subsegment, maybe we can call it `preCondition` and save the condition next to the builders. So when we find the builder, we have to check the precondition.

#### Booleans

For solving boolean equation we need truth tables. We can express truth tables the same way as we did it with numbers but we have to provide the input values for the input parameters and we have to provide value for the `return`'s value. I use the syntax for defualt values here.

```
Boolean::or(self: Boolean, other: Boolean)
    => or(self=true,  other=true)  == return(true);
    => or(self=true,  other=false) == return(true);
    => or(self=false, other=true)  == return(true);
    => or(self=false, other=false) == return(false);
```

Solving boolean equation basically defined as a search operation on this truth table. We can use the `X` value as a captured variable in regex. So `false or X = true` meaning search for equations that looks like this:
`or(self=false, other=X) == return(true)` where the X value comes from the stored values. For details see `SolverStateNode::checkAndCaptureValue` method and usage in the code.

#### ARC-AGI example

We can imagine a hypotetical ARC-AGI example with taskpairs
- input1:2, output1:4
- input2:1, output2:3
- input3:5, output3:7
- testInput1:3

We can put these values to a truth table, I just named it as `DescriptionKind::knowledge` in the code. Then we can search for the equations in that table.

We can find the conversion equations with every tool (add, sub, mul, ...) by generating every possible equations (see `2 + X == 4`, `X + 2 == 4`, `X * 2 = 4`, ...).

The big idea here is that we can use these equations as a search expression in the knowledge truth table by replacing the value `2` with `inupt2`, `input3` ...!
So we can delay the actual code generation after every search is finished. The `consequences` segments contains every info about how to "rebuild" the search expression to constructors.
Basically building a parser in runtime and we can convert the parser expressions to actual code.

For actual code implementations and unot tests, see `src\cells\ToolFinder.cc`, `src\cells\tests\CellTest.cc` and `src\cells\tests\ToolFinderTest.cc`.
```
// Given:   pixel.color = 5
// Request: get(pixel, green)
// Result:  5, so we expect constant folding here
TEST_F(CellTest, ToolFinderTestForGet)

// Given:   empty pixel
//          theme.color = green
// Request: get(pixel, theme.color) == 5
// Result:  set(pixel, theme.color, 5)
TEST_F(CellTest, ToolFinderTestForGetInGet)
...
// map<String, Number>.getValue("key1") == 111
//
// Given:   map: std::Map<String, Number> which is constructed and empty
// Request: map.getValue("key1") == 111
// Result:  map.add("key1", 111)
TEST_F(CellTest, ToolFinderTestForMap)
```

### Conversion between objects

Object can contain primitive values (see file `src\arc\ArcLib.cc`) or other objects.

To obtain the referenced member values we have to use the getters of that object. We can always travers runtime members through the `__type__` connections as the compiler generates reflection info for the objects. The consequence of a setter usually an equation where the corresponding getter return value is the value from input of that setter. In short: `SET(cell, key, value) => GET(cell, key) == value`. The SET is the primitive constructor as it can change a value of an UNKNOWN value. As pointed out earlier the `GET(cell, KEY) == VALUE` can be a search expression in a truth table (currently implemented `ToolFinder` class). If we lookup the search expression `GET(cell, KEY) == VALUE` in the consequences truth table we get back the builder of the `SET` op and the builder can rebuild the `GET(cell, KEY) == VALUE` eqation to `SET(cell, key, value)`. Here we asking a question: what tool can cause this effect? Here the tool `SET` can cause the effect `GET(cell, KEY) == VALUE`.

Here is the actual truth table for a setter:
```
    Cell.addPrimitiveFunction(std.Cell.Set, op.Set, "set")
        .memberMapping(
            kvPair(id.self, "cell"),
            kvPair("key", "key"),
            kvPair("value", "value"))
        .constructor()
        .parameters(
            parameter("key", "Cell"),
            parameter("value", "Cell"))
        .descriptionBegin()
            .consequences(
                equal(get(self(), p_("key")), p_("value")))
        .descriptionEnd();
```

### Conversion between containers

(set, maps, lists ...) containing objects or other containers

For simplicity we can treat every container as a mathemtaical set, so we allow lost the ordering inside the containers. We can deal with the order in a later phase.

If we want express the fact that every element of the source set is also contained in the destination set, which may contain additional elements, we need to express asymmetric connection between sets.

We can invent a for loop like thing for equation where the unknown X filled in the for loop "header" () and used in the for loop "body". 

Actual code in `ToolFinder::findConversionToolsByContainer2(ToolFinder& localContext, CellI& examples, List& result)`
where `localContext` contains the truth table for `fromX`.

We can use the `for` keyword as a search expression inside the truth table. This alters the search process in a way that the unknown value in the first expression captured and the search continues with the child expression. At the same time we also searching for a `for` as a value in the truth table, actually both happening at the same time. See below.

```
w.ast.for_(w.ast.unknown_(fromX)).in(w.ast.equal(w.ast.call(w.ast._(otherFrom), "contains")(id.value, w.ast.unknown_(fromX)), w.ast.true_()))(
    w.ast.equal(w.ast.call(w.ast._(otherTo), "contains")(id.value, w.ast.unknown_(fromX)), w.ast.true_()));
```

Hypotetical C++ syntax can be something like.
```
for (X in set1.contains(X) == true) { // this is the for loop header, here the X filled by the truth table
   set2.cotains(X) == true;           // this is the for loop body, here we can use the X value
}
```

The first equation (`set1.contains(X) == true`) fills the `X` the second one (`set2.cotains(X) == true`) only use the X values from the first equation.

This can be "solved" as

```
for (X in set1) {
   set2.insert(X);
}
```

The trick is we can register the connection between an iterator and a getter. Here we express the fact that if we iterate over a set the following expression is true:
`set.contains(x)`. As we registered an unknown value here this can be used a search expression.

As mentianed above, the `for` keyword can be used as value in the truth table. This is needed to find the consequences of a for loop. In case we actually matching with a for loop we can find tools that actually realize that.

Actual code in `src\cells\StdLib.cc`. 
```
    implIterableTraitForSetT.addMethod("iterator")
        .returnType(at_("Iterator"))
        .descriptionBegin()
            .consequences(
                for_(unknown_(var_("value"))).in(equal(self()("contains")("value", unknown_(w.id.value)), true_()))(self()))
            .selfBuilders(
                for_(unknown_(var_("value"))).in(self()("iterator"))(self()))
            .descriptionEnd()
        .instructions(
            return_(new_(at_("Iterator"), "constructor")("set", self())));
```
Here the Set class implements the Iterator trait (the trait concept come from Rust). `at_` is associated type here.

An other important aspect of dealing with containers is that we try all possible way to convert a value from SetFrom to SetTo, creating a hypotesis equation from it and immediately test it on other examples.

For actual ARC-AGI task we can use a `ShapeField` class which has a `width` and `height` integer values and contains a shape map. There is a complete edge detection code written in C++ and also in InfoCell in the source (see `EdgeDetector::detect(const std::string& jsonStr)` and `EdgeDetector::frameProcess()`. Actually there is no point using the InfoCell code as the C++ variant much faster. Both variant generates a `ShapeField` InfoCell class.

We can write a `ShapeField` iterator where the correspoind getter is two parameter, something like:
```
for (POS, SHAPE in shapeFieldFrom.getValue(POS) == SHAPE) {
   shapeFieldInsert.cotains(POS) == SHAPE;
}
```

Where `ShapeField` declared as follows:
```
    auto& shapeFieldStruct
        = scope.add<Struct>("ShapeField")
              .members(
                  member("width", _(std.Number)),
                  member("height", _(std.Number)),
                  member("shapesMap", tt_("std::TrieMap", "keyType", "Vector", "valueType", "Shape")));
```

And `std::TrieMap<keyType, valueType>::getValue(key: keyType) -> valueType` declared as follows:
```
    trieMapStructT.addMethod("getValue")
        .parameters(
            parameter("key", tp_("KeyType")))
        .returnType(tp_("ValueType"))
```

### Searching repeating pattern

There is a demonstration of pattern continuation in the tests `TEST_F(ToolFinderTest, FindPatternContinuations)`.

`testPatternContinuation({ &_8_, &_4_, &_2_ });`

Some results:
```
Pattern: 1 2 3  ... 4 with tool op type op::Add lhs op variable rhs 1
Pattern: 1 2 3  ... 4 with tool op type op::Subtract lhs op variable rhs -1
Pattern: 1 2 4  ... 8 with tool op type op::Multiply lhs op variable rhs 2
Pattern: 8 4 2  ... 1 with tool op type op::Divide lhs op variable rhs 2
Pattern: 8 4 2  ... 0 with tool op type op::Multiply lhs op variable rhs 0
Pattern: 1 3 9  ... 27 with tool op type op::Multiply lhs op variable rhs 3
Pattern: 1 3 5  ... 7 with tool op type op::Add lhs op variable rhs 2
Pattern: 4 4 4  ... 4 with tool op type op::Add lhs op variable rhs 0
```

There are some issues here currently. We treat the `X + 2` and `X - (-2)` as separate case, we need find a way to filter these things out. Also currently we using integers for numbers and `0.5` is just treated as `0` (we generated a `X * 0`, instead of `X * 0.5`). We have to extend the Number class with at least the Rational numbers.

### Description segment in detail

We can express a mathematical truth tables with the description segment. Here the mathematical `OR` operation is described within the `consequences` subsegment. The `OR` operation is symmetric so we can build it two way, as `OR(LHS, RHS) == OR(RHS, LHS)`. As the mathematical `OR` is happens to be an ASM like OP primitive we

```
    Boolean.addPrimitiveFunction(std.Boolean.Or, op.Or, "or")
        .memberMapping(
            kvPair(id.self, "lhs"),
            kvPair("other", "rhs"))
        .parameters(
            parameter("other", "Boolean"))
        .descriptionBegin()
            .consequences(
                equal(or_(self(true_()), p_("other", true_())), return_(true_())),
                equal(or_(self(true_()), p_("other", false_())), return_(true_())),
                equal(or_(self(false_()), p_("other", true_())), return_(true_())),
                equal(or_(self(false_()), p_("other", false_())), return_(false_())))
            .selfBuilders(
                or_(self(), p_("other")),
                or_(p_("other"), self()))
        .descriptionEnd()
        .returnType("Boolean");
```

This syntax means, if the `self` pointer (in C++ it is called `this` pointer) value is the value `true` and the parameter `other` is `true` then this function will return `true`.
As the `OR` is also a OP node we just put the compilation instructions here. As an instruction we use the lhs (left hand side) and rhs (right hand side) parameter names.
As this function is a method, the lhs parameter is the `this pointer`.

We can put the `consequences` subsegment to a lookup trie map (a lookup table made from trie). The key is the serialized form of the expression, the value will be the builders.

In this case the builders will build back the original equation. So if you look up the input expression `false or true = true` we got back a builder that builds back the input from the input. So we got back the original input `false or true = true`. This make more sense when we look up with `false or X = true` where `X` is unknown. In this case we give back `X = true`.

We can use this concept for sets later. For ARC-AGI perspective this is a crucial concept as we want find a transition between two values. If want to find a transition from `false` to `true` we can find those by looking it in the `consequences` segment. For the record we use an additinal lookup table caled `conversionTools` to map input types to output types.

We put the `selfBuilders` segment to a different lookup table. This segment discribes the fact that the `OR` operation is symmetric. This will be useful to write mathematical equation solvers.

Here is a log from the program. The serialization use `op push`, `op pop` and `op variable xx` control cells to be able to traverse the OP tree with values.

```
op::Or =>
  consequence: op type equal lhs op push op type or lhs op variable true rhs op variable true op pop rhs op variable true
  consequence: op type equal lhs op push op type or lhs op variable true rhs op variable false op pop rhs op variable true
  consequence: op type equal lhs op push op type or lhs op variable false rhs op variable true op pop rhs op variable true
  consequence: op type equal lhs op push op type or lhs op variable false rhs op variable false op pop rhs op variable false
  selfBuilder: op type or lhs op variable rhs op variable
  selfBuilder: op type or lhs op variable rhs op variable
```


```
    Cell.addPrimitiveFunction(std.Cell.Set, op.Set, "set")
        .memberMapping(
            kvPair(id.self, "cell"),
            kvPair("key", "key"),
            kvPair("value", "value"))
        .constructor()
        .parameters(
            parameter("key", "Cell"),
            parameter("value", "Cell"))
        .descriptionBegin()
            .consequences(
                equal(get(self(), p_("key")), p_("value")))
        .descriptionEnd();
```
Here the `get(cell, key) == value` can be "solved to" set(cell, key, value).

## Conclusion

Although this solution is nowhere near complete, I think it clearly shows a different approach to solve ARC-AGI tasks. It currently doesn't solve any realy ARC-AGI test, but I think I quite close to it. It can solve simpler than ARC-AGI tasks although. The program doesn't use GPU and the memory consumption tops at 6.7Gb after 33 seconds runtime for the biggest unit test. (CMake target: `arctest`). For the record I submitted an empty solution to ARC-AGI-2 that scores 0 and does nothing, the submission name is `Solving ARC-AGI with InfoCells`.