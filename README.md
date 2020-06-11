# About
Exact Pattern Matching using [Burrows-Wheeler Transformation](https://en.wikipedia.org/wiki/Burrows%E2%80%93Wheeler_transform) and [Ferragina-Manzini index](https://en.wikipedia.org/wiki/FM-index).

# Primer

Burrows–Wheeler transform  rearranges character string into the runs of similar characters and can be reversed using only the position of the first character.

FM-index is a compressed full-text substring index based on the Burrows-Wheeler transform.

It can be used to efficiently find the number of occurrences of a pattern within the compressed text, as well as locate the position of each occurrence.

The query time, as well as the required storage space, has a sublinear complexity with respect to the size of the input data.

# Installation

```bash
$ git clone https://github.com/salmoor/burrows-wheeler-transform.git
$ cd burrows-wheeler-transform
$ make
```

# How to use

Execution of the program is performed in two steps.

**Step 1 - Index generation:** In this step program will take one reference input file in [FASTA](https://en.wikipedia.org/wiki/FASTA_format) format.  After index generation we will have two files:

- First, with extension **bwt** that will contain the **BWT** string,
- Second, with extension **fm** that will include the FM-index tables and the suffix array.

Example of index generation:

```bash
$ ./bwtfm --index mytext.fa
```

This should generate two files: `mytext.fa.bwt` and `mytext.fa.fm`.

**Step 2 - Search:** After step 1 we can perform the search of the query pattern within reference text by running

```bash
$ ./bwtfm --search mytext.fa --pattern query.fa
```

The program will determine the `bwt` and `fm` files from the name of the reference text file and will perform the search of the pattern from the `query.fa` file.

Below is the sample output using files from `test_texts` and `test_patterns` folders.

``` Pattern query found in HSBGPG Human gene for bone gla protein (BGP) 1 time(s) at position(s):
Pattern query found in HSBGPG Human gene for bone gla protein (BGP) 1 time(s) at position(s):
        pos 1: 1201
Search completed in 914 microseconds
```



