/*******************************************
 * Projeto 2				   *
 * SCC - 0661 Multimídia e Hipermídia	   *
 * 28/11/2014				   *
 *******************************************/

 _______________________________________________________
|Integrante		     |      	(Nº USP) 	|
|----------------------------|--------------------------|
| Adailto Aparecido Caramano |		(7126802)	|
| Gustavo Luvizotto Cesar    |		(6783544)	|
| Ivan Donisete Lonel        | 		(6766044)	|
 _______________________________________________________|
 
 Como usar:
 Escolha uma opção entre comprimir, descomprimir ou informações sobre o trabalho.
 
 "Choose compress(c), decompress(d) or help (h) for further informations
  Option: c"
  
  No exemplo usou-se a opção de compressão. Em seguida, entre com o caminho do arquivo
a ser comprimido.

  "Enter with the path and name of the sound file (including the .wav extension): resources/ghost.wav"
  
  No exemplo o arquivo ghost.wav estava no diretorio resources. Se fosse escolhido o modo de 
descompressão, a entrada nesse caso deveria ser resources/ghosts.wav.bin
  
  Em seguida escolha a compressão que deseja
  
  "Choose the compression you want
   0. Exit
   1. Huffman
   2. Differences
   1
   2"
   
   No exemplo foi inserido compressão por huffman e por diferenças. A compressão por 
diferenças utiliza códigos de huffman para servir de separador. O programa aceita 
compressão apenas por huffman, apenas differenças, huffman e diferenças ou diferenças e 
huffman.
   O arquivo de som wav original pode possuir até 16 canais, deve ter 8 bits por 
amostra e não pode possuir algum tipo de compressão.
   Obs.: A compressão/descompressão por Huffman (opção 1) demora muito tempo dependendo
do tamanho do arquivo pois o alcance das amostras são de 256 posições, diferente da 
compressão/descompressão por diferenças que o alcance é de 9 posições (de 0 à 8 bits
usado no sss).