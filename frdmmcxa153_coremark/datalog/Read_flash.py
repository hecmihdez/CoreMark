import struct
import csv
import os

# === CONFIGURACIÓN ===
formato_struct = '<8s6I6H4s'  # Sin padding: 48 bytes
nombres_campos = [
    'StartMsg', 'Temperature', 'CoreMarkSize', 'TotalTicks',
    'TotalTime', 'IterationsPerSec', 'Iterations',
    'SeedCrc', 'CrcList', 'CrcMatrix', 'CrcState',
    'CrcFinal', 'NumErrors', 'EndMsg'
]

archivo_bin = os.path.join(os.path.dirname(__file__), 'flash_dump.bin')
archivo_txt = os.path.join(os.path.dirname(__file__), 'Data_log.txt')
archivo_csv = os.path.join(os.path.dirname(__file__), 'Data_log.csv')
# =====================

tamano_struct = struct.calcsize(formato_struct)

def interpretar_flash_bin(path_bin):
    with open(path_bin, 'rb') as f:
        datos = f.read()

    total = len(datos) // tamano_struct
    print(f"Total de estructuras encontradas: {total}")

    registros_validos = []
    for i in range(total):
        offset = i * tamano_struct
        valores = struct.unpack_from(formato_struct, datos, offset)

        valores = list(valores)
        # Decodificar cadenas
        start_msg = valores[0].decode('ascii', errors='ignore').strip('\x00')
        end_msg = valores[-1].decode('ascii', errors='ignore').strip('\x00')

        # Validación
        if not start_msg.startswith('**START*'):
            continue
        if 'END*' not in end_msg:
            continue


        valores[0] = start_msg
        valores[-1] = end_msg
        registro = dict(zip(nombres_campos, valores))

        # Ajustar factor de temperatura
        registro['Temperature'] = registro['Temperature'] / 1000.0
        
        registros_validos.append(registro)

    return registros_validos

def guardar_como_txt(registros, path_txt):
    with open(path_txt, 'w') as f:
        for i, reg in enumerate(registros):
            f.write(f"Registro {i + 1}:\n")
            for key, value in reg.items():
                f.write(f"  {key}: {value}\n")
            f.write("\n")

def guardar_como_csv(registros, path_csv):
    if not registros:
        return

    # Campos a incluir, excluyendo StartMsg y EndMsg
    campos_csv = ['Registro'] + [c for c in nombres_campos if c not in ('StartMsg', 'EndMsg')]

    with open(path_csv, 'w', newline='') as f:
        writer = csv.DictWriter(f, fieldnames=campos_csv)
        writer.writeheader()

        for i, reg in enumerate(registros, start=1):
            fila = {'Registro': i}
            for campo in campos_csv[1:]:
                fila[campo] = reg[campo]
            writer.writerow(fila)

# === USO ===
registros = interpretar_flash_bin(archivo_bin)
guardar_como_txt(registros, archivo_txt)
guardar_como_csv(registros, archivo_csv)

print(f"\n{len(registros)} registros válidos guardados en:")
print(f" - TXT: {archivo_txt}")
print(f" - CSV: {archivo_csv}")