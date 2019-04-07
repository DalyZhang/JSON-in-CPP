HashTable::Iterator::Iterator(Node *begin) {
	current = begin;
}

bool HashTable::Iterator::operator!=(Iterator &end) {
	return current != end.current;
}

HashTable::Iterator &HashTable::Iterator::operator++() {
	current = current->ordNext;
	return *this;
}

UnicodeString &HashTable::Iterator::operator*() {
	return *current->key;
}

int HashTable::hash(const UnicodeString &key) const {
	unsigned long long index = 0;
	for (int i1 = 0; i1 < key.getLength(); i1++) {
		index = (((index + (unsigned)key.at(i1)) << 5) - 1) % (unsigned)linkListLength;
	}
	return (int)index;
}

int HashTable::rest = 0;

HashTable::HashTable(const HashTable &copied) : HashTable(copied.linkListLength) {
	Var *var = nullptr;
	for (auto &key : copied) {
		this->set(key, new Var(*copied.get(key)));
	}
}

HashTable::HashTable(int linkListLength) {
	rest++;
	this->linkListLength = linkListLength;
	linkList = new Node *[linkListLength]{};
	ordSentinel = new Node{};
	ordSentinel->ordPrev = ordSentinel->ordNext = ordSentinel;
}

HashTable::~HashTable() {
	// puts("fuckHashTable");
	rest--;
	clear();
	delete ordSentinel;
}

Var *HashTable::get(const char *cStringKey, Coding coding) const {
	return get(UnicodeString(String(cStringKey), coding));
}

Var *HashTable::get(const UnicodeString &key) const {

	Node *np1 = nullptr;

	np1 = linkList[hash(key)];
	while (np1 != nullptr) {
		if (*np1->key == key) {
			break; 
		} else {
			np1 = np1->linkNext;
		}
	}

	return np1->value;

}

HashTable *HashTable::set(const char *cStringKey, Var *value, bool deleteOccupied, Coding coding) {
	return set(UnicodeString(String(cStringKey), coding), value, deleteOccupied);
}

HashTable *HashTable::set(const UnicodeString &key, Var *value, bool deleteOccupied) {

	Node *np1 = nullptr, *np2 = nullptr;

	int index = hash(key);
	np1 = linkList[index];
	np2 = nullptr;

	if (np1 == nullptr) {
		np1 = linkList[index] = new Node;
	} else {
		while (np1 != nullptr) {
			if (*np1->key == key) {
				if (deleteOccupied) {
					delete np1->value;
				}
				np1->value = value;
				return this;
			} else {
				np2 = np1;
				np1 = np1->linkNext;
			}
		}
		np1 = np2->linkNext = new Node;
	}

	np1->key = new UnicodeString(key);
	np1->value = value;
	np1->linkPrev = np2;
	np1->linkNext = nullptr;
	np1->ordPrev = ordSentinel->ordPrev;
	np1->ordNext = ordSentinel;
	ordSentinel->ordPrev->ordNext = np1;
	ordSentinel->ordPrev = np1;
	length++;

	return this;

}

bool HashTable::has(const char *cStringKey, Coding coding) {
	return has(UnicodeString(String(cStringKey), coding));
}

bool HashTable::has(const UnicodeString &key) {

	Node *np1 = nullptr;

	np1 = linkList[hash(key)];
	while (np1 != nullptr) {
		if (*np1->key == key) {
			return true;
		}
		np1 = np1->linkNext;
	}

	return false;

}

HashTable *HashTable::remove(const char *cStringKey, bool deleteOccupied, Coding coding) {
	return remove(UnicodeString(String(cStringKey), coding), deleteOccupied);
}

HashTable *HashTable::remove(const UnicodeString &key, bool deleteOccupied) {

	Node *np1 = nullptr;

	int index = hash(key);
	np1 = linkList[index];
	while (np1 != nullptr) {
		if (*np1->key == key) {
			if (np1->linkNext == nullptr && np1->linkPrev == nullptr) {
				linkList[index] = nullptr;
			} else {
				if (np1->linkNext != nullptr) {
					np1->linkNext->linkPrev = np1->linkPrev;
				}
				if (np1->linkPrev != nullptr) {
					np1->linkPrev->linkNext = np1->linkNext;
				}
			}
			np1->ordNext->ordPrev = np1->ordPrev;
			np1->ordPrev->ordNext = np1->ordNext;
			delete np1->key;
			if (deleteOccupied) {
				delete np1->value;
			}
			delete np1;
			length--;
			return this;
		}
		np1 = np1->linkNext;
	}

	return this;

}

HashTable *HashTable::clear(bool deleteOccupied) {
	
	Node *np1 = ordSentinel->ordNext, *np2 = nullptr;

	while (np1 != ordSentinel) {
		np2 = np1;
		np1 = np1->ordNext;
		linkList[hash(*np2->key)] = nullptr;
		delete np2->key;
		if (deleteOccupied) {
			delete np2->value;
		}
		delete np2;
	}

	ordSentinel->ordPrev = ordSentinel->ordNext = ordSentinel;
	length = 0;

	return this;

}

void HashTable::write(Coding coding, FILE *fp) const {
	fprintf(fp, "{");
	bool isFirst = true;
	for (auto &key : *this) {
		if (isFirst) {
			isFirst = false;
		} else {
			fputc(',', fp);
		}
		fputc('"', fp);
		key.write(coding, fp);
		fputc('"', fp);
		fputc(':', fp);
		get(key)->write(coding, fp);
	}
	fprintf(fp, "}");
}

int HashTable::getLinkListLength() const {
	return linkListLength;
}

int HashTable::getLength() const {
	return length;
}

HashTable::Iterator HashTable::begin() const {
	return Iterator(ordSentinel->ordNext);
}

HashTable::Iterator HashTable::end() const {
	return Iterator(ordSentinel);
}

Var &HashTable::operator[](const char *cStringKey) {
	return (*this)[UnicodeString(cStringKey, Coding::RAW)];
}

Var &HashTable::operator[](const UnicodeString &key) {
	if (!has(key)) {
		set(key, nullptr);
	}
	return *get(key);
}

void HashTable::writeLinkList(Coding coding, FILE *fp) {

	int i1;
	Node *np1 = nullptr;

	static char buffer[32], format[32];
	sprintf(buffer, "%i", linkListLength - 1);
	int ordMaxLength = strlen(buffer);
	sprintf(format, "%%%ii", ordMaxLength);

	for (i1 = 0; i1 < linkListLength; i1++) {
		fprintf(fp, format, i1);
		if (linkList[i1] != nullptr) {
			np1 = linkList[i1];
			while (np1 != nullptr) {
				fprintf(fp, " -> ");
				np1->key->write(coding, fp);
				np1 = np1->linkNext;
			}
		}
		fprintf(fp, "\n");
	}

}
