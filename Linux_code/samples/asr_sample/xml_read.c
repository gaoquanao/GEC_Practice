#include "xml_read.h"

int object = -1;
int status = -1;

static void print_element_names(xmlNode * a_node)
{
    xmlNode *cur_node = NULL;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) 
    {
        if (cur_node->type == XML_ELEMENT_NODE) 
        {
            if(!strcmp(cur_node->name, "open")){
                printf("[xml_read] on!\n");
                status = ON;
            }
            
            if(!strcmp(cur_node->name, "close")){
                printf("[xml_read] off!\n");
                status = OFF;
            }
            
            if(!strcmp(cur_node->name, "light")){
                printf("[xml_read] led!\n");
                object = BOARD_LED;
            }
            
            if(!strcmp(cur_node->name, "window")){
                printf("[xml_read] window!\n");
                object = BOARD_WIN;
            }

            // printf("node type: Element, name: %s\n", cur_node->name);
        }

        print_element_names(cur_node->children);
    }
}


int xml_read(char * filename,  protocol * pro)
{
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    doc = xmlReadFile(filename, NULL, 0);

    if (doc == NULL) {
        printf("error: could not parse file %s\n", filename);
    }

    root_element = xmlDocGetRootElement(doc);

    print_element_names(root_element);
    xmlFreeDoc(doc);

    xmlCleanupParser();
    printf("object:%d, status:%d\n", object, status);
    putData(pro, BOARD, object, 0, 0, status);
    return 0;
}
