#include <stdio.h>
#include <stdlib.h>

int fact(int n){
   if(n<=0) return 0;
   if(n==1) return 1;
   return n * fact(n-1);
}

void fib(int n, int fib_sequence[]){
   if(n >= 1) fib_sequence[0] = 0; 
   if(n >= 2) fib_sequence[1] = 1; 

   for(int i = 2; i < n; i++){
       fib_sequence[i] = fib_sequence[i - 1] + fib_sequence[i - 2];
   }
}

int sum_sq_of_odd(int n){
   int sum = 0;
   for(int i = 1; i <= n; i++){
       if(i % 2 == 1){
           sum += (i * i);
       }
   }
   return sum;
}

int main(){
   int n = 5;

   // Factorial
   printf("%d \n", fact(n));

   // Fibonacci series
   int fib_sequence[n];
   fib(n, fib_sequence);
   for(int i = 0; i < n; i++){
       printf("%d ", fib_sequence[i]);
   }
   printf("\n");

   // Sum of squares of odd numbers
   printf("%d \n", sum_sq_of_odd(n));

   return 0;
}
