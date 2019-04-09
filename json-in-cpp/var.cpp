int Var::rest = 0;

Var::Var() {
	rest++;
	set();
}

Var::Var(const Var &copied) {
	rest++;
	switch (copied.type) {
	case T_NULL:
		set();
		break;
	case T_NUMBER:
		set(copied.value.n);
		break;
	case T_BOOLEAN:
		set(copied.value.b);
		break;
	case T_STRING:
		set(*copied.value.s);
		break;
	case T_OBJECT:
		set(copied.value.o);
		break;
	case T_ARRAY:
		set(copied.value.a);
		break;
	}
}

Var::Var(void *null) {
	rest++;
	set(null);
}

Var::Var(int number) {
	rest++;
	set(number);
}

Var::Var(double number) {
	rest++;
	set(number);
}

Var::Var(bool boolean) {
	rest++;
	set(boolean);
}

Var::Var(const UnicodeString &string) {
	rest++;
	set(string);
}

Var::Var(const String &rawString, Coding coding) {
	rest++;
	set(rawString, coding);
}

Var::Var(const char *cString, Coding coding) {
	rest++;
	set(cString, coding);
}

Var::Var(char character, Coding coding) {
	rest++;
	set(character, coding);
}

Var::Var(HashTable *object) {
	rest++;
	set(object);
}

Var::Var(VarArray *array) {
	rest++;
	set(array);
}

Var::~Var() {
	// puts("fuckVar");
	rest--;
	set();
}

Var::Type Var::getType() const {
	return type;
}

double Var::number() const {
	return value.n;
}

bool Var::boolean() const {
	return value.b;
}

UnicodeString &Var::string() const {
	return *value.s;
}

HashTable &Var::object() const {
	return *value.o;
}

VarArray &Var::array() const {
	return *value.a;
}

bool Var::is(Type type) const {
	return this->type == type;
}

bool Var::isNull() const {
	return is(T_NULL);
}

bool Var::isNumber() const {
	return is(T_NUMBER);
}

bool Var::isBoolean() const {
	return is(T_BOOLEAN);
}

bool Var::isString() const {
	return is(T_STRING);
}

bool Var::isObject() const {
	return is(T_OBJECT);
}

bool Var::isArray() const {
	return is(T_ARRAY);
}

void Var::set() {
	switch (type) {
	case T_STRING:
		delete value.s;
		break;
	case T_OBJECT:
		delete value.o;
		break;
	case T_ARRAY:
		delete value.a;
		break;
	}
	memset(&value, 0, sizeof (value));
	type = T_NULL;
}

void Var::set(void *null) {
	if (null == nullptr) {
		set();
	} else {
		set((int)null);
	}
}

void Var::set(int number) {
	set();
	type = T_NUMBER;
	value.n = (double)number;
}

void Var::set(double number) {
	set();
	type = T_NUMBER;
	value.n = number;
}

void Var::set(bool boolean) {
	set();
	type = T_BOOLEAN;
	value.b = boolean;
}

void Var::set(const UnicodeString &string) {
	set();
	type = T_STRING;
	value.s = new UnicodeString(string);
}

void Var::set(const String &rawString, Coding coding) {
	set(UnicodeString(rawString, coding));
}

void Var::set(const char *cString, Coding coding) {
	set(UnicodeString(cString, coding));
}

void Var::set(char character, Coding coding) {
	String rawString(character);
	set(rawString, coding);
}

void Var::set(HashTable *object) {
	set();
	type = T_OBJECT;
	value.o = object;
}

void Var::set(VarArray *array) {
	set();
	type = T_ARRAY;
	value.a = array;
}

void Var::write(Coding coding, FILE *fp) const {
	switch (type) {
	case T_NULL:
		fprintf(fp, "null");
		break;
	case T_NUMBER:
		fprintf(fp, "%g", value.n);
		break;
	case T_BOOLEAN:
		fprintf(fp, value.b ? "true" : "false");
		break;
	case T_STRING:
		fputc('"', fp);
		value.s->write(coding, fp);
		fputc('"', fp);
		break;
	case T_OBJECT:
		value.o->write(coding, fp);
		break;
	case T_ARRAY:
		value.a->write(coding, fp);
		break;
	}
}
