2025-09-07
==========

Idea behind a blog format is to be able to write down ideas as fast as possible without orginizing it to prevent forgetting.

Current problem is how to do the description of infocells.

Set
---

In case of a SET(cell, key, value) the description can be that it make it possible to do a GET(cell, key) and the value of GET will be value.

So I can create an index struct 

let m_cell is an InfoCell input
let m_key is an InfoCell input
let m_value is an InfoCell input

SAME(lhs: GET(cell: m_cell, key: m_key), rhs: m_value)

Get
---

In case of GET(cell, key) the description can be that this().value will be the same as cell.key
let m_cell is an InfoCell input
let m_key is an InfoCell input
let m_value is an InfoCell output

GET(cell: m_cell,  key: m_key)
SAME(lhs: GET(cell: m_cell, key: m_key), rhs: m_value)

But what about EQUAL and SAME?

Request (or maybe expectations?)
-------

So a I want to write a request where only the end result specified. For example

let the color of the LED is green.

The LED is in kb.output.led. The green is in kb.colors.green.

So with ASTs I can write

SAME(GET(_(kb.output.led), _(kb.ids.color)), _(kb.colors.green))


2025-09-14
==========

Ok so I went to a direction of sorting out the for loop as I want see how can handle multiple values in a container.

The Rust way:

pub trait IntoIterator {
    type Item;
    type IntoIter: Iterator<Item = Self::Item>;

    // Required method
    fn into_iter(self) -> Self::IntoIter;
}

pub trait Iterator {
    type Item;
    ...
}

impl<T, A: Allocator> IntoIterator for LinkedList<T, A> {
    type Item = T;
    type IntoIter = Iter<T>;

    fn into_iter(self) -> Iter<T> {
        self.iter()
    }
}

impl<T, A: Allocator> LinkedList<T, A> {

    pub fn iter(&self) -> Iter<'_, T> {
        Iter { head: self.head, tail: self.tail, len: self.len, marker: PhantomData }
    }

}

pub struct Iter<T> {
    head: Node<T>,
    tail: Node<T>,
    len: usize
}

impl<'a, T> Iterator for Iter<'a, T> {
    type Item = &'a T;

    #[inline]
    fn next(&mut self) -> Option<&'a T> {
      ...
    }

}

My idea

    List<T> has trait Iterable

    so

	var list = List<int>(); // for a list instance
	var iteratorForList = cast<Iterable>(list).iterator(); // we can have an iterator


	trait Iterable {
        type Iterator : Iterator;

        fn iterator() -> Self::Iterator;
    }

    trait Iterator
    {
        type ValueType;

        bool isEmpty();
        void setFirstValue();
        Self::ValueType getCurrentValue();
        bool hasNextValue();
        void setNextValue();
    }

2025-09-14
==========

Ok, this should work. A trait doesn't contains member definitions.

struct Map {
    traits Container; // this will be registered when impl<T> Container for List<T>
}

2025-09-23
==========

Ok, the trait concept just do the trick. I can go back to sort out the InfoTool concept. So the ide here is that an InfoTool is a tool that knows it's own effect.
The effect is written down by description. So the problem how to do this descrition. If I have a description language I can write down the effect that I want to achive
and now I can find InfoTools that do these effects.

I playing with the idea to create a Storage trait. And also I need a description of the InfoCell somehow.

The InfoCell

- can store slots
- contains an address which is an opaque type


The Slot can contains

- a name which is an InfoCellAddress
- a value which is an InfoCellAddress

```
trait Storage {
    type Item;

    fn store(item: Self::Item)
       description {
          effect {
             after(self.contains(item) == true);
          }
       }

    fn remove(item: Self::Item)
       description {
          effect {
             after(self.contains(item) == false);
          }
       }

    fn isEmpty() -> bool
       description {
           // no state change so no effect block
           match (return) {
             case true:
              var item: Self::Item;
              self.contains(item) == false;
             case false:
              var item: Self::Item;
              self.contains(item) == true;
           }
       }

    fn contains(item: Self::Item) -> bool
       description {
           // no state change so no effect block
           if (return == true) {
              before(store(item));
           } else {
              before(remove(item)) or never(store(item));
           }
       }

    fn get() -> Self::Item {
    }
}

trait AssociativeStorage {
    type Key;
    type Value;

    fn store(key: Self::Key, value: Self::Value)
       description {
          effect {
             after(self.hasKey(key) == true);
             after(self.getValue(key) == value);
          }
       }

    fn remove(key: Self::Key)
       description {
          effect {
             after(self.hasKey(key) == false);
          }
       }

    fn isEmpty() -> bool
       description {
           // no state change so no effect block
           if (return == true) {
              var item: Self::Item; // so any item
              self.contains(item) == false;
           }
       }

    fn hasKey(key: Self::Key) -> bool
       description {
           // no state change so no effect block
           var value: Unknown;
           if (return == true) {
              before(store(key, value));
           } else {
              before(remove(key)) or never(store(key, value));
           }
       }

    fn getValue(key: Self::Key) -> Self::Value
       description {
          if (hasKey(key) == false) {
             #error
          }
          var value: Unknown;
          before(store(key, value));

          return == value;
       }
}

```

The trait Storage is implemented for Slot and InfoCell also.


2025-09-25
==========

1. still not fully satisfied with the descriptions. I am thinking on the microcode way, so creating a descriptor thing just
   to explcitliy stating the what cell and what slot changed from this to that. So this will be some kind of transition language.

1. Maybe I should use the "return" or "result" names instead of "value". There is name collision with SET. There we have cell, name (role, key), value.
   But that value is not a result value.

```
concept Storage {
    type Item;
    enum State { Empty, Store }; // when capacity is 1

    premise store(item: Self::Item) {
        self.contains(item) == true;
        self.get() == item;
        reason(self.store(item));
    }

    premise missing(item: Self::Item) {
        self.contains(item) == false;
        reason(self.remove(item) or never(self.store(item)));
    }

    premise empty() {
        any_var item;
        self.contains(item) == false;
        reason(self.remove(item) or never(self.store(item)));
    }

    fn store(item: Self::Item)
       description {
          consequence(premise_(store(item)));
       }

    fn remove(item: Self::Item)
       description {
          consequence(premise_(empty()) and premise_(missing(item)));
       }

    fn getState() -> State
       description {
           var item = return_();
           match item {
             State::Empty => premise_(empty());
             State::Store => premise_(store(item));
           }
       }

    fn contains(item: Self::Item) -> bool
       description {
           match return_() {
               true => premise_(store(item));
               false => premise_(missing(item));
           }
       }

    fn get() -> Self::Item {
       description {
           if (premise_(empty())) {
              #error
           }
           var item;
           premise_(store(item));
           return_() == item;
       }
    }
}
```


2025-09-27
==========

```
concept Storage {
    type Item;
    enum State { Empty, Store }; // when capacity is 1

    statement store(item: Self::Item) {
        self.contains(item) == true;
        self.get() == item;
        reason(self.store(item));
    }

    statement missing(item: Self::Item) {
        self.contains(item) == false;
        reason(self.remove(item) or never(self.store(item)));
    }

    statement empty() {
        any_var item;
        self.contains(item) == false;
        reason(self.remove(item) or never(self.store(item)));
    }

    fn store(item: Self::Item)
       description {
          consequence(statement_(store(item)));
       }

    fn remove(item: Self::Item)
       description {
          consequence(statement_(empty()) and statement_(missing(item)));
       }

    fn getState() -> State
       description {
           any_var item;
           match return_() {
             State::Empty => statement_(empty());
             State::Store => statement_(store(item));
           }
       }

    fn contains(item: Self::Item) -> bool
       description {
           match return_() {
               true => statement_(store(item));
               false => statement_(missing(item));
           }
       }

    fn get() -> Self::Item {
       description {
           if (statement_(empty())) {
              #error
           }
           var item;
           statement_(store(item));
           return_() == item;
       }
    }
}
```

```
concept AssociativeStorage {
    type Key;
    type Value;
    enum State { Empty = empty(), Store = !empty() };

    statement init() {
        statement(empty());
    }

    statement store(key: Key, value: Self::Value) {
        self.hasKey(key) == true;
        self.getValue(key) == value;
        reason(self.store(key, value)); // Maybe this can be generated?
    }

    statement missing(key: Key) {
        self.hasKey(key) == false;
        reason(self.remove(key) or never(self.store(key, unknown(value))));
    }

    statement empty() {
        any_var item;
        statement(missing(item));
        reason(self.remove(item) or never(self.store(item)));
    }

    fn store(key: Self::Key, value: Self::Value)
       description {
          consequence(statement_(store(key, value)));
       }

    fn remove(key: Self::Key)
       description {
          consequence(statement_(missing(key)));
       }

    fn getState() -> State
       description {
           map (return_()) {
             State::Empty when statement_(empty());
             State::Store when !statement_(empty());
           }

    fn hasKey(key: Self::Key) -> bool
       description {
           map (return_())
               true => {
                   var_var value: Self::Value; // so any value
                   statement_(store(key, value));
               };
               false => statement_(missing(key));
           }
       }

    fn getValue(key: Self::Key) -> Self::Value
       description {
           if (statement_(empty())) {
              #error
           }
           var value;
           statement_(store(key, value));
           return_() == value;
       }
}
```

```
concept Activatable : AssociativeStorage {
    type OriginalStruct;
    type ResultStruct;

    enum State { Original = !activated(), Activated = activated() };

    statement activated() {
        <self As AssociativeStorage>.statement_(hasKey("result"));
    }

    fn getState() -> State
    fn activate();
    fn getOriginal() -> OriginalStruct
    fn getResult() -> ResultStruct
}

concept ArcPixel : AssociativeStorage
{
}
```

2025-09-28
==========

```
concept Activatable : AssociativeStorage {
    type OriginalStruct;
    type ResultStruct;

    enum State { Original = !activated(), Activated = activated() };

    statement activated() {
        <self As AssociativeStorage>.statement_(hasKey("result"));
    }

    fn getState() -> State
    fn activate();
    fn getOriginal() -> OriginalStruct
    fn getResult() -> ResultStruct
}
```
```
impl Storage for ArcPixel
{
    type Item = arc::Color;

}
```
```
concept Storage {
    type Item;
    enum State { Empty, Store }; // when capacity is 1

    statement store(item: Self::Item) {
        self.contains(item) == true;
        self.get() == item;
    }

    statement missing(item: Self::Item) {
        self.contains(item) == false;
    }

    statement empty() {
        any_var item;
        self.contains(item) == false;
    }

    fn store(item: Self::Item)
       description {
          consequence(statement_(store(item)));
       }

    fn remove(item: Self::Item)
       description {
          consequence(statement_(empty()) and statement_(missing(item)));
       }

    fn getState() -> State
       description {
           any_var item;
           match return_() {
             State::Empty => statement_(empty());
             State::Store => statement_(store(item));
           }
       }

    fn contains(item: Self::Item) -> bool
       description {
           match return_() {
               true => statement_(store(item));
               false => statement_(missing(item));
           }
       }

    fn get() -> Self::Item {
       description {
           if (statement_(empty())) {
              #error
           }
           var item;
           statement_(store(item));
           return_() == item;
       }
    }
}
```
```
concept Number : Storage {
    statement larger(lhs: Number, rhs: Number) {
        self.subtract(lhs) == rhs;
    }

    statement smaller(lhs: Number, rhs: Number) {
        self.add(lhs) == rhs;
    }

    fn add(number: Number) -> Number
       description {
          consequence(statement_(larger(number, Storage::get())));
       }

    fn subtract(number: Number) -> Number
       description {
          consequence(statement_(smaller(number, Storage::get())));
       }
}
```

2025-09-28
==========

```
concept Storage {
    type Item;
    enum State { Empty, Store }; // when capacity is 1

    statement store(item: Self::Item) {
        self.contains(item) == true;
        self.get() == item;
        self.getState() == State::Store;
    }

    statement missing(item: Self::Item) {
        self.contains(item) == false;
    }

    statement empty() {
        any_var item;
        self.contains(item) == false;
    }

    fn store(item: Self::Item)
       description {
          consequence(statement_(store(item)));
       }

    fn remove(item: Self::Item)
       description {
          consequence(statement_(empty()) and statement_(missing(item)));
       }

    fn getState() -> State
       description {
           any_var item;
           match return_() {
             State::Empty => statement_(empty());
             State::Store => statement_(store(item));
           }
       }

    fn contains(item: Self::Item) -> bool
       description {
           match return_() {
               true => statement_(store(item));
               false => statement_(missing(item));
           }
       }

    fn get() -> Self::Item {
       description {
           if (statement_(empty())) {
              #error
           } else {
              var item;
              reason(statement_(store(item)));
              consequence(return_().statement_(store(item)));
           }
       }
    }
}
```

2025-10-02
==========

https://abuseofnotation.github.io/category-theory-illustrated/01_set/

The empty set has some special properties, for example, it is a subset of every other set. Mathematically speaking,
 (∀A → ∅ ⊆ A  (∀means “for all”)


2025-10-06
==========

```
 // capacity is 1
 // storage for a slot.name or slot.value where always has a value
concept Storage {
    type Item;

    statement store(item: Self::Item) {
        self.get() == item;
    }

    fn store(item: Self::Item)
       description {
          consequence(statement_(store(item)));
       }

    fn get() -> Self::Item {
       description {
          var item;
          reason(statement_(store(item)));
          consequence(return_().statement_(store(item)));
       }
    }
}
```

```
concept UnlimitedStorage {
    type Item;
    enum State { Empty, Store };

    statement store(item: Self::Item) {
        self.contains(item) == true;
        self.get() == item;
        self.getState() == State::Full;
    }

    statement missing(item: Self::Item) {
        self.contains(item) == false;
    }

    statement empty() {
        any_var item;
        self.contains(item) == false;
        self.getState() == State::Empty;
    }

    fn store(item: Self::Item)
       description {
          consequence(statement_(store(item)));
       }

    fn remove(item: Self::Item)
       description {
          consequence(statement_(empty()) and statement_(missing(item)));
       }

    fn getState() -> State
       description {
           var result = statement_(empty()) ? State::Empty : State::Full;
           consequence(return_().statement_(store(result)));
       }

    fn contains(item: Self::Item) -> bool
       description {
           var result = statement_(store(item)) ? true : false;
           consequence(return_().statement_(store(result)));
       }

    fn get() -> Self::Item {
       description {
           if (statement_(empty())) {
              consequence(error());
           } else {
              var item;
              reason(statement_(store(item)));
              consequence(return_().statement_(store(item)));
           }
       }
    }
}
```




2025-10-07
==========

- I just rejected the "microcode idea" from 2025-09-25
- I use the "result" name instead of "value". There is name collision with SET. There we have cell, name (role, key), value.
   But that value is not a result value.
- I don't know how to express that the "concept InfoCell" is just exist for every infocell.

```
concept InfoCell {
    type Key: InfoCell;
    type Value: InfoCell;

    statement store(key: Key, value: Self::Value) {
        self.has(key) == true;
        self.get(key) == value;
    }

    statement exist(key: Key) {
        self.has(key) == true;
    }

    statement missing(key: Key) {
        self.has(key) == false;
    }

    fn set(key: Self::Key, value: Self::Value)
       description {
          consequence(self().statement_(store(key, value)));
       }

    fn has(key: Self::Key) -> bool
       description {
           consequence(return_().statement_(store("result", statement_(exist(key)))));
       }

    fn get(key: Self::Key) -> Self::Value
       description {
           if (statement_(empty())) {
              consequence(error());
           } else {
              var value;
              reason(self().statement_(store(key, value)));
              consequence(return_().statement_(store("result", value)));
           }
       }

    fn remove(key: Self::Key)
       description {
          consequence(self().statement_(missing(key)));
       }

}
```

```
statement MemberStruct()
{
    InfoCell::store("key", Slot("key", InfoCell));
    InfoCell::store("value", Slot("value", Struct));
}

statement ListNodeStruct()
{
    InfoCell::store("previous", Member("previous", ListItem));
    InfoCell::store("next", Member("next", ListItem));
    InfoCell::store("value", Member("value", InfoCell));
}

statement ListHeadStruct()
{
    InfoCell::store("first", Member("previous", ListItem));
    InfoCell::store("last", Member("next", ListItem));
    InfoCell::store("size", Member("size", Number));
}

concept List {
    type Value;
    type Node;

    statement empty() {
        InfoCell::has("first") == true;
    }

    statement connect(from: Infocell, name: Infocell, value: Self::Value) {
        <from As InfoCell>.has(name) == true;
        <from As InfoCell>.get(name).get(value) == value;
    }

    fn add(value: Self::Value) -> Node
       description {
          var lastNode = self().statement_(empty()) ? self() : last();
          consequence(self().statement_(connect(lastNode, "next", value)));
       }

    fn empty() -> bool
       description {
          consequence(return_().statement_(store("result", statement_(empty()))));
       }

    fn size() -> Number
       description {
          consequence(return_().statement_(store("result", statement_(count()))));
       }
}
```

```
concept Iterable {
    type Value;

    fn isEmpty() -> bool;

    fn setFirstValue();

    fn getCurrentValue() -> Value;

    fn hasNextValue() -> bool;

    fn setNextValue();
}
```

2025-10-31
==========

- Maybe I shouldn't reject the microcode idea yet
- I started focus on one cell -> more cells transition
- So during storage in one cell we can say we exchange place for a value
- For a multicell data-structure we still exchange place for a value where every value will have a uniwue place (a path)

```
concept InfoCell {
    type Place: InfoCell;
    type Value: InfoCell;

    statement store(place: Place, value: Self::Value) {
        self.has(place) == true;
        self.get(place) == value;
    }
    ...
```

```
concept List {
    type Value: InfoCell;

    statement add(value: Self::Value) {
        self.contains(value) == true;
    }
    ...
}
```
```
concept ListIterator {
    type Value: InfoCell;

    ...
}
```

2025-11-07
==========

- I started focus on single linked list methods
- in this case we have only an add() method and an iter
- somehow we have to take into account the time as we can call the add(method multiple times)

```

concept Iterator {
    type Value: InfoCell;

    ...
}

concept List<T> {
    type Value: InfoCell;
    type Iterator: Iterator<T>;

    statement contains(value: Self::Value) {
        ???
    }

    void add(value: Self::Value) {
    }

    fn getIterator() -> Self::Iterator
    ...
}
```

2025-11-08
==========

- I try describe the method calls with gherkin like DSL, the idea is that creating a repeated, but numbered scenarios and
  referencing the unknow values X from the add(X) scenario to the usage scenario.

```
Given
   list = List<T>
IndexedWhen(1)
   { X=unknown; list.add(X); }
And Repeated(N times)
   { X=unknown; list.add(X); }
Then
   iter = list.iter();
   iter.isEmpty() == false;
After
   iter.setFirstValue();
Then
   iter.getCurrentValue() == IndexedWhen(1).X;
And Repeated(N, i)
{
   iter.hasNextValue() == true;
   iter.getCurrentValue() == IndexedWhen(i).X;
After
   iter.setFirstValue();
}

Then
   iter.hasNextValue() == false;
```


2025-11-10
==========

- I dont have to index anything as I can just reference everything

```
addScenario()
.given(
   member("list", tt_("List", "valueType", tp_("valueType")))
.when(
   var_("item") = unknown(),
   list.add(X);
)
.when(N times)( // TODO
   var_("item") = unknown(),
   list.add(X);
)
.then(
   var_("iter") = list.iter(),
   var_("iter")("isEmpty")() == false
)
After
   iter.setFirstValue();
Then
   iter.getCurrentValue() == IndexedWhen(1).X;
And Repeated(N, i)
{
   iter.hasNextValue() == true;
   iter.getCurrentValue() == IndexedWhen(i).X;
After
   iter.setFirstValue();
}

Then
   iter.hasNextValue() == false;
```

- an other idea is to use control cells (if, do, while, ...) for struct description
- so a list can be

```
struct List<T>
{
    struct ListItem<T>
    {
        value -> T;
    };
    size -> Number; number of connected ListItems
    first -> ListItem<T>; the first element of connected ListItems
             ListItem<T>*(connection: next) // the '*' is regexp like zero or more kleenstar
             repeated(name: first, type: ListItem<T>, amount: zeroOrMore, connection: next)

    last -> ListItem<T>; the last element of connected ListItems
};
```

2025-11-11
==========

- The scenario way doesn't seems to work as it can not be composed but the structure description way seems promising.
- XML DTD kinda similar but now I can use the indicative mood like statements
- Maybe I should just define extension points where the structure can grow (where you can add connections to new cells
  that will be part of the structure)
- This way I can represent a binary tree as every node can be extended at two extension points where you can add extra two
  nodes for every node.
- Now the task, EXACTLY how can I express this thing
- I can use true statements for this and I can use get/has + controls do, while, if ...

2025-11-12
==========

- So I am thinking on where to store this data member information. So list.first is only exists if the add(x) method creates it.
  Why not just describe this member in the add(x) then? list.size is always exists so I can document it in the description of ListHead.
- An other point is, how and where to document the reason of this data structure. The short reason is that I want to make reachable
  N amount of unknown X cells from the ListHead node and modifying the unknown X cells are not allowed. So this is the original problem.
  And one solution to this problem is that the ListHead
    - represent an empty list if size is 0 and first is missing
    - represent a list with one element if size is 1 and the first element connects to a ListNode cell. The listNode.value cell will contains
      the value that we want to store
    - represent a list with more then one element if the first element connects to a ListNode cell and ...

2025-11-15
==========

- I'm thinking on the reachability of cells in case of composed cells. So every stored value in a datastructure can be reached from the entry node of the
  datastructure. We can express this thing a statement.
  ```
  reachable(from: Cell, to: Cell, path: Cell) -> bool
  {
     get(from, path) == to;
  }

  // ex.: list.first.next.value reachable(list, value, { first, next })
  statement reachable(from: Cell, to: Cell, path: List<Cell>)
  {
     var currentNode = from;
     for (link in path) {
        currentNode = get(currentNode, link);
     }
     currentNode == to;
  }
  ```

2025-11-16
==========

- Maybe I need a reachability plan?
- How to express first element?
  - For the first element it is true, that there is no previous element
  - For the last element it is true, that there is no next element
  - From first element the last element is always reachable through repeated "next" edges
- Ok, so for linked nodes we have to know the first element, and the edge for the next element


```
pub trait SingleLinkedList {
    type First;
    type Last;
    type Previous;
    type Next;

    statement next(from, to)
    {
       get(from, Next) == to;
    }

    statement first(from, to)
    {
       has(from, Previous) == false;
    }

    statement last(from, to)
    {
       has(from, Next) == false;
    }
}

statement next(from, to)
{
   get(from, next) == to;
}
statement first(from, to)
{
   has(from, previous) == false;
}
statement last(from, to)
{
   has(from, next) == false;
}
```

2025-11-17
==========

- We can say that a
  - ListHead represent a group
  - ListNode.value represent an item inside a group
  - a "next" between listNode1 and listNode2 represent and order between listNode1.value and listNode2.value
- Actually I can use a cell that hold numbered items with the following
  ```
  inputValues
    type -> Index
    1 -> apple   // started from 1
    2 -> banana  // increased by 1
    3 -> carrot  // last element as 4 is missing
  ```
  but Index use List under the hood so nevermind ...


2025-11-18
==========

- Thinking about the "in". When we can say that an element is in the group.We should state that every element is in the group.

2025-11-23
==========

- I'm a little bit optimistic now. Looks like I can express the one statement from multiple infocells problem with reassigning or creating new variables.
  - by setting new infocell value to a variable we can say that we are switching to an other infocell so the statements will be true for that variable. So we
    we can create some kind of sequence points. So I can just stepping a variable in a do oor while loop.
- I can't see more problems now, so I can switch back to thinking on implementing the tool usage algorithm (where the x.set(y, z) consequence is x.get(y) == z so I can
  write a request to let this pixel green, so pixel.get(color) == green and the algorithm should generate a pixel.set(color, green)).

```
Expr := | Data | Get | Has | Equal;

Set :=
cell: Get(cursor, type) == ast.Equal &
role: Get(cursor, cell) == ast.Expr &
value: Get(cursor, type) == ast.Expr

        InfoCellGrammar() : Grammar("InfoCell")
        {
            Data;
            Expr | Data | Get | Has | Equal;
            Stmt | Set | If | Do | While;

            Get & .cell(Expr)
                & .role(Expr)

            Set & Equal
                & .lhs(Get)
                & .rhs(Expr)
                ;

            Prec1 | Mul | Div | DecimalLiteral;

            Mul & DecimalLiteral
                & '*'
                & Prec1
                ;

            Div & DecimalLiteral
                & '/'
                & Prec1
                ;

            Prec2 | Add | Sub | Prec1;

            Add & Prec1
                & '+'
                & Prec2
                ;

            Sub & Prec1
                & '-'
                & Prec2
                ;

            Expr | Equal | NotEqual | Prec2;

            Equal
                & Prec2
                & "=="
                & Expr
                ;

            NotEqual
                & Prec2
                & "!="
                & Expr
                ;
        }
```

2025-12-02
==========

- trying to create a lookup structure for `x.set(y, z)` => `x.get(y) == z`

```
ids.struct_: std.op.Equal
ids.lhs:     { ids.struct_: std.op.Get, ids.cell: var("x"), ids.role: var("y") }
ids.rhs:     var("z")
```

So this is bad as `var("x")`, `var("y")` and `var("z")` is only meaningful in a `x.set(y, z)` context. So the `x, y, z` is come frome the query and it shouldn't be
in the lookaup table. But we should emit a query where we can express the following: whatever is here put me that value to `var("x")`. So we need two structure,
the query and the lookup one.

```
<ids.struct_, std.op.Equal>: [](this, key, value) { key == ids.struct_; value == std.op.Equal; }
<ids.lhs, *>:     [](this, key, value) {
                     key == ids.lhs;
                     child = this / value; pair = firstPair(child);
                     <ids.struct_, std.op.Get>: [](this = child, key = pair / key, value = pair / value) { key == ids.struct_; value == std.op.Get; }
                     <ids.cell, *>: [](this = child, key = pair.key, value = pair.value) { key == ids.cell; }
                     <ids.role, *>: [](this = child, key = pair.key, value = pair.value) { key == ids.role; } }
<ids.rhs, *>:     [](this, key, value) { key == ids.rhs; }

=>

set(this / ids.lhs / ids.cell, this / ids.lhs / ids.role, this / ids.rhs)
```

2025-12-11
==========

Created a proof of concept tool finder for `x.set(y, z)` => `x.get(y) == z`


2025-12-12
==========

Ok so the variable names should be started with capital as in prolog. So `X.set(Y, Z)` => `X.get(Y) == Z` but `pixel.get(green) == 5`
To extending the idea I want ot express the request of `pixel.get(currentTheme / color) == 5`. In this case `currentTheme / color` is a `get`
but it is equals to an output variable. So the `currentTheme / color` will be searched as a `currentTheme / color == $output`

2026-01-04
==========

To moving forward I want to experiment with numbers.
So the description of `x.add(1)` is that the future value of x is larger by 1 from the current value. And the statement will have two parameters.
so the description is something like `x.greater(future(x), by=1)` and the `x.greater(than, by)` statement will make true the following expression `x.subtract(by) == than`.

2026-01-16
==========

I think the `add` operation can be expressed as `x.add(y) == z` => `z.subtract(y) == x`. With a matcher of
`struct ast::Equal lhs op push struct ast::Subtract lhs op variable rhs op variable op pop rhs op variable `.
To enforce some effect there needs to be an output like variable, probably `output` where we can put a value for example to `output.value` so output is just a variable.
In this case we have to mark the original request, for example `x + 2 == 3` as a context, and add `output.get(value) == x` as a request.

After thinking on numbers there are some problem. First the x + x == 2 can't be solved without dividing. Second, the known - unknown problems, so in case
of `x.add(y) == z` this is only relevant if and only if the `x` and `y` are known. Otherwise `x + x == 2` leads to `x == 2 - x` which is not really useful.

2026-02-14
==========

Ok, so math tools are kinda working now. At least the `x + 2 = 4` can be solved if we write it as `equal(add(get(x, value), 2)), 4)` but
`2 + x = 4` == `equal(add(2, get(x, value))), 4)` doesn't work. There are two problem here.

In the first case `equal(add(get(x, value), 2)), 4)` will match with `subtract` effect of `equal(add(return_(), m_("rhs")), m_("lhs"))` so we go forward with
`equal(get(x, value), subtract(4, 2))` (basically `x = 4 -2`) will match with `set` effect of `equal(m_("cell") / m_("role"), m_("value"))` so we end up with
a good tool `set(x, value, subtract(4, 2))`.

In the second case `equal(add(2, get(x, value))), 4)` will also match with `subtract` effect of `equal(add(return_(), m_("rhs")), m_("lhs"))` so we go forward with
`equal(2, subtract(4, get(x, value)))` (basically `2 = 4 - x`) which is a dead end.
- First there is no `equal(variable, fnX())` pattern registered as variable is on the right side now.
- Second `2 = 4 - x` is mathematically doesn``t bring closer to the solution as `x` and constant values are not separated.

```
2 + x + 2 + x + x + 2 + 2 + 2 + x
add(2, add(x, add(2, add(x, add(x, add(2, add(2, add(2, x))))))))
...
add(2, add(x, add(2, add(x, add(x,         add(2, add(2, add(x, 2))))))))
add(2, add(x, add(2, add(x, add(x,         add(2, add(x, add(2, 2))))))))
add(2, add(x, add(2, add(x, add(x,  add(x, add(2, add(2, 2))))))))
add(2, add(x, add(2, add(x, add(mul(2, x), add(2, add(2, 2)))))))
add(2, add(x, add(2, add(mul(3, x),        add(2, add(2, 2))))))
add(2, add(x, add(mul(3, x), add(2,        add(2, add(2, 2))))))
add(2, add(mul(4, x),        add(2,        add(2, add(2, 2)))))
add(mul(4, x), add(2,        add(2,        add(2, add(2, 2)))))
```

Sooo I need to add some types to the matchers...

- `x + 2 = 4` =>
    - `equal(add(return_():Any, m_("rhs"):Number), m_("lhs"):Number)` match
    - `equal(add(m_("rhs"):Number, return_():Any), m_("lhs"):Number)` doesn''t match
- `2 + x = 4` => `equal(add(return_():Any, m_("rhs"):Number), m_("lhs"):Number)` doesn''t match
    - `equal(add(return_():Any, m_("rhs"):Number), m_("lhs"):Number)` doesn''t match
    - `equal(add(m_("rhs"):Number, return_():Any), m_("lhs"):Number)` match

We can do this with a precondition check for every tool effect
```
if_(and(equal(m_("lhs") / _(ids.struct_), _(std.Number)), equal(m_("rhs") / _(ids.struct_), _(std.Number))).then_(
   `equal(add(return_():Any, m_("rhs"):Number), m_("lhs"):Number)`
)
```

Other problems

- need a forest diff kinda thing, idea here is to find a tool that do a transformation

  The first pair of tree in forest contains two number `pair1.number = 1` and `pair2.number = 3`. The known tools that input two numbers and output one are
  `add`, `subtract`, `divide`, `multiply`.

  - for `add` => `1 + x = 3` so the transformation rule candidate `add(pair1.number, 2)` as `x = 2`
  - for `subtract` => `1 - x = 3` so the transformation rule candidate `subtract(pair1.number, -2)` as `x = -2`
  - for `divide` => `1 / x = 3`so the transformation rule candidate `divide(pair1.number1, 1/3)` as `x = 1/3`
  - for `multiply` => `1 * x = 3` so the transformation rule candidate `multiply(pair1.number, 3)` as `x = 3`

  The second pair of tree in forest contains two number `pair1.number = 3` and `pair2.number = 5`. The known tools that input two numbers and output one are
  `add`, `subtract`, `divide`, `multiply`.

  - for `add` => `3 + x = 5` so the transformation rule candidate `add(pair1.number, 2)` as `x = 2`
  - for `subtract` => `3 - x = 5` so the transformation rule candidate `subtract(pair1.number, -2)` as `x = -2`
  - for `divide` => `3 / x = 5`so the transformation rule candidate `divide(pair1.number1, 3/5)` as `x = 3/5`
  - for `multiply` => `3 * x = 5` so the transformation rule candidate `multiply(pair1.number, 5/3)` as `x = 5/3`

  So the add/subtract and divide/multiply solutions are connected.

- if/do/while cells ...

  I need a good request example for these

- the list

  maybe time for iteration trait

2026-02-17
==========

For the list, there is a simple problem idea:

- given an empty list
- result described as
  - every list item is 1
  - the list size is 5

Expressing every item without iterator can be

```
Given an empty list

var node = list(ids.first) // TODO: what is an empty node?!
while (has(node, ids.value)) {
   get(node, ids.value) == _(_1_);
   get(node, ids.value) == get(node, ids.next);
}
list(ids.size) == 5

for (var listValue : list) {
   listValue.value == 1;
}
```

As `list.size` defined as increase a number on every item and `every list item is ..` also involve the same iteration we have to unify the two thing.


2026-02-22
==========

`1 + 2 + x` = `1 + x + 2` = `2 + 1 + x` = `2 + x + 1` = `x + 1 + 2` = `x + 2 + 1`

How to prove that `1 + 2 + x` equals `x + 2 + 1`?
- `1 + 2 + x` can be written as `add(1, add(2, x))` and also `add(add(1, 2), x)` as we can swap lhs and rhs the followings are also true:
  - `add(1, add(2, x))` with swapped lhs and rhs is `add(add(2, x), 1)`
  - `add(add(1, 2), x)` with swapped lhs and rhs is `add(x, add(1, 2))` so

- But how to prove that `1 + 2 + x` can be written as `add(1, add(2, x))` and also `add(add(1, 2), x)`?
  - `add(1, add(2, x))` with swapped lhs and rhs is `add(add(2, x), 1)`
  - `add(add(1, 2), x)` with swapped lhs and rhs is `add(x, add(1, 2))`
