extern randinit(seed);
extern randk();
extern timek();
extern printval(x controlchar);
global A[10];
def inssort() {
   for (var i=1; i<10; ++i) {
       var pivot = A[i];
       var step = 1;
       for (var j = i-1; -1<j; j=j-step)
           if (pivot < A[j]) A[j+1] = A[j]
           else {
             A[j+1] = pivot;
             step = 10
           };
       if (step==1) A[0] = pivot
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
