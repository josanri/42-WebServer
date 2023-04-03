#include <map>
#include "HttpResponse.hpp"

HttpResponse ()
{
	this->headers["date"] = asctime(getTimePointer());
}