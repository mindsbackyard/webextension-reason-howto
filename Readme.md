# A guide to creating webextensions with ReasonML

**Disclaimer:** I write this to the best of my knowledge which is very limited in the area of web development---as a consequence what I write here might not be fully correct.

## Motivation/Scope

The document's main intention is to capture the steps in the development process---mostly for myself, but if it helps someone else it would be even better.

I've rather specialised on operations research, algorithms & data structures, data science, and backend development so far.
Nevetheless, I wanted to add newer a "front-end" language and framework to my tool belt for a time now---because older dabbling in Microsoft's MFC (if anyone remembers that), Java's Swing/JSP/JSF, or C++'s Qt got a bit dusty (Do you remember the tomb in the initial scenes of Indiana Jones? Like that.)---but could never bring myself to learn JavaScript properly.
I can read it, but I just don't like the language and eco-system enough---no insult intended---and haven't had any professional incentive to learn to do active development with it.

I got more intrigued with ReasonML as it's a functional language at its core with a simple, yet sufficiently powerful type system and language grammar.
Further, the development of the code---which does what it is intended to do---was a team effort, but the mistakes in the write-up be on me.

The first three parts focus on getting a simple browser extension from Mozilla's webextension examples running in ReasonML.
The write-up is mostly targeted at beginners in ReasonML such as myself.

The browser used to test the extensions is Firefox---so it might or might not work in another browser supporting webextensions.

## Extension 1: Borderify
A simple extension to draw a red border around websites in the `mozilla.org` domain.

* Part 1: A basic webextension to build on
* Part 2: Enabling ReasonML in our webextension
* Part 3: Using actual ReasonML in our webextension

## Extension 2: Cookie Freezer [TBD]
A small extension to manage and manipulate cookies.

* Part 4 [TBD]: Building a UI with reason-react
* Part 5 [TBD]: Writing wrappers for the browser's webextension API
* Part 6: ???
