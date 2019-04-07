char32_t UnicodeString::NotATable = createCodingTable();

char32_t UnicodeString::GBKToUnicodeTable[0x10000]{};

char32_t UnicodeString::UnicodeToGBKTable[0x10000]{};

char32_t UnicodeString::createCodingTable() {
	createCodingTableGBK();
	return 0;
}

void UnicodeString::createCodingTableGBK() {
	FILE *GBKFile = fopen(CODING_FILE_GBK, "rb");
	if (GBKFile == nullptr) {
		puts("GBK coding file does not exist!");
		exit(1);
	}
	int GBKChar, UnicodeChar;
	while (fscanf(GBKFile, "%X %X", &GBKChar, &UnicodeChar) != EOF) {
		GBKToUnicodeTable[(unsigned)GBKChar] = UnicodeChar;
		UnicodeToGBKTable[(unsigned)UnicodeChar] = GBKChar;
	}
}

int UnicodeString::countByteLead1(char byte) {
	unsigned char andMask = 0b10000000;
	for (int i1 = 0; i1 < 8; i1++, andMask >>= 1) {
		if (!(byte & andMask)) {
			return i1;
		}
	}
	return 8;
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

	int i1, i2, i3, i4;
	char32_t uc1, uc2;

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
			uc1 = 0;
			if ((unsigned)encoded.source[i1] > 0x80) {
				if (i1 + 1 == encoded.length) {
					isValid = false;
					break;
				}
				memset(&uc1, encoded.source[i1 + 1], 1);
				memset((char *)&uc1 + 1, encoded.source[i1], 1);
				i1 += 2;
			} else {
				memset(&uc1, encoded.source[i1], 1);
				i1++;
			}
			uc2 = GBKToUnicodeTable[(unsigned)uc1];
			if (uc2 != 0 || uc1 == 0) {
				source[i2] = uc2;
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
			i3 = countByteLead1(encoded.source[i1]);
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
					if (i1 == encoded.length || countByteLead1(encoded.source[i1]) != 1) {
						isValid = false;
						break;
					}
					source[i2] = (source[i2] << 6) | (encoded.source[i1] & 0b00111111);
				}
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
		delete[] source;
		source = new char32_t[1]{};
		length = 0;
	}
	target.length = length;
	target.source = source;

}

void UnicodeString::decodeAssign(String &target, const UnicodeString &decoded, Coding coding) {

	int i1, i2, i3, i4;
	char32_t uc1, uc2;

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
			uc1 = decoded.source[i1];
			uc2 = UnicodeToGBKTable[(unsigned)uc1];
			if (uc2 != 0 || uc1 == 0) {
				if (uc2 & ~0xFF) {
					source[i2] = (char)((uc2 >> 8) & 0xFF);
					source[i2 + 1] = (char)(uc2 & 0xFF);
					i2 += 2;
				} else {
					source[i2] = (char)uc2;
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
			uc1 = decoded.source[i1];
			if (uc1 < 0) {
				isValid = false;
				break;
			} else if (uc1 < 0x80) {
				source[i2] = (char)uc1;
				i2++;
				continue;
			} else if (uc1 < 0x800) {
				i3 = 2;
			} else if (uc1 < 0x10000) {
				i3 = 3;
			} else if (uc1 < 0x110000) {
				i3 = 4;
			} else {
				isValid = false;
				break;
			}
			source[i2] = (~0 << (8 - i3)) | (uc1 >> ((i3 - 1) * 6));
			i2++;
			for (i4 = i3 - 2; i4 > -1; i4--, i2++) {
				source[i2] = 0b10000000 | ((uc1 >> (i4 * 6)) & 0b00111111);
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
