# nodelinkdb
Node Link DB, nldb for short, is a C++ library that uses SQLite to manage a database file, giving you the power of graph database functionality in your application.

## SQLite integration
You'll need to get the SQLite amalgamation and place the sqlite3.c and sqlite3.h files in a directory sqlite at the same level as the nodelinkdb directory.

## nldblib
The nldblib project contains the code for the C++ library that you integrate into your project.
Include "nldb.h", in includes everything that you need to use nldblib.