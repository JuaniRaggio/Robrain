#set document(
  title: "Especificaciones BCI Robot",
  author: "Robrain",
)

#set page(
  paper: "a4",
  margin: (
    top: 2.5cm,
    bottom: 2.5cm,
    left: 2cm,
    right: 2cm,
  ),
  numbering: "1",
  number-align: bottom + right,

  header: [
    #set text(size: 9pt, fill: gray)
    #grid(
      columns: (1fr, auto),
      align: (left, right),
      [BCI-Drived Robot],
      [#datetime.today().display("[day]/[month]/[year]")]
    )
    #line(length: 100%, stroke: 0.5pt + gray)
  ],

  footer: context [
    #set text(size: 9pt, fill: gray)
    #line(length: 100%, stroke: 0.5pt + gray)
    #v(0.2em)
    #align(center)[
      Pagina #counter(page).display() / #counter(page).final().first()
    ]
  ]
)

#set text(
  font: "New Computer Modern",
  size: 11pt,
  lang: "es",
  hyphenate: true,
)

#set par(
  justify: true,
  leading: 0.65em,
  first-line-indent: 0em,
  spacing: 1.2em,
)

#set heading(numbering: "1.1")
#show heading.where(level: 1): set text(size: 16pt, weight: "bold")
#show heading.where(level: 2): set text(size: 14pt, weight: "bold")
#show heading.where(level: 3): set text(size: 12pt, weight: "bold")

#show heading: it => {
  v(0.5em)
  it
  v(0.3em)
}

#set list(indent: 1em, marker: ("•", "◦", "▪"))
#set enum(indent: 1em, numbering: "1.a.")

#show raw.where(block: false): box.with(
  fill: luma(240),
  inset: (x: 3pt, y: 0pt),
  outset: (y: 3pt),
  radius: 2pt,
)

#show raw.where(block: true): block.with(
  fill: luma(240),
  inset: 10pt,
  radius: 4pt,
  width: 100%,
)

#show link: underline
// ====================================
// PORTADA
// ====================================

#align(center)[
  #v(2em)
  #text(size: 24pt, weight: "bold")[Brain Computer Interface]
  #v(0.3em)
  #text(size: 16pt)[Control de Robot mediante EEG y EMG]
  #v(1.5em)
  #text(size: 14pt, weight: "bold")[Especificaciones tecnicas Robrain]
  #v(2em)

  #text(size: 11pt, fill: gray)[Autores:]
  #v(0.5em)
  #grid(
    columns: (1fr, 1fr),
    row-gutter: 0.8em,
    column-gutter: 2em,
    text(size: 11pt)[Juan Ignacio Raggio],
    text(size: 11pt)[Bianca Rodriguez Lomazzi],
    text(size: 11pt)[Matias Chimente],
    text(size: 11pt)[Victoria Helena Park],
  )
  #v(1.5em)
]

#line(length: 100%, stroke: 1pt)
#v(1em)

#align(left)[
  #text(size: 12pt, weight: "bold")[Introduccion]
  #v(0.5em)
  #text(size: 11pt)[
    Este documento describe las especificaciones tecnicas del proyecto *Robrain*, un sistema de control de robot mediante interfaces cerebro-computadora (BCI). El proyecto explora dos enfoques principales: el uso de señales electromiograficas (EMG) captadas mediante sensores MyoWare para detectar actividad muscular, y señales electroencefalograficas (EEG) mediante placas OpenBCI para captar actividad cerebral.

    La arquitectura propuesta utiliza microcontroladores ESP32 para la comunicacion inalambrica, Raspberry Pi 5 para el procesamiento de señales y machine learning, y Arduino para el control de motores del robot. El objetivo es lograr un sistema que permita a los usuarios controlar un robot de forma intuitiva mediante sus señales biologicas.
  ]
]

#v(1em)

// ====================================
// FUNCIONES UTILES
// ====================================

// Funcion para crear una caja de nota/observacion
#let nota(contenido) = {
  block(
    fill: rgb("#E3F2FD"),
    stroke: rgb("#1976D2") + 1pt,
    inset: 10pt,
    radius: 4pt,
    width: 100%,
  )[
    #text(weight: "bold", fill: rgb("#1976D2"))[Nota:] #contenido
  ]
}

// Funcion para crear una caja de advertencia
#let importante(contenido) = {
  block(
    fill: rgb("#FFF3E0"),
    stroke: rgb("#F57C00") + 1pt,
    inset: 10pt,
    radius: 4pt,
    width: 100%,
  )[
    #text(weight: "bold", fill: rgb("#F57C00"))[Importante:] #contenido
  ]
}

// Funcion para crear una caja de error comun
#let error(contenido) = {
  block(
    fill: rgb("#FFEBEE"),
    stroke: rgb("#D32F2F") + 1pt,
    inset: 10pt,
    radius: 4pt,
    width: 100%,
  )[
    #text(weight: "bold", fill: rgb("#D32F2F"))[Error Comun:] #contenido
  ]
}

// Funcion para crear una caja de tip
#let tip(contenido) = {
  block(
    fill: rgb("#E8F5E9"),
    stroke: rgb("#388E3C") + 1pt,
    inset: 10pt,
    radius: 4pt,
    width: 100%,
  )[
    #text(weight: "bold", fill: rgb("#388E3C"))[Tip:] #contenido
  ]
}

// Funcion para crear una caja de duda con pregunta y respuesta
#let doubt(pregunta, respuesta) = {
  block(
    fill: rgb("#F3E5F5"),
    stroke: rgb("#7B1FA2") + 1pt,
    inset: 10pt,
    radius: 4pt,
    width: 100%,
  )[
    #text(weight: "bold", fill: rgb("#7B1FA2"), size: 11pt)[Pregunta:]
    #v(0.3em)
    #pregunta
    #v(0.5em)
    #line(length: 100%, stroke: 0.5pt + rgb("#7B1FA2"))
    #v(0.5em)
    #text(weight: "bold", fill: rgb("#7B1FA2"), size: 11pt)[Respuesta:]
    #v(0.3em)
    #respuesta
  ]
}

#pagebreak()

// ====================================
// TABLA DE CONTENIDOS
// ====================================

#outline(
  title: [Índice],
  depth: 3,
  indent: auto,
)

#pagebreak()



= Hardware - MVP EMG

#line(length: 100%)

- MyoWare
- ESP32
- RPI5
- Arduino
- Electrodos
- Robot

#nota[
  La diferencia clave entre EMG y EEG va a ser que con EMG no necesitamos procesamiento "exhaustivo", mientras que con EEG vamos a necesitar ML para procesar las digitales emitidas. Ademas el *sensor a utilizar para EEG* va a ser una placa OpenBCI ya sea *ganglion o Cyton*
]

== Para EMG

```sh
en la persona:                    en una mochila/base:       en el robot:
──────────────                    ──────────────────────     ────────────
músculos → myoware → esp32 → wifi → esp32/arduino → motores
```

== Opcion con mayor procesamiento (sigue siendo EMG)

_Para poder tener EEG, la diferencia esta en el sensor myoware, deberia ser una placa OpenBCI_

```sh
en la persona:                    en una mochila/base:       en el robot:
──────────────                    ──────────────────────     ────────────
músculos → myoware → esp32 → wifi → rpi5 → wifi → esp32/arduino → motores
                                    ↓
                              (procesa, graba,
                               muestra en pantalla)
```

= Hardware - MVP EEG

#line(length: 100%)

#doubt[
  Cual es el problema de usar *MyoWare* para EEG?
][
  El filtro de MyoWare es pasabanda de 20Hz a 500Hz. Pero las ondas
  cerebrales importantes estan por *debajo de 20Hz*

  - *Delta: 0.5-4 Hz*
  - *Theta: 4-8Hz* (meditacion)
  - *Alfa: 8-13Hz* (ojos cerrados)
  - *Beta: 13-30Hz* (concentracion)

  Entonces el filtro del MyoWare *descarta todo lo que esta por 
  debajo de 20Hz o por arriba de 500Hz*. Por lo tanto se pierden 
  las señales en la instacia de hardware, entonces no hay nada que
  hacer a nivel software que pueda solucionarlo
]


```sh
en la persona:                         en una mochila/base:       en el robot:
──────────────                         ──────────────────────     ────────────
cerebro → electrodos EEG → headset → bluetooth/usb → rpi5 → wifi → esp32 → motores
          (cuero cabelludo)   (OpenBCI,                  ↓
                               Muse,              (procesa EEG,
                               NeuroSky)          machine learning,
                                                  detecta intención)
```


