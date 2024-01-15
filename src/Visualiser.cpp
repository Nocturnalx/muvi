#include "Visualiser.h"

void quitProgram(){
  
  if (!quitProgramRun){
    
    //(destroy timeout to stop timeout from trying to call on null data)
    if (glAreaTimeoutID > 0){
      g_source_remove(glAreaTimeoutID);
      glAreaTimeoutID = 0;
    }

    audioRunning = false;

    while(!threadComplete){

      //wait for the audio thread to complete all of its memory tasks
    }

    quitProgramRun = true;
  }
}

static gboolean loadNewFFTValues(gpointer user_data){

  //only call when buffer is completely full to prevent capturing a half baked frame
  if (audioManager->processor->frameReady){

    GtkGLArea * glArea = GTK_GL_AREA(user_data);

    gtk_gl_area_make_current(glArea);

    if (gtk_gl_area_get_error(glArea) != NULL) {
        return false;
    }

    // setVBOs(data);
    setVBOs(audioManager->getDisplayData());

    gtk_gl_area_queue_render(glArea);

    audioManager->processor->frameReady = false;
  }

  return true;
}

static void setVBOs(GLfloat * magnitudeData){

  //here we should only really want to update every second index of instance data not the rect or positioning data

  glBindBuffer(GL_ARRAY_BUFFER, magnitudeVBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * DISPLAY_BUF_LENGTH, magnitudeData);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static gboolean render_30 (GtkGLArea *area, GdkGLContext *context){
  // inside this function it's safe to use GL; the given
  // `GdkGLContext` has been made current to the drawable
  // surface used by the `GtkGLArea` and the viewport has
  // already been set to be the size of the allocation

  // we can start by clearing the buffer
  glClearColor (0.1568627451, 0.1568627451, 0.1568627451, 1);
  glClear (GL_COLOR_BUFFER_BIT);

  glUseProgram(program);
  glBindVertexArray(vao);
  glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, DISPLAY_BUF_LENGTH);
  glBindVertexArray(0);

  // we completed our drawing; the draw commands will be
  // flushed at the end of the signal emission chain, and
  // the buffers will be drawn on the window
  return TRUE;
}

static gboolean render_21 (GtkGLArea *area, GdkGLContext *context){
  // inside this function it's safe to use GL; the given
  // `GdkGLContext` has been made current to the drawable
  // surface used by the `GtkGLArea` and the viewport has
  // already been set to be the size of the allocation

  // we can start by clearing the buffer
  glClearColor (0.1568627451, 0.1568627451, 0.1568627451, 1);
  glClear (GL_COLOR_BUFFER_BIT);
  glUseProgram(program);
  glBindVertexArray(vao);
  for(int i = 0; i < DISPLAY_BUF_LENGTH; i++){

    glBindBuffer(GL_ARRAY_BUFFER, xOffsetVBO);
    glVertexAttribPointer(1,1,GL_FLOAT, GL_FALSE, sizeof(float), (void*)(i * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, magnitudeVBO);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)(i * sizeof(float)));
    glEnableVertexAttribArray(2);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  }

  glBindVertexArray(0);

  // we completed our drawing; the draw commands will be
  // flushed at the end of the signal emission chain, and
  // the buffers will be drawn on the window
  return TRUE;
}

static void realize(GtkGLArea *area) {

  gtk_gl_area_make_current(area);

  if (gtk_gl_area_get_error(area) != NULL) {
    return;
  }

  // OpenGL initialization code here, like setting up VBOs, shaders, etc.

  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

  int version = epoxy_gl_version(); //version checking

  if (version < 30) {
    //We don't have at least OpenGL ES 3.0.

    std::cout << "openGL version below 3.0, currently set to: " << version << "\n";

    glShaderSource(vertex_shader, 1, &vertex_shader_source_21, NULL);
    glCompileShader(vertex_shader);

    glShaderSource(fragment_shader, 1, &fragment_shader_source_21, NULL);
    glCompileShader(fragment_shader);

    render = &render_21;

  } else {

    glShaderSource(vertex_shader, 1, &vertex_shader_source_30, NULL);
    glCompileShader(vertex_shader);

    glShaderSource(fragment_shader, 1, &fragment_shader_source_30, NULL);
    glCompileShader(fragment_shader);

    render = &render_30;
  }

  //render signal cb set here as we need to know which function to use
  g_signal_connect(area, "render", G_CALLBACK(*render), NULL); //render callback

  // Compile shaders and setup program
  
  program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  //VAOs and VBOs
  //here we should setup the rect buffer and the instance position data first then call the setVBO to do only the values

  GLfloat xOffsetData[DISPLAY_BUF_LENGTH];
  GLfloat initMagnitudeData[DISPLAY_BUF_LENGTH];

  for (int i = 0; i < DISPLAY_BUF_LENGTH; i++){
    xOffsetData[i] = (float)1 - ((float)i * 2.0f / (float)DISPLAY_BUF_LENGTH); ///0.02 is 2/100 (100 rects in a range of 2)
    initMagnitudeData[i] = 0.5;
  }

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &rectangleVBO);
  glBindBuffer(GL_ARRAY_BUFFER, rectangleVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(baseVertices), baseVertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glGenBuffers(1, &xOffsetVBO);
  glBindBuffer(GL_ARRAY_BUFFER, xOffsetVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*DISPLAY_BUF_LENGTH, xOffsetData, GL_STATIC_DRAW);
  glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribDivisor(1, 1);

  glGenBuffers(1, &magnitudeVBO);
  glBindBuffer(GL_ARRAY_BUFFER, magnitudeVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * DISPLAY_BUF_LENGTH, initMagnitudeData, GL_STATIC_DRAW);
  glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
  glEnableVertexAttribArray(2);
  glVertexAttribDivisor(2, 1);

  glBindVertexArray(0);
}

//tidy up threads when glArea gets destroyed
static void glAreaDestroy(GtkWidget * widget, gpointer user_data){

  quitProgram();
}

void activate (GtkApplication *app, gpointer user_data) {
    
  GtkWidget *window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Window");
  gtk_window_set_default_size(GTK_WINDOW(window), 700, 500);
  // gtk_window_fullscreen(GTK_WINDOW(window));

  GtkGLArea *glArea = GTK_GL_AREA(gtk_gl_area_new());
  gtk_gl_area_set_has_depth_buffer(glArea, FALSE);
  g_signal_connect(glArea, "realize", G_CALLBACK(realize), NULL); //realisation

  glAreaTimeoutID = g_timeout_add(1000/60, loadNewFFTValues, glArea); //set rendering timeout (frame rate)

  g_signal_connect(glArea, "destroy", G_CALLBACK(glAreaDestroy), NULL); //on glArea destroy

  gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(glArea));

  gtk_widget_show_all (window);

  audioManager = std::make_shared<AudioManager>();

  //start pulse audio
  std::thread audioThread(setupAudio, audioManager);
  audioThread.detach();
}