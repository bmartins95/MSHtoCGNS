template<typename T, typename U>
T* determine_array_1d(const std::vector<U>& a) {
	T* b = new T[a.size()];
	for (typename std::vector<U>::const_iterator i = a.cbegin(); i < a.cend(); i++) b[i - a.cbegin()] = static_cast<T>(*i);
	return b;
}

template<typename T, typename U>
T* determine_array_1d(typename std::vector<U>::const_iterator begin, typename std::vector<U>::const_iterator end) {
	T* b = new T[end - begin];
	for (typename std::vector<U>::const_iterator i = begin; i < begin; i++) b[i - begin] = static_cast<T>(*i);
	return b;
}

template<typename T, typename U>
T* determine_array_1d(const std::vector<std::vector<U>>& a) {
	T* b = new T[a.size()*a[0].size()];
	for (typename std::vector<std::vector<U>>::const_iterator i = a.cbegin(); i < a.cend(); i++) {
		for (typename std::vector<U>::const_iterator j = i->cbegin(); j < i->cend(); j++) {
			b[(i-a.cbegin())*i->size() + (j-i->cbegin())] = static_cast<T>(*j);
		}
	}
	return b;
}

// template<typename T, typename U>
// T* determine_array_1d(typename std::vector<std::vector<U>>::const_iterator begin, typename std::vector<std::vector<U>>::const_iterator end) {
// 	T* b = new T[(end - begin) * (begin->cend() - begin->cbegin())];
// 	for (typename std::vector<std::vector<U>>::const_iterator i = begin; i < end; i++) {
// 		for (typename std::vector<U>::const_iterator j = i->cbegin(); j < i->cend(); j++) {
// 			b[(i-begin)*i->size() + (j-i->cbegin())] = static_cast<T>(*j);
// 		}
// 	}
// 	return b;
// }

template<typename T>
void print(const std::vector<T>& a, std::string&& message) {
	std::cout << std::fixed << std::setprecision(3);
	std::cout << "\t" << message << std::endl;
	for (auto i = a.cbegin(); i != a.cend(); i++) std::cout << "\t" << *i;
	std::cout << std::endl;
}

template<typename T>
void print(const std::vector<std::vector<T>>& a, std::string&& message) {
	std::cout << std::fixed << std::setprecision(3);
	std::cout << "\t" << message << std::endl;
	for (auto i = a.cbegin(); i != a.cend(); i++) {
		for (auto j = i->cbegin(); j != i->cend(); j++) {
			std::cout << "\t" << *j;
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

template<typename T>
void output(const std::vector<T>& a, std::ofstream& file) {
	file << std::fixed << std::setprecision(3);
	for (auto i = a.cbegin(); i != a.cend(); i ++) file << "\t" << std::setw(6) << std::right << *i;		
	file << std::endl;
}

template<typename T>
void output(const std::vector<std::vector<T>>& a, std::ofstream& file) {
	file << std::fixed << std::setprecision(3) << std::right;
	for (auto i = a.cbegin(); i != a.cend(); i ++) {
		for (auto j = i->cbegin(); j != i->cend(); j ++) {
			file << "\t" << std::setw(6) << std::right << *j;
		}
		file << std::endl;
	}
}