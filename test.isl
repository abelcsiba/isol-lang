
module main; // This is a comment

import std;#

var sum : i8 = 3 + 5 * 4;

record Person {
  var first_name : str;
}

entity Car {
  var type : str;

  proc foo(input : str) {
	this.type = input;
  }

  proc baz() -> i8 {
    return 42;
  }
}

pure bar(first : i8, second: i16) -> i8 {
  return first + second;
}

entry {
  var car : Car;
  var result : i8 = car.baz();
}
