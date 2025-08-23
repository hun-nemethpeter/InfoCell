## World model of an ARC picture


It is all about one picture. In an ARC task it is in input - output pairs.

So one picture consist of pixels on a 2D plane. These pixels have colors, the ARC dataset define 10 colors.

We can put an ARC pixel to a Cartisian coordinate system just by using the pixel's corner points and its edges.

I choose the origo to be the up-left corner, for easier interaction with the SVG/HTML world.

The X is the horizontal axis, starting from 0, increasing from left to right. Every number on this axis represent a distance which is equal to the length of an ARC pixel.
The Y is the vertical axis, starting from zero, increasing from top to bottom. And as in the X axis, every number on this axis represent a distance which is equal to the length of an ARC pixel.

```
   X
  ∙──►
Y │
  ▼
```

A pixel has an edge, as a square has for sides we can say that a pixel has 4 edge-segment


A pixel actually an area between 4 points.

Objects

- 2D plane
- grid

    it is a 2D plane, devided by horizontal and vertical lines
    a grid contains pixels. It has an X axis and a Y one. The origo is in the up-left corner.

- pixel

    a pixel is colored square area. It has for corners, 4 sides (edge segments) and one edge. The 4 corner points can be put on a Cartisian cordinate-system

- edge
  - edge segment
- coordinate-system
    - X axis
    - Y axis
    - origo
- point

    it has X axis number value and a Y one.

- line

    a line can be defined by two point on a 2D plane

- area
- number // can represent quantity
    - digit
- quantity // can represented by number
- counting // the act of quantify things

A thing has a quantity of 1
An infocell represent a thing

Counting is a method to get the quantity (number of elements)
  - What is it?
      - a process
      - Counting is the process of determining the number of elements in a set or group.
        count(something (an item description), somewhere (a collection of item)) -> Number (represent quantity)
  - How do you do it?
      - It involves assigning a numerical value to each item in the set, starting from 1 and proceeding sequentially (1, 2, 3, etc.) until all items are accounted for.

We need a language

Question - need some info
Instruction - do something
Describe - tell me about something

We have to our infocell's anatomy. For example in a list, which key is part of the data structure and which one holds a value.


So, instead of writing codes, we should define the problem or a task.

We should take a simple example with the most simplest data structure, for example a list.

We should define what a list is. And also define what an infoCell is.

We should create an agent and teach it to be able to write a program from a task.

Program p = agent.writeProgram(task)

- ok, but what if the task itself is to write a program?

What is a task?

So our starting world model is simple.

We have numbers, number line, list, info cell (data cell, op cell)
Numbers can be compared, added, subtracted, multiplied, divided

InfoCells can represent a real world thing.

Every program needs a requirement.

Every requirement can be translated to human language.

Every program which is generated from a requirement must be explainable.

Real world things -> Info cell represantations

We need an agent which can access to the info cell represantations and it can be feed with tasks

The agent is a program in InfoCell -> there is a circle here as the agent is a program, so we have to write it.

We have to teach the agent to able to create / use datastructures.

We have to define what is a task / requirement / question / command.


InfoCell
--------

 - can represent real world things
 - can have properties we call it slots, the properties are actually represent connections between InfoCells
 - one slot consist of two InfoCell reference
     - the first one is called role, it is just to uniqly identify the slot
     - the second one is called value

 -  What we can do with it?
     - We can query if it has a slot with a role
     - We can get the value of a slot by asking the role
     - If this InfoCell is an operation type InfoCell we can activate / evaluate it. After activating the slot identified by "result" will holds the result of the activation

Data structure
--------------

- we can only use InfoCells

Agent
-----

We need an agent who execute requests from us

 - has to know his abilities
 - able to handle numbers
 - create and handle data structures
 - 

Request
-------

Can contains a query, a task or command

 - Answer tha following question, ...
 - Imagine that, ... so we can create a scenario where things are referenced in an arbitrary way ...
 - Learn that, ... so we actually will modify the knowledge base
 - Write a program that satisfy the following requirement, ...
   - write is an alias for create in context of program

Create request
--------------

In this case the agent instructed to create InfoCells. If not specified otherwise, the result will be in the answer.

Create
 - what: a thing that is defined
 - into (optional): the place where the result will be placed


Requirement
-----------

A detailed description about what kind of program we expect


Knowledge base
==============

Things we know

Numbers
-------

- can be compared, added, subtracted, multiplied, divided
- A szám elején lehet az előjel. Akkor rakjuk ki, ha szám negatív
- A szám számjegyekből áll
- 

2D space
--------

- Cartesian coordinate system

Number line
-----------

Can represent relations between numbers


SVG canvas
----------

Output graphics

ARC Task
--------

- Contains input / output pairs of grids and one test input grid



List
----

isA(dataStructure)
purpose()
isA(collection) as other elements are reachable from this InfoCell
For every element it store we assign one structural IncoCell to hold a value
The first structural element is available from this InfoCell
The last structural element is available from this InfoCell

These structural nodes are connected, so that always reachable the next and previous item if such a thing exists.

Real world references
---------------------

ConsistOf(<List<Thing>>)
Name(string)

Quantity()

InfoCell
--------

    name("InfoCell")
    isA(this, element)
    isA(this, dataStructure)
    purpose(this, can(represent(anything)))
    Can(ConsistOf(InfoCell::Member*))

    can be modified
    can be created
    can be deleted

    It can contain InfoCell reference pairs called members // currently slots
    An InfoCell always exists as a refernce in an other InfoCell's member

    In other words:
    The IncoCell can have properties. A property inside an IncoCell can be identified by its name.
    A Property of InfoCell consist of
      - a name which is an InfoCell reference
      - a value which is an InfoCell reference

    An InfoCell property can be also seen as a path to other InfoCells, where the property name is the name of the road.
    We can also say that the property value is reachabale from this InfoCell through the property name.
    Every value InfoCell is directly reachabale. Or we can say that reachable in one step. Where one step is a Get(this cell, property name) which leads to property.value.

can directly connect other InfoCells

    // core data handling
    Get(cell, index)        // get the referenced cell from a connection
    Set(cell, index, value) // create a connection with "index" between the cell and the referenced cell
                               Set the "index" connection of "cell" to "ref".
                               set(object="index" connection of "cell", target=ref)
                               or
                               Set the "name" property of "cell" to "value".

    Has(cell, index)        // check if a "cell" has connection with "index"
    Missing(cell, index)    // check if a "cell" has no connection with "index"
    Erase(cell, index)      // erase a connection by index from a cell
    New
    Delete(cell)

    // code running
    Activate(cell)          // an op to activate other cells
    Call()                  // Call a function
    Function                // it is just a compiled function, that can be activated
    Return                  // a dedicated Activate like op for easier return handling

    // compare
    Same
    NotSame
    Equal
    NotEqual
    GreaterThan
    GreaterThanOrEqual
    LessThan
    LessThanOrEqual

    // logic
    And
    Or
    Not

    // branching
    If
    Do
    While
    Block

    // math
    Add
    Subtract
    Multiply
    Divide

Element
-------

By adding something to a container it will be an element inside a container.
An element can be anything inside a collection /container.

Collection / container
----------------------

Is a data-structure where an InfoCell holds other InfoCells by the act of adding algorithm.


When multiple other InfoCell can be reacable from a InfoCell, it is a collection.
When an InfoCell hold representations for multiple things then it is a collection
can contains elements or collections
if there is no element in this collection then the collection is empty


Concept of sequence
===================

actors:
    element1, element2 ... elementN to store
    head is optional to store additional info
    items are optional to create connections between elements and to connect to elements

    The goal is to able to reach every element once from a single InfoCell and
    it is known how to get a next element from a first one and
    it is known how to recognize the last element.

If we know the first element then we have to know the next element if exist and we have to identify the last element
If we know the last element rhen we have to know the previous element if exeist and we have to identify the last element

So the next is the opposite direction of previous
So the previous is the opposite direction of next
So the first is the opposite position of last
So the last is the opposite position of first

consequence(there is only one route to traverse the elements)

    if colllection is empty:
        the first elemnent is unknown
        the last elemnent is unknown
    else:
        the first elemnent is known
        the first element doesn't have previous element
        the last elemnent is known
        the last element doesn't have next element
        there is always a next element from the first one until the last one
        there is always a previous element from the last one until the first one

Algorithm
=========

Contains intructions

Counting
========

counting = description(isA(algorith()), )

Is an algorithm

Inputs
 - "the": the element we want to count
 - "in": the container where the elements are

Output:
 - size of the container

Counting elements in a container is the process where
- we are creating a counter and we set it to zero on start.
- we are iterating over on every item in the datastructure exactly once and on every iteration we increase the counter by 1.
- the last value is the counter is the result which is represent the amount number of that datastructure

Quantity
========

Is an integer number representing the number of something.

Traversing a datastructure
==========================

Traversing a datastructure is the process where
- we get every element of the datastructure exactly once
- after we get the element we can put it to a local variable to be able to do something with it

Function
========

function = description(

A function can be specified by
 - input parameters if there is any
 - ouput parameters if there is any
 - an algorithm which specify what steps needs to be done with the input parameters and to reach output if it exist

Single linked list
==================

singleLinkedList = description(isA(container()), )

// for every given InfoCell there is a new 
auto& formation = addFormation("List").
    given(InfoCell("x").amount().zeroOrMore(), InfoCell("n"), InfoCell("head"))

The single linked list formation consist of 3 types of cells

 - The list head cell
   
   can refer to many cells by providing a one-way route to them and this cell directly connected to the first cell of the one-way route formation if exist.

   The size of the list is directly reachable from this cell. This value is the same as the amount of target cells.

 - The list node cells

   are forming a one-way route formation to the target cells. One node cell is directly connected to one target cell.

 - The list target cells

   The target cells are the cells that we want to refer to with the list head cell.

We can approach this problem with the idea of "everything is a tool". Yes, this data-structure is a tool.
And a tool purpose, with other word goal is to solve a problem.
Every infocell and even every connection can be considered a tool that solve a problem. When a tool solve a problem it provide a feature / a function.

For example a connection provide access. A Get infocell actually do access to an other info cell.

With this approach we can say, that a Single linked list formation provide ordered access to InfoCells with the help of

- a list head cell that represent the list itself
- the node cells help creating order and provide access to the target cells
- the target cells are the cells where we want ordered access to.

The list head cell provide indirect access to the target cells by
   using a connection named First to the first cell of an an Ordered Cells Formation which
     connects the target cells in the required order.

Is a container.

Connection named "first" is connected to the first element of the data-structure if exist.

When a data-structure description
 - define a start element and
 - there is a way to decide what is the last element and
 - the only way to get from one element to the next element is through a connection with a specific name.

then those elements are forming a single linked list

A list define an order between items by creating an extra node for every item and these extra items are connected with a common connection.
This way the items can only visited through these extra nodes. The extra nodes are needed when the items can not be modified.

A list is a multi cell data structure (a formation). There are three main cell type in this structure:

- the entry cell, also know as head of the list
- the connector cells that are connected with a common connection named "next" and evey connector cell connected to one value cell.
- the value cells, these are the cells that the list "contains"

What is it, why it exist, when to use, how it works

The idea here is to describe every cells and every cell connection for a structure. So this description is the anwer for the question of WHAT.

The goal is to create a structure which can holds other cells.

Holding is a synonim for reachable in the context of structure.


An other approach:


A list tool consist of three types of cells.
This is a passive tool. Can read with an active tool.

The list-head cells provide access to other cells.
It solve the problem of "how to represent multiple cells with one."
Can be useful in a case where we need to remeber cells in a given order.

There are two possible solution here.
- First solution is, where the other cells directly connected to a head cell. In this case the order can be defined by numbering the connections.
  So the first cells is available with throuh connection one. The second item is available through connection two. And so on.
- the second solution is where we create a new connectior cell for every cells we want to store. In this case the order can be defined with a common connection name, usualy called "next".
  In this case the cell-head directly connected to the first connector cell through connection first. The connector cells are connected with connection next. The cells we want to store directly connected to the connector cells through connection value.

In prolog:
connected(head, listNode1).
connected(listNode1, value1).
connected(listNode1, listNode2).
connected(listNode2, value2).

accesible(X, Y) :- connected(X, Y), !.
accesible(X, Y) :- connected(X, Z), accesible(Z, Y).

A connection

TODO:
Double linked list
Grid


Example scenario:

Write a function that is counting the elements of a single linked list.

request().create(function().spec(counting().the(element()).in(singleLinkedList())))