# nodelinkdb
Node Link DB, nldb for short, gives you the power of a graph database in your C++ application.\
As far as nldb goes, being a graph database is nothing fancier than nodes, links between nodes, and properties on nodes and links.\
More background on graph databases can be found on [Wikipedia](https://en.wikipedia.org/wiki/Graph_database).

## Projects
The repo root directory is a solution that contains the following projects:

### nldblib
The nldblib project is the library you integrate into your project.\
Include "nldb.h" and it includes everything that you need to use nldblib.\
Add #pragma comment(lib, "nldblib.lib") with your library directory set up, and you're all set.

### nldb
The nldb project is a command-line proof-of-concept.  It is not the central project; nldblib is.\
This project is just for putting the library through its paces with a ready supply of graph data: the file system.

### nldbtests
The nldbtests project contains the unit tests for the nldblib project.

## SQLite integration
nldb uses the file-based database SQLite to do the heavy lifting.\
SQLite is a powerful and easy to use file-based database.
There is no server; instead, you direct the SQLite API to use a file on disk, 
and do database operations using SQL.\
\
To get nldb working with SQLite, get the 
[SQLite amalgamation](https://www.sqlite.org/download.html), get the "C source code as an amalgamation, version ..." ZIP.
Extract it, go into the folder, then copy the sqlite3.c and sqlite3.h files into a directory named sqlite at the same level as the nodelinkdb directory.

## Building
With the solution cloned and the sqlite directory set up alongside the solution directory, 
the solution should then build and you'll have a static library to link against and a test program to experiment with.

## Code Walkthrough
### Setting Up
The nldb namespace is used throughout.  Classes in this workspace are what you call into from your code.\
This is a database-driven system, so you'll use the db class, with the constructor that takes the path to the database filename.\
Create a db object with the file path where you want your database.  A .db extension is common, but none of this code cares.\
If you're creating a new database call the global function nldb::setup_nldb(db) to initialize the database with the tables.\
You only call setup_nldb() when you want to set up a new database or reset an existing database to have empty tables.  
It wipes the datbase clean.  Be careful.

### Nodes and Links
nldb gives you two primary data structures to work with: nodes and links.

#### Nodes
In node.h you'll find the struct node:
```c++
int64_t id;
int64_t parentId;
int64_t nameStringId;
int64_t typeStringId;
std::optional<std::wstring> payload;
```
Every node has 
a 64-bit autonumbered primary key, 
a parent ID for hierarchies, 
the ID in the strings table of the node's name (think filename), 
the string ID of what type of node this is (think file vs. directory), 
and what the payload of the node is.  
The payload is  optional to indicate whether it was read from the database when this struct was loaded.\
\
There is a special node, the null node, zero ID and parent ID.  You can always get it and it's never modified.\
\
Within a parent, names must be unique, irregardless of type, like in a file system.\
\
With names, they can be anything but cannot contain /'s as these are reserved for hierarchical string paths.

#### Links
In link.h is the struct link:
```c++
int64_t id;
int64_t fromNodeId;
int64_t toNodeId;
int64_t typeStringId;
std::optional<std::wstring> payload;
```
Every link has 
an autonumbered primary key, 
the ID of node the link comes from, 
the ID of node the link goes to, 
what type of link this is (think "resolved by" from Jira),
and a payload, like node has.\
\
There is no null link.

#### Strings
As you gather, there's a string table with all unique strings in the system, 
and nldb leans hard on this string table for every identifier in the system.\
Only payloads cause string storage in the DB outside the string table.
```c++
int64_t get_id(db& db, const std::wstring& str);
std::wstring get_val(db& db, int64_t id);
std::unordered_map<int64_t, std::wstring> get_vals(db& db, const std::vector<int64_t>& ids);
void flush_caches();
```
Strings are cached in memory, forwards and back; call strings::flush_caches() to empty these caches.

### Node Operations
From nodes.h:
```c++
// Create a node, optionally specifying a type and payload
node create(db& db, int64_t parentNodeId, int64_t nameStringId, int64_t typeStringId = 0, const std::optional<std::wstring>& payload = std::nullopt);

// Copy a node under another parent node
void copy(db& db, int64_t nodeId, int64_t newParentNodeId);

// Move a node under another parent node
void move(db& db, int64_t nodeId, int64_t newParentNodeId);

// Remove a node from the system, recursively
void remove(db& db, int64_t nodeId);

// Change the name of a node
void rename(db& db, int64_t nodeId, int64_t newNameStringId);

// Get a node
// The nodes class maintains a global cache node ID => node pointer
// This cache only returns copies, and it uses RW locks, so it's thread-safe
// All nodes class function invalidate the cache as they modify things
node get(db& db, int64_t nodeId);
void invalidate_cache(int64_t nodeId);
void invalidate_cache(const std::vector<int64_t>& nodeIds);

// Get the payload of a node
std::wstring get_payload(db& db, int64_t nodeId);

// Set a payload onto a node
void set_payload(db& db, int64_t nodeId, const std::wstring& payload);

// Look in a parent node for a child node with a given name
std::optional<node> get_node_in_parent(db& db, int64_t parentNodeId, int64_t nameStringId);

// Get the parent of a node; this may return the null node
node get_parent(db& db, int64_t nodeId);

// Get the ancestors of this, root forward
std::vector<node> get_parents(db& db, int64_t nodeId);

// Get the immediate children of a node
std::vector<node> get_children(db& db, int64_t nodeId);

// Get all descendants of a node
std::vector<node> get_all_children(db& db, int64_t nodeId);

// Get the path of a node as a string with / separators
std::wstring get_path_str(db& db, const node& cur);

// Given a string path, hand back the nodes along the path, or std::nullopt if the path is not resolved
std::optional<std::vector<node>> get_path_nodes(db& db, const std::wstring& path);
```

### Link Operations
From links.h:
```c++
// Given from and to nodes and an optional type name of the link and an optional payload, return a new link
link create(db& db, int64_t fromNodeId, int64_t toNodeId, int64_t typeStringId = 0, const std::optional<std::wstring>& payload = std::nullopt);

// Remove the link of the given type between two nodes
bool remove(db& db, int64_t fromNodeId, int64_t toNodeId, int64_t typeStringId = 0);

// Give a link ID, get back the link, or std::nullopt
std::optional<link> get(db& db, int64_t linkId);

// Get the payload of a link
std::wstring get_payload(db& db, int64_t nodeId);

// Set a payload onto a link
void set_payload(db& db, int64_t nodeId, const std::wstring& payload);

// Get out links from a node
std::vector<link> get_out_links(db& db, int64_t fromNodeId);

// Get in links to a node
std::vector<link> get_in_links(db& db, int64_t toNodeId);
```

## Properties and Search
With properties you can associate name-value pairs of strings with nodes and links, 
then look up nodes and links by names and values, with the search class.\
\
From props.h:
```c++
// Set a property onto...somthing...itemTypeStringId should refer to "node" or "link"...this is left open for future development
void set(db& db, int64_t itemTypeStringId, int64_t itemId, int64_t nameStringId, int64_t valueStringId); // use < 0 to delete

// Get the type of thing and its ID, get all name-value string ID pairs
std::unordered_map<int64_t, int64_t> get(db& db, int64_t itemTypeStringId, int64_t itemId);

// Convenience routine for filling string values into name-value pairs
std::map<std::wstring, std::wstring> fill(db& db, const std::unordered_map<int64_t, int64_t>& map);

// Troubleshooting / POC routine for filling name-value string ID pairs into name-space-value lines of text
std::wstring summarize(db& db, const std::unordered_map<int64_t, int64_t>& map);
```
From search.h:
```c++
// Define the name-value pair to look for, optionally using LIKE for the lookup
struct search_criteria 
{
    int64_t nameStringId;
    std::wstring valueString;
    bool useLike;
};

// Define the list of criteria to look for, and other search options
struct search_query 
{
    std::vector<search_criteria> criteria;
    std::wstring orderBy;
    bool orderAscending;
    int64_t limit;
    bool includePayload;
};

class search
{
    // Run a search yielding nodes
    static std::vector<node> find_nodes(db& db, const search_query& query);

    // Run a search yielding links
    static std::vector<link> find_links(db& db, const search_query& query);
};
```
There are a few built-in search names that have special behaviors:
1. parent: Results will all be direct children of the parent at the given path (shallow)
2. path: Results will all be descendent of the parent at the given path (deep)
3. name: Results will match this name.  LIKE is supported.
4. payload: Results will have payload that matches.  LIKE is supported.
5. type: Results will be of the given type.  Exact match only.

## Cloud Computing
When working with nodes and links, a notion of clouds full of nodes connected by links comes to mind.\
\
In cloud.h you find...
```c++
class cloud
{
    // You start with a seed node
    cloud(db& db, int64_t seedNodeId) {}

    // You create a database table based on the seed node ID
    void init();

    // You seed the cloud with the immediate links from and to the node
    // Returns the number of links added
    // NOTE: The seed node must have at least one link for this to work
    int64_t seed();

    // Grow the cloud out from the current set of links
    // Returns the number of links added
    int64_t expand();

    // What is the maximum generation in the cloud?
    int max_generation() const;

    // What are the links, starting out from the given minimum generation?
    std::vector<link> links(int minGeneration) const;
};
```