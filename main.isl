
module main;

import io;

record Person {
	var name : String;
	var age : i8;
}

pure incAge(age : i8) -> i8 {
	return age + 1;
}

entity Reader {
	var store : i8;

	proc inVal(input : i8) {
		this.store = input;
	}

	proc loadVal() -> i8 {
		return this.store;
	}
}

entry {

}