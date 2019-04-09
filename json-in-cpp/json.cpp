int JSON::currentPos;

const UnicodeString *JSON::currentDecoded = nullptr;

bool JSON::explore(Var &container) {
	TokenType tt = getNextTokenType();
	printf("token: %i\n", tt);
	switch (tt) {
	case TT_NULL:
		return fetchNull(container);
	case TT_NUMBER:
		return fetchNumber(container);
	case TT_BOOLEAN:
		return fetchBoolean(container);
	case TT_STRING:
		return fetchString(container);
	case TT_OBJECT:
		return exploreObject(container);
	case TT_ARRAY:
		return exploreArray(container);
	default:
		return false;
	}
}

JSON::TokenType JSON::getNextTokenType() {

	char32_t u4c1;

	while (true) {
		if (currentPos >= currentDecoded->getLength()) {
			return TT_END;
		}
		u4c1 = currentDecoded->at(currentPos);
		currentPos++;
		switch (u4c1) {
		case U'n': return TT_NULL;
		case U't': case U'f': return TT_BOOLEAN;
		case U'"': return TT_STRING;
		case U'{': return TT_OBJECT;
		case U'}': return TT_OBJECT_END;
		case U'[': return TT_ARRAY;
		case U']': return TT_ARRAY_END;
		case U':': return TT_COLON;
		case U',': return TT_COMMA;
		case U' ': case U'\t': case U'\r': case U'\n': break;
		default:
			if (u4c1 == U'-' || (u4c1 >= U'0' && u4c1 <= U'9')) {
				return TT_NUMBER;
			}
			return TT_ERROR;
		}
	}

}

bool JSON::fetchNull(Var &container) {
	currentPos += 3;
	if (currentPos > currentDecoded->getLength()) {
		return false;
	}
	return currentDecoded->at(currentPos - 3) == U'u' && currentDecoded->at(currentPos - 2) == U'l'
		&& currentDecoded->at(currentPos - 1) == U'l';
}

bool JSON::fetchNumber(Var &container) {

	char32_t u4c1;

	String numberString;

	currentPos--;

	u4c1 = currentDecoded->at(currentPos);
	if (u4c1 == U'-') {
		numberString.push('-');
		currentPos++;
		if (currentPos == currentDecoded->getLength()) {
			return false;
		}
	}
	puts("module 1 finished");
	numberString.write();
	puts("");
	
	u4c1 = currentDecoded->at(currentPos);
	if (u4c1 == U'0') {
		numberString.push('0');
		currentPos++;
	} else if (u4c1 > U'0' && u4c1 <= U'9') {
		numberString.push((char)u4c1);
		while (true) {
			currentPos++;
			if (currentPos == currentDecoded->getLength()) {
				container.set(atof(numberString.cString()));
				return true;
			}
			u4c1 = currentDecoded->at(currentPos);
			if (u4c1 >= U'0' && u4c1 <= U'9') {
				numberString.push((char)u4c1);
			} else {
				break;
			}
		}
	}
	puts("module 2 finished");
	numberString.write();
	puts("");

	int fractionDigitNum = 0;
	u4c1 = currentDecoded->at(currentPos);
	if (u4c1 == U'.') {
		numberString.push('.');
		while (true) {
			currentPos++;
			printf("currentPos: %i\n", currentPos);
			if (currentPos == currentDecoded->getLength()) {
				break;
			}
			u4c1 = currentDecoded->at(currentPos);
			if (u4c1 >= U'0' && u4c1 <= U'9') {
				numberString.push((char)u4c1);
				fractionDigitNum++;
			} else {
				break;
			}
		}
		if (fractionDigitNum == 0) {
			puts("die2");
			return false;
		} else if (currentPos == currentDecoded->getLength()) {
			container.set(atof(numberString.cString()));
			return true;
		}
	}
	puts("module 3 finished");
	numberString.write();
	puts("");
	
	u4c1 = currentDecoded->at(currentPos);
	if (u4c1 == U'e' || u4c1 == U'E') {

		numberString.push('e');

		currentPos++;
		if (currentPos == currentDecoded->getLength()) {
			return false;
		}
		u4c1 = currentDecoded->at(currentPos);
		if (u4c1 == U'+' || u4c1 == U'-') {
			numberString.push(u4c1);
			currentPos++;
			if (currentPos == currentDecoded->getLength()) {
				return false;
			}
		}
		puts("module 4.1 finished");
		numberString.write();
		puts("");

		fractionDigitNum = 0;
		while (true) {
			u4c1 = currentDecoded->at(currentPos);
			if (u4c1 >= U'0' && u4c1 <= U'9') {
				numberString.push((char)u4c1);
				fractionDigitNum++;
			} else {
				break;
			}
			currentPos++;
			if (currentPos == currentDecoded->getLength()) {
				break;
			}
		}
		if (fractionDigitNum == 0) {
			numberString.write();
			return false;
		}
		puts("module 4.2 finished");
		numberString.write();
		puts("");

	}
	puts("module 4 finished");
	numberString.write();
	puts("");
	
	container.set(atof(numberString.cString()));
	return true;

}

bool JSON::fetchBoolean(Var &container) {
	if (currentDecoded->at(currentPos - 1) == U'f') {
		currentPos += 4;
		if (currentPos > currentDecoded->getLength()) {
			return false;
		}
		container.set(false);
		return currentDecoded->at(currentPos - 4) == U'a' && currentDecoded->at(currentPos - 3) == U'l'
			&& currentDecoded->at(currentPos - 2) == U's' && currentDecoded->at(currentPos - 1) == U'e';
	} else {
		currentPos += 3;
		if (currentPos > currentDecoded->getLength()) {
			return false;
		}
		container.set(true);
		return currentDecoded->at(currentPos - 3) == U'r' && currentDecoded->at(currentPos - 2) == U'u'
			&& currentDecoded->at(currentPos - 1) == U'e';
	}
}

bool JSON::fetchString(Var &container) {

	int i1;
	char32_t u4c1, u4c2;
	char16_t u2c1;

	UnicodeString string;
	bool isFinished = false;

	while (true) {

		if (currentPos >= currentDecoded->getLength()) {
			return false;
		}
		
		switch (currentDecoded->at(currentPos)) {

		case U'\\':
		
			currentPos++;
			if (currentPos == currentDecoded->getLength()) {
				return false;
			}

			switch (currentDecoded->at(currentPos)) {
			case U'"': case U'\\': case U'/':
				string.push(currentDecoded->at(currentPos));
				break;
			case U'b': string.push(U'\b'); break;
			case U'f': string.push(U'\f'); break;
			case U'n': string.push(U'\n'); break;
			case U'r': string.push(U'\r'); break;
			case U't': string.push(U'\t'); break;
			case U'u':
				currentPos += 4;
				if (currentPos >= currentDecoded->getLength()) {
					return false;
				}
				for (i1 = 3, u2c1 = 0; i1 > -1; i1--) {
					u4c1 = currentDecoded->at(currentPos - i1);
					if (u4c1 >= U'0' && u4c1 <= U'9') {
						u4c2 = u4c1 - U'0';
					} else if (u4c1 >= U'A' && u4c1 <= U'F') {
						u4c2 = u4c1 - (U'A' - 10);
					} else if (u4c1 >= U'a' && u4c1 <= U'f') {
						u4c2 = u4c1 - (U'a' - 10);
					} else {
						return false;
					}
					u2c1 = (u2c1 << 4) + (char16_t)u4c2;
				}
				if ((u2c1 ^ 0xDC00) >> 10) {
					string.push(u2c1);
				} else {
					if ((string[-1] ^ 0xD800) >> 10) {
						string.push(u2c1);
					} else {
						string[-1] = (((string[-1] ^ 0xD800) << 10) | (u2c1 ^ 0xDC00)) + 0x10000;
					}
				}
				break;

			default:
				return false;
			}
			break;

		case U'"':
			isFinished = true;
			break;

		default:
			if (currentDecoded->at(currentPos) >= 0 && currentDecoded->at(currentPos) < 0x20) {
				return false;
			}
			printf("string before: ");
			string.write();
			puts("");
			string.push(currentDecoded->at(currentPos));
			printf("push: %c\n", currentDecoded->at(currentPos));
			printf("string after: ");
			string.write();
			puts("");

		}

		currentPos++;

		if (isFinished) {
			break;
		}

	}

	container.set(string);
	return true;

}

bool JSON::exploreObject(Var &container) {

}

bool JSON::exploreArray(Var &container) {
	
}

Var *JSON::decode(const UnicodeString &decoded) {
	currentPos = 0;
	currentDecoded = &decoded;
	Var *var = new Var;
	if (!explore(*var) || getNextTokenType() != TT_END) {
		delete var;
		var = nullptr;
	}
	return var;
}
