char32_t UnicodeString::NotATable = createCodingTable();

char32_t UnicodeString::GBKToUnicodeTable[0x10000]{};

char32_t UnicodeString::UnicodeToGBKTable[0x10000]{};

char32_t UnicodeString::createCodingTable() {
	createCodingTableGBK();
	return 0;
}

void UnicodeString::createCodingTableGBK() {
	int rawTable[] = CODING_FILE_GBK;
	int rawTableLength = sizeof (rawTable) / sizeof (rawTable[0]);
	unsigned int GBKChar, unicodeCharacter;
	for (int i1 = 0; i1 < rawTableLength; i1 += 2) {
		GBKChar = rawTable[i1];
		unicodeCharacter = rawTable[i1 + 1];
		GBKToUnicodeTable[GBKChar] = unicodeCharacter;
		UnicodeToGBKTable[unicodeCharacter] = GBKChar;
	}
}

int UnicodeString::countByteLeadOne(char byte) {
	unsigned char andMask = 0b10000000;
	for (int i1 = 0; i1 < 8; i1++, andMask >>= 1) {
		if (!(byte & andMask)) {
			return i1;
		}
	}
	return 8;
}

void UnicodeString::combineTwoBytes(char16_t &twoBytes, unsigned char nearStart, unsigned char nearEnd, bool isBigEndian) {
	twoBytes = isBigEndian ? (nearStart << 8) | nearEnd : (nearEnd << 8) | nearStart;
}

void UnicodeString::divideTwoBytes(unsigned short twoBytes, char &nearStart, char &nearEnd, bool isBigEndian) {
	if (isBigEndian) {
		nearStart = (char)(twoBytes >> 8);
		nearEnd = (char)twoBytes;
	} else {
		nearStart = (char)twoBytes;
		nearEnd = (char)(twoBytes >> 8);
	}
}

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

UnicodeString::UnicodeString(const char *cString, Coding coding)
	: UnicodeString(String(cString), coding) {}

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

char32_t &UnicodeString::at(const int &offset) const {
	if (offset < 0) {
		return source[length + offset];
	} else {
		return source[offset];
	}
}

char32_t &UnicodeString::operator[](const int &offset) const {
	return at(offset);
}

UnicodeString &UnicodeString::push(char32_t unicodeCharacter) {
	char32_t *oldSource = source;
	source = new char32_t[length + 2]{};
	memcpy(source, oldSource, sizeof (source[0]) * length);
	source[length] = unicodeCharacter;
	length++;
	delete[] oldSource;
	return *this;
}

UnicodeString UnicodeString::operator+=(char32_t unicodeCharacter) {
	return push(unicodeCharacter);
}

UnicodeString &UnicodeString::concat(const UnicodeString &right) {
	char32_t *oldSource = source;
	source = new char32_t[length + right.length + 1];
	memcpy(source, oldSource, sizeof (source[0]) * length);
	memcpy(source + length, right.source, sizeof (source[0]) * right.length);
	length += right.length;
	delete[] oldSource;
	return *this;
}

UnicodeString &UnicodeString::concat(const char *cString, Coding coding) {
	return concat(UnicodeString(cString, coding));
}

UnicodeString UnicodeString::operator+=(const UnicodeString &right) {
	return concat(right);
}

UnicodeString UnicodeString::operator+=(const char *cString) {
	return concat(cString);
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

	int i1, i2, i3, i4;
	char32_t u4c1, u4c2;
	char16_t u2c1, u2c2;

	int length = 0;
	char32_t *source = nullptr;
	bool isValid = true;
	switch (coding) {
		
	case Coding::RAW:
		length = encoded.length;
		source = new char32_t[length + 1]{};
		for (i1 = 0; i1 < encoded.length; i1++) {
			source[i1] = (char32_t)encoded.source[i1];
		}
		break;

	case Coding::GBK:
		source = new char32_t[encoded.length + 1]{};
		for (i1 = i2 = 0; i1 < encoded.length; i2++) {
			u4c1 = 0;
			if ((unsigned)encoded.source[i1] > 0x80) {
				if (i1 + 1 == encoded.length) {
					isValid = false;
					break;
				}
				memset(&u4c1, encoded.source[i1 + 1], 1);
				memset((char *)&u4c1 + 1, encoded.source[i1], 1);
				i1 += 2;
			} else {
				memset(&u4c1, encoded.source[i1], 1);
				i1++;
			}
			u4c2 = GBKToUnicodeTable[(unsigned)u4c1];
			if (u4c2 != 0 || u4c1 == 0) {
				source[i2] = u4c2;
			} else {
				isValid = false;
				break;
			}
		}
		length = i2;
		break;

	case Coding::UTF8:
		source = new char32_t[encoded.length + 1]{};
		for (i1 = i2 = 0; i1 < encoded.length; i2++) {
			i3 = countByteLeadOne(encoded.source[i1]);
			if (i3 == 1) {
				isValid = false;
				break;
			} else if (i3 == 0) {
				source[i2] = (char32_t)encoded.source[i1];
				i1++;
			} else {
				source[i2] = (char32_t)(encoded.source[i1] & ~(~0 << (8 - i3)));
				i1++;
				for (i4 = 1; i4 < i3; i4++, i1++) {
					if (i1 == encoded.length || countByteLeadOne(encoded.source[i1]) != 1) {
						isValid = false;
						break;
					}
					source[i2] = (source[i2] << 6) | (encoded.source[i1] & 0b00111111);
				}
			}
		}
		length = i2;
		break;

	case Coding::UTF16BE: case Coding::UTF16LE:
		if (encoded.length & 1) {
			isValid = true;
			break;
		}
		source = new char32_t[(encoded.length >> 1) + 1];
		for (i1 = i2 = 0; i1 < encoded.length; i2++) {
			combineTwoBytes(u2c1, encoded.source[i1], encoded.source[i1 + 1],
				coding == Coding::UTF16BE);
			i1 += 2;
			if ((u2c1 ^ 0xD800) >> 10) {
				source[i2] = u2c1;
			} else {
				if (i1 == encoded.length) {
					isValid = false;
					break;
				}
				combineTwoBytes(u2c2, encoded.source[i1], encoded.source[i1 + 1],
					coding == Coding::UTF16BE);
				if ((u2c2 ^ 0xDC00) >> 10) {
					isValid = false;
					break;
				}
				i1 += 2;
				source[i2] = (((u2c1 & ~(~0 << 10)) << 10) | (u2c2 & ~(~0 << 10))) + 0x10000;
			}
		}
		length = i2;
		break;

	}

	if (target.source != nullptr) {
		delete[] target.source;
	}
	if (isValid) {
		char32_t *oldSource = source;
		source = new char32_t[length + 1]{};
		memcpy(source, oldSource, sizeof (source[0]) * length);
		delete[] oldSource;
	} else {
		if (source != nullptr) {
			delete[] source;
		}
		source = new char32_t[1]{};
		length = 0;
	}
	target.length = length;
	target.source = source;

}

void UnicodeString::decodeAssign(String &target, const UnicodeString &decoded, Coding coding) {

	int i1, i2, i3, i4;
	char32_t u4c1, u4c2;

	int length = 0;
	char *source = nullptr;
	bool isValid = true;
	switch (coding) {

	case Coding::RAW:
		length = decoded.length;
		source = new char[length + 1]{};
		for (i1 = 0; i1 < decoded.length; i1++) {
			source[i1] = (char)decoded.source[i1];
		}
		break;

	case Coding::GBK:
		source = new char[decoded.length * 2 + 1]{};
		for (i1 = i2 = 0; i1 < decoded.length; i1++) {
			u4c1 = decoded.source[i1];
			u4c2 = UnicodeToGBKTable[(unsigned)u4c1];
			if (u4c2 != 0 || u4c1 == 0) {
				if (u4c2 & ~0xFF) {
					source[i2] = (char)((u4c2 >> 8) & 0xFF);
					source[i2 + 1] = (char)(u4c2 & 0xFF);
					i2 += 2;
				} else {
					source[i2] = (char)u4c2;
					i2++;
				}
			} else {
				isValid = false;
				break;
			}
		}
		length = i2;
		break;

	case Coding::UTF8:
		source = new char[decoded.length * 4 + 1]{};
		for (i1 = i2 = 0; i1 < decoded.length; i1++) {
			u4c1 = decoded.source[i1];
			if (u4c1 < 0) {
				isValid = false;
				break;
			} else if (u4c1 < 0x80) {
				source[i2] = (char)u4c1;
				i2++;
				continue;
			} else if (u4c1 < 0x800) {
				i3 = 2;
			} else if (u4c1 < 0x10000) {
				i3 = 3;
			} else if (u4c1 < 0x110000) {
				i3 = 4;
			} else {
				isValid = false;
				break;
			}
			source[i2] = (~0 << (8 - i3)) | (u4c1 >> ((i3 - 1) * 6));
			i2++;
			for (i4 = i3 - 2; i4 > -1; i4--, i2++) {
				source[i2] = 0b10000000 | ((u4c1 >> (i4 * 6)) & 0b00111111);
			}
		}
		length = i2;
		break;

	case Coding::UTF16BE: case Coding::UTF16LE:
		source = new char[decoded.length * 4 + 1]{};
		for (i1 = i2 = 0; i1 < decoded.length; i1++) {
			u4c1 = decoded.source[i1];
			if ((u4c1 >= 0 && u4c1 < 0xD800) || (u4c1 >= 0xE000 && u4c1 < 0x10000)) {
				divideTwoBytes((unsigned short)u4c1, source[i2], source[i2 + 1],
					coding == Coding::UTF16BE);
				i2 += 2;
			} else if (u4c1 >= 0x10000 && u4c1 < 0x110000) {
				u4c1 -= 0x10000;
				divideTwoBytes((unsigned short)((u4c1 >> 10) | 0xD800), source[i2], source[i2 + 1],
					coding == Coding::UTF16BE);
				i2 += 2;
				divideTwoBytes((unsigned short)((u4c1 & ~(~0 << 10)) | 0xDC00), source[i2], source[i2 + 1],
					coding == Coding::UTF16BE);
				i2 += 2;
			} else {
				isValid = false;
				break;
			}
		}
		length = i2;
		break;

	}

	if (target.source != nullptr) {
		delete[] target.source;
	}
	if (isValid) {
		char *oldSource = source;
		source = new char[length + 1]{};
		memcpy(source, oldSource, sizeof (source[0]) * length);
		delete[] oldSource;
	} else {
		delete[] source;
		source = new char[1]{};
		length = 0;
	}
	target.length = length;
	target.source = source;

}
