# Information-cell theory
(work in progress draft)

## Abstract

This work presents a symbolic AI approach, commonly referred to as GOFAI ([Good Old-Fashioned Artificial Intelligence](https://en.wikipedia.org/wiki/GOFAI)). The central idea is a novel kind of programming language that does not rely on traditional syntax and instead exists primarily as a pure data structure. Because it lacks explicit syntax, it is not a programming language in the conventional sense; rather, it can be viewed as a programming material or a system of programmable particles. For simplicity and clarity, however, it is referred to here as a programming language. The language is [homoiconic](https://en.wikipedia.org/wiki/Homoiconicity), meaning that programs are represented using the same structures as the data they manipulate.

With the help of these cells we can easily add new grammar structures to this language. The main assumption is that current programming languages describe mostly parameterized executable commands (grammatically known as [imperative mood](https://en.wikipedia.org/wiki/Imperative_mood)) and lack the ability to express parameterized observations (grammatically known as [realis mood](https://en.wikipedia.org/wiki/Realis_mood)). Observations are made using the same commands created in imperative mood. A novel pattern-matching-based algorithm can connect the two grammatical moods, so we can synthetise regular programs from descriptions. We can also create other algorithms currently present only in neural networks, such as back propagation and image recognition. There is a proof of concept implementation for this theory in a fully debuggable C++ form.

We can describe a system where observer functions can describe the observed state of the system and the effects within it. We try to match the internal world model with the reality. We can use that internal world model to predict future states.

## A brief history

This project originally started in a gap year around 1998 or 1999. The data cell was the idea and I was able to create a C program to play with it. This design comes from the fact that I wanted to be able to create a reference for everything. For example in JSON you can reference items with JSONPath, or in XML with XPath. I didn't want to use a path-like reference, I wanted to direct reference for everything. Here in the infocell world everything has a unique address, although we can create relative path similar to a normal path if we need such a thing. In that time period I realized that I can use active cells also to describe a whole program with infocells. I wasn't able to create a fully working solution for active cells then due to lack of skills.

Years later I was able to implement the active cells, but the real breakthrough to use monad-like data cells for wrapping is only 10 years old maybe. I tried to create a new language for infocells, so I started studying parsers and programming languages. I wrote a very generic parser, just to realize that I do not really need it. I was able to embed infocells to the C++ language utilizing its flexible features. After that point I was able to create complex infocell structures, lists, maps, sets, and tries around 5 years ago.

So I started experimenting with algorithmic AI. I invented a simple problem to test how I could use infocells to solve it. I wrote down the numbers from 0 to 30 in one column "0 1 2 3", in the other column I wrote the written form of numbers, for example "zero one two three". The task was, to write a C program that is able to continue the columns. And I can test program creation capability by introducing different human languages, for example Hungarian or Chinese. There was a problem although, this problem was hard to invent and way too heavily relied on numbers. I started searching on the internet how can I find similar problems. The breakthrough was, when I googled program-synthesis. I found the first ARC challenge which was already over by one year. I read the [On the measure of intelligence by François Chollet](https://arxiv.org/abs/1911.01547) paper. It was a perfect match with my ideas. It was 2021.

I refocused my work to be able to create a program that solves ARC challenges. I created a whole language with infocells by introducing many concepts from C++ and Rust to infocells. I wrote template systems, methods, trait-like mechanism, enums, ... I wrote a whole compiler in it, so I separated the active cells from the language construct describer AST (here AST is not really abstract syntax tree, but I just refer to the "parsed format of a language" here) data cells. I was able to create and compile relatively complex programs in it. Running the infocells doesn't require a conventional stack anymore, every state of execution is inside the infocells. So theoretically I can pause the execution and save the state and later continue from there.

So after 5 years, in 2025 I had a fairly complex and well tested working infocell program execution environment that solve zero ARC challenge problems. It solves some sub problems although. But there was the DSL (domain specific language) problem. How can I describe the ARC grid and transformations? How do they interact? How can I describe anything?

The breakthrough was around 2025 August. Instead of describing already complex things, I tried to describe what is the most primitive instruction in the infocell world. What does the `SET(cell, key, value)` instruction do? Then I realized that I have to think in time. The world state before execution and the world state after execution and examine the state with other tools. So I realized, that after executing `SET(cell, key, value)` I can get the value from `cell` at slot `key`. So I can write `GET(cell, key) == value`. This is very similar to a unit test but I do not intend to execute it but I wanted pattern matching to this expression. I asked myself, what should I do to be able to get the "value" from "cell.key"?

We can describe the consequence of an action, and the plan is to write observer functions to observe "effects". And also we have reasons of why something happened. The reason getting a `true` value from a `HAS(cell, key)` functions is that `SET(cell, key, UNKNOWN value)` command was issued. What is the reason for a `HAS(cell, key) == TRUE` observation? A `SET(cell, key, UNKNOWN value)` command caused this.

I set a value so I can get it. The cell "stores" a value in a slot. What is the word "store" here? Store is a verb but not a command. So what does it mean  writing `STORE(cell, key, value)`? What if I rename the SET instruction to STORE? Actually in English I can say "CELL stores a VALUE in KEY" or I can say "CELL, store a value in KEY.". Same words but different grammar mood. One is describing a state, the other one is an executable command. We use the same words to observe the world and issuing commands. There is an extra `s` letter after the word `store` that indicates description, not instruction. I realized that the "a cell stores a value" is a description not an instruction. I can define the "store" word as something like `STORE(cell, key, value): GET(cell, key) == value`. The cell stores a value so you can get it. 

With this description grammar I can describe a task, or a prompt. An algorithm can find tools to fulfill the requirement. Basically it is an equation solving with pattern matching technique. Around 2026 February I was able to create some description grammars and use that segment to create simple prompts and solve those in a unit tested environment. There are limitations currently but looks like those are solvable but it takes time.

In short, the infocell as a data structure and executable cell as a concept is quite an old and established concept. The description - instruction duality is quite new idea and needs to be explored more.

## Introduction

The fundamental unit of the infocell theory is the information cell (infocell), which is the only primitive type in the language. As in LISP, code and data share a unified representation. Unlike LISP, however, the core structure here is an object composed of key–value pairs, where both keys and values are object references. This system can be described as an executable domain-specific language (DSL). Because the language has no native syntax, it must be embedded in a host language. The reference implementation uses C++, leveraging its operator overloading capabilities to express and manipulate infocells effectively.

![Representing an infocell](doc/diagrams/SampleCell.svg)

The following example illustrates this representation. The comment section contains regular C++ code snippet. The actual infocell representation follows below. The infocell code is also in C++ but it actually creates infocells at runtime.
```cpp
/*
void Index::remove(CellI& role)
{
    if (!m_type->hasSlot(role)) {
        return;
    }
    m_slots.erase(&role);
    m_type->removeSlot(role);
}
*/
indexStruct.addMethod("remove")
    .parameters(
        parameter("key", _(std.Cell)))
    .instructions(
        if_(not_(m_("struct")("hasSlot")("slotRole", p_("key"))))
            .then_(return_()),
        erase(self(), p_("key")),
        m_("struct")("removeSlot")("slotRole", p_("key")));
```

The above C++ code creates infocells. We can actually print those infocells and which can be rendered in the following pseudo-syntax. The infocell pseudo syntax is a mix of C++ and Rust. There is no parser for this syntax; currently, it can only be printed. The idea is that we embed the language to an existing one, so we do not need a new parser. This is similar to the DOM API in web development, where HTML nodes are constructed programmatically. In this case we create infocell representation nodes that can be compiled into executable code on the fly.

```
fn Index::remove(self: Index, p_key: Cell)
{
    if not(m_struct.hasSlot(slotRole: p_key)) then
        return;
    self.erase(p_key);
    m_struct.removeSlot(slotRole: p_key);
}
```


The primary advantage of this representation is that the infocell language can be extended easily, as it does not require a dedicated parser - it only requires implementing additional C++ helper methods. We extend the language by adding a second representational layer, described here as a new [grammatical mood](https://en.wikipedia.org/wiki/Grammatical_mood).

One of the cornerstones of this theory is the distinction between instructions and descriptions. Conventional code is treated as instructions, corresponding to the [imperative mood](https://en.wikipedia.org/wiki/Imperative_mood). In addition, we introduce another grammatical mood - which we refer to here as description, drawing on the notion of the [realis mood](https://en.wikipedia.org/wiki/Realis_mood) - which we call description. In natural languages, this mood is used to express statements of fact. In our system, the description section is used to express what changes after a sequence of instructions has executed (that is, what consequences follow), as well as to define the intended meaning or purpose of the data members within an infocell. We can also express why a given statement is true, for example if a data cell has a value in it, the reason is that somebody stored a value there earlier. We can express observations about what changed in time in both directions (what is true before and after).

In summary, most modern programming languages (such as C++, C#, TypeScript), with a few exceptions like Prolog, focus solely on instruction grammars written in the imperative mood. With infocells, we can implement all components related to instruction grammars and further expand by introducing a new grammar a description grammar, used to express observed states and causal relations, which uses the realis mood.

The conceptual framework behind the description section is to treat instructions as tools, which we call infotools. In conventional programming languages, we issue only commands that invoke tools. In the description section, however, getter functions are treated as measurement tools, and we record the results of those measurements. In other words, we describe observations. These observations structured record of observed facts about the system. Within this section, we specify the observed outcomes of tool usage.

Based on this mechanism, requirements, or prompts, in the terminology of contemporary AI systems-can be expressed as desired observations. Rather than prescribing how to produce a result, we describe how the final outcome is verified or observed. When generating an answer to a prompt, the guiding question is: “Which tool produces this observed effect?” The prompt can be decomposed into tool usages, and from these we can construct a tool composition that achieves the desired effect.

Measurements are expressed using the form `measurementTool(parameters) == result`. This representation is effectively a mathematical equation, which the system then solves. These equations can be extended with data-manipulation instructions. In this framework, mathematical functions are simply infotools whose behavior is defined by their observable effects.

The following example illustrates a description segment:

```cpp
    astScope.add<Struct>("Set")
        .memberOf(
            _(std.ast.Base))
        .description(
            equal(m_("cell") / m_("role"), m_("value")))
        .members(
            member("cell", "Base"),
            member("role", "Base"),
            member("value", "Base"));
```

Here we express the fact that after executing a primitive cell `Set` you can actually get the `value` from `cell.role`.

This description segment is also the basis for a prompt in the terminology of current AI systemss. For example:

```cpp
// It is true that I can get a number from an unknown cell `X` at `value` and after I add number 2 to it I get 4.
// so 2 + x = 4
Prompt:
Var& x = var_("x");
equal(add(_(_2_), _(x) / _(kb.ids.value)), _(_4_))
```
The generated answer is

```cpp
// x = 4 - 2
set(_(x), _(kb.ids.value), subtract(_(_4_), _(_2_)))
```

In effect, the prompt resembles a unit-test-like specification from which code can be generated. This answer-generation process may share structural similarities with transformer-style generation; I discuss this in more detail below.

Actually you can interpret even feelings as a measurement tool, for example `feel(ego, hungryLevel) == High`. You can register a tool that makes you feel low hunger, for example `eat(ego)` with an effect of `feel(ego, hungryLevel) == Low`.


With the help of these description structures we can actually get a pair of descriptions, which can represent a `before` and an `after` state, and ask: `What was changed?`. For example one description is a `before = Line(from: Point(x: 0, y: 0), to: Point(x:1, y:0)` the other one is `after = Line(from: Point(x: 0, y: 0), to: Point(x: 5, y: 0)` we can conclude that the change is `line.to.x * 5` or `line.to.x + 4`. If we have more examples we can narrow down the change to one of them. This is not implemented yet. It is the next planned feature.

The program incorporates a fundamental image recognition component specifically developed for the [ARC-AGI](https://arcprize.org/arc-agi) challenge. This module identifies `objects`, defined as groups of connected pixels sharing the same color. It features a basic edge detection algorithm capable of recognizing patterns of equality, rotation, mirroring, and scaling; however, the functionality to identify object subcomponents is not yet included. The algorithm has been implemented in C++, and, through the use of infocells, can be applied to real-time image recognition tasks, including scenarios that require runtime infocell processing. Further details regarding ARC-AGI and visual aspects are provided below.

## What is an infocell and how it works

### Data cells

An infocell is essentially an object just like an object in conventional object-oriented programming. It has a unique address and can contain key-value pairs of cell addresses. I call these key-value pairs slots. Here is a simple reference implementation in C++. This is the only primitive type that can exist, no native integer or real number, nothing beyond this primitive type.

In C++, an infocell is implemented as a class that stores pairs of pointers to instances of the same class. Essentially, it serves as a wrapper around `std::map`. While it resembles a mathematical set, each element in this context possesses a unique identifier, which itself is also a reference to another cell. Also the elements in a set are reference to another cell.

```cpp
class Cell
{
    std::map<Cell*, Cell*> m_slots;
};
```

As we defined a slot as "if a key exists for a slot, a value is also present" we can say that these data cells are forming a graph as we can go from a cell to another through the connection named `key`. To avoid confusion I usually use the `slotKey` and `slotValue` names in code for this reason. These are inseparable entangled references.

A data cell can store the result of a tool usage. An active cell (a tool) can modify data cells or itself. We can have a description segment in data-cells which can describe what is true also when that slot in the data-cell is present. For example in a container we can describe a `size` slot which contains a number in a way that we can state that this number is the result of a `count` algorithm.

Data cells on their own are very primitive key-value storage containers. They do not know what keys they contain. They can give back a value if you know the key. If you try to access a non-existent key it raises a hard error. If you want some kind of reflection, so iterating over the keys, other cells must be involved. For this reason every cell has a dedicated slot, called `struct` which points to a describer cell which contains a list data structure created from other cells.

Data cells are forming data structures. There is a mini standard library implemented already which contains the most popular data structures, namely lists, maps, sets, tries, vectors, and stacks. The vector structure is actually emulated with maps as no low-level memory access is possible and the address of an infocell is not accessible from standard instructions. So there is no such thing here as infocells right next to each other. In C++ we can get the address of an integer and do arithmetic on it, for example subtract or add a number which correspond to the size of an integer so we can go to the previous or next integer. We have to explicitly introduce a `next` or similar slot to be able to point to an other cell, so we can connect them. This connection is one way. If cell `A` has a `next` slot to `B`, `B` doesn't know anything about this fact. So we can go from `A` to `B` through the `next` connection but cannot go back. We have to introduce a `previous` or similar slot into `B` which have to point to `A` if we want two way connection.

There are "emulated" data structures in the system those are pretending that they were created from regular infocells, but actually we just emulate them from C++ code. This is the `hybrid` namespace in the code, these behave like adapters. Also every input sensor is represented by hybrid cells, for example an image sensor from C++ side is a regular std::vector but from inside the infocells looks like data structure created from infocells.

There are some ARC-AGI challenge-related cells in the hybrid namespace these represent ARC grid and colors that can be fed with a regular ARC-AGI json file, but can be read as an infocell data structure.

### Hybrid cells

We can create hybrid cells that pretend to be regular data cells but actually a bridge between a native C++ object and an infocell. We have to implement the following interface for it.

```cpp
class CellI
{
public:
    virtual bool has(CellI& role)               = 0;
    virtual void set(CellI& role, CellI& value) = 0;
    virtual void erase(CellI& role)             = 0;
    virtual CellI& operator[](CellI& role)      = 0;
```
The getter `operator[]` returns a slot value if there is a slot named `role`. For historical reasons, instead of key-value pair, I started using role:value. It uses C++ operator overloading to be able to write expressions like `dataCell[ids.list][ids.first][ids.value]` we can also use the `get()` method for it, but it is longer to write `cell.get(ids.list).get(ids.first).get(ids.value)`. From the infocell world we have to use an active cell which is of type `Get`. That active cell `Get` will call this `operator[]` method.

The `has(CellI& role)` will check that a slot exists for a given role. The active cell type `Has` will call this method.

The `set(CellI& role, CellI& value)` can change a slot value if we want. The active cell type `Set` will call this method.

The `void erase(CellI& role)` removes a slot named `role` if that slot exists. The active cell type `Erase` will call this method.

These 4 operations are the core functions for a data cell.

### Data cell reflection

A data cell is very primitive and does not know which slots it contains. We have to know in advance what are the available slots there. For this reason there is a dedicated slot in every data cell that contains a list of slots. In this diagram the `mapSlotList` is the list itself. That descriptor cell is not just a list; it contains a map-based structure that also stores the slot list. The reason for this design is that slot descriptors must also be accessible by key.

![Map type struct](doc/diagrams/MapTypeStruct.svg)

### Active cells / primitive tools

There are limited operations in this world. Currently there are around 30 primitive operations. In my mental framework these are the primitive tools that can form bigger, complex composite tools.

These are the following by category:

- core data handling
  - `Get` - get the value from a `cell` based on a `key`. If the key not found it is a hard error, program execution stops. The `value` slot will contain the result.
  - `Set` - set the `key` and `value` on a `cell` without any condition. If `key` doesn't exist it will create one. If `key` exists corresponding `value` will be overwritten.
  - `Has` - check that the `key` exists in a cell. The `value` slot will contains a `True` or `False` based on the check.
  - `Missing` - the negate of the `Has` operation, just for convenient
  - `Erase` - delete a slot based on `key` from a `cell` without any condition. If `key` doesn't exists in `cell` that is ok, nothing will happen.
  - `New` - create a new cell. A cell type must be given. The created cell will be in the `value` slot. Must be inserted with a `Set` to another cell otherwise it is a leak.
  - `Delete` - deletes a cell. If cell references are found in other cells it is undefined behavior. If the host language is garbage collected, then this is a no op.
  - `Same` - compare infocells based on unique address
  - `NotSame` - negate of `Same`
  - `Equal` - compare infocells based on content
  - `NotEqual` - negation of `Equal`

- code running / infocell activation
  - `Activate` - activates a `cell`
  - `Call` - call a function, requires a stack also and save / restore local variables on it, can be created from other cells but this is here for debug purposes
  - `Function` - start a function, but first activates the input cells, can be created from other cells but this is here for debug purposes
  - `If` - a standard if operator
  - `Do` - a standard `do` .. `while` loop
  - `While` - a standard `while` loop
  - `Block` - a standard code block, emulates a line by line execution
  - `Return` - return from a code block

- math related
  - `LessThan` - compare two number with operator `<`
  - `LessThanOrEqual` - compare two number with operator `<=`
  - `GreaterThan`- compare two number with operator `>`
  - `GreaterThanOrEqual`- compare two number with operator `>=`
  - `Add` - `+`
  - `Subtract` - `-`
  - `Multiply` - `*`
  - `Divide` - `/`

There is a clear distinction between data cells and active cells (those act like assembly instructions). An active cell can be activated, and it can cause a state change, but a regular data cell does nothing after activation, there isn't any observable state change after it. It is not an error if we activate (in other words execute) a data cell. It is a well-defined behavior, namely the cell does nothing. It acts like an insulator where the active cells are the conductors. The activation is very similar to executing an ASM instruction in a regular processor. Here, the instruction is not executed by a central processing unit; instead, the cell itself acts as a mini-CPU. So, there are no central registers or there is no stack or instruction pointer. EEvery active cell is responsible for tracking who activated it and those cells must give back the activation. These instructions can be composed just like in a regular programming language.

We can compose these primitive tools through an activation process that resembles a monadic composition model. An active cell always gets the input value from another cell's `value` slot. When an active cell gives back a result it creates or updates a `value` slot inside the cell. For example, an `Add` cell which adds two numbers together has two input slots `lhs` and `rhs` where `lhs` is the abbreviation of "left hand side" and rhs is "right hand side". After activation a `value` slot can be accessed. So in this system we do not have a stack, every instruction has its own result register. After the `value` slot appears the activation goes back to the cell which activated it. So every active cell has a built-in instruction register (also called this register as program counter PC on some CPUs). In practice every active cell has a slot called `previous` which points to the cell where the activation arrived. And there is a slot `state` which represents a state machine state. If the state reaches the last state the cell gives back the activation to the cells referenced in the slot `previous`.

Accessing data members or slot values is possible with the `Get` cell. It expects a `cell` input parameter and currently called `role`. Why not `key`? Well the code was rewritten many times. My thought process was the following. I must name two cells in a slot. Every slot has a unique role so call it `role` and every role has a value call it then `value`. If we have a data cell called `pixel` and it has `x` role in it with a value `1`. We can get the value `1` with a `Get` instruction. `Get(cell: pixel, role: x)` after activation the `Get` instruction creates a new slot, called `value` with a value of `1`.

### Composing primitive cells

The primitive tools are the same concept as assembly language for a processor. To be able to create more complex things we need a language for it. There is a language defined inside the infocell world which is defined by data cells and not by syntax. So this language doesn't have a syntax but I actually created an ad-hoc one just to be able to print out but I never tried to parse it back. The language actually consists of intermediate representation (AST - abstract semantic tree) nodes, similar to the popular compiler Clang (see the [Declarations](https://clang.llvm.org/doxygen/classclang_1_1Decl.html) or [Statements](https://clang.llvm.org/doxygen/classclang_1_1Stmt.html) classes).

This pseudo language actually contains many C++ and Rust concepts and there is an internal compiler that can compile this language to primitive cells. This compiler is currently written in C++ but the data members are hybrid infocells. In this language we have namespaces, classes, and methods, template system and a small standard library supporting Numbers, Strings, Lists, Maps, ...

The plan is to attach a "description" section to all composed tools to use it to inspect or modify objects. For tool usage the Rust trait-like system looks a better fit than a pure OOP concept. The main difference is that inheriting data members are not allowed.

## Pattern matching based tool usage system

How to define primitive tools? With other primitive tools!

In the previous chapters the tool concept was introduced. We have the ingredients:
- a description of a result which is the requirement (or prompt in current AI terminology)
- the description of the effects of the available tools

Let's say we have a requirement (in other words request or prompt): `pixel.get(green) == 5` where the `pixel` cell exists. So our request is that we want to retrieve from the cell `pixel` a slot called `green` and the expected slot value in this case is `5`. In order to make the equation true, we need two things.

![Requirement example](doc/diagrams/RequestExample.svg)

First we need to find a tool whose effect is equivalent to the equation itself. Remember we described the tool's effect with equations. We need a "database" of effects for tools and we just look up the requirement in that database to find a tool where the tool effect equals the requirement. The idea here is we register the tool, in this case the `CELL.set(KEY, VALUE)` tool with an effect of `CELL.get(KEY) == VALUE`. With this Prolog syntax the CAPITAL words represent variables. In this case the `CELL` variable is pixel, the `KEY` is green and the `VALUE` is `5`. We have a perfect match for the original request.

Second, we need a tool synthesizer or tool builder functionality here. We matched the tool `CELL.set(KEY, VALUE)` and we have to synthesize the concrete non-parameterized tool which is `pixel.set(green, 5)`.

![Requirement example](doc/diagrams/ResultExample.svg)

For database lookup the idea is as follows. We create a data structure and we use the self reflection capability of the data cells. For the original request we use two main cells: the `ast.Equal` and `ast.Get`. For the monadic parameter wrappers we use extra three data cells, those were not represented on the above diagram for clarity reasons. The purpose of those omitted cells is to provide a `value` slot for the active cells as active cells are only able to get values from a `value` slot. Here is the full picture, but representing those grey `ast.Cell` type cells just add more confusion than clarity so I usually do not represent them.

![Requirement example with wrappers](doc/diagrams/RequestExampleWithWrappers.svg)

The first cell in this example is called `requestForSet`, it has 3 slots. First slot in there is the describer slot, called `struct` points to the describer cell `ast.Equal` which contains a list of possible slots for the cell. The second and third are the `ids.lhs` and `ids.rhs` slots. The left hand side slot points to the `ast.Get` and the right hand side of the equal node points to the wrapped value of `5`. We look up cells only by content, since there is no dedicated `name` field anywhere. A cell only contains key-value pairs. If want to lookup a cell we have to know those key-value pairs. We want to find a tool which has an effect that match with this request. So we have to register matchers to our database. The matcher is a regex like mechanism but for infocell representation nodes. The idea is similar to the [Clang AST matchers](https://clang.llvm.org/docs/LibASTMatchersReference.html).


We can establish a standard way of serializing data cells. For example serializing the content of request cells, it starts with the first slot called `struct` and the slot value is `ast.Equal`. We first "print out" the key, then the corresponding value. Imagine putting these cell references to a list, where the first value is `struct` the second is `ast.Equal`. For simplicity I just separate those references with a simple space now. So we start with `struct ast.Equal` this represents the first slot in `requestForSet`. For `ids.lhs` we have a problem, as we have to describe a sub-cell by content. For this reason a command node is introduced which instructing the lookup algorithm to go inside the value cell and start matching content there. So we continue the serialization with a key node and a command: `ids.lhs op push`. Which tells the lookup algorithm to go to the pointed cell in `ids.lhs` and start matching from there. The `ast.Get` node only contains wrapped constants. For simplicity the wrapper cells named the same as the actual values here, so the serialization of `ast.Get` is `struct ast.Get ids.cell pixel ids.role ids.green`. Here we have to go back to ast.Equal cell to be able to match with the slot `ids.rhs` so we need a command to "go back". For this reason a `op pop` nodes are created. Then we can finish the serialization with the ids.rhs slot as follows: `ids.rhs 5`. The full serialization is the following: `struct ast.Equal lhs op push struct ast.Get cell pixel role green op pop rhs 5`. So we can represent our cell structure in a list. In this case we need 15 cells for it.

Based on this serialization method, we can create ast matchers with the help of variables. The serialized request above contains very specific values. To describe a tool effect, we have to express the fact that whatever the slot value in `ids.lhs` or `ids.rhs` slots it is a match. To make the above structure generic we can just replace those specific values with a generic expression called `op variable`. And that is it! We have a matcher. `struct ast.Equal lhs op push struct ast.Get cell op variable role op variable op pop rhs op variable`. So we replaced the specific value `pixel` with a generic variable `op variable`. But how we can know which variable is the CELL, KEY or VALUE if we named those as `op variable`?

We need another structure which describes how we can build a tool from a matched structure. We utilize the same serialized format but in reverse. The first value will be the key but the second is interpreted as a "path from matched root node". Imagine we matched with the original request, where the root node is `requestForSet`. We have to express the fact that the CELL variable is available from the following path `ids.lhs ids.cell`. As the `ids.lhs` slot points to the `ast.Get` cell where there is the `ids.cell` slot. And we need that cell to fill the CELL variable. The KEY is available from path `ids.lhs ids.role`. The VALUE is available from the root cell `requestForSet` from slot `ids.rhs`. This is the builder structure that is represented on the diagram.

What about more complex requirements. What if I put extra nodes to the requirement?

Let's take the following request: `get(get(currentTheme, std.Color), green) == 5`. We actually do find the same set tool `CELL.set(KEY, VALUE)`, but here the CELL variable is not a data cell. For an active cell we need a tool which has an effect of `get(currentTheme, std.Color)`. But this is not exactly what we want here. We actually want a cell that returns a value, so we can get the input from a `value` slot here. That is why we start looking for a tool which returns a value, so it is a measurement tool `return get(currentTheme, std.Color)`.


```cpp
    astScope.add<Struct>("Get")
        .memberOf(
            _(std.ast.Base))
        .description(
            return_(m_("cell") / m_("role")))
        .members(
            member("cell", "Base"),
            member("role", "Base"));
```

How to deal with math functions? How can we express what is `1 + 2`? In the infocell world those are regular tools. We can define a connection between the input parameters and the return value. We can use the `subtract` tool to "measure" the effect. The effect is in the return value, so we can state, if we subtract the rhs input value from the return value we got the lhs input value, in short the effect of `lhs + rhs == return` is `return - rhs == lhs`. And also we have to register the `add` tool as a measurement tool 
`return rhs + lhs`. So a math tool is a dual-purpose tool.

There is a side effect of putting the return value to the effect description. Let's say the request is `x.get(value) - 2 == 1` where `x` is an empty cell. Here we find the tool `add` with effect `return - rhs == lhs`, because that is a perfect match equation. But what is `return` here? Here we have to unify ([unification](https://en.wikipedia.org/wiki/Unification_(computer_science)) is a core Prolog algorithm) the `return` with `x.get(value)` so we end up with the equation of `x.get(value) == 2 + 1` and we can find the tool set for this, where CELL is `x` KEY is `value` and VALUE is `return 2 + 1`.

There are some other issues here that we need to address here, for example how to handle the fact that `measurementTool(parameters) == result` is the same as `result == measurementTool(parameters)`. Here we can register both sides of the equation to the tool finder trie database or we can do a double lookup. Another problem is that we have to introduce the constness to symbols as for math equation the tool finder can find a solution where it tries to rewrite the number `2` to `1` which is a bad solution.

```cpp
    astScope.add<Struct>("Add")
        .memberOf(
            _(std.ast.Base))
        .description(
            equal(subtract(return_(), m_("rhs")), m_("lhs")),
            equal(subtract(return_(), m_("lhs")), m_("rhs")),
            return_(add(m_("lhs"), m_("rhs"))),
            return_(add(m_("rhs"), m_("lhs"))))
        .members(
            member("lhs", "std::Number"),
            member("rhs", "std::Number"));
```

How to deal with multi line request? Let's say given an empty list the request is
  1. every list item value is `1`
  1. the length of the list is `3`

What makes this requirement true? To "solve" this we have to insert the element `1` three times to the list. There are some non-implemented approach to this also.

![Pattern matching based tool usage system](doc/diagrams/ToolFinder.svg)


# Pattern matching based image recognition

The image recognition currently focuses on [ARC-AGI](https://arcprize.org/) grids.

![A sample ARC grid](doc/diagrams/ArcGridSample.svg)

The ARC grid input is a JSON file and as a first step we have to bring it to the infocell world. Here is the internal infocell representation of a grid.

![Representation of a sample ARC grid with infocells](doc/diagrams/ArcGridSampleWithInfocells.svg)

Image recognition is currently based mostly on edge detection. First we calculate an edge of a same-colored pixel "patch". Then we get the unit vectors of that edge (for a pixel there are only four unit vectors toUp, toRight, toDown, toLeft). This approach basically the serialization of the edge so we can put it in a trie lookup structure the same way as the tool finding works. The external edge vectors point clockwise and the internal edges are counter clockwise.

Edge detection is based on detecting which pixel configuration starts an external or internal edge. We always examine only 4 pixels at a time on a 2x2 area. The external edge is always around a same colored pixel blob. If that pixel blob contains holes, around the holes are the internal edges for that blob. But there will be a corresponding external edge for the blob which fits the hole.

![External edges around two pixel blob on a 3 x 3 pixel grid ](doc/diagrams/PixelConfigurationOverview.svg)

The edge detection algorithm starts with detecting the start of an edge, based on 2 x 2 moving window. If we detect the start of an edge we try to continue that edge. The edge continuation is based on a similar approach. The same 2 x 2 window is checked but with different pixel patterns.

![Edge detection is based on local pattern matching](doc/diagrams/PixelConfigurationForEdgeStart.svg)

# Discussion and Future Work

Infocell theory proposes a unified framework for symbolic computation, program synthesis, and perception based on a single primitive abstraction. By treating instructions as tools and descriptions as observable facts, the system bridges declarative and imperative paradigms in a way that enables reasoning over programs themselves.

Planned future work includes:

- Automated change inference between before/after descriptions
- Constraint narrowing using multiple examples
- Expanded image decomposition capabilities
- Performance optimization and scalability analysis
- Formalization of the pattern‑matching algorithm

# Conclusion

This work introduces information‑cell theory as a syntax‑free, description‑driven programming model. By unifying code, data, and execution state within a single reflective structure, the system offers a novel perspective on symbolic AI and program synthesis. While still experimental, the framework demonstrates that many capabilities commonly associated with neural architectures can emerge from purely symbolic mechanisms when equipped with sufficient introspection, pattern matching, and a descriptive grammar.
