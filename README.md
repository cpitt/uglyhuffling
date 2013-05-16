UglyHuffling
============

A simple and ugly (read don't judge me) Huffman compression and decompression written in c++ for the final project
of my CS301 algorithms and complexity class.

THE ZIP301 FILE FORMAT
======================
A ZIP301 file is in two parts: a plaintext header and a binary body.

The first several lines of the header are formatted thus:
[binary string] [whitespace] [character]

In the event that the [character] to encode is itself whitespace, we use the
following surrogate encodings:
* ' '  <==> 'space'
* '\n' <==> 'newline'
* '\r' <==> 'return'
* '\t' <==> 'tab'

After the binary strings, there is a single line of five asterisks: '*****'

Following the asterisks, there is a single line containing the total number of
bits, N, in the encoded document. This is necessary because APIs for
reading/writing files do not support granularity smaller than one byte.
However, a Huffman encoding for an entire document may not necessarily be a
multiple of 8. Hence, the final few bits of the final byte in the encoding may
not be significant.

This concludes the header. The body (in binary) begins on the next line. It is
simply the contents of the original document, encoded using the codes listed in
the file header. In the (likely) event that the encoding is not an exact
multiple of 8 bits, the final byte will have some insignificant bits at the end
of it.
