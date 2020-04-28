#ifndef QUICKSEARCH_H_
#define QUICKSEARCH_H_

#include <cstring>
#include <vector>

namespace my {

class QuickSearch {
private:
	struct Item {
			Item(const char* pItem) {
				_pItem = _p = pItem;
				_itemLen = strlen(_pItem);
				_pEnd = _p + _itemLen;
			}
			bool _found = false;
			const char* _pItem;
			const char* _p;
			const char* _pEnd;
			int _itemLen;
		};
public:
	struct Result {
			const char* pSearchItem;
			bool found;
	};
public:
	QuickSearch(const char* pSrc);
	virtual ~QuickSearch(){};

	void addSearchItem(const char* p);
	void search();
	int getSearchItemCount();
	bool found(int searchItemIdx);
	Result getResult(int searchItemIdx);
private:
	const char* _pSrc; //string to be examined
	std::vector<Item*> _pItems;
};

}//namespace my

#endif /* QUICKSEARCH_H_ */
