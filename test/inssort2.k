extern randinit(seed);
extern randk();
extern timek();
extern printval(x controlchar);
global A[10];
def inssort() {
   for (var i=1; i<10; ++i) {
       var pivot = A[i];
       var j;
       for (j = i-1; -1<j and pivot<A[j] ; --j)
           A[j+1] = A[j];
       A[j+1] = pivot
    }
};
def main() {
  var seed = timek();
  randinit(seed);
  for (var i=0; i<10; ++i) {
     A[i] = randk();
     printval(A[i],0)
  };
  printval(0,1);
  inssort();
  for (var i=0; i<10; ++i)
     printval(A[i],0)
};
