def pow2(x i) {
   x<2*i ? i : pow2(x,2*i)
};
def intpart(x acc) {
   var y = x<1 ? 0 : pow2(x,1);
   y == 0 ? acc : intpart(x-y,acc+y)
};
def floor(x) {
   intpart(x,0)
};
