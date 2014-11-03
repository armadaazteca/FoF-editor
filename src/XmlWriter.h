#ifndef _XML_WRITER_H_
#define _XML_WRITER_H_

#include <fstream>
#include <vector>
#include <memory>
#include "Interfaces.h"
#include "DatSprStructs.h"

using namespace std;

class XmlWriter
{
public:
	typedef vector <shared_ptr <DatObject>> DatObjectList;

	static XmlWriter & getInstance();
	bool writeItemsXML(shared_ptr <DatObjectList> items, const wxString & filename, ProgressUpdatable * progressUpdatable);
	bool writeCreaturesXML(shared_ptr <DatObjectList> creatures, const wxString & filename, ProgressUpdatable * progressUpdatable);
	bool writeMaterialsXML(const wxString & filename);

private:
	fstream file;
};

#endif // _XML_WRITER_H_
