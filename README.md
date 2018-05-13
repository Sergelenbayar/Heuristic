# Heuristic
this implementation of A.Arulselvan "Detecting critical nodes in sparse graph"
et al./Computers&Operations Research 36(2009) 2193-2200 is very slowly
when you work it in graph with more than 100 nodes.if you work in this case,
I recommended to you don't use localSearch procedure. Another word
if you set localSearchItr to 2, it will work very slowly, and you don't wish it.
you can to use localSearchItr equal to 0. In other case,
you can use maxItr set equal to k, and the localSearchItr set to 2.
