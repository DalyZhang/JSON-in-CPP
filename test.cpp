#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "json-in-cpp/index.cpp"

void testHashTable1() {

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

void testHashTable2() {

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
	printf("ht[\"e\"][\"a\"] = %g\n", ht["e"].object()["a"].number());
	// delete &ht;

	printf("String rest: %i\n", String::rest);
	printf("UnicodeString rest: %i\n", UnicodeString::rest);
	printf("Var rest: %i\n", Var::rest);
	printf("HashTable rest: %i\n", HashTable::rest);

}

void testVar() {

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
				->push(new Var(false))
				->push(new Var(1.6))
				->push(new Var((new VarArray)
					->push(new Var)
				))
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
	printf("var[3][\"array\"][0] = ");
	var->array()[3].object()["array"].array()[0].write();
	puts("");

	printf("String rest: %i\n", String::rest);
	printf("UnicodeString rest: %i\n", UnicodeString::rest);
	printf("Var rest: %i\n", Var::rest);
	printf("HashTable rest: %i\n", HashTable::rest);

}

void testCoding() {

	FILE *fp = fopen("gbk-test.txt", "rb");

	char buffer[1000];
	fgets(buffer, 1000, fp);
	String string(buffer);
	UnicodeString uString(string, Coding::UTF8);

	printf("used buffer length: %i\n", strlen(buffer));
	printf("string length: %i\n", string.getLength());
	printf("unicode string length: %i\n", uString.getLength());
	uString.write(Coding::GBK);

}

int main() {

	testCoding();

	return 0;

}
