#ifndef XMLDOC_H
#define XMLDOC_H

#include <string>

namespace my {

class XmlDoc {
public:
	XmlDoc();
	void addTag( const std::string &path, const std::string &tag );
	virtual ~XmlDoc();
private:
	std::string _xml;
};

} //my

#endif

