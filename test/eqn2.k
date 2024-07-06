extern sqrt(x);
extern printval(x1 x2 flag);
def eqn2(a b c) {
   var delta2 = b*b-4*a*c;
   if (delta2<0) {
      var delta = sqrt(-delta2);
      var re = -b/(2*a);
      var im = delta/(2*a);
      printval(re,im,0)
   } else {
     if (delta2==0) {
        var x12 = -b/(2*a);
        printval(x12,0,0)
     } else {
        var delta = sqrt(delta2);
        var x1 = (-b+delta)/(2*a);
        var x2 = (-b-delta)/(2*a);
        printval(x1,x2,1)
     }
   }
};
