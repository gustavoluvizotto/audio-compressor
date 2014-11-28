/*******************************************
 * Projeto 2				   *
 * SCC - 0661 Multim�dia e Hiperm�dia	   *
 * 28/11/2014				   *
 *******************************************/

 _______________________________________________________
|Integrante		     |      	(N� USP) 	|
|----------------------------|--------------------------|
| Adailto Aparecido Caramano |		(7126802)	|
| Gustavo Luvizotto Cesar    |		(6783544)	|
| Ivan Donisete Lonel        | 		(6766044)	|
 _______________________________________________________|
 
 Como usar:
 Escolha uma op��o entre comprimir, descomprimir ou informa��es sobre o trabalho.
 
 "Choose compress(c), decompress(d) or help (h) for further informations
  Option: c"
  
  No exemplo usou-se a op��o de compress�o. Em seguida, entre com o caminho do arquivo
a ser comprimido.

  "Enter with the path and name of the sound file (including the .wav extension): resources/ghost.wav"
  
  No exemplo o arquivo ghost.wav estava no diretorio resources. Se fosse escolhido o modo de 
descompress�o, a entrada nesse caso deveria ser resources/ghosts.wav.bin
  
  Em seguida escolha a compress�o que deseja
  
  "Choose the compression you want
   0. Exit
   1. Huffman
   2. Differences
   1
   2"
   
   No exemplo foi inserido compress�o por huffman e por diferen�as. A compress�o por 
diferen�as utiliza c�digos de huffman para servir de separador. O programa aceita 
compress�o apenas por huffman, apenas differen�as, huffman e diferen�as ou diferen�as e 
huffman.
   O arquivo de som wav original pode possuir at� 16 canais, deve ter 8 bits por 
amostra e n�o pode possuir algum tipo de compress�o.
   Obs.: A compress�o/descompress�o por Huffman (op��o 1) demora muito tempo dependendo
do tamanho do arquivo pois o alcance das amostras s�o de 256 posi��es, diferente da 
compress�o/descompress�o por diferen�as que o alcance � de 9 posi��es (de 0 � 8 bits
usado no sss).