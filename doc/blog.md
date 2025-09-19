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