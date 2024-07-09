def f() {
  var res = 0;
  var v[5] = {1, 2, 3, 4, 5};
  var t = 10;
  
  for (var i=0; i<5; i = i+1) v[i] = 1;
  for (var i=0; i<5; i = i+1) res = res + v[i];
  
  res
};

def g() {
  var res = 0;
  var v[5] = {1, 2, 3, 4, 5};
  var t = 10;

  for (var i=0; i<5; i = i+1) res = res + v[i];
  
  res
};