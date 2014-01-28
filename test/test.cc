/*
    Copyright (C) 2014 Carl Hetherington <cth@carlh.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libsub_test
#include <boost/test/unit_test.hpp>
#include <fstream>
#include <string>

using std::string;
using std::ifstream;
using std::getline;

void
check_text (string a, string b)
{
	ifstream p (a.c_str ());
	ifstream q (b.c_str ());

	string x;
	string y;
	while (p.good() && q.good()) {
		getline (p, x);
		getline (q, y);
		BOOST_CHECK_EQUAL (x, y);
	}

	BOOST_CHECK (p.good() == false);
	BOOST_CHECK (q.good() == false);
}
