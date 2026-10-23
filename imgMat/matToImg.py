
from PIL import Image

# Lê a matriz de pixels do arquivo txt
with open("saida.txt", "r") as arquivo:
    matriz = []

    for linha in arquivo:
        valores = [int(valor) for valor in linha.split()]

        if valores:
            matriz.append(valores)

# Verifica se a matriz não está vazia
if not matriz:
    raise ValueError("A matriz está vazia.")

# Verifica se todas as linhas têm o mesmo tamanho
largura = len(matriz[0])

for linha in matriz:
    if len(linha) != largura:
        raise ValueError("A matriz possui linhas de tamanhos diferentes.")

# Cria a imagem em escala de cinza
altura = len(matriz)
imagem = Image.new("L", (largura, altura))

# Insere os pixels na imagem
pixels = imagem.load()

for y in range(altura):
    for x in range(largura):
        valor = matriz[y][x]

        if valor < 0 or valor > 255:
            raise ValueError("Pixel fora do intervalo de 0 a 255.")

        pixels[x, y] = valor

# Salva a imagem PNG
imagem.save("imgPng/imagem_saida.png")

print("Imagem gerada com sucesso!")
