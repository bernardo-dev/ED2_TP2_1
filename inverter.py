# Lê o conteúdo do arquivo PROVAO_ASCENDENTE.TXT
with open("PROVAO_ASCENDENTE.TXT", "r") as file:
  content = file.readlines()

# Inverte a ordem das linhas
content.reverse()

# Cria o arquivo PROVAO_DESCENDENTE.TXT e escreve o conteúdo invertido
with open("PROVAO_DESCENDENTE.TXT", "w") as file:
  file.writelines(content)
