# cpponsea2019

Code samples and information for my talk "Quickly Testing Legacy Code" at [C++ on Sea 2019](https://cpponsea.uk/).

The samples here demonstrate use of [ApprovalTests.cpp](https://github.com/approvals/ApprovalTests.cpp), which is a new C++ implementation of Llewellyn Falco's [Approval Tests](http://approvaltests.com/) approach to testing legacy code.

## About the talk

* Abstract: [Quickly Testing Legacy Code](https://cpponsea.uk/sessions/quickly-testing-legacy-code.html)
* Slides: [PowerPoint](https://www.slideshare.net/ClareMacrae/quickly-testing-legacy-code)
* Slides: [PDF](https://github.com/philsquared/cpponsea-slides/raw/master/2019/Clare%20Macrae%20-%20Quickly%20Testing%20Legacy%20Code.pdf)
* Code: [Github](https://github.com/claremacrae/cpponsea2019) (here)
* Video: Not yet published...

## ApprovalTests.cpp code samples

Purpose of the sub-directories in this repo:

* [demo_approvals_and_catch2/](demo_approvals_and_catch2)
    * Some example uses of [ApprovalTests.cpp](https://github.com/approvals/ApprovalTests.cpp) with the [Catch2 test framework](https://github.com/catchorg/Catch2), with detailed explanatory comments in the code.
* [demo_approvals_and_googletest/](demo_approvals_and_googletest)
    * Some example uses of [ApprovalTests.cpp](https://github.com/approvals/ApprovalTests.cpp) with the [Google Test framework](https://github.com/google/googletest), with detailed explanatory comments in the code.
* [gilded_rose_refactoring_kata/](gilded_rose_refactoring_kata)
    * A worked example of the [cpp version of Emily Bache's GildedRose Refactoring Kata](https://github.com/emilybache/GildedRose-Refactoring-Kata/tree/master/cpp)

## Corrections

* Lambdas to print values don't need a newline character.
    * On slide 65 of [the talk](https://github.com/philsquared/cpponsea-slides/raw/master/2019/Clare%20Macrae%20-%20Quickly%20Testing%20Legacy%20Code.pdf), I had a code snippet using `Approvals::verifyAll()` in which I said it was important to put a `'\n'` at the end of the lambda.
    * This is in fact untrue.
    * I said that because just before the talk, I had opened one of the output files on my laptop, and found that all the outputs ran on to a single line.
    * I later realised this was due to opening a file with Unix line endings in a dumb text editor on a PC.
