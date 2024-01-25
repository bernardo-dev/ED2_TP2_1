import csv
import re
import subprocess
import os

metodo = {1: "2f fitas", 2: "f+1 fitas", 3: "Quicksort Externo"}
quantidade = [100, 200, 2000, 20000, 471705]
situacao = {1: "Ascendente", 2: "Descendente", 3: "Aleatorio"}

# Cria uma pasta se ela não existir
pasta = "metricas"
os.makedirs(pasta, exist_ok=True)

for q in quantidade:
  for s_key, s_value in situacao.items():
    arquivo = f"{pasta}/metricas_{q}_{s_value}.csv"
    with open(arquivo, 'w', newline='') as arquivo_csv:
      escritor_csv = csv.writer(arquivo_csv, delimiter=';', quotechar='"', quoting=csv.QUOTE_MINIMAL)
      
      # Escreve o cabeçalho se necessário
      escritor_csv.writerow(["Metodos", "2f fitas", "f+1 fitas", "Quicksort Externo"])
    
      leituras = []
      escritas = []
      comparacoes = []
      tempo_execucao = []

      for m_key, m_value in metodo.items():
        comando_gerar = f"./ordena {m_key} {q} {s_key}"
        print(comando_gerar)

        # Executa o comando no terminal
        saida_terminal = subprocess.check_output(comando_gerar, universal_newlines=True, shell=True)

        # Expressão regular para extrair as métricas
        padrao_leituras = re.compile(r"NUMERO TOTAL DE LEITURAS: (\d+)")
        padrao_escritas = re.compile(r"NUMERO TOTAL DE ESCRITAS: (\d+)")
        padrao_comparacoes = re.compile(r"NUMERO TOTAL DE COMPARACOES: (\d+)")
        padrao_tempo_execucao = re.compile(r"TEMPO TOTAL DE EXECUCAO: ([\d.]+)")

        # Encontra a primeira correspondência na saída do terminal
        leituras.append(padrao_leituras.search(saida_terminal))
        escritas.append(padrao_escritas.search(saida_terminal))
        comparacoes.append(padrao_comparacoes.search(saida_terminal))
        tempo_execucao.append(padrao_tempo_execucao.search(saida_terminal))

        print(saida_terminal)
      
      escritor_csv.writerow(["Total de Leituras", leituras[0].group(1), leituras[1].group(1), leituras[2].group(1)])
      escritor_csv.writerow(["Total de escritas", escritas[0].group(1), escritas[1].group(1), escritas[2].group(1)])
      escritor_csv.writerow(["Total de comparacoes", comparacoes[0].group(1), comparacoes[1].group(1), comparacoes[2].group(1)])
      escritor_csv.writerow(["Tempo de execucao", tempo_execucao[0].group(1), tempo_execucao[1].group(1), tempo_execucao[2].group(1)])
