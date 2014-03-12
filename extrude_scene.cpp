#include "extrude_scene.hpp"


void igr::extrude_scene::on_begin () {
  cam.eye  = {10.f, 10.f, 10.f};
  cam.look = {0.f,  0.f,  0.f};
  cam.up   = {0.f,  1.f,  0.f};

  /* Setup the projection */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45, (float) window.getSize().x / (float) window.getSize().y, 1, 1000);
}

void igr::extrude_scene::on_draw () {
  /* Clear the screen */
  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);

  /* Draw axis to check everything ok */
  glBegin(GL_LINES);
    glColor3f(1.f, 0.f, 0.f);
    glVertex3f(-100.f, 0.f, 0.f);
    glVertex3f(+100.f, 0.f, 0.f);

    glColor3f(0.f, 1.f, 0.f);
    glVertex3f(0.f, -100.f, 0.f);
    glVertex3f(0.f, +100.f, 0.f);

    glColor3f(0.f, 0.f, 1.f);
    glVertex3f(0.f, 0.f, -100.f);
    glVertex3f(0.f, 0.f, +100.f);
  glEnd();
}

bool igr::extrude_scene::on_event (event_t event) {
  switch (event.type) {
    case sf::Event::Resized: {

      /* Setup the projection */
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      gluPerspective(45, (float) event.size.width / (float) event.size.height, 1, 1000);

      return true;
    }

    default:
      return false;
  }
}

/* Definition of main here */
int main () {
  igr::extrude_scene{}.run();
}