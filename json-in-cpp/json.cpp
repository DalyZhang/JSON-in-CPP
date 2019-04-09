int JSON::currentPos;

const UnicodeString *JSON::currentDecoded = nullptr;

bool JSON::explore(Var &container) {
	switch (getNextTokenType()) {
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

	int fractionDigitNum = 0;
	u4c1 = currentDecoded->at(currentPos);
	if (u4c1 == U'.') {
		numberString.push('.');
		while (true) {
			currentPos++;
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
			return false;
		} else if (currentPos == currentDecoded->getLength()) {
			container.set(atof(numberString.cString()));
			return true;
		}
	}
	
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
			return false;
		}

	}
	
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
			string.push(currentDecoded->at(currentPos));

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

	bool isFirst = true;
	container.set(new HashTable);
	Var key, *value = nullptr;
	while (true) {

		if (isFirst) {
			isFirst = false;
			switch (getNextTokenType()) {
			case TT_OBJECT_END:
				return true;
			case TT_STRING:
				break;
			default:
				return false;
			}
		} else {

			switch (getNextTokenType()) {
			case TT_OBJECT_END:
				return true;
			case TT_COMMA:
				break;
			default:
				return false;
			}

			if (getNextTokenType() != TT_STRING) {
				return false;
			}

		}

		if (!fetchString(key)) {
			return false;
		}
		if (getNextTokenType() != TT_COLON) {
			return false;
		}
		value = new Var;
		if (!explore(*value)) {
			delete value;
			return false;
		}
		container.object().set(key.string(), value);

	}

}

bool JSON::exploreArray(Var &container) {
	
	bool isFirst = true;
	container.set(new VarArray);
	Var *item = nullptr;
	TokenType tokenType;
	while (true) {

		if (isFirst) {
			isFirst = false;
			switch (getNextTokenType()) {
			case TT_ARRAY_END:
				return true;
			case TT_NULL: case TT_NUMBER: case TT_BOOLEAN:
			case TT_STRING: case TT_ARRAY: case TT_OBJECT:
				break;
			default:
				return false;
			}
		} else {

			switch (getNextTokenType()) {
			case TT_ARRAY_END:
				return true;
			case TT_COMMA:
				break;
			default:
				return false;
			}

			switch (getNextTokenType()) {
			case TT_NULL: case TT_NUMBER: case TT_BOOLEAN:
			case TT_STRING: case TT_ARRAY: case TT_OBJECT:
				break;
			default:
				return false;
			}

		}

		currentPos--;
		item = new Var;
		if (!explore(*item)) {
			delete item;
			return false;
		}
		container.array().push(item);

	}

}

UnicodeString *JSON::currentMaked = nullptr;

void JSON::recursivelyMakeString(const Var &container) {
	char buffer[128];
	switch (container.getType()) {

	case Var::T_NULL:
		currentMaked->concat("null");
		return;

	case Var::T_NUMBER:
		sprintf(buffer, "%g", container.number());
		if (buffer[2] == '#' || buffer[3] == '#') {
			currentMaked->concat("null");
		} else {
			currentMaked->concat(buffer);
		}
		return;

	case Var::T_BOOLEAN:
		currentMaked->concat(container.boolean() ? "true" : "false");
		return;

	case Var::T_STRING: {
			UnicodeString &string = container.string();
			currentMaked->push(U'"');
			for (int i1 = 0; i1 < string.getLength(); i1++) {
				if ((string[i1] >= 0x0 && string[i1] < 0x20) ||
					string[i1] == U'\\' || string[i1] == U'"') {
					currentMaked->push(U'\\');
				}
				switch (string[i1]) {
				case U'\\': case U'"':
					currentMaked->push(string[i1]);
					break;
				case U'\b': currentMaked->push(U'b'); break;
				case U'\f': currentMaked->push(U'f'); break;
				case U'\n': currentMaked->push(U'n'); break;
				case U'\r': currentMaked->push(U'r'); break;
				case U'\t': currentMaked->push(U't'); break;
				default:
					if (string[i1] >= 0x0 && string[i1] < 0x20) {
						sprintf(buffer, "u%04x", string[i1]);
						currentMaked->concat(buffer);
					} else {
						currentMaked->push(string[i1]);
					}
				}
			}
			currentMaked->push(U'"');
		}
		return;

	case Var::T_OBJECT: {
			HashTable &object = container.object();
			bool isFirst = true;
			currentMaked->push(U'{');
			for (auto &key : object) {
				if (isFirst) {
					isFirst = false;
				} else {
					currentMaked->push(U',');
				}
				recursivelyMakeString(Var(key));
				currentMaked->push(U':');
				recursivelyMakeString(object[key]);
			}
			currentMaked->push(U'}');
		}
		return;

	case Var::T_ARRAY: {
			VarArray &array = container.array();
			bool isFirst = true;
			currentMaked->push(U'[');
			for (auto &item : array) {
				if (isFirst) {
					isFirst = false;
				} else {
					currentMaked->push(U',');
				}
				recursivelyMakeString(item);
			}
			currentMaked->push(U']');
		}
		return;

	}
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

UnicodeString *JSON::encode(const Var &encoded) {
	currentMaked = new UnicodeString;
	recursivelyMakeString(encoded);
	return currentMaked;
}
