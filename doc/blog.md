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