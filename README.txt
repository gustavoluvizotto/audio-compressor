/*******************************************
 * Project 2				   *
 * SCC0661 - Multimedia and Hypermedia	   *
 * 29/11/2014				   *
 *******************************************/

 _______________________________________________________
| Group members		     |      	(USP ID) 	|
|----------------------------|--------------------------|
| Gustavo Luvizotto Cesar    |		(6783544)	|
| Ivan Donisete Lonel        | 		(6766044)	|
|____________________________|__________________________|

  1) About the project
  Objectives: The principal objective is to implement some compression/decompression 
tecniques applyed to digital audio.
  
  Will be employed for coding techniques of differences and Huffman. The techniques 
should be applied, combined or isolated way, in WAV format for an audio file 
without compression. The resulting data compression should be stored in a binary 
file recording only the bits that represent each generated code.
  
  Expected inputs: 
    compression: a WAV file without compression.
    decompression: a binary file containing the representation of the compressed 
    audio file.
    
  Expected outputs:
    compression: binary file containing the bitstream that represents the compressed
    audio file.
    decompression: an approximation of original WAV file not compressed.
      
  2) How to build and execute
  To build, you need to go to build folder:
  
    cd build
    
  And then, build the project using make:
  
    make all
    
  Will generate "audio-compressor" binary. To execute, just do:
  
    ./audio-compressor
      
  3) How to use
  Choose one option between compress, decompress or informations about the work.
 
 "Choose compress(c), decompress(d) or help (h) for further informations
  Option: c"
  
  In the example, was used the compression option. Next, enter with the path of the file
to be compressed.

  "Enter with the path and name of the sound file (including the .wav extension): ../resources/ghost.wav"
  
  In the example the "ghost.wav" file was in the directory "resources", above your actual directory (if 
you are executing inside "build" directory). If has choosen the decompress mode, then the input should 
be "../resources/ghost.wav.bin".
  Next, choose the compression you want.
  
  "Choose the compression you want
   0. Exit
   1. Huffman
   2. Differences
   1
   2"
   
  In the example above, was chosen huffman and differences compression. The compression by 
differences uses the huffman code to serve as a delimiter. The program accept compression 
only huffman, only differences, huffman and differences or differences and huffman. 
The original wav file must be up to 16 channels, 8 bits per sample and do not own 
any kind of compression previously. 

PS1: The compression/decompression by Huffman (1st option) takes too long depending on 
the size of the file, because the samples range could be 0 to 255 (256 positions), that's 
different from differences compression/decompression that uses 9 positions (from 0 to 8 
bits used in SSS). Therefore, the method could take about 32 times more using Huffman 
compression. 
PS2: The program uses the compressed file (.bin) in the decompression operation as a 
intermediate file to unpack previously compressions. In other words, if it is necessary 
analyze the compress rate of the program, might be analysed the resultant file (.bin) 
after the compression.