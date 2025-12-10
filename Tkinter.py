import tkinter as tk
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import serial
import time
import json


BAUD_RATE = 9600
TIMEOUT = 1

PORTA_SERIAL_RAMPA = 'COM19'
hist_tempo = []
hist_velocidade = []
hist_aceleracao = []
hist_distancia = []


fig_rampa = Figure(figsize=(6, 8), dpi=100)

eixo_vel = fig_rampa.add_subplot(311)
eixo_vel.set_title("Velocidade (V x T)")
eixo_vel.set_ylabel("Vel. (m/s)")
eixo_vel.tick_params(labelbottom=False)

eixo_acc = fig_rampa.add_subplot(312)
eixo_acc.set_title("Aceleração (A x T)")
eixo_acc.set_ylabel("Acc. (m/s²)")
eixo_acc.tick_params(labelbottom=False)

eixo_dist = fig_rampa.add_subplot(313)
eixo_dist.set_title("Distância (D x T)")
eixo_dist.set_xlabel("Tempo (s)")
eixo_dist.set_ylabel("Dist. (m)")

fig_rampa.tight_layout()

ser_rampa = serial.Serial(port=PORTA_SERIAL_RAMPA, baudrate=BAUD_RATE, timeout=TIMEOUT)
time.sleep(2)


PORTA_SERIAL_EXP2 = 'COM34'
hist_tempo_exp2 = []
hist_velocidade_giro = []
hist_corrente = []

fig_exp2 = Figure(figsize=(6, 6), dpi=100)

eixo_vel_giro = fig_exp2.add_subplot(211)
eixo_vel_giro.set_title("Velocidade de Giro (V.Giro x T)")
eixo_vel_giro.set_ylabel("Vel. Giro (RPM ou rad/s)")
eixo_vel_giro.tick_params(labelbottom=False)

eixo_corrente = fig_exp2.add_subplot(212)
eixo_corrente.set_title("Corrente (C x T)")
eixo_corrente.set_xlabel("Tempo (s)")
eixo_corrente.set_ylabel("Corrente (A)")

fig_exp2.tight_layout()

ser_exp2 = serial.Serial(port=PORTA_SERIAL_EXP2, baudrate=BAUD_RATE, timeout=TIMEOUT)
time.sleep(2)


PORTA_SERIAL_ELETROIMA = 'COM12'
hist_tempo_eletro = []
hist_corrente_eletro = []
hist_tensao = []
hist_campo = []


fig_eletro = Figure(figsize=(6, 8), dpi=100)

eixo_corrente_t = fig_eletro.add_subplot(311)
eixo_corrente_t.set_title("Corrente (C x T)")
eixo_corrente_t.set_ylabel("Corrente (A)")
eixo_corrente_t.tick_params(labelbottom=False)

eixo_campo_t = fig_eletro.add_subplot(312)
eixo_campo_t.set_title("Campo Magnético (B x T)")
eixo_campo_t.set_ylabel("Campo (Tesla ou Gauss)")
eixo_campo_t.tick_params(labelbottom=False)

eixo_chaveavel = fig_eletro.add_subplot(313)
eixo_chaveavel.set_title("Campo vs Tensão (Chaveável)")
eixo_chaveavel.set_xlabel("Tensão (V)")
eixo_chaveavel.set_ylabel("Campo (Tesla ou Gauss)")

fig_eletro.tight_layout()

ser_eletro = serial.Serial(port=PORTA_SERIAL_ELETROIMA, baudrate=BAUD_RATE, timeout=TIMEOUT)
time.sleep(2)


janela = tk.Tk()
janela.title("Monitoramento de Experimentos")

janela.grid_rowconfigure(0, weight=1)
janela.grid_columnconfigure(0, weight=1)


frame_exp_rampa = tk.Frame(janela)
frame_exp_rampa.grid(row=0, column=0, sticky="nsew")

canvas_rampa = FigureCanvasTkAgg(fig_rampa, master=frame_exp_rampa)
canvas_rampa_widget = canvas_rampa.get_tk_widget()
canvas_rampa_widget.pack(side=tk.TOP, fill=tk.BOTH, expand=True)


frame_exp_exp2 = tk.Frame(janela)
frame_exp_exp2.grid(row=0, column=0, sticky="nsew")

canvas_exp2 = FigureCanvasTkAgg(fig_exp2, master=frame_exp_exp2)
canvas_exp2_widget = canvas_exp2.get_tk_widget()
canvas_exp2_widget.pack(side=tk.TOP, fill=tk.BOTH, expand=True)


frame_exp_eletroima = tk.Frame(janela)
frame_exp_eletroima.grid(row=0, column=0, sticky="nsew")

canvas_eletro = FigureCanvasTkAgg(fig_eletro, master=frame_exp_eletroima)
canvas_eletro_widget = canvas_eletro.get_tk_widget()
canvas_eletro_widget.pack(side=tk.TOP, fill=tk.BOTH, expand=True)

frame_eletro_controles = tk.Frame(frame_exp_eletroima)
frame_eletro_controles.pack(side=tk.BOTTOM, fill=tk.X, pady=5)

eletroima_eixo_y_mode = tk.StringVar(value='tensao')

botao_vs_tensao = tk.Button(frame_eletro_controles, text="Plotar: Campo vs Tensão (V)", command=lambda: alterna_eixo_eletroima('tensao'))
botao_vs_tensao.pack(side=tk.LEFT, expand=True, padx=5)

botao_vs_corrente = tk.Button(frame_eletro_controles, text="Plotar: Campo vs Corrente (A)", command=lambda: alterna_eixo_eletroima('corrente'))
botao_vs_corrente.pack(side=tk.LEFT, expand=True, padx=5)


def le_dados_rampa():
    global hist_tempo, hist_velocidade, hist_aceleracao, hist_distancia

    if not ser_rampa.is_open:
        return

    dados_coletados = []
    coletando = False

    while ser_rampa.in_waiting > 0 or coletando:
        linha_bytes = ser_rampa.readline()
        linha_str = linha_bytes.decode('utf-8').strip()

        if linha_str == "#LISTA_ULTRASSONICO":
            coletando = True
            continue

        if linha_str == "#FIM_ULTRASSONICO":
            coletando = False
            break

        if coletando and linha_str:
            partes = linha_str.split(',')
            if len(partes) == 4:
                registro = []
                for item_str in partes:
                    valor_float = float(item_str.strip())
                    registro.append(valor_float)
                   
                dados_coletados.append(registro)

        time.sleep(0.005)

    if dados_coletados:

        hist_tempo.clear()
        hist_velocidade.clear()
        hist_aceleracao.clear()
        hist_distancia.clear()

        for reg in dados_coletados:
            hist_tempo.append(reg[0])
            hist_velocidade.append(reg[1])
            hist_aceleracao.append(reg[2])
            hist_distancia.append(reg[3])

        eixo_vel.clear()
        eixo_vel.plot(hist_tempo, hist_velocidade, color='red')
        eixo_vel.set_title("Velocidade (V x T)")
        eixo_vel.set_ylabel("Vel. (m/s)")
        eixo_vel.tick_params(labelbottom=False)

        eixo_acc.clear()
        eixo_acc.plot(hist_tempo, hist_aceleracao, color='green')
        eixo_acc.set_title("Aceleração (A x T)")
        eixo_acc.set_ylabel("Acc. (m/s²)")
        eixo_acc.tick_params(labelbottom=False)

        eixo_dist.clear()
        eixo_dist.plot(hist_tempo, hist_distancia, color='blue')
        eixo_dist.set_title("Distância (D x T)")
        eixo_dist.set_ylabel("Dist. (m)")
        eixo_dist.set_xlabel("Tempo (s)")
        eixo_dist.tick_params(labelbottom=True)

        canvas_rampa.draw()


def le_dados_exp2():
    global hist_tempo_exp2, hist_velocidade_giro, hist_corrente

    if ser_exp2.in_waiting > 0:
        linha_bytes = ser_exp2.readline()
        dados_str = linha_bytes.decode('utf-8').strip()

        if dados_str.count(',') == 2:
            partes = dados_str.split(',')

            tempo = float(partes[0])
            vel_giro = float(partes[1])
            corrente = float(partes[2])

            hist_tempo_exp2.append(tempo)
            hist_velocidade_giro.append(vel_giro)
            hist_corrente.append(corrente)

            eixo_vel_giro.clear()
            eixo_vel_giro.plot(hist_tempo_exp2, hist_velocidade_giro, color='orange')
            eixo_vel_giro.set_title("Velocidade de Giro (V.Giro x T)")
            eixo_vel_giro.set_ylabel("Vel. Giro (RPM ou rad/s)")
            eixo_vel_giro.tick_params(labelbottom=False)

            eixo_corrente.clear()
            eixo_corrente.plot(hist_tempo_exp2, hist_corrente, color='purple')
            eixo_corrente.set_title("Corrente (C x T)")
            eixo_corrente.set_ylabel("Corrente (A)")
            eixo_corrente.set_xlabel("Tempo (s)")

            canvas_exp2.draw()


def le_dados_eletroima():
    global hist_tempo_eletro, hist_corrente_eletro, hist_tensao, hist_campo

    if ser_eletro.in_waiting > 0:
        linha_bytes = ser_eletro.readline()
        dados_str = linha_bytes.decode('utf-8').strip()
        if dados_str.count(',') == 3:
            partes = dados_str.split(',')

            tempo = float(partes[0])
            corrente = float(partes[1])
            tensao = float(partes[2])
            campo = float(partes[3])

            hist_tempo_eletro.append(tempo)
            hist_corrente_eletro.append(corrente)
            hist_tensao.append(tensao)
            hist_campo.append(campo)

            eixo_corrente_t.clear()
            eixo_corrente_t.plot(hist_tempo_eletro, hist_corrente_eletro, color='brown')
            eixo_corrente_t.set_title("Corrente (C x T)")
            eixo_corrente_t.set_ylabel("Corrente (A)")
            eixo_corrente_t.tick_params(labelbottom=False)

            eixo_campo_t.clear()
            eixo_campo_t.plot(hist_tempo_eletro, hist_campo, color='black')
            eixo_campo_t.set_title("Campo Magnético (B x T)")
            eixo_campo_t.set_ylabel("Campo (Tesla ou Gauss)")
            eixo_campo_t.tick_params(labelbottom=False)

            eixo_chaveavel.clear()
            modo = eletroima_eixo_y_mode.get()

            if modo == 'tensao':
                eixo_chaveavel.plot(hist_tensao, hist_campo, color='red', marker='o',linestyle='None')
                eixo_chaveavel.set_title("Campo vs Tensão")
                eixo_chaveavel.set_xlabel("Tensão (V)")

            elif modo == 'corrente':
                eixo_chaveavel.plot(hist_corrente_eletro, hist_campo, color='blue', marker='x',linestyle='None')
                eixo_chaveavel.set_title("Campo vs Corrente")
                eixo_chaveavel.set_xlabel("Corrente (A)")

            eixo_chaveavel.set_ylabel("Campo (Tesla ou Gauss)")
            eixo_chaveavel.tick_params(labelbottom=True)

            canvas_eletro.draw()


def alterna_eixo_eletroima(modo):
    eletroima_eixo_y_mode.set(modo)


def zerar_historico_geral():
    global hist_tempo, hist_velocidade, hist_aceleracao, hist_distancia
    global hist_tempo_exp2, hist_velocidade_giro, hist_corrente
    global hist_tempo_eletro, hist_corrente_eletro, hist_tensao, hist_campo

    hist_tempo, hist_velocidade, hist_aceleracao, hist_distancia = [], [], [], []
    eixo_vel.clear(); eixo_acc.clear(); eixo_dist.clear();
    eixo_vel.set_title("Velocidade (V x T)"); eixo_acc.set_title("Aceleração (A x T)"); eixo_dist.set_title("Distância (D x T)");
    eixo_vel.tick_params(labelbottom=False); eixo_acc.tick_params(labelbottom=False); eixo_dist.tick_params(labelbottom=True);
    canvas_rampa.draw()

    hist_tempo_exp2, hist_velocidade_giro, hist_corrente = [], [], []
    eixo_vel_giro.clear(); eixo_corrente.clear();
    eixo_vel_giro.set_title("Velocidade de Giro (V.Giro x T)"); eixo_corrente.set_title("Corrente (C x T)");
    eixo_vel_giro.tick_params(labelbottom=False); eixo_corrente.tick_params(labelbottom=True);
    canvas_exp2.draw()

    hist_tempo_eletro, hist_corrente_eletro, hist_tensao, hist_campo = [], [], [], []
    eixo_corrente_t.clear(); eixo_campo_t.clear(); eixo_chaveavel.clear();
    eixo_corrente_t.set_title("Corrente (C x T)"); eixo_campo_t.set_title("Campo Magnético (B x T)"); eixo_chaveavel.set_title("Campo vs Tensão (Chaveável)");
    eixo_corrente_t.tick_params(labelbottom=False); eixo_campo_t.tick_params(labelbottom=False); eixo_chaveavel.tick_params(labelbottom=True);
    eixo_chaveavel.set_xlabel("Tensão (V)")
    canvas_eletro.draw()


def leitura_geral():
    le_dados_rampa()
    le_dados_exp2()
    le_dados_eletroima()
    janela.after(1000, leitura_geral)


frame_menu = tk.Frame(janela)
frame_menu.grid(row=1, column=0, sticky="ew", pady=10)

botao_exp1 = tk.Button(frame_menu, text="Experimento 1: Rampa", command=lambda: mostrar_experimento('rampa'))
botao_exp1.pack(side=tk.LEFT, expand=True, padx=5)

botao_exp2 = tk.Button(frame_menu, text="Experimento 2: Nome?", command=lambda: mostrar_experimento('exp2'))
botao_exp2.pack(side=tk.LEFT, expand=True, padx=5)

botao_exp3 = tk.Button(frame_menu, text="Experimento 3: Eletroímã", command=lambda: mostrar_experimento('eletroima'))
botao_exp3.pack(side=tk.LEFT, expand=True, padx=5)

botao_reset = tk.Button(frame_menu, text="Zerar Todos os Dados", command=zerar_historico_geral)
botao_reset.pack(side=tk.RIGHT, expand=False, padx=10)


def mostrar_experimento(nome_exp):
    if nome_exp == 'rampa':
        frame_exp_rampa.tkraise()
    elif nome_exp == 'exp2':
        frame_exp_exp2.tkraise()
    elif nome_exp == 'eletroima':
        frame_exp_eletroima.tkraise()


mostrar_experimento('rampa')
leitura_geral()
janela.mainloop()
