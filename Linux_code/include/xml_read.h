#ifndef _XML_READ_H__
#define _XML_READ_H__

#include <stdio.h>  
#include <unistd.h>  
#include <sys/types.h>  
#include <string.h>

#include <libxml/tree.h>
#include <libxml/parser.h>  
#include <libxml/xmlmemory.h>

#include "handle_protcol.h"

int xml_read(char * filename, protocol * pro);
static void print_element_names(xmlNode * a_node);

#endif  