# Solving ARC-AGI with the InfoCell symbolic AI programming language
Using mathematical equations as regex-like search expressions on extended truth tables that can be "solved" into a regular program

## Abstract

The InfoCell programming language is embedded in C++ syntax, so it is a regular C++ program. This syntax creates InfoCell AST cells at runtime, and these can also be compiled into executable InfoCell OP cells at runtime. Regular object-oriented programs can be created with this language. This work extends the regular OOP constructs with a new concept, called descriptions. A regular OOP function/method can be considered a set of parameterised instructions corresponding to the imperative grammatical mood. We give orders to the computer with parameterized commands. With the new description concept, we can describe how these regular OOP methods behave. This corresponds to the indicative grammatical mood. We can use these descriptions as extended mathematical truth tables. These truth tables behave very similarly to the context window concept in LLMs. We can also put parameterised expressions into the truth tables, and we can use this concept to solve mathematical equations. We can use unknown variables as parameters and treat them as regex-like captured variables. Thus, mathematical equations become search expressions. We use this equation-solving capability to create conversion tools that convert one number to another. We can put the generated conversion tool program back into the truth table and filter out repeated ones. I find this approach similar to the attention mechanism in LLMs. We can also put container contents into these extended truth tables and introduce a new kind of mathematical expression: hierarchical equations, where one equation can be considered a parent equation and the solutions for that equation can be used in the child equation. "Solving" this hierarchical equation results in a regular for loop, as we can connect container iterator(s) to the container getter(s). As these mathematical search expressions were introduced as descriptions (in C++ terms, postconditions) of regular OOP methods or for loops, we can generate regular OOP solver code for search expressions. The InfoCell objects are typed, so we can use runtime reflection on these nodes. We can get all getters from an object at runtime. This is crucial, as we can only consider getters used in search expressions that are already true for an object. These concepts are already implemented and have unit tests. The implementation of for-loop generation is currently partial.

## The InfoCell program

There are regular classes, methods, templates and traits in the language. There is a complete standard library and a dedicated ARC-AGI-related library. The classes are in namespaces. A dedicated compiler class is responsible for compiling the source-code-encoded AST cells into runnable OP cells.

Because the language has no native syntax, it must be embedded in a host language. The reference implementation uses C++, leveraging its operator overloading capabilities to express and manipulate infocells effectively. The following example illustrates this representation.

Here is an example of source-code-encoded AST nodes. It is very similar to a builder pattern.
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
Here, `var_("name")` is a variable, `p_("name")` is a function parameter, `m_("name")` is a member, `_()` is a const value, `ta_` is a type alias, `tp_()` is a template parameter, and so on. Similarly, other common instruction keywords can be used in this way. We have `if(...).then(...).else(...)`, `return_()`, `do_(...).while()`, `new_(...)`. The methods are in actual classes. For more examples, see `src\cells\StdLib.cc` or `src\arc\ArcLib.cc`.

### The description segment

The idea of this project is a new component, the `ToolFinder`, and the description segment for AST nodes (which compiles to OP descriptions). The description segment provides a declarative way to express what is happening between two program states. We express what the next state is according to the current one. We can store this segment in extended truth tables.

### The individual information cell (InfoCell)

The fundamental unit of the InfoCell programming language is the information cell (infocell), which is the only primitive type in the language. As in LISP, code and data share a unified representation. Unlike LISP, however, the core structure here is an object composed of key–value pairs, where both keys and values are object references.

![SampleCell](https://www.googleapis.com/download/storage/v1/b/kaggle-user-content/o/inbox%2F11326355%2F25369812686490c1567895d89b05cb0b%2FSampleCell.svg?generation=1793890194995114&alt=media)


Every AST cell compiles to OP cells, which are assembly instructions of the InfoCell language. Every instruction is compiled to these cells. There are cases when a direct translation is possible; for example, a logical `OR` AST can be directly compiled to an `OR` OP cell. In other cases, for example, a `for` loop or accessing a variable compiles to multiple `OP` cells. When a method or function has a description segment, we can refer to this thing as a "tool". Basically, instruction manual + instruction = tool.


## The ARC-AGI solver concept

We can decompose an ARC-AGI task into conversions between primitive values, objects and containers. An ARC-AGI task can be viewed as finding a conversion function from value `A` (referred to as input or from value) to value `B` (referred to as output or to value). The type of values `A` and `B` in the case of ARC-AGI is a pixel matrix, which is a type of container. A pixel matrix contains pixels. Pixels have colors, and the pixels are connected to each other (up, down, left and right). We can also assign `X` and `Y` coordinate numbers to the pixels. We can create relative vectors to express a connection between two pixels by counting how many up, down, left and right connections are in use.

### Conversion between primitive values (boolean, numbers)

Creating conversion functions for numbers and boolean values is quite different, but we can still use a common algorithm for both.

#### Numbers

If one pixel is at [X:1, Y:1] and the expected output is at [X:1, Y:2], the conversion function will be something like `pixel.Y + 1` or `pixel.Y * 2`. We have to take into account every operation (add, subtract, multiply, ...). For every operation, we can form an equation, something like: `1 + X == 2` or `1 * X == 2` and so on. Solving these equations needs to take into account how the mathematical functions interact. We have to define how mathematical functions interact so that we can measure the world state. In C++ concepts terms, what is the postcondition?

For addition, we can state that:
```
Number::add(other: Number) =>  return - other == self
Number::divide(other: Number) =>  return * other" == self
Number::multiply(other: Number) =>  return / self == other
Number::subtract(other: Number) =>  return + other == self
```
Here, `self` refers to the current object (as in Python; in C++, it is called the this pointer). The `return` keyword here is the value from an equation. For example, in the equation `1 + 2 == 3`, `1` is self, `2` is other, and `3` is return. This conclusion tells us how the expression behaves in an equation. We can rebuild our equation into another one. From `1 + 2 == 3` we can build a `3 - 2 == 1` equation. The interesting thing is that we can also use unknown values here. For
example, we can replace the value `2` with an uninitialized variable called `X`.

We can generate conversion tools for numbers with the help of the `consequences` subsegment. The actual code is in `ToolFinder::exploreSlotManipulations()`.
The algorithm calculates how the consequences behave when an unknown variable is given. We rebuild the equation with the help of self-builders. A self-builder tells the algorithm that `2 + X` is the same as `X + 2`. So this is the experimentation phase for the tools; here we can remember how an uninitialized variable (the unknown X) interacts with the tool's consequences. Here we can store which const/unknown combination leads to a simpler case (in code: `ToolFinder::isConstantFoldingPossible`), where all inputs to a consequence tool will be constant variables. So we can transform an equation from one form to a simpler one.

We store these changes in the solver subsegment. Basically, we just pattern-match functions with const/unknown input parameters, then reapply the transformation steps, just like solving a Rubik's cube: pattern-match a color combination and apply rotations.

Basically, it looks something like this in the actual code:
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

A currently known limitation is that we don't handle preconditions, for example, to express the fact that dividing by `0` is not possible. The planned solution is to introduce a new subsegment, perhaps called `preCondition`, and save the condition next to the builders. So, when we find the builder, we have to check the precondition.

#### Booleans

To solve boolean equations, we need truth tables. We can express truth tables in the same way as we did with numbers, but we have to provide the input values for the input parameters and a value for `return`. I use the syntax for default values here.

```
Boolean::or(self: Boolean, other: Boolean)
    => or(self=true,  other=true)  == return(true);
    => or(self=true,  other=false) == return(true);
    => or(self=false, other=true)  == return(true);
    => or(self=false, other=false) == return(false);
```

Solving a boolean equation is basically defined as a search operation on this truth table. We can use the `X` value as a captured variable in a regex. So `false or X = true` means searching for equations that look like this:
`or(self=false, other=X) == return(true)` where the X value comes from the stored values. For details, see the `SolverStateNode::checkAndCaptureValue` method and its usage in the code.

#### ARC-AGI example

We can imagine a hypothetical ARC-AGI example with task pairs:
- input1:2, output1:4
- input2:1, output2:3
- input3:5, output3:7
- testInput1:3

We can put these values into a truth table, which I named `DescriptionKind::knowledge` in the code. Then we can search for the equations in that table.

We can find the conversion equations with every tool (add, sub, mul, ...) by generating all possible equations (see `2 + X == 4`, `X + 2 == 4`, `X * 2 = 4`, ...).

The big idea here is that we can use these equations as search expressions in the knowledge truth table by replacing the value `2` with `inupt2`, `input3` ...!
So we can delay the actual code generation until every search is finished. The `consequences` segment contains all the information about how to "rebuild" the search expression into constructors.
Basically, we build a parser at runtime and can convert the parser expressions into actual code.

For actual code implementations and unit tests, see `src\cells\ToolFinder.cc`, `src\cells\tests\CellTest.cc` and `src\cells\tests\ToolFinderTest.cc`.
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

An object can contain primitive values (see file `src\arc\ArcLib.cc`) or other objects.

To obtain the referenced member values, we have to use the getters of that object. We can always traverse runtime members through the `__type__` connections, as the compiler generates reflection info for the objects. The consequence of a setter is usually an equation where the corresponding getter's return value is the value from the input of that setter. In short: `SET(cell, key, value) => GET(cell, key) == value`. The SET is the primitive constructor, as it can change a value of an UNKNOWN value. As pointed out earlier, the `GET(cell, KEY) == VALUE` can be a search expression in a truth table (currently implemented in the `ToolFinder` class). If we look up the search expression `GET(cell, KEY) == VALUE` in the consequences truth table, we get back the builder of the `SET` op, and the builder can rebuild the `GET(cell, KEY) == VALUE` equation into `SET(cell, key, value)`. Here, we are asking a question: what tool can cause this effect? Here, the tool `SET` can cause the effect `GET(cell, KEY) == VALUE`.

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

(sets, maps, lists ...) containing objects or other containers

For simplicity, we can treat every container as a mathematical set, so we allow the loss of ordering inside the containers. We can deal with the order in a later phase.

If we want to express the fact that every element of the source set is also contained in the destination set, which may contain additional elements, we need to express an asymmetric connection between sets.

We can invent a for-loop-like construct for an equation where the unknown X is filled in the for-loop "header" () and used in the for-loop "body".

The actual code is in `ToolFinder::findConversionToolsByContainer2(ToolFinder& localContext, CellI& examples, List& result)`.
Here, `localContext` contains the truth table for `fromX`.

We can use the `for` keyword as a search expression inside the truth table. This alters the search process in such a way that the unknown value in the first expression is captured and the search continues with the child expression. At the same time, we are also searching for a `for` as a value in the truth table; actually, both happen at the same time. See below.

```
w.ast.for_(w.ast.unknown_(fromX)).in(w.ast.equal(w.ast.call(w.ast._(otherFrom), "contains")(id.value, w.ast.unknown_(fromX)), w.ast.true_()))(
    w.ast.equal(w.ast.call(w.ast._(otherTo), "contains")(id.value, w.ast.unknown_(fromX)), w.ast.true_()));
```

Hypothetical C++ syntax could look something like this:
```
for (X in set1.contains(X) == true) { // this is the for loop header, here the X filled by the truth table
   set2.cotains(X) == true;           // this is the for loop body, here we can use the X value
}
```

The first equation (`set1.contains(X) == true`) fills the `X`, while the second one (`set2.cotains(X) == true`) only uses the X values from the first equation.

This can be "solved" as:

```
for (X in set1) {
   set2.insert(X);
}
```

The trick is that we can register the connection between an iterator and a getter. Here, we express the fact that if we iterate over a set, the following expression is true:
`set.contains(x)`. As we registered an unknown value here, this can be used as a search expression.

As mentioned above, the `for` keyword can be used as a value in the truth table. This is needed to find the consequences of a for loop. In case we are actually matching with a for loop, we can find tools that actually realize that.

The actual code is in `src\cells\StdLib.cc`.
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
Here the Set class implements the Iterator trait (the trait concept comes from Rust). `at_` is an associated type here.

Another important aspect of dealing with containers is that we try all possible ways to convert a value from SetFrom to SetTo, creating a hypothesis equation from it and immediately testing it on other examples.

For an actual ARC-AGI task, we can use a `ShapeField` class, which has integer `width` and `height` values and contains a shape map. There is complete edge-detection code written in C++ and also in InfoCell in the source (see `EdgeDetector::detect(const std::string& jsonStr)` and `EdgeDetector::frameProcess()`). Actually, there is no point in using the InfoCell code, as the C++ variant is much faster. Both variants generate a `ShapeField` InfoCell class.

We can write a `ShapeField` iterator where the corresponding getter has two parameters, something like:
```
for (POS, SHAPE in shapeFieldFrom.getValue(POS) == SHAPE) {
   shapeFieldInsert.cotains(POS) == SHAPE;
}
```

Here, `ShapeField` is declared as follows:
```
    auto& shapeFieldStruct
        = scope.add<Struct>("ShapeField")
              .members(
                  member("width", _(std.Number)),
                  member("height", _(std.Number)),
                  member("shapesMap", tt_("std::TrieMap", "keyType", "Vector", "valueType", "Shape")));
```

And `std::TrieMap<keyType, valueType>::getValue(key: keyType) -> valueType` is declared as follows:
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

There are some issues here currently. We treat the `X + 2` and `X - (-2)` as separate cases; we need to find a way to filter these things out. Also, currently, we are using integers for numbers, and `0.5` is just treated as `0` (we generated `X * 0` instead of `X * 0.5`). We have to extend the Number class with at least the Rational numbers.

### Description segment in detail

We can express mathematical truth tables with the description segment. Here, the mathematical `OR` operation is described within the `consequences` subsegment. The `OR` operation is symmetric, so we can build it in two ways, as `OR(LHS, RHS) == OR(RHS, LHS)`. The mathematical `OR` operation happens to be an ASM-like OP primitive:

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

This syntax means that if the value of the `self` pointer (in C++, it is called the `this` pointer) is `true` and the parameter `other` is `true`, then this function will return `true`.
As `OR` is also an OP node, we just put the compilation instructions here. As an instruction, we use the lhs (left hand side) and rhs (right hand side) parameter names.
As this function is a method, the lhs parameter is the `this pointer`.

We can put the `consequences` subsegment into a lookup trie map (a lookup table made from a trie). The key is the serialized form of the expression, and the value will be the builders.

In this case, the builders will build back the original equation. So if you look up the input expression `false or true = true`, you get back a builder that builds back the input from the input. So you get back the original input `false or true = true`. This makes more sense when we look up with `false or X = true`, where `X` is unknown. In this case, we give back `X = true`.

We can use this concept for sets later. From an ARC-AGI perspective, this is a crucial concept, as we want to find a transition between two values. If we want to find a transition from `false` to `true`, we can find those by looking it up in the `consequences` segment. For the record, we use an additional lookup table called `conversionTools` to map input types to output types.

We put the `selfBuilders` segment to a different lookup table. This segment describes the fact that the `OR` operation is symmetric. This will be useful to write mathematical equation solvers.

Here is a log from the program. The serialization uses `op push`, `op pop` and `op variable xx` control cells to be able to traverse the OP tree with values.

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
Here, the `get(cell, key) == value` expression can be "solved" to produce set(cell, key, value).

## Conclusion

Although this solution is nowhere near complete, I think it clearly shows a different approach to solve ARC-AGI tasks. It currently doesn't solve any real ARC-AGI test, but I think I am quite close to it. It can solve tasks simpler than ARC-AGI tasks, though. The program doesn't use a GPU, and the memory consumption tops out at 6.7Gb after 33 seconds of runtime for the biggest unit test. (CMake target: `arctest`). For the record, I submitted an empty solution to ARC-AGI-2 that scores 0 and does nothing; the submission name is `Solving ARC-AGI with InfoCells`.