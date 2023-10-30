#pragma once

#include <gtk/gtk.h>
#include <epoxy/gl.h>
#include <memory>
#include <thread>

#include "AudioManager.h"

#define BINS 513

//deals with shutting down threads
void quitProgram();

static const char *vertex_shader_source_30 = R"(
    #version 330
    layout(location = 0) in vec2 aPos;
    layout(location = 1) in float xOffset;
    layout(location = 2) in float yScale;

    out float scale;

    void main() {
      scale = yScale;
      float yPos = aPos.y * yScale - 1;
      float xPos = aPos.x - xOffset;
      gl_Position = vec4(xPos, yPos, 0.0, 1.0);
    }
)";

static const char *fragment_shader_source_30 = R"(
    #version 330
    in float scale;
    out vec4 frag_color;
    void main() {
        frag_color = vec4(1.0 - scale, scale, 0.0, 1.0); // red color
    }
)";

static const char *vertex_shader_source_21 = R"(
    #version 120
    attribute vec2 aPos;
    attribute float xOffset;
    attribute float yScale;

    varying float scale;

    void main() {
      scale = yScale;
      float yPos = aPos.y * yScale - 1;
      float xPos = aPos.x - xOffset;
      gl_Position = vec4(xPos, yPos, 0.0, 1.0);
    }
)";

static const char *fragment_shader_source_21 = R"(
    #version 120
    varying float scale;

    void main() {
        gl_FragColor = vec4(1.0 - scale, 0.0, scale, 1.0); // red color
    }
)";

static std::shared_ptr<AudioManager> audioManager;

static GLuint program;
static GLuint vao, rectangleVBO, xOffsetVBO, magnitudeVBO;

static GLfloat baseVertices[] = {
  0.0f,  1.0f,  // Top-left (1)
  2.0f / (float)BINS,  1.0f,  // Top-right (2)
  2.0f / (float)BINS, 0.0f,  // Bottom-right (3)
  0.0f, 0.0f   // Bottom-left (4)
};

static guint glAreaTimeoutID;

static gboolean loadNewFFTValues(gpointer user_data);
static void setVBOs(GLfloat * instanceData);
static void glAreaDestroy(GtkWidget * widget, gpointer user_data);
static void realize(GtkGLArea *area);
static gboolean (*render)(GtkGLArea *area, GdkGLContext *context);

void activate (GtkApplication *app, gpointer user_data);