global stack_garbage1;
global val[10];
global stack_garbage1;

def f(i) {
  var res = val[i];
  val[i] = 101;
  res
};