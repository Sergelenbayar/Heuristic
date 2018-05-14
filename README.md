# Heuristic
Implementation of A.Arulselvan "Detecting critical nodes in sparse graph"
et al./Computers&Operations Research 36(2009) 2193-2200.

This implementation's results will be shown same results in the article above. 
But LocalSearch procedure in the implementation is very slowly
when you work it in graph with more than 100 nodes.if you work in this case,
I recommended to you don't use localSearch procedure. Another word
if you set localSearchItr to 2, it will work very slowly, and you don't wish it.
you can use localSearchItr equal to 0. In other case,
maxItr set equal to k, and the localSearchItr set to 2.
