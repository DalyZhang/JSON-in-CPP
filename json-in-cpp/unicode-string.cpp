int UnicodeString::rest = 0;

UnicodeString::UnicodeString() {
	rest++;
	length = 0;
	source = new char32_t[1]{};
}

UnicodeString::UnicodeString(const UnicodeString &copied)
	: UnicodeString(copied.source, copied.length) {}

UnicodeString::UnicodeString(const char32_t *copiedSource, int length) {
	rest++;
	this->length = length;
	source = new char32_t[length + 1]{};
	memcpy(source, copiedSource, length * sizeof (source[0]));
}

UnicodeString::UnicodeString(const String &encoded, Coding coding) {
	rest++;
	encodeAssign(encoded, coding);
}

UnicodeString::~UnicodeString() {
	// puts("fuckUnicodeString");
	rest--;
	delete[] source;
}

int UnicodeString::getLength() const {
	return length;
}

char32_t &UnicodeString::at(int offset) const {
	return source[offset];
}

UnicodeString &UnicodeString::copyAssign(const UnicodeString &copied) {
	delete[] source;
	length = copied.length;
	source = new char32_t[length + 1];
	memcpy(source, copied.source, length * sizeof (source[0]));
}

UnicodeString UnicodeString::operator=(const UnicodeString &copied) {
	copyAssign(copied);
	return UnicodeString(copyAssign(copied));
}

void UnicodeString::write(Coding coding, FILE *fp) const {
	String rawString;
	UnicodeString::decodeAssign(rawString, *this, coding);
	// rawString.decodeAssign(*this, coding);
	rawString.write(fp);
}

bool UnicodeString::compare(const UnicodeString &a, const UnicodeString &b) {
	if (a.length != b.length) {
		return false;
	}
	return !memcmp(a.source, b.source, sizeof (a.source[0]) * a.length);
}

bool UnicodeString::operator==(const UnicodeString &right) {
	return UnicodeString::compare(*this, right);
}

String *UnicodeString::decode(Coding coding) {
	String *newString = new String;
	UnicodeString::decodeAssign(*newString, *this, coding);
	return newString;
}

void UnicodeString::encodeAssign(const String &encoded, Coding coding) {
	UnicodeString::encodeAssign(*this, encoded, coding);
}

void UnicodeString::encodeAssign(UnicodeString &target, const String &encoded, Coding coding) {

	int i1;

	int length = 0, encodedLength = encoded.length;
	char32_t *source = nullptr, *oldSource = nullptr;
	switch (coding) {
	case C_ASCII: case C_GBK: case C_UTF8:
		length = encodedLength;
		source = new char32_t[length + 1];
		for (i1 = 0; i1 < encodedLength; i1++) {
			source[i1] = (char32_t)encoded.source[i1];
		}
		break;
	}

	if (target.source != nullptr) {
		delete[] target.source;
	}
	target.length = length;
	target.source = source;

}

void UnicodeString::decodeAssign(String &target, const UnicodeString &decoded, Coding coding) {

	int i1;

	int length = 0, decodedLength = decoded.length;
	char *source = nullptr, *oldSource = nullptr;
	switch (coding) {
	case C_ASCII: case C_GBK: case C_UTF8:
		length = decodedLength;
		source = new char[length + 1];
		for (i1 = 0; i1 < decodedLength; i1++) {
			source[i1] = (char)decoded.source[i1];
		}
		break;
	}

	if (target.source != nullptr) {
		delete[] target.source;
	}
	target.length = length;
	target.source = source;

}
