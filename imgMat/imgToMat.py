from PIL import Image
import numpy as np

imagem = Image.open("imagem3_1024.png") #insert the name of the PNG

matriz = np.array(imagem)

np.savetxt("imagemMatriz1024.txt", matriz, fmt="%d", delimiter=" ") #choose a name for the txt

print(matriz)
