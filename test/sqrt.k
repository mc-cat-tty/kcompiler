def err(a b) {
  a<b ? b-a : a-b
};

def iterate(y x) {
   var eps = 0.0001;
   for (var z = x*x; eps<err(z,y); x = (x+y/x)/2) z = x*x;
   x
};

def sqrt(y) {
   y == 1 ? 1 : (y<1 ? iterate(y,1-y) : iterate(y,y/2))
};	
