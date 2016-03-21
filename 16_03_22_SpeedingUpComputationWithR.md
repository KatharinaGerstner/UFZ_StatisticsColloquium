<style>
  .small-code pre code {
    font-size: 1em;
  }
</style>

Speeding up computation with R
========================================================  
author: Felix May, Katharina Gerstner
date: 22 March 2016
transition: none
width: 1000
height: 700

UFZ Statistics Colloquium  
![](C:/Users/hoppek/Pictures/HIGRADE_LOGO.jpg)


Content Overview
========================================================  
incremental: true

Visser et al. (2015) Speeding Up Ecological and Evolutionary Computations in R; Essentials of High Performance Computing for Biologists. PLOS Comput Biol

- Introduction: Why to optimize?
- When to optimize?
- What to optimize?
- How to optimize? 
  1. Simple rules to consider while writing the code 
  2. More sophisticated methods for code optimization such as outsourcing code into low-level language C++


Introduction: Why to optimize?
========================================================
- Ecological and environmental research is becoming more quantitative and computationally demanding
- Inefficient computation limits the scope and quality of research


When and what to optimize I
========================================================
incremental: true
**Best coding practices**
  1. Start writing correct code, easy to debug in R, don't worry about efficiency
  2. Profile your program if it is too slow and guesstimate expected gains (e.g. using R-functions `base::Rprof()`, `aprof::aprof()`)
  3. Consider optimization only if it is worthwile:
    1. pure R solution
    2. Parallelisation
    3. Rewrite certain key parts in low-level language, e.g. C++


When and what to optimize II
========================================================
incremental: true
- Consider optimization only after the code works correctly and produces trustworthy result
- Check if results are in agreement after each change in the code using `identical()`
- Trade-off between speed and simplicity, however knowing some general rules help to speed up your code when writing
- Optimization is time consuming and only turned out to be useful when result in a rather large decrease in run time, optimize code sections which consume large amounts of run time, identifying these code sections allows effective and targeted optimization


How fast is my code?
========================================================
incremental: true
- `system.time()` calls the function `proc.time()` before and after evaluating the expression, returning the difference between the two proc.time-call   
  NOTE: Timing is random, for various reasons!
- `microbenchmark::microbenchmark()` runs the expression multiple times to reduce variation
- Code profiling to identify bottlenecks using `Rprof()` and `aprof()` 


Simple rules I
========================================================
class: small-code
**Non-essential operations** 
- Eliminating unnecessary function calls, printing statements, plotting, or memory references

```r
IQ1<-function(N){
  for (j in 1:N) { N/(1+N) }
}
IQ2<-function(N){
  for (j in 1:N) { (((N/(1+N)))) }
}
system.time(IQ1(100000))
```

```
   user  system elapsed 
   0.14    0.00    0.14 
```

```r
system.time(IQ2(100000))
```

```
   user  system elapsed 
   0.14    0.02    0.16 
```

```r
identical(IQ1(100000),IQ2(100000))
```

```
[1] TRUE
```

Simple rules II
========================================================
class: small-code
**Memoization** 
- Store the results of expensive function calls that are used repeatedly

```r
foo1 <- function(d,R){
  results <- array(NA, dim = c(R,ncol(d)))
  for(i in 1:R){
    ind <- sample(seq(nrow(d)), replace=T)
    results[i,] <- apply(d[ind,],2,function(x) mean(x)-mean(d))
  }
}
foo2 <- function(d,R,avg=mean(d)){ 
  results <- array(NA, dim = c(R,ncol(d)))
  for(i in 1:R){
    ind <- sample(seq(nrow(d)), replace=T)
    results[i,] <- apply(d[ind,],2,function(x) mean(x)-avg)
  }
}
```

Simple rules III
========================================================
class: small-code
**Memoization**

```r
require(microbenchmark)
d <- matrix(100*runif(100),nrow=10)
R <- 10
microbenchmark(foo1(d,R),
               foo2(d,R,avg=mean(d)),
               times=5)
```

```
Unit: milliseconds
                      expr      min       lq     mean   median       uq
                foo1(d, R) 4.227178 4.337308 4.462043 4.568226 4.574147
 foo2(d, R, avg = mean(d)) 3.067063 3.128641 3.487452 3.170482 3.211929
      max neval
 4.603357     5
 4.859143     5
```
Speed-up factor = 1.5

Simple rules IV
========================================================
class: small-code
**Pre-allocate-and-fill operation when growing data**
- Code profiling

```r
foo <- function(N){
             preallocate<-numeric(N)
             grow<-NULL
              for(i in 1:N){
                  preallocate[i]<-N/(i+1)
                  grow<-c(grow,N/(i+1))
                 }
            }
dump("foo",file="foo.R")
source("foo.R")
tmp <- tempfile() # create file in getwd() to save profiler output
Rprof(tmp,line.profiling=TRUE) # start profiling
foo(5e4)
Rprof(append=FALSE) # stop profiling
```

Simple rules V
========================================================
class: small-code
**Pre-allocate-and-fill operation when growing data**
- Code profiling











```
processing file: 16_03_22_SpeedingUpComputationWithR.Rpres
Loading required package: microbenchmark
Loading required package: aprof
Quitting from lines 150-155 (16_03_22_SpeedingUpComputationWithR.Rpres) 
Fehler in plot.window(...) : endliche 'xlim' Werte nötig
Ruft auf: knit ... <Anonymous> -> plot.default -> localWindow -> plot.window
Zusätzlich: Warnmeldungen:
1: In readLineDensity(aprofobject, Memprof = AddMemProf) :
  specified source
      file foo.R is not in the list of files in the
      profiler output: 
 
2: In readLineDensity(aprofobject, Memprof = AddMemProf) :
  Some aprof functions may fail --> user supplied source filefoo.R does not seem to correspond to any file in the profiler output.
 Possible causes: 
1) Source file was not profiled?
2) Spelling?

3: In readLineDensity(aprofobject) : specified source
      file foo.R is not in the list of files in the
      profiler output: 
 
4: In readLineDensity(aprofobject) :
  Some aprof functions may fail --> user supplied source filefoo.R does not seem to correspond to any file in the profiler output.
 Possible causes: 
1) Source file was not profiled?
2) Spelling?

5: In max(LineDensity$Call.Density/LineDensity$Total.Calls) :
  kein nicht-fehlendes Argument für max; gebe -Inf zurück
Ausführung angehalten
```
