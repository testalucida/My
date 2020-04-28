/* 
 * File:   typedefs.h
 * Author: testalucida
 *
 * Created on 21. November 2011, 11:43
 */

#ifndef MYSTD_H
#define	MYSTD_H

//#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <iostream>

using namespace std;

typedef std::shared_ptr<string> StringPtr;

#include "my/exceptions.h"
#include "my/datetime.h"


#endif	/* TYPEDEFS_H */

