#include <my/QuickSearch.h>

namespace my {

QuickSearch::QuickSearch(const char *pSrc) :
		_pSrc(pSrc) {
}

void QuickSearch::addSearchItem(const char *p) {
	Item *pItem = new Item(p);
	_pItems.push_back(pItem);
}

void QuickSearch::search() {
	for (const char *p = _pSrc; p < _pSrc + strlen(_pSrc); p++) {
		//for(auto pItem : _pItems) {
		for (auto pItem : _pItems) {
			if (pItem->_found)
				continue;
			if (*p == *pItem->_p) {
				pItem->_p++;
				if (pItem->_p == pItem->_pEnd)
					pItem->_found = true;
			} else {
				pItem->_p = pItem->_pItem; //start search from the beginning
				if (*p == *pItem->_p) {
					pItem->_p++;
				}
			}
		} //for(auto pItem : _pItems)
	} //for (const char* p = _pSrc; p < _pSrc + strlen...
}

int QuickSearch::getSearchItemCount() {
	return _pItems.size();
}

bool QuickSearch::found(int searchItemIdx) {
	Item *pItem = _pItems.at(searchItemIdx);
	return pItem->_found;
}

QuickSearch::Result QuickSearch::getResult(int searchItemIdx) {
	Item *pItem = _pItems.at(searchItemIdx);
	Result res;
	res.pSearchItem = pItem->_pItem;
	res.found = pItem->_found;
	return res;
}

} /* namespace my */
