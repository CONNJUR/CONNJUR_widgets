/*
 * Copyright (C) 2018 Michael R. Gryk
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

/* 
 * File:   main.c
 * Author: mgryk
 *
 * Created on May 31, 2018, 3:59 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <libxml/parser.h>
#include <libxml/xmlreader.h>
#include "connjur.h"

int is_Leaf(xmlNode *node)
{
	xmlNode *child = node->children;
	while(child)
	{
		if(child->type == XML_ELEMENT_NODE) return 0;
		child = child->next;
	}
	return 1;
}

void print_xml(xmlNode *node, int indent_len)
{
	while(node)
	{
		if ((node->type != XML_ELEMENT_NODE) && (node->type != XML_TEXT_NODE)) printf("%d:%s|\n", node->type, node->name);
		if (node->type == XML_TEXT_NODE) printf("%d, %s:%s|\n", node->type, node->name, xmlNodeGetContent(node));
		if(node->type == XML_ELEMENT_NODE)
		{
			printf("%*c%s:%s\n", indent_len*2, '-', node->name, is_Leaf(node)?(char *)xmlNodeGetContent(node):"NOT LEAF");
		}
		print_xml(node->children, indent_len+1);
		if (xmlStrcmp(node->name, "spectrum") == 0) {
			if (xmlGetProp(node, "current")) printf("Found current spectrum\n");
		}
		if (xmlStrcmp(node->name, "spectralDimensions") == 0) printf("Found spectralDimensions\n");
		node = node->next;
	}
}

xmlNode *getCurrentSpectrumNode(xmlNode *node)
{
	xmlNode *ret = NULL;

	while(node)
	{
		if (xmlStrcmp(node->name, "spectrum") == 0) {
			if (xmlGetProp(node, "current")) { 
//				printf("Found current spectrum\n");  // For debug
				return node;
			}
		}
	ret = getCurrentSpectrumNode(node->children);
//	printf("gCSN %s\n", node->name);				// For debug
	if (ret == NULL) node = node->next; else return ret;
	}		
	return ret;
}

void getDimensions(xmlNode *node, int* dims)
{
	while(node)
	{
		if (xmlStrcmp(node->name, "spectralDimensions") == 0) {
			sscanf(xmlNodeGetContent(node),"%d", dims);
		//	return;
		} else getDimensions(node->children, dims);		
		node = node->next;
	}
}	

void getSpectralData(xmlNode *node, struct _SpecParm *spectrum)
{
	xmlNode *axisNode;
	int dim = 0;
	
	node = node->children;
	while(node)
	{
//		printf("%s\n", node->name);
		if (xmlStrcmp(node->name, "spectralDimensions") == 0) {
//		printf("Found spectral dimensions node\n");
//		sscanf(xmlNodeGetContent(node),"%d", &spectrum->dimensions);
		}
		if (xmlStrcmp(node->name, "spectralAxis") == 0)
		{
//			printf("Found axis node\n");
			axisNode = node->children;
			while(axisNode)
			{
				if (xmlStrcmp(axisNode->name, "sign") == 0) sscanf(xmlNodeGetContent(axisNode),"%c", &spectrum->axis[dim].sign);
				if (xmlStrcmp(axisNode->name, "sweepwidth") == 0) sscanf(xmlNodeGetContent(axisNode),"%f", &spectrum->axis[dim].sweepWidth);
				if (xmlStrcmp(axisNode->name, "spectralFrequency") == 0) sscanf(xmlNodeGetContent(axisNode),"%f", &spectrum->axis[dim].specFreq);
				if (xmlStrcmp(axisNode->name, "constantPhase") == 0) sscanf(xmlNodeGetContent(axisNode),"%f", &spectrum->axis[dim].cnstPhase);
				if (xmlStrcmp(axisNode->name, "linearPhase") == 0) sscanf(xmlNodeGetContent(axisNode),"%f", &spectrum->axis[dim].linPhase);
				if (xmlStrcmp(axisNode->name, "firstScalePoint") == 0) sscanf(xmlNodeGetContent(axisNode),"%d", &spectrum->axis[dim].firstScalePoint);

//				if (xmlStrcmp(axisNode->name, "spectralFrequency") == 0) sscanf(xmlNodeGetContent(axisNode),"%f", &(*spectrum).axis[dim].specFreq);

				axisNode = axisNode->next;
			}
			dim++;
//			printf("%f\n", &spectrum->axis[dim].specFreq);
//			printf("%d\n", dim);
		}		
		node = node->next;
	}
	(*spectrum).dimensions = dim;
}

void printSpectralData(SpecParm spectrum)
{
	int i;

//	printf("This file has %d dimensions\n", spectrum.dimensions);
	for (i = 0; i<spectrum.dimensions; i++) {
		printf("\nAxis:\t\t\t%d\n", i);
		printf("Precession Sign:\t%c\n", spectrum.axis[i].sign);
		printf("Sweep Width:\t\t%.2f\n", spectrum.axis[i].sweepWidth);
		printf("Spectral Frequency:\t%.2f\n", spectrum.axis[i].specFreq);
		printf("Constant Phase:\t\t%.2f\n", spectrum.axis[i].cnstPhase);
		printf("Linear Phase:\t\t%.2f\n", spectrum.axis[i].linPhase);
		printf("First Scale Point:\t%d\n", spectrum.axis[i].firstScalePoint);
	} 
}

/*
 * 
 */
int main(int argc, char** argv) {

    if (argc != 2) {
		printf("Usage: %s filename\n", argv[0]);
		exit(1);
	}

	xmlDoc *document = NULL;
	xmlNode *root_element = NULL;
	xmlNode *curSpecNode = NULL;
//	xmlNode *dimNode = NULL;
	SpecParm mySpectrum;
	ptr_SpecParm = &mySpectrum;

	document = xmlReadFile(argv[1], NULL, 0);  //last argument can validate against DTD
	if (document == NULL) {
		printf("%s: Unable to parse XML\n", argv[0]);
		exit(1);
	}
	root_element = xmlDocGetRootElement(document);

//	mySpectrum.dimensions = 0;

	curSpecNode = getCurrentSpectrumNode(root_element);
//	print_xml(curSpecNode, 1); 			// for debug

//	getDimensions(curSpecNode, &mySpectrum.dimensions);
	getSpectralData(curSpecNode, ptr_SpecParm);
	printSpectralData(mySpectrum);
//	printf("This file has %d dimensions\n", mySpectrum.dimensions);	

	xmlFreeDoc(document);
	xmlCleanupParser();	
        
    return (EXIT_SUCCESS);
}

