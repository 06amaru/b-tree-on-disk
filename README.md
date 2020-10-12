B+Tree project
==========================
This project is a C++ project with CLion.
Disk and memory implementation run with 1000000 registers on secondary memory.
Insert, **delete** and search were developed.
This project use templates , and fixed record.

Main idea is connect the the key stored in the B+Tree with the page-id or position in the record file.
So this way we can search for a target key (example: id-student) in the data structure and
then get the position where the record is stored in the binary file.

The B+Tree posses an iterator class and a pagemanager to write and read in the binary file.

This projects has been developed in memory and disk. Thus there are g-test for both parts.
The test in disk consists of insert a 1000000 records of Students(id, name, courses) in the B+Tree.
The following tests consist of doing a search by range through an iterator.

Requirements
-------------
The basic requirements for this example is CLion or just run cmake ( v3.15 or earlier).

