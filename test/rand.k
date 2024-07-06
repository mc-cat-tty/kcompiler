extern floor(x);
global seed;
global a;
global m;
def randk() {
   var tmp = a*seed;
   seed = tmp-m*floor(tmp/m);
   seed/m
};
def randinit(x) {
   a = 16897.0;
   m = 2147483647.0;
   seed = x-m*floor(x/m);
   0.0
};
