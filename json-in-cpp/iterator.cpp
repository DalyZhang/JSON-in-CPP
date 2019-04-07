template<class Pointer, class Result>
Iterator<Pointer, Result>::Iterator(Pointer begin) {
	current = begin;
}

template<class Pointer, class Result>
bool Iterator<Pointer, Result>::operator!=(Iterator &right) {
	return current != right.current;
}

template<class Pointer, class Result>
Iterator<Pointer, Result> &Iterator<Pointer, Result>::operator++() {
	current++;
	return *this;
}

template<class Pointer, class Result>
Result &Iterator<Pointer, Result>::operator*() {
	return *current;
}