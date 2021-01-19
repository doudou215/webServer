#include <iostream>
#include <vector>

using namespace std;


class MinHeap {
 public:
	 MinHeap(int size = 100) : size_(size), end_(1) { 
		 array_.reserve(size_);
		 array_[0] = 10000000;
	 }
	
	 void push(int val) {
		 if (end_ == size_ - 1) {
			size_ = 2 * size_;
			array_.resize(size_);
		 }
	 	 array_[end_] = val;
		 int index = end_;
		 end_++;

		 while (index != 1) {
			int parent = index / 2;
			if (array_[parent] < array_[index])
				break;
			swap(array_[parent], array_[index]);
			index = parent;
		 }
		 return; 
	 }
	
	 int top() {
		 if (end_ == 1)
			 return -1000000;
		 return array_[1];
	 }

	 void pop() {
		 if (end_ == 1)
			 return;
		 swap(array_[1], array_[end_ - 1]);
		 end_ -= 1;
		 int index = 1;
		 
		 while (2 * index + 1 < end_) {
			int son = array_[index * 2] < array_[index * 2 + 1] ? index * 2 : index * 2 + 1;
			swap(array_[index], array_[son]);
			index = son;
		 }
	 }

 private:
	int size_;
	vector<int> array_;
	int end_;
};


int main() {
	MinHeap m;
	m.push(100);
	m.push(3);
	m.push(4);
	m.push(-1000);
	int i = 4;
	while (i) {
		cout<<m.top()<<endl;
		m.pop();
		i--;
	}
	return 0;
}
