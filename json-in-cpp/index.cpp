#ifndef __DALY_JSON_IN_CPP
#define __DALY_JSON_IN_CPP

#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "coding-files/index.cpp"

#define HASH_TABLC_LINK_LIST_LENGTH_DEFAULT 255
#define CODING_SETTING_DEFAULT Coding::UTF8

namespace Coding {
	enum Coding {RAW, UTF8, UTF16BE, UTF16LE, GBK, EASCII, UCS2, UCS4};
}

class String;
class UnicodeString;
class HashTable;
class VarArray;
class Var;

class String {
	friend class UnicodeString;
private:
	int length;
	char *source = nullptr;
public:
	static int rest;

	String();
	String(const String &copied);
	String(const char *cString);
	String(char copiedSource);
	String(const char *copiedSource, int length);
	~String();

	int getLength() const;

	const char *cString();

	char &at(const int &offset) const;
	char &operator[](const int &offset) const;

	String &copyAssign(const char *cString);
	String &copyAssign(const String &copied);
	String operator=(const char *cString);
	String operator=(const String &copied);

	// String &concat(const String &rightValue);
	// String operator+(const String &rightValue);

	void write(FILE *fp = stdout) const;

	// UnicodeString *encode(Coding coding = CODING_SETTING_DEFAULT);
	// void decodeAssign(UnicodeString &decoded, Coding coding = CODING_SETTING_DEFAULT);
};

class UnicodeString {
	// friend class String;
private:
	static char32_t NotATable;
	static char32_t GBKToUnicodeTable[0x10000];
	static char32_t UnicodeToGBKTable[0x10000];
	static char32_t createCodingTable();
	static void createCodingTableGBK();
	int length;
	char32_t *source = nullptr;
	UnicodeString(const char32_t *copiedSource, int length);
	static int countByteLeadOne(char byte);
	static void combineTwoBytes(char16_t &twoBytes, unsigned char nearStart, unsigned char nearEnd, bool isBigEndian = true);
	static void divideTwoBytes(unsigned short bytes, char &nearStart, char &nearEnd, bool isBigEndian = true);
public:
	static int rest;

	typedef Coding::Coding Coding;

	UnicodeString();
	UnicodeString(const UnicodeString &copied);
	UnicodeString(const char *cString, Coding coding = CODING_SETTING_DEFAULT);
	UnicodeString(const String &encoded, Coding coding = CODING_SETTING_DEFAULT);
	~UnicodeString();

	int getLength() const;

	char32_t &at(const int &offset) const;
	char32_t &operator[](const int &offset) const;

	UnicodeString &copyAssign(const UnicodeString &copied);
	UnicodeString operator=(const UnicodeString &copied);

	void write(Coding coding = CODING_SETTING_DEFAULT, FILE *fp = stdout) const;

	static bool compare(const UnicodeString &a, const UnicodeString &b);
	bool operator==(const UnicodeString &right);

	String *decode(Coding coding = CODING_SETTING_DEFAULT);
	void encodeAssign(const String &encoded, Coding coding = CODING_SETTING_DEFAULT);
	static void encodeAssign(UnicodeString &target, const String &encoded, Coding coding = CODING_SETTING_DEFAULT);
	static void decodeAssign(String &target, const UnicodeString &decoded, Coding coding = CODING_SETTING_DEFAULT);
};

class HashTable {
private:
	class Iterator;
	class Node {
		friend class HashTable;
		friend class Iterator;
	private:
		UnicodeString *key = nullptr;
		Var *value = nullptr;
		Node *linkPrev = nullptr, *linkNext = nullptr;
		Node *ordPrev = nullptr, *ordNext = nullptr;
	};
	class Iterator {
	private:
		Node *current = nullptr;
	public:
		Iterator(Node *begin);
		bool operator!=(Iterator &end);
		Iterator &operator++();
		UnicodeString &operator*();
	};
	int length = 0, linkListLength;
	Node **linkList = nullptr;
	Node *ordSentinel = nullptr;
	int hash(const UnicodeString &key) const;
public:
	static int rest;

	typedef Coding::Coding Coding;

	HashTable(const HashTable &copied);
	HashTable(int linkListLength = HASH_TABLC_LINK_LIST_LENGTH_DEFAULT);
	~HashTable();

	Var *get(const char *cStringKey, Coding coding = CODING_SETTING_DEFAULT) const;
	Var *get(const UnicodeString &key) const;

	HashTable *set(const char *cStringKey, Var *value, bool deleteOccupied = true, Coding coding = CODING_SETTING_DEFAULT);
	HashTable *set(const UnicodeString &key, Var *value, bool deleteOccupied = true);

	bool has(const char *cStringKey, Coding coding = CODING_SETTING_DEFAULT);
	bool has(const UnicodeString &key);

	HashTable *remove(const char *cStringKey, bool deleteOccupied = true, Coding coding = CODING_SETTING_DEFAULT);
	HashTable *remove(const UnicodeString &key, bool deleteOccupied = true);

	HashTable *clear(bool deleteOccupied = true);

	void write(Coding coding = CODING_SETTING_DEFAULT, FILE *fp = stdout) const;

	int getLinkListLength() const;
	int getLength() const;

	Iterator begin() const;
	Iterator end() const;

	Var &operator[](const char *cStringKey);
	Var &operator[](const UnicodeString &key);
	
	void writeLinkList(Coding coding = CODING_SETTING_DEFAULT, FILE *fp = stdout);
};

class VarArray {
private:
	class Iterator {
	private:
		Var **current = nullptr;
	public:
		Iterator(Var **begin);
		bool operator!=(Iterator &end);
		Iterator &operator++();
		Var &operator*();
	};
	int length = 0;
	Var **source = nullptr;
	void correctOrd(int &ord, int length);
public:
	static int rest;

	typedef Coding::Coding Coding;

	VarArray();
	VarArray(const VarArray &copied, int start = 0);
	VarArray(const VarArray &copied, int start, int end);
	// VarArray(Var *array, int length);
	~VarArray();

	Var &at(const int &offset) const;
	Var &operator[](const int &offset) const;

	VarArray *push(Var *item);
	VarArray *pop(bool deleteOccupied = true);
	// void unshift(Var *item);
	// void shift(bool deleteOccupied = true);
	VarArray *clear(bool deleteOccupied = true);

	int getLength() const;

	Iterator begin() const;
	Iterator end() const;

	void write(Coding coding = CODING_SETTING_DEFAULT, FILE *fp = stdout) const;
};

class Var {
public:
	enum Type {T_NULL, T_NUMBER, T_BOOLEAN, T_STRING, T_OBJECT, T_ARRAY};
private:
	Type type = T_NULL;
	union {
		double n;
		bool b;
		UnicodeString *s;
		HashTable *o;
		VarArray *a;
	} value;
public:
	static int rest;

	typedef Coding::Coding Coding;

	Var();
	Var(const Var& copied);
	Var(void *null);
	Var(int number);
	Var(double number);
	Var(bool boolean);
	Var(const UnicodeString &string);
	Var(const String &rawString, Coding coding = CODING_SETTING_DEFAULT);
	Var(const char *cString, Coding coding = CODING_SETTING_DEFAULT);
	Var(char character, Coding coding = CODING_SETTING_DEFAULT);
	Var(HashTable *object);
	Var(VarArray *array);
	~Var();

	Type getType();

	double number();
	bool boolean();
	UnicodeString &string();
	HashTable &object();
	VarArray &array();

	bool is(Type type);
	bool isNull();
	bool isNumber();
	bool isBoolean();
	bool isString();
	bool isObject();
	bool isArray();

	void set();
	void set(void *null);
	void set(int number);
	void set(double number);
	void set(bool boolean);
	void set(const UnicodeString &string);
	void set(const String &rawString, Coding coding = CODING_SETTING_DEFAULT);
	void set(const char *cString, Coding coding = CODING_SETTING_DEFAULT);
	void set(char character, Coding coding = CODING_SETTING_DEFAULT);
	void set(HashTable *object);
	void set(VarArray *array);

	void write(Coding coding = CODING_SETTING_DEFAULT, FILE *fp = stdout);

};

#include "string.cpp"
#include "unicode-string.cpp"
#include "hash-table.cpp"
#include "var-array.cpp"
#include "var.cpp"

#endif
