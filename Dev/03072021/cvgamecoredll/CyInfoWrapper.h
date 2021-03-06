#pragma once

#ifndef CY_INFO_WRAPPER_H
#define CY_INFO_WRAPPER_H

/*  advc.003x: Macros for defining Python wrappers (adapters) for member functions
	of CvInfo classes. Specifically for functions that take an enum type parameter
	and therefore can't (or shouldn't) be directly exposed to Python.
	The PY_WRAP macro can be placed right after the declaration of a public CvInfo
	member function. It defines an in-line wrapper with private accessibility.
	A pointer to the wrapper needs to be inserted into the proper .def call in
	CyInfoInterface[1-4].cpp. That's just a matter of prepending 'py_' to the existing
	call argument. Additionally, the CvInfo class needs to declare the (global)
	CyInfoPythonInterface[1-4] function that contains the def call as a friend, e.g.:
		friend void CyInfoPythonInterface1();
	For convenience, there are macros that take fewer parameters:
	iPY_WRAP: Takes the name of an XML tag and prepends "get" to generate the
			function name. Return type is 'int'.
	bPY_WRAP: Prepends "is", return type is 'bool'. */

#define PY_WRAP(ReturnType, fnName, enumName) \
	private: \
		ReturnType py_##fnName(int i) const \
		{ \
			return fnName((enumName##Types)i); \
		} \
	public:

#define iPY_WRAP(tagName, enumName) PY_WRAP(int, get##tagName, enumName)
#define bPY_WRAP(tagName, enumName) PY_WRAP(bool, is##tagName, enumName)

#endif
