int String::rest = 0;

String::String() {
	rest++;
	length = 0;
	source = new char[1]{};
}

String::String(const String &copied) : String(copied.source, copied.length) {}

String::String(const char *cString) : String(cString, strlen(cString)) {}

String::String(char copiedSource) {
	rest++;
	length = 1;
	source = new char[2]{copiedSource};
}

String::String(const char *copiedSource, int length) {
	rest++;
	this->length = length;
	source = new char[length + 1]{};
	memcpy(source, copiedSource, length * sizeof (source[0]));
}

String::~String() {
	// puts("fuckString");
	rest--;
	delete[] source;
}

int String::getLength() const {
	return length;
}

const char *String::cString() {
	return source;
}

char &String::at(const int &offset) const {
	return source[offset];
}

char &String::operator[](const int &offset) const {
	return source[offset];
}

String &String::copyAssign(const char *cString) {
	delete[] source;
	length = strlen(cString);
	source = new char[length + 1]{};
	memcpy(source, cString, length);
	return *this;
}

String &String::copyAssign(const String &copied) {
	delete[] source;
	length = copied.length;
	source = new char[length + 1]{};
	memcpy(source, copied.source, length);
	return *this;
}

String String::operator=(const char *cString) {
	return String(copyAssign(cString));
}

String String::operator=(const String &copied) {
	return String(copyAssign(copied));
}

void String::write(FILE *fp) const {
	fwrite(source, sizeof (char), length, fp);
}

// UnicodeString *String::encode(Coding coding) {
// 	return new UnicodeString(*this, coding);
// }

// void String::decodeAssign(UnicodeString &decoded, Coding coding) {
// 	UnicodeString::decodeAssign(*this, decoded, coding);
// }
