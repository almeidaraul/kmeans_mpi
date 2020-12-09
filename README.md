# kmeans_mpi
Parallelizing a (given) implementation of the K-Means algorithm with MPI

## Makefile's rules
* `sequential`: compiles 

## Using the scripts
Scripts are at the `scripts` directory
### `geninput.py`
`python3 geninput.py K N` (`K` is the number of clusters and `N` is the number of points)

## TODO
* [ ] sequential .c file (outputs `output\nexec time`)
* [ ] parallel .c file (output `output\nexec time`)
* [ ] sequential .c file for sequential % analysis (outputs % of time spent in sequential areas of the code)
* [ ] parallel .c file for sequential % analysis (outputs % of time spent in sequential areas of the code)
* [ ] script for initialization (create inputs directory, etc)
* [ ] script for testing using the four .c files above and input files
