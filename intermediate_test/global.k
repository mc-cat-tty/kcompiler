global GLOBAL;

def f() {
  GLOBAL = 3;
  {
    GLOBAL = 4;
    GLOBAL = 5
  }
};

def g() {
  var GLOBAL;
  GLOBAL = 101;
  GLOBAL
};
