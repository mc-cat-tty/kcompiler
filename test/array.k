def f() {
  var v[10];
  var res = 0;

  for (var i = 0; i < 10; i = i+1) v[i]=i;
  for (var i = 0; i < 10; i = i+1) res = res+v[i];

  res
};