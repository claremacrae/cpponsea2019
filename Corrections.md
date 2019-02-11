# Corrections

* Lambdas to print values don't need a newline character.
    * On slide 65 of [the talk](https://github.com/philsquared/cpponsea-slides/raw/master/2019/Clare%20Macrae%20-%20Quickly%20Testing%20Legacy%20Code.pdf), I had a code snippet using `Approvals::verifyAll()` in which I said it was important to put a `'\n'` at the end of the lambda.
    * This is in fact untrue.
    * I said that because just before the talk, I had opened one of the output files on my laptop, and found that all the outputs ran on to a single line.
    * I later realised this was due to opening a file with Unix line endings in a dumb text editor on a PC.
