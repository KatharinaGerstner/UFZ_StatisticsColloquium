//Integrating R and C++
//Felix May
//22. 03. 2015
//
//C++ file  
//MyCPPcode.cpp
//This is a C++ comment

# include <Rcpp.h>
using namespace Rcpp;
using namespace std;


// [[Rcpp::export]]
int add(int x, int y, int z)
{
   int sum = x + y + z;
   return sum;
}

// [[Rcpp::export]]
double addC(double a, double b, double c)
{
   double sum = a+b+c;
   return sum;
}

//-------------------------------------------------------------
//Mean in Rcpp

// [[Rcpp::export]]
double meanC(NumericVector x)
{
   double sum = 0;
   int len = x.size();
   for (int i=0; i<len; i++)
      sum += x[i];
   return sum/len;
}

//--------------------------------------------------------------
//Row and column sums 

// [[Rcpp::export]]
NumericVector marginSums(NumericMatrix x, bool rows)
{
   int nrows = x.nrow();
   int ncols = x.ncol();
   
   if (rows==true){
      NumericVector outvec(nrows);
      
      for (int irow=0; irow < nrows; ++irow){
         for (int icol=0; icol < ncols; ++icol){
            outvec[irow] = outvec[irow] + x(irow,icol);
         }
      }
      
      return outvec;
   } 
      
   else {
      NumericVector outvec(x.ncol());
      
      for (int irow=0; irow < nrows; ++irow){
         for (int icol=0; icol < ncols; ++icol){
            outvec[icol] = outvec[icol] + x(irow,icol);
         }
      }
      
      return outvec;
   }
}


//--------------------------------------------------------------
//Dataframes 

// [[Rcpp::export]]
DataFrame rowRange(NumericMatrix x)
{
   int nrows = x.nrow();
   int ncols = x.ncol();
   
   NumericVector RowMin(nrows);
   NumericVector RowMax(nrows);
   
   //Initialize vector
   for (int irow=0; irow < nrows; ++irow){
      RowMin(irow) = x(irow,0);
      RowMax(irow) = x(irow,0);
   }
   
   for (int irow=0; irow < nrows; ++irow){
      for (int icol=0; icol < ncols; ++icol){
         if (x(irow,icol) < RowMin(irow))
            RowMin(irow) = x(irow,icol);
         if (x(irow,icol) > RowMax(irow))
            RowMax(irow) = x(irow,icol);
      }
   }
   
   DataFrame out = DataFrame::create(_["Min"] = RowMin,
                                     _["Max"] = RowMax); 
   return out; 
}


//---------------------------------------------------------------
//Data frames
// from http://gallery.rcpp.org/articles/modifying-a-data-frame/

// [[Rcpp::export]]
DataFrame modifyDataFrame(DataFrame df) {
   
   // access the columns
   IntegerVector a = df["a"];
   CharacterVector b = df["b"];
   
   // make some changes
   a[2] = 42;
   b[1] = "foo";       
   
   // return a new data frame
   return DataFrame::create(_["a"]= a, _["b"]= b);
}

//---------------------------------------------------------------
//Rcpp sugar

// [[Rcpp::export]]
NumericVector cumsum_cpp(NumericVector x){
   
   // initialize an accumulator variable
   double acc = 0;
   
   // initialize the result vector
   NumericVector res(x.size());
   
   for(int i = 0; i < x.size(); i++){
      acc += x[i];
      res[i] = acc;
   }
   return res;
}

// [[Rcpp::export]]
NumericVector cumsum_sugar(NumericVector x){
   return cumsum(x);    // compute the result vector and return it
}


//---------------------------------------------------------------
//Simulate PI

// [[Rcpp::export]]
double piSugar(const int N) {
   NumericVector x = runif(N);
   NumericVector y = runif(N);
   NumericVector d = sqrt(x*x + y*y);
   return 4.0 * sum(d < 1.0) / N;
}

//---------------------------------------------------------------
//Population dynamics

// [[Rcpp::export]]
IntegerVector popDynC(int N0,double b, double d,int tmax)
{
   IntegerVector Nt(tmax);
   Nt[0] = N0;
   
   int Birth,Death, Nnew;
   double death_rate;
   
   for (int i=1; i<tmax; i++){
      
      if (Nt[i-1]>0){
         Birth = as<int>(rbinom(1,Nt[i-1],b));
         
         death_rate = std::min(1.0,d*Nt[i-1]);
         Death = as<int>(rbinom(1,Nt[i-1],death_rate));
         
         Nnew = Nt[i-1] + Birth - Death;
         if (Nnew > 0) Nt[i] = Nnew;
         else Nt[i] = 0;
      }
   }
   return Nt;
}


