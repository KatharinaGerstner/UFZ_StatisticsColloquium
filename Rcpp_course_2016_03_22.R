# Integrating R and C++
# Felix May
# 22. 03. 2016

# R Code file


library(Rcpp)
library(microbenchmark)

sourceCpp("MyCPPCode.cpp")

add(1,2,3)
add(1.5,2.5,3.5)

addR <- function(a,b,c)
{
   a+b+c
}

addR(1.5,2.5,3.5)

addC(1.5,2.5,3.5)

# -------------------------------------------------------------
# Mean in Rcpp
meanR <- function(x)
{
   sum <- 0
   for (i in 1:length(x))
      sum <- sum + x[i]
   return (sum/length(x))
}

dat1 <- rnorm(1000,mean=5,sd=3)
meanR(dat1)
meanC(dat1)
mean(dat1)

library(microbenchmark)
microbenchmark(meanR(dat1),meanC(dat1),mean(dat1))

# --------------------------------------------------------------
# Row and column sums 

Mat1 <- matrix(1:100,nrow=10)
rowSums(Mat1)
colSums(Mat1)

marginSums(Mat1,rows=T)
marginSums(Mat1,rows=F)

# --------------------------------------------------------------
# Dataframes 
MinMax <- rowRange(Mat1)
MinMax

apply(Mat1,MARGIN=1,range)

microbenchmark(rowRange(Mat1),apply(Mat1,MARGIN=1,range))


# ---------------------------------------------------------------
x <- 1:999
microbenchmark(cumsum_cpp(x),cumsum_sugar(x),cumsum(x))

# Simulate PI
piSugar(1000000)

# ---------------------------------------------------------------
#Population dynamics
popDynR <- function(N0,b,d,tmax)
{
   Nt <- numeric(tmax)
   Nt[1] <- N0
   for (i in 2:tmax){
      if (Nt[i-1]>0){
         Birth <- rbinom(1,size=Nt[i-1],prob=b)
         
         death.rate <- min(1.0,d*Nt[i-1])
         Death <- rbinom(1,size=Nt[i-1],prob=death.rate)
         
         Nnew <- Nt[i-1] + Birth - Death

         if (Nnew > 0) Nt[i] <- Nnew
         else Nt[i] <- 0
      }
   }
   Nt
}


N <- popDynR(N0=10,b=0.05,d=0.001,tmax=1000)
plot(N,type="l")

N2 <- popDynC(N0=10,b=0.05,d=0.001,tmax=1000)
lines(1:length(N2),N2,type="l",col="red")


microbenchmark(popDynR(N0=10,b=0.5,d=0.01,tmax=1000),popDynC(N0=10,b=0.5,d=0.01,tmax=1000))

