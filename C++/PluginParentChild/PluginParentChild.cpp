/* Copyright (C) 2017 Rainmeter Project Developers
 *
 * This Source Code Form is subject to the terms of the GNU General Public
 * License; either version 2 of the License, or (at your option) any later
 * version. If a copy of the GPL was not distributed with this file, You can
 * obtain one at <https://www.gnu.org/licenses/gpl-2.0.html>. */

#include <Windows.h>
#include "../../API/RainmeterAPI.h"
#include <algorithm>
#include <vector>

// Overview: This example demonstrates a basic implementation of a parent/child
// measure structure. In this particular example, we have a "parent" measure
// which contains the values for the options "ValueA", "ValueB", and "ValueC".
// The child measures are used to return a specific value from the parent.

// Use case: You could, for example, have a "main" parent measure that queries
// information from some data set. The child measures can then be used to return
// specific information from the data queried by the parent measure.

// Sample skin:
/*
	[Rainmeter]
	Update=1000
	DynamicWindowSize=1
	BackgroundMode=2
	SolidColor=255,255,255

	[mParent]
	Measure=Plugin
	Plugin=ParentChild
	ValueA=111
	ValueB=222
	ValueC=333
	Type=A

	[mChild1]
	Measure=Plugin
	Plugin=ParentChild
	ParentName=mParent
	Type=B

	[mChild2]
	Measure=Plugin
	Plugin=ParentChild
	ParentName=mParent
	Type=C

	[Text]
	Meter=String
	MeasureName=mParent
	MeasureName2=mChild1
	MeasureName3=mChild2
	Text="mParent: %1#CRLF#mChild1: %2#CRLF#mChild2: %3"
*/

enum MeasureType
{
	MEASURE_A,
	MEASURE_B,
	MEASURE_C
};

struct ChildMeasure;

struct ParentMeasure
{
	void* skin;
	LPCWSTR name;
	ChildMeasure* ownerChild;

	int valueA;
	int valueB;
	int valueC;

	ParentMeasure() : 
		skin(nullptr),
		name(nullptr),
		ownerChild(nullptr),
		valueA(0),
		valueB(0),
		valueC(0) {}
};

struct ChildMeasure
{
	MeasureType type;
	ParentMeasure* parent;

	ChildMeasure() : 
		type(MEASURE_A),
		parent(nullptr) {}
};

std::vector<ParentMeasure*> g_ParentMeasures;

PLUGIN_EXPORT void Initialize(void** data, void* rm)
{
	ChildMeasure* child = new ChildMeasure;
	*data = child;

	void* skin = RmGetSkin(rm);

	LPCWSTR parentName = RmReadString(rm, L"ParentName", L"");
	if (!*parentName)
	{
		child->parent = new ParentMeasure;
		child->parent->name = RmGetMeasureName(rm);
		child->parent->skin = skin;
		child->parent->ownerChild = child;
		g_ParentMeasures.push_back(child->parent);
	}
	else
	{
		// Find parent using name AND the skin handle to be sure that it's the right one
		std::vector<ParentMeasure*>::const_iterator iter = g_ParentMeasures.begin();
		for ( ; iter != g_ParentMeasures.end(); ++iter)
		{
			if (_wcsicmp((*iter)->name, parentName) == 0 &&
				(*iter)->skin == skin)
			{
				child->parent = (*iter);
				return;
			}
		}

		RmLog(rm, LOG_ERROR, L"Invalid \"ParentName\"");
	}
}

PLUGIN_EXPORT void Reload(void* data, void* rm, double* maxValue)
{
	ChildMeasure* child = (ChildMeasure*)data;
	ParentMeasure* parent = child->parent;

	if (!parent)
	{
		return;
	}

	// Read common options
	LPCWSTR type = RmReadString(rm, L"Type", L"");
	if (_wcsicmp(type, L"A") == 0)
	{
		child->type = MEASURE_A;
	}
	else if (_wcsicmp(type, L"B") == 0)
	{
		child->type = MEASURE_B;
	}
	else if (_wcsicmp(type, L"C") == 0)
	{
		child->type = MEASURE_C;
	}
	else
	{
		RmLog(rm, LOG_ERROR, L"Invalid \"Type\"");
	}

	// Read parent specific options
	if (parent->ownerChild == child)
	{
		parent->valueA = RmReadInt(rm, L"ValueA", 0);
		parent->valueB = RmReadInt(rm, L"ValueB", 0);
		parent->valueC = RmReadInt(rm, L"ValueC", 0);
	}
}

PLUGIN_EXPORT double Update(void* data)
{
	ChildMeasure* child = (ChildMeasure*)data;
	ParentMeasure* parent = child->parent;

	if (!parent)
	{
		return 0.0;
	}

	switch (child->type)
	{
	case MEASURE_A:
		return (double)parent->valueA;

	case MEASURE_B:
		return (double)parent->valueB;

	case MEASURE_C:
		return (double)parent->valueC;
	}

	return 0.0;
}

PLUGIN_EXPORT void Finalize(void* data)
{
	ChildMeasure* child = (ChildMeasure*)data;
	ParentMeasure* parent = child->parent;

	if (parent && parent->ownerChild == child)
	{
		g_ParentMeasures.erase(
			std::remove(g_ParentMeasures.begin(),g_ParentMeasures.end(),parent),
			g_ParentMeasures.end());
		delete parent;
	}

	delete child;
}
