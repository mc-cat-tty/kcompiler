def f() {
  var res = 0;
  var v[5] = {1, 2, 3, 4, 5};
  
  for (var i=0; i<5; i = i+1) v[i] = 1;
  for (var i=0; i<5; ++i) res = res + v[i];
  
  res
};

def g() {
  var res = 0;
  var v[5] = {1, 2, 3, 4, 5, 6};  # Expressly longer

  for (var i=0; i<5; ++i) res = res + v[i];
  
  res
};