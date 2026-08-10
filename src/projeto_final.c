#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint tex_chao;
GLuint tex_parede;
GLuint tex_parede_externa;
GLuint tex_grama; 
GLuint tex_telhado;

// Configurações da câmera 
float camX = 0.0;
float camY = 1.5; 
float camZ = 8.0;
float camAngle = 0.0;

GLuint carregarTextura(const char* nome_arquivo) {
    GLuint texturaID;
    glGenTextures(1, &texturaID);
    glBindTexture(GL_TEXTURE_2D, texturaID);

    // Configuração de repetição e filtragem da textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int largura, altura, canais;
    unsigned char *dados = stbi_load(nome_arquivo, &largura, &altura, &canais, 4);
    
    if (dados) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, largura, altura, 0, GL_RGBA, GL_UNSIGNED_BYTE, dados);
    } else {
        printf("FALHA AO CARREGAR: %s\n", nome_arquivo);
    }
    
    stbi_image_free(dados);
    return texturaID;
}

void desenhaCuboTexturizado(float repX, float repY) {
    glBegin(GL_QUADS);
    // Face Frontal
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f,  0.5f);
    glTexCoord2f(repX, 0.0f); glVertex3f( 0.5f, -0.5f,  0.5f);
    glTexCoord2f(repX, repY); glVertex3f( 0.5f,  0.5f,  0.5f);
    glTexCoord2f(0.0f, repY); glVertex3f(-0.5f,  0.5f,  0.5f);

    // Face Traseira
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 0.5f, -0.5f, -0.5f);
    glTexCoord2f(repX, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(repX, repY); glVertex3f(-0.5f,  0.5f, -0.5f);
    glTexCoord2f(0.0f, repY); glVertex3f( 0.5f,  0.5f, -0.5f);

    // Face Superior
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f,  0.5f,  0.5f);
    glTexCoord2f(repX, 0.0f); glVertex3f( 0.5f,  0.5f,  0.5f);
    glTexCoord2f(repX, repY); glVertex3f( 0.5f,  0.5f, -0.5f);
    glTexCoord2f(0.0f, repY); glVertex3f(-0.5f,  0.5f, -0.5f);

    // Face Inferior
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(repX, 0.0f); glVertex3f( 0.5f, -0.5f, -0.5f);
    glTexCoord2f(repX, repY); glVertex3f( 0.5f, -0.5f,  0.5f);
    glTexCoord2f(0.0f, repY); glVertex3f(-0.5f, -0.5f,  0.5f);

    // Face Direita
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 0.5f, -0.5f,  0.5f);
    glTexCoord2f(repX, 0.0f); glVertex3f( 0.5f, -0.5f, -0.5f);
    glTexCoord2f(repX, repY); glVertex3f( 0.5f,  0.5f, -0.5f);
    glTexCoord2f(0.0f, repY); glVertex3f( 0.5f,  0.5f,  0.5f);

    // Face Esquerda
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(repX, 0.0f); glVertex3f(-0.5f, -0.5f,  0.5f);
    glTexCoord2f(repX, repY); glVertex3f(-0.5f,  0.5f,  0.5f);
    glTexCoord2f(0.0f, repY); glVertex3f(-0.5f,  0.5f, -0.5f);
    glEnd();
}

void desenhaParedeComTextura(float x, float y, float z, float sx, float sy, float sz, float repX, float repY) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(sx, sy, sz);
    desenhaCuboTexturizado(repX, repY);
    glPopMatrix();
}

void desenhaCaminhoBézier() {
    glColor3f(0.6f, 0.55f, 0.5f);
    for (float t = 0.0f; t <= 1.0f; t += 0.05f) {
        float u = 1.0f - t;
        float tt = t * t;
        float uu = u * u;
        float uuu = uu * u;
        float ttt = tt * t;

        // Pontos de controle da curva paramétrica
        float p0x = 0.0f, p0z = 15.0f;
        float p1x = -1.5f, p1z = 10.0f;
        float p2x =  1.5f, p2z =  7.0f;
        float p3x =  0.0f, p3z =  5.0f;

        // Cálculo das coordenadas (x, z) ao longo da curva cúbica de Bézier
        float x = uuu * p0x + 3 * uu * t * p1x + 3 * u * tt * p2x + ttt * p3x;
        float z = uuu * p0z + 3 * uu * t * p1z + 3 * u * tt * p2z + ttt * p3z;

        glPushMatrix();
        glTranslatef(x, 0.01f, z);
        desenhaParedeComTextura(0.0f, 0.0f, 0.0f, 0.8f, 0.02f, 0.6f, 1.0f, 1.0f);
        glPopMatrix();
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    float dirX = sin(camAngle);
    float dirZ = -cos(camAngle);
    gluLookAt(camX, camY, camZ, camX + dirX, camY, camZ + dirZ, 0.0, 1.0, 0.0);

    glColor3f(1.0, 1.0, 1.0); 

    // Renderização do Quintal 
    glBindTexture(GL_TEXTURE_2D, tex_grama);
    desenhaParedeComTextura(0.0, -0.1, 0.0, 50.0, 0.1, 50.0, 20.0, 20.0); 

    // Desenho do caminho de entrada com Curvas de Bézier
    glDisable(GL_TEXTURE_2D);
    desenhaCaminhoBézier();
    glEnable(GL_TEXTURE_2D);

    // Chão interno da casa
    glBindTexture(GL_TEXTURE_2D, tex_chao);
    desenhaParedeComTextura(0.0, 0.0, 0.0, 10.0, 0.1, 10.0, 5.0, 5.0); 

    // Paredes externas (laterais e fundo)
    glBindTexture(GL_TEXTURE_2D, tex_parede);
    desenhaParedeComTextura(-5.0, 1.5,  0.0, 0.2, 3.0, 10.0, 4.0, 1.0);
    desenhaParedeComTextura( 5.0, 1.5,  0.0, 0.2, 3.0, 10.0, 4.0, 1.0);
    desenhaParedeComTextura( 0.0, 1.5, -5.0, 10.0, 3.0, 0.2, 4.0, 1.0);
    
    // Divisão e aberturas da parede da frente
    desenhaParedeComTextura(-4.500f, 1.5f, 5.0f, 1.00f, 3.0f, 0.2f, 1.0f, 1.0f); 
    desenhaParedeComTextura(-3.250f, 0.5f, 5.0f, 1.50f, 1.0f, 0.2f, 1.0f, 0.5f); 
    desenhaParedeComTextura(-3.250f, 2.5f, 5.0f, 1.50f, 1.0f, 0.2f, 1.0f, 0.5f); 
    desenhaParedeComTextura(-1.625f, 1.5f, 5.0f, 1.75f, 3.0f, 0.2f, 1.0f, 1.0f); 
    desenhaParedeComTextura( 0.000f, 2.5f, 5.0f, 1.50f, 1.0f, 0.2f, 1.0f, 0.5f); 
    desenhaParedeComTextura( 1.625f, 1.5f, 5.0f, 1.75f, 3.0f, 0.2f, 1.0f, 1.0f); 
    desenhaParedeComTextura( 3.250f, 0.5f, 5.0f, 1.50f, 1.0f, 0.2f, 1.0f, 0.5f); 
    desenhaParedeComTextura( 3.250f, 2.5f, 5.0f, 1.50f, 1.0f, 0.2f, 1.0f, 0.5f); 
    desenhaParedeComTextura( 4.500f, 1.5f, 5.0f, 1.00f, 3.0f, 0.2f, 1.0f, 1.0f); 
    
    // Paredes divisórias internas dos cômodos
    desenhaParedeComTextura(-2.5, 1.5, 0.0, 4.8, 3.0, 0.2, 2.0, 1.0); 
    desenhaParedeComTextura( 4.0, 1.5, 0.0, 2.0, 3.0, 0.2, 1.0, 1.0); 

    // Forro interno do teto
    desenhaParedeComTextura(0.0, 3.0, 0.0, 10.0, 0.1, 10.0, 4.0, 4.0); 
    glBindTexture(GL_TEXTURE_2D, 0); 

    glDisable(GL_TEXTURE_2D);

    // --- Sala de Estar ---
    glColor3f(0.12f, 0.12f, 0.12f);
    desenhaParedeComTextura(-4.85f, 1.6f, 2.5f, 0.15f, 1.2f, 2.4f, 1.0f, 1.0f);
    glColor3f(0.15f, 0.2f, 0.35f);
    desenhaParedeComTextura(-4.77f, 1.6f, 2.5f, 0.08f, 1.0f, 2.2f, 1.0f, 1.0f);

    glColor3f(0.75f, 0.72f, 0.68f); 
    desenhaParedeComTextura(3.8f, 0.4f, 2.5f, 1.6f, 0.8f, 2.2f, 1.0f, 1.0f); 
    desenhaParedeComTextura(4.6f, 1.1f, 2.5f, 0.2f, 1.1f, 2.2f, 1.0f, 1.0f); 

    // Mesinha de centro da sala
    glColor3f(0.45f, 0.28f, 0.15f); 
    desenhaParedeComTextura(1.5f, 0.35f, 2.5f, 1.2f, 0.1f, 1.4f, 1.0f, 1.0f);
    glColor3f(0.15f, 0.15f, 0.15f); 
    desenhaParedeComTextura(1.0f, 0.15f, 2.0f, 0.08f, 0.3f, 0.08f, 1.0f, 1.0f);
    desenhaParedeComTextura(2.0f, 0.15f, 2.0f, 0.08f, 0.3f, 0.08f, 1.0f, 1.0f);
    desenhaParedeComTextura(1.0f, 0.15f, 3.0f, 0.08f, 0.3f, 0.08f, 1.0f, 1.0f);
    desenhaParedeComTextura(2.0f, 0.15f, 3.0f, 0.08f, 0.3f, 0.08f, 1.0f, 1.0f);

    // --- Cozinha ---
    glColor3f(0.65f, 0.65f, 0.68f); 
    desenhaParedeComTextura(0.4f, 1.4f, -4.3f, 1.0f, 2.8f, 1.0f, 1.0f, 1.0f);
    glColor3f(0.2f, 0.2f, 0.2f); 
    desenhaParedeComTextura(0.0f, 2.0f, -3.78f, 0.05f, 0.6f, 0.08f, 1.0f, 1.0f); 
    desenhaParedeComTextura(0.0f, 1.0f, -3.78f, 0.05f, 0.8f, 0.08f, 1.0f, 1.0f); 

    glColor3f(0.3f, 0.35f, 0.4f); 
    desenhaParedeComTextura(2.2f, 0.45f, -4.2f, 2.0f, 0.9f, 1.0f, 1.0f, 1.0f);
    glColor3f(0.9f, 0.9f, 0.9f); 
    desenhaParedeComTextura(2.2f, 0.92f, -4.2f, 2.1f, 0.08f, 1.1f, 1.0f, 1.0f);

    glColor3f(0.2f, 0.2f, 0.2f); 
    desenhaParedeComTextura(4.2f, 0.45f, -4.2f, 1.0f, 0.9f, 1.0f, 1.0f, 1.0f);
    glColor3f(0.9f, 0.9f, 0.9f); 
    desenhaParedeComTextura(4.2f, 0.93f, -4.2f, 1.02f, 0.06f, 1.05f, 1.0f, 1.0f);
    
    // Bocas do fogão
    glColor3f(0.1f, 0.1f, 0.1f); 
    desenhaParedeComTextura(4.0f, 0.97f, -4.4f, 0.2f, 0.02f, 0.2f, 1.0f, 1.0f);
    desenhaParedeComTextura(4.4f, 0.97f, -4.4f, 0.2f, 0.02f, 0.2f, 1.0f, 1.0f);
    desenhaParedeComTextura(4.0f, 0.97f, -4.0f, 0.2f, 0.02f, 0.2f, 1.0f, 1.0f);
    desenhaParedeComTextura(4.4f, 0.97f, -4.0f, 0.2f, 0.02f, 0.2f, 1.0f, 1.0f);

    // --- Sala de Janta ---
    glColor3f(0.45f, 0.28f, 0.15f); 
    desenhaParedeComTextura(-3.0f, 0.55f, -2.5f, 1.4f, 0.08f, 1.0f, 1.0f, 1.0f);
    
    // Pés da mesa
    glColor3f(0.2f, 0.2f, 0.2f);
    desenhaParedeComTextura(-3.5f, 0.25f, -2.9f, 0.08f, 0.5f, 0.08f, 1.0f, 1.0f);
    desenhaParedeComTextura(-2.5f, 0.25f, -2.9f, 0.08f, 0.5f, 0.08f, 1.0f, 1.0f);
    desenhaParedeComTextura(-3.5f, 0.25f, -2.1f, 0.08f, 0.5f, 0.08f, 1.0f, 1.0f);
    desenhaParedeComTextura(-2.5f, 0.25f, -2.1f, 0.08f, 0.5f, 0.08f, 1.0f, 1.0f);

    // Cadeiras 
    glColor3f(0.6f, 0.55f, 0.5f); 
    desenhaParedeComTextura(-3.0f, 0.35f, -1.6f, 0.4f, 0.4f, 0.4f, 1.0f, 1.0f);
    desenhaParedeComTextura(-3.0f, 0.65f, -1.4f, 0.4f, 0.5f, 0.1f, 1.0f, 1.0f); 

    desenhaParedeComTextura(-3.0f, 0.35f, -3.4f, 0.4f, 0.4f, 0.4f, 1.0f, 1.0f);
    desenhaParedeComTextura(-3.0f, 0.65f, -3.6f, 0.4f, 0.5f, 0.1f, 1.0f, 1.0f); 

    desenhaParedeComTextura(-4.0f, 0.35f, -2.5f, 0.4f, 0.4f, 0.4f, 1.0f, 1.0f);
    desenhaParedeComTextura(-4.2f, 0.65f, -2.5f, 0.1f, 0.5f, 0.4f, 1.0f, 1.0f); 

    desenhaParedeComTextura(-2.0f, 0.35f, -2.5f, 0.4f, 0.4f, 0.4f, 1.0f, 1.0f);
    desenhaParedeComTextura(-1.8f, 0.65f, -2.5f, 0.1f, 0.5f, 0.4f, 1.0f, 1.0f); 
    glEnable(GL_TEXTURE_2D);

    // --- Telhado da Casa ---
    glBindTexture(GL_TEXTURE_2D, tex_telhado);
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-5.2f, 3.0f,  5.2f); 
    glTexCoord2f(5.0f, 0.0f); glVertex3f( 5.2f, 3.0f,  5.2f); 
    glTexCoord2f(5.0f, 3.0f); glVertex3f( 5.2f, 4.5f,  0.0f); 
    glTexCoord2f(0.0f, 3.0f); glVertex3f(-5.2f, 4.5f,  0.0f); 

    glTexCoord2f(0.0f, 0.0f); glVertex3f(-5.2f, 3.0f, -5.2f); 
    glTexCoord2f(5.0f, 0.0f); glVertex3f( 5.2f, 3.0f, -5.2f); 
    glTexCoord2f(5.0f, 3.0f); glVertex3f( 5.2f, 4.5f,  0.0f); 
    glTexCoord2f(0.0f, 3.0f); glVertex3f(-5.2f, 4.5f,  0.0f); 
    glEnd();

    glBindTexture(GL_TEXTURE_2D, tex_telhado);
    glBegin(GL_TRIANGLES);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-5.0f, 3.0f,  5.0f); 
    glTexCoord2f(2.0f, 0.0f); glVertex3f(-5.0f, 3.0f, -5.0f); 
    glTexCoord2f(1.0f, 1.5f); glVertex3f(-5.0f, 4.5f,  0.0f); 
    
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 5.0f, 3.0f,  5.0f); 
    glTexCoord2f(2.0f, 0.0f); glVertex3f( 5.0f, 3.0f, -5.0f); 
    glTexCoord2f(1.0f, 1.5f); glVertex3f( 5.0f, 4.5f,  0.0f); 
    glEnd();

    // --- Acabamentos e Testreiras da Estrutura ---
    glDisable(GL_TEXTURE_2D); 
    glColor3f(0.25f, 0.25f, 0.25f);
    desenhaParedeComTextura(0.0f, 0.05f, 5.1f, 1.6f, 0.1f, 0.3f, 1.0f, 1.0f);
    desenhaParedeComTextura(-3.25f, 0.9f, 5.1f, 1.6f, 0.1f, 0.2f, 1.0f, 1.0f);
    desenhaParedeComTextura(3.25f, 0.9f, 5.1f, 1.6f, 1.6f, 0.2f, 1.0f, 1.0f);
    
    glColor3f(0.85f, 0.85f, 0.85f);
    desenhaParedeComTextura(0.0f, 3.0f, 5.15f, 10.2f, 0.2f, 0.1f, 1.0f, 1.0f);
    desenhaParedeComTextura(0.0f, 3.0f, -5.15f, 10.2f, 0.2f, 0.1f, 1.0f, 1.0f);
    desenhaParedeComTextura(-5.15f, 3.0f, 0.0f, 0.1f, 0.2f, 10.2f, 1.0f, 1.0f);
    desenhaParedeComTextura(5.15f, 3.0f, 0.0f, 0.1f, 0.2f, 10.2f, 1.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
    glutSwapBuffers();
}

// Função de colisão simples para impedir que a câmera atravesse paredes
int checaParede(float proximoX, float proximoZ) {
    if (proximoX < -24.0 || proximoX > 24.0) return 0;
    if (proximoZ < -24.0 || proximoZ > 24.0) return 0;

    if (proximoZ > -5.0 && proximoZ < 5.0) {
        if (proximoX < -4.5 && proximoX > -5.5) return 0; 
        if (proximoX > 4.5 && proximoX < 5.5) return 0;  
    }
    if (proximoX > -5.0 && proximoX < 5.0) {
        if (proximoZ < -4.5 && proximoZ > -5.5) return 0;
    }
    if (proximoZ > 4.5 && proximoZ < 5.5) {
        if (proximoX < -0.75 || proximoX > 0.75) return 0; 
    }
    if (proximoZ > -0.5 && proximoZ < 0.5) {
        if (proximoX < -0.1 || proximoX > 3.0) return 0; 
    }
    return 1; 
}

void keyboard(unsigned char key, int x, int y) {
    float passo = 0.4; 
    float giro = 0.1; 
    float dirX = sin(camAngle);
    float dirZ = -cos(camAngle);

    switch (key) {
        case 'w': 
            if (checaParede(camX + dirX * passo, camZ + dirZ * passo)) {
                camX += dirX * passo; camZ += dirZ * passo; 
            } break; 
        case 's': 
            if (checaParede(camX - dirX * passo, camZ - dirZ * passo)) {
                camX -= dirX * passo; camZ -= dirZ * passo; 
            } break; 
        case 'a': camAngle -= giro; break; 
        case 'd': camAngle += giro; break; 
        case 27: exit(0); break; 
    }
    glutPostRedisplay(); 
}

void init() {
    glClearColor(0.6f, 0.8f, 0.9f, 1.0f); 
    glEnable(GL_DEPTH_TEST); 
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);   
    glEnable(GL_LIGHT0);
    
    GLfloat luz_pos[] = { 0.0, 4.0, 2.0, 1.0 };
    GLfloat luz_ambiente[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat luz_difusa[] = { 1.0f, 0.9f, 0.7f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, luz_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, luz_ambiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luz_difusa);

    glEnable(GL_TEXTURE_2D); 

    tex_chao = carregarTextura("texturas/chao.jpg");
    tex_parede = carregarTextura("texturas/parede_interna.jpg");
    tex_parede_externa = carregarTextura("texturas/parede_externa.jpg");
    tex_grama = carregarTextura("texturas/grama.jpeg"); 
    tex_telhado = carregarTextura("texturas/telhado.jpeg");
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float)w / h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Casa");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard); 
    glutMainLoop();
    return 0;
}