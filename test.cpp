#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "json-in-cpp/index.cpp"

long long getFileSize(FILE *fp) {
	fpos_t size;
	fpos_t currentPos;
	fgetpos(fp, &currentPos);
	fseek(fp, 0, SEEK_END);
	fgetpos(fp, &size);
	fseek(fp, currentPos, SEEK_SET);
	return size;
}

UnicodeString getFileContent(const char *fileName, Coding::Coding coding = Coding::UTF8) {
	FILE *read = fopen(fileName, "rb");
	long long fileSize = getFileSize(read);
	char *buffer = new char[fileSize];
	fread(buffer, 1, fileSize, read);
	fclose(read);
	return UnicodeString(String(buffer, fileSize), coding);
}

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

	FILE *fp = fopen("utf16le-test.txt", "rb");
	char buffer[1000];
	fseek(fp, 2, SEEK_SET);
	fread(buffer, 1, 1000, fp);
	String string(buffer, getFileSize(fp) - 2);
	UnicodeString uString(string, Coding::UTF16LE);
	
	// FILE *fp = fopen("cc.txt", "rb");
	// char buffer[1000];
	// fgets(buffer, 1000, fp);
	// String string(buffer);
	// UnicodeString uString(string, Coding::UTF8);

	printf("cString length: %i\n", strlen(buffer));
	printf("string length: %i\n", string.getLength());
	printf("unicode string length: %i\n", uString.getLength());

	FILE *writen = fopen("result.txt", "wb");
	// fputc(0xFE, writen);
	// fputc(0xFF, writen);
	// fputc(0xFE, writen);
	uString.write(Coding::UTF8, writen);
	for (int i1 = 0; i1 < uString.getLength(); i1++) {
		printf(" 0x%X", uString[i1]);
	}

}

void JSONTest() {

	// // Var *data = JSON::decode(UnicodeString("\"4\\r\\n\\u4E3a\\uD83D\\uDC7F\\u4E3a\\t56\""));
	// Var *data = JSON::decode(UnicodeString("[\"5\", {\"k\": [1, 2, false, \"哈哈哈哈哈\"]}, null, []]"));
	// if (data == nullptr) {
	// 	puts("invalid json string");
	// } else {
	// 	printf("type: %i\n", data->getType());
	// 	printf("value: ");
	// 	// data->write(Coding::UTF8, fopen("result.txt", "wb"));
	// 	data->write(Coding::GBK);
	// 	puts("");
	// }

	UnicodeString data = getFileContent("test.json");
	Var *parsedData = JSON::decode(data);
	parsedData->write(Coding::GBK);
	puts("");

	printf("String rest: %i\n", String::rest);
	printf("UnicodeString rest: %i\n", UnicodeString::rest);
	printf("Var rest: %i\n", Var::rest);
	printf("HashTable rest: %i\n", HashTable::rest);
	
}

int main() {

	JSONTest();

	return 0;

}
