B+Tree project
==========================
B-Tree is a self-balancing search tree. For this project the goal was the implementation of B+Tree unclustered index.
Have been tested with 1000000 registers on secondary memory.
This project uses fixed record.

Main idea is connect the the key stored in the B+Tree with the page-id or position in the record file.
So this way we can search for a target key (example: id-student) in the data structure and
then get the position where the record is stored in the binary file.

The B+Tree posses an iterator class and a pagemanager to write and read in the binary file.

Properties of B+Tree
--------------------

1. All leaves are at the same level and have a pointer to the next leaf.
2. A B-Tree is defined by the term minimum degree ‘t’. The value of t depends upon disk block size.
3. Every node except root must contain at least (ceiling)([t-1]/2) keys. The root may contain minimum 1 key.
4. All nodes (including root) may contain at most t – 1 keys.
5. Number of children of a node is equal to the number of keys in it plus 1.
6. All keys of a node are sorted in increasing order. The child between two keys k1 and k2 contains all keys in the range from k1 and k2.
7. B-Tree grows and shrinks from the root which is unlike Binary Search Tree. Binary Search Trees grow downward and also shrink from downward.
8. Like other balanced Binary Search Trees, time complexity to search, insert and delete is O(log n).

Requirements
-------------
The basic requirements for this example is CLion or just run cmake ( v3.15 or earlier).

