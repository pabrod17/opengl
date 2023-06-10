# Practicas Render 3D

  * Prazo entrega: 23:59h luns 12 de xuño 2023
  * Miembros
    * Lema Carril, Ana María
    * López García, María
    * Rodríguez Miñambres, Pablo

## Práctica OpenGL: Iluminación con Phong

A partir do esqueleto de código do arquivo [**spinningcube_withlight_SKEL.cpp**](https://gitlab.citic.udc.es/emilio.padron/igm-opengl/-/tree/practicaOpenGL) tedes que implementar unha iluminación simple con Phong sobre un cubo 3D en movemento. A implementación ten que funcionar para cámara(s), punto(s) de luz e cubo 3D en posicións arbitrarias.

### Primeira parte (ata 4 puntos sobre 10)

Cunha soa fonte de luz (luz punto) e co cubo cun único material homoxéneo (comportamento idéntico fronte á luz en todos os puntos da superficie do cubo), trátase de aplicar Phong shading básico.

* Ver ata diapositiva 11 (de 28) en igm_render_teoria3.pdf

### Segunda parte (ata +2 puntos sobre o anterior)

Engadide un segundo modelo 3D á escena (conservando o cubo): unha pirámide triangular (tetraedro), e un segundo punto de luz. No tiro de cámara téñense que apreciar as dúas luces.

### Terceira parte (ata +2 puntos sobre o anterior)

Engadide un mapa difuso nas caras do(s) modelo(s) para o cálculo de Phong, o que permite ter distintas propiedades de reflexión difusa por fragmento.

* Diapositiva 12 (de 28) en igm_render_teoria3.pdf

### Cuarta parte (ata + 2 puntos sobre o anterior)

Engadide tamén un mapa especular, que permita distintos comportamentos da compoñente especular da luz en función do punto iluminado nos modelos. O mapa especular ten que ser distinto do mapa difuso empregado.

* Diapositiva 13 (de 28) en igm_render_teoria3.pdf
* Ver tamén mapeado de texturas a partir de diapositiva 7 en igm_render_lab3.pdf

En este apartado se realizó una mejora de la textura del tetraedro con respecto al ejercicio 3.

### Quinta parte (ata + 2 puntos, substituíndo opcionalmente algunha das partes 2, 3 ou 4)

Engadide unha segunda cámara, permitindo cun atallo de teclado conmutar o render entre as dúas cámaras.

* Para realizar el cambio de cámaras utilizamos la letra "C".

### Entregables

   * URL a repositorio git co código, no que se vexa o progreso nas versións do traballo.

No repositorio ten que haber:

* Os arquivos co código fonte e un makefile que constrúa o(s) binario(s).

* Capturas de pantalla nas que se vexan renders da práctica e, ademais, a versión de OpenGL na saída estándar.