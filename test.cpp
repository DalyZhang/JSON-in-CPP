#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "json-in-cpp/index.cpp"

// void test1() {

// 	// set get [] has remove clear getLength begin end
// 	HashTable ht(255);
// 	printf("length: %i\n", ht.getLength());
// 	ht.set("123", (JSON *)"789");
// 	ht.set("123", (JSON *)"12");
// 	ht.set("123", (JSON *)"41");
// 	ht.set("124", (JSON *)"36");
// 	ht.set("aaavvv", (JSON *)"3456ss6");
// 	for (auto &key : ht) {
// 		printf("for key[%s]: %s\n", key, &ht[key]);
// 	}
// 	// printf("%s", ht.get("123"));
// 	printf("length: %i\n", ht.getLength());
// 	printf("key[123]: %s\n", &ht["123"]);
// 	puts(ht.has("124") ? "true" : "false");
// 	puts(ht.has("125") ? "true" : "false");
// 	puts(ht.remove("124") ? "true" : "false");
// 	printf("length: %i\n", ht.getLength());
// 	puts(ht.has("124") ? "true" : "false");
// 	ht.set("124", (JSON *)"36");
// 	printf("length: %i\n", ht.getLength());
// 	printf("key[124]: %s\n", &ht["124"]);
// 	puts(ht.has("124") ? "true" : "false");
// 	puts(ht.has("123") ? "true" : "false");
// 	printf("key[123]: %s\n", &ht["123"]);
// 	ht.clear();
// 	printf("length: %i\n", ht.getLength());
// 	puts(ht.has("123") ? "true" : "false");
// 	printf("linkListLength: %i\n", ht.getLinkListLength());

// }

void test2() {

	HashTable ht(255);

	FILE *fp = fopen("hash-table.txt", "rb");
	char keyBuffer[128], valueBuffer[128];
	String str;
	Var *var = nullptr;

	int ord = 0;
	while (fscanf(fp, "%s %s", keyBuffer, valueBuffer) != EOF) {
		str = valueBuffer;
		var = new Var;
		var->set(str);
		ht.set(keyBuffer, var);
		ord++;
	}
	ht.writeLinkList();
	// for (auto &key : ht) {
	// 	printf("for key[");
	// 	key.write();
	// 	printf("]: ");
	// 	ht[key].write();
	// 	puts("");
	// }
	ht.write();
	puts("");
	printf("length: %i\n", ht.getLength());

}

// void test3() {

// 	const char *raw = "123456aaa\000334";

// 	String str = String(raw, 12);
// 	UnicodeString ustr(str);

// 	printf("str[0] = '%c'\n", str.at(0));
// 	printf("ustr[0] = %i\n", ustr.at(0));
// 	ustr.write();
// 	puts("");

// 	String *newStr = ustr.decode();
// 	newStr->write();
// 	puts("");

// 	UnicodeString newUstr = ustr;
// 	ustr.write();
// 	puts("==");

// 	puts(str.cString());

// }

void test4() {

	HashTable &ht = *((new HashTable(101))
		->set("addd", new Var("123.12"))
		->set("b", new Var((void *)0))
		->set("c", new Var(true))
		->set("d", new Var(7))
		->set("e", new Var('7'))
		->set("e", new Var((new HashTable(51))
			->set("a", new Var(1.1))
			->set("ddd", new Var("123"))
			->set("apple", new Var(""))
			->set("daly", new Var((void *)0))
		))
		->set("f", new Var(123))
	);
	ht.writeLinkList();
	ht.write();
	puts("");
	printf("number: %g\n", ht["e"].object()["a"].number());
	// delete &ht;

	printf("String rest: %i\n", String::rest);
	printf("UnicodeString rest: %i\n", UnicodeString::rest);
	printf("Var rest: %i\n", Var::rest);
	printf("HashTable rest: %i\n", HashTable::rest);

}

void test5() {

	Var *var = new Var((new VarArray)
		->push(new Var(1.1))
		->push(new Var("5"))
		->push(new Var(true))
		->push(new Var)
		->pop()
		->push(new Var((new HashTable(51))
			->set("12", new Var(1))
			->set("array", new Var((new VarArray)
				->push(new Var)
			))
			->set("hhh", new Var('['))
			->remove("12")
		))
		->push(new Var((new VarArray)
			->push(new Var(1))
			->push(new Var(""))
		))
		->push(new Var((new VarArray)
			->push(new Var(1))
			->push(new Var(""))
		))
	);

	// VarArray va(var->array());

	// va.write();
	// puts("");

	var->write();
	puts("");

	printf("String rest: %i\n", String::rest);
	printf("UnicodeString rest: %i\n", UnicodeString::rest);
	printf("Var rest: %i\n", Var::rest);
	printf("HashTable rest: %i\n", HashTable::rest);

}

// class A {
// public:
// 	int a;
// 	A(int num = 0) : a(num) {
// 		printf("new: %i in %p\n", num, this);
// 	}
// 	A(const A &origin) {
// 		a = origin.a;
// 	}
// 	~A() {
// 		printf("delete: %i\n", a);
// 	}
// 	A operator+(const A &right) {
// 		return A(this->a + right.a);
// 	}
// };

// void p(const A &a) {
// 	printf("%i\n", a.a);
// }

// void test5() {
// 	p(A());
// }

// A plus(const A &a, const A &b) {
// 	return A(a.a + b.a);
// }

int main() {

	// A a = A(1) + A(2) + A(4);
	// printf("get a in %p\n", &a);
	// int b = 5;
	// printf("get b in %p\n", &b);
	// int c = 5;
	// printf("get c in %p\n", &c);
	// A d = A(6) + A(3) + A(1);
	// printf("get d in %p\n", &d);
	// puts("get");
	// A e = plus(A(1), A(2));

	test5();

	return 0;

}
