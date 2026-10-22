from PIL import Image

# Abre a imagem
imagem = Image.open("imagem1_256.png")

# Converte para escala de cinza (8 bits por pixel)
imagem = imagem.convert("L")

# Transforma os pixels em uma matriz
matriz = [
    list(imagem.crop((0, y, imagem.width, y + 1)).getdata())
    for y in range(imagem.height)
]

print(matriz)
