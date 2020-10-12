B+Tree project
==========================
This project is a C++ project with CLion.
Disk and memory implementation run with 1000000 registers on secondary memory.
Insert, delete and search were developed.
This project use templates , and fixed record.

Main idea is connect the the key stored in the B+Tree with the page-id of the record file.
So this way we can search for a target key (example: id-student) in the data structure and
then get the position where the record is stored in the binary file.

Requirements
-------------
The basic requirements for this example is CLion or just run cmake ( v3.15 or earlier).

