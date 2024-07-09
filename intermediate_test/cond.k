global not_test;
global and_test;
global or_test;

def f(x y) {
  not_test = not (x>0) ? 1 : 0;
  and_test = x>0 and y>0 ? 1 : 0;
  or_test = (x>0 or y>0) ? 1 : 0
};