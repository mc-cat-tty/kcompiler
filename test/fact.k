extern n();
extern printval(x y);
def fact(n) {
   n<1 ? 1 : n*fact(n-1)
};
def main() {
  var N = n();
  printval(N,fact(N))
};
