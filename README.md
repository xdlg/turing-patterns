# Multi-Scale Turing Patterns

[Turing patterns](https://en.wikipedia.org/wiki/Turing_pattern) are a mathematical model for natural patterns such as stripes and spots. The model explains how these patterns can emerge from random initial conditions thanks to the interaction of two substances, called activator and inhibitor in the following. [Jonathan McCabe](http://jonathanmccabe.com/) published an [article](http://www.jonathanmccabe.com/Cyclic_Symmetric_Multi-Scale_Turing_Patterns.pdf) describing an algorithm to generate more complex Turing patterns using several layers, or scales, of activator/inhibitor couples. Implementation details were given later on [Softology's blog](https://softologyblog.wordpress.com/2011/07/05/multi-scale-turing-patterns/).

Conceptually, the idea is similar to that of Conway's [Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life): create an image with random pixel values, then repeatedly apply some simple rule on each pixel that changes its value based on the value of its neighbors. If the rule is chosen wisely, interesting and unexpected patterns emerge.

This project is an SDL3/C++ implementation of a multi-scale Turing patterns generator based on McCabe's algorithm.

![](res/pattern.png)
