VarArray::Iterator::Iterator(Var **begin) {
	current = begin;
}

bool VarArray::Iterator::operator!=(Iterator &end) {
	return current != end.current;
}

VarArray::Iterator &VarArray::Iterator::operator++() {
	current++;
	return *this;
}

Var &VarArray::Iterator::operator*() {
	return **current;
}

void VarArray::correctOrd(int &ord, int length) {
	if (ord > length) {
		ord = length;
		return;
	} else if (ord < -length) {
		ord = 0;
		return;
	}
	if (ord < 0) {
		ord += length;
	}
}

int VarArray::rest = 0;

VarArray::VarArray() {
	rest++;
	source = new Var *[1]{};
}

VarArray::VarArray(const VarArray &copied, int start)
	: VarArray(copied, start, copied.length) {}

VarArray::VarArray(const VarArray &copied, int start, int end) {
	rest++;
	correctOrd(start, copied.length);
	correctOrd(end, copied.length);
	if (start > end) {
		start = end;
	}
	length = end - start;
	source = new Var *[length + 1]{};
	for (int i1 = start; i1 < end; i1++) {
		source[i1] = new Var(*copied.source[i1]);
	}
}

VarArray::~VarArray() {
	rest--;
	clear();
	delete[] source;
}

Var &VarArray::at(int offset) const {
	return *source[offset];
}

Var &VarArray::operator[](int offset) const {
	return *source[offset];
}

VarArray *VarArray::push(Var *item) {
	Var **oldSource = source;
	source = new Var *[length + 2]{};
	memcpy(source, oldSource, sizeof (source[0]) * length);
	source[length] = item;
	delete[] oldSource;
	length++;
	return this;
}

VarArray *VarArray::pop(bool deleteOccupied) {
	if (length == 0) {
		return this;
	}
	Var **oldSource = source;
	source = new Var *[length]{};
	memcpy(source, oldSource, sizeof (source[0]) * (length - 1));
	if (deleteOccupied) {
		delete oldSource[length - 1];
	}
	delete[] oldSource;
	length--;
	return this;
}

VarArray *VarArray::clear(bool deleteOccupied) {
	if (deleteOccupied) {
		for (int i1 = 0; i1 < length; i1++) {
			delete source[i1];
		}
	}
	delete[] source;
	length = 0;
	source = new Var *[1]{};
	return this;
}

int VarArray::getLength() const {
	return length;
}

VarArray::Iterator VarArray::begin() const {
	return Iterator(source);
}

VarArray::Iterator VarArray::end() const {
	return Iterator(source + length);
}

void VarArray::write(Coding coding, FILE *fp) const {
	bool isFirst = true;
	fputc('[', fp);
	for (auto &item : *this) {
		if (isFirst) {
			isFirst = false;
		} else {
			fputc(',', fp);
		}
		item.write(coding, fp);
	}
	fputc(']', fp);
}